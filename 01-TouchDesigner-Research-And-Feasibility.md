# TouchDesigner Research & Feasibility Reference

> **Purpose:** Permanent company reference capturing what TouchDesigner is, whether building a similar system is possible, scope/cost, architecture concepts, and strategic options.  
> **Audience:** Founders, architects, and coding agents.  
> **Status:** Internal / private company use.  
> **Last updated:** 2026-08-04

---

## 1. Executive Summary

| Question | Answer |
|----------|--------|
| What is TouchDesigner? | A real-time **node-based media programming environment** by **Derivative** for interactive visuals, installations, live performance, projection mapping, VR/AR, and data-driven experiences. |
| Can we build the same *class* of product? | **Yes** — with multi-year investment, senior real-time graphics talent, and a phased platform approach. |
| Can we ship “almost everything TD has” quickly? | **No.** Feature parity with a mature product (20+ years of iteration) is a multi-year platform bet. |
| Recommended approach | Build a **TD-class engine** with original UI/branding, starting from a **narrow MVP**, expanding operator families phase by phase. |
| Private company use | Still treat this as **clean-room design**: independent architecture, original UI, original operator names. Do not reverse-engineer binaries or copy assets. |

**Bottom line:** Building a TouchDesigner-*class* system is possible. Treating it as “clone everything including the same UI overnight” is not realistic. Treat it as a specialized real-time engine + visual IDE + operator library.

---

## 2. What TouchDesigner Actually Is

TouchDesigner is not a simple effects app. It is closer to:

- A **real-time graphics / media engine**
- Plus a **visual programming IDE**
- Plus a **huge library of operators**
- Plus a **hardware/protocol I/O layer**
- Plus **deep Python scripting**

### 2.1 Node-based paradigm

Everything is built by connecting **operators** (nodes) into **networks** (graphs).

- Data flows through typed connections.
- Operators **cook** (evaluate) when inputs change or on demand.
- Networks can be hierarchical (components encapsulate sub-networks).
- Parameters can be constants, expressions, or driven by other operators.
- Live iteration: change a wire/parameter → result updates immediately.

### 2.2 Operator families (core building blocks)

| Family | Full name | Role |
|--------|-----------|------|
| **TOP** | Texture Operators | Images, video, GPU effects, compositing, render targets, particles (GPU) |
| **CHOP** | Channel Operators | Time-series / motion / audio / control data (MIDI, sensors, curves, filters) |
| **SOP** | Surface Operators | 3D geometry (points, polygons, meshes, procedural modeling, deformation) |
| **MAT** | Materials | Shading / materials for 3D (including GLSL) |
| **DAT** | Data Operators | Tables, text, scripts, JSON/XML, networking, Python execution |
| **COMP** | Components | Containers, 3D objects, UI panels, cameras, lights, windows, reusable modules |

### 2.3 Key product capabilities

- Real-time 2D/3D compositing and generative graphics
- Procedural 3D modeling, animation, rendering
- Audio analysis / audio-reactive visuals
- UI building (panels/widgets) for control surfaces and installations
- Hardware & protocols: cameras, depth sensors, MIDI, OSC, DMX/Art-Net, NDI, Spout/Syphon, serial, web, databases
- Projection mapping, multi-display / multi-output, show control
- Python everywhere (parameters, events, network generation)
- Custom C++ operators (SDK)
- Component cloning, replicators, instancing
- Project formats: `.toe` (project), `.tox` (component)

### 2.4 Typical use cases

- Immersive installations
- Live visuals / VJ / concerts
- Projection mapping
- Interactive museum / brand experiences
- LED walls / media servers (often as part of a pipeline)
- Generative design R&D
- Sensor-driven art and show control

---

## 3. Architecture Overview (Conceptual — Clean Room)

These are **public, conceptual** architecture ideas used by node-based real-time tools. They are not a dump of Derivative’s private source design.

### 3.1 High-level layers

```
┌─────────────────────────────────────────────────────────────┐
│  Application Shell (UI, project load/save, licensing)       │
├─────────────────────────────────────────────────────────────┤
│  Network Editor + Parameter System + Viewers                │
├─────────────────────────────────────────────────────────────┤
│  Graph Runtime (nodes, wires, dirty/cook, scheduling)       │
├─────────────────────────────────────────────────────────────┤
│  Operator Library (TOP / CHOP / SOP / MAT / DAT / COMP)     │
├─────────────────────────────────────────────────────────────┤
│  Scripting (Python embed) + Expression engine               │
├─────────────────────────────────────────────────────────────┤
│  Render / GPU backend + Audio + I/O devices                 │
├─────────────────────────────────────────────────────────────┤
│  OS / Drivers / GPU / Capture cards / Controllers           │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Cooking model (critical concept)

- Operators mark themselves **dirty** when inputs/params change.
- A **cook** pass evaluates only what is needed for outputs/viewers.
- Different data rates: textures (frame), channels (sample/time-slice), tables (event/batch).
- Performance depends on selective evaluation, caching, bypass flags, GPU resource reuse.

Getting cooking wrong = either incorrect results or unusable performance.

### 3.3 Data types flowing through the graph

| Type | Typical carrier | Notes |
|------|-----------------|-------|
| Texture / image | GPU texture / FBO | TOPs |
| Channels | Float arrays / time slices | CHOPs |
| Geometry | Point/prim buffers | SOPs |
| Material | Shader + params | MATs |
| Table / text | Rows/cols or strings | DATs |
| Object / hierarchy | Scene graph nodes | COMPs |

### 3.4 Rendering pipeline (conceptual)

1. SOP geometry + MAT materials + Camera/Light COMPs  
2. Render into a texture (Render TOP equivalent)  
3. Further TOP processing / compositing  
4. Output to Window / NDI / Spout / displays / projectors  

### 3.5 What TouchDesigner-class tools typically use (public knowledge)

| Area | TD-era stack (public) | Modern “same or better” choice |
|------|------------------------|--------------------------------|
| Core language | C++ | **C++20/23** (primary) |
| GPU API | OpenGL + GLSL (classic custom TOP path); newer builds explore Vulkan paths | **Vulkan 1.3+** primary, GLSL/HLSL → SPIR-V; keep OpenGL interop if needed |
| Scripting | Python | **Python 3.11+** embedded (CPython) |
| Shaders | GLSL | GLSL + **Slang** or HLSL cross-compile to SPIR-V |
| Custom plugins | C++ operator SDK | Same: DLL/plugin ABI for custom ops |
| UI | Custom tool UI | **Qt 6** shell *or* **Dear ImGui** tool UI + node editor |
| Video | Capture / codecs / NDI etc. | FFmpeg (careful licensing), NDI SDK, Spout, Media Foundation / AVFoundation |
| Platform | Windows primary; macOS secondary | **Windows first**, then macOS, Linux later |

---

## 4. Feasibility: Is It Possible?

### 4.1 Scope levels

| Scope | What you get | Timeline (strong team) | Ballpark cost (fully loaded) |
|-------|--------------|------------------------|------------------------------|
| **Demo / toy** | Basic graph, few nodes, simple video/3D | 2–6 months | Low |
| **Internal MVP** | Solid cook system, limited operators, real-time output, basic UI | **12–24 months**, 4–8 people | **~$1.5M–$4M** |
| **Serious product** | Many operators, I/O, Python, stability, docs | **3–5 years**, 8–15 people | **~$5M–$15M+** |
| **Near feature breadth of TD** | Hundreds of ops, polish, ecosystem | **5–10+ years** continuous | **~$15M–$50M+** |

### 4.2 Why full parity is hard

1. **Cook / dependency system** — correctness + performance under live load  
2. **GPU resource lifetime** — continuous real-time, multi-output  
3. **Operator breadth** — years of content, not one sprint  
4. **UI with live previews** in large graphs  
5. **Hardware I/O jungle** — capture cards, NDI, DMX, multi-GPU, projectors  
6. **Show-time stability** — crashes are unacceptable live  
7. **Ecosystem** — examples, docs, community assets, third-party ops  

### 4.3 What is realistic for a company private tool

| Goal | Realistic? |
|------|------------|
| TD-like node graph + cook engine | Yes |
| Core TOP/CHOP/SOP/DAT/COMP families (subset first) | Yes |
| Python parameter driving + scripting | Yes |
| Same *product category* as TD | Yes |
| 1:1 UI clone of Derivative’s product | Bad idea / unnecessary for private tool |
| Every operator TD has in year 1 | No |
| Production-ready multi-output mapping + full device matrix in year 1 | Unlikely unless team is large and specialized |

---

## 5. Legal / IP Notes (Even for Private Use)

You stated this is **private company use, not commercial distribution**. That reduces *market* risk, but does **not** mean “copy TD freely” is free of risk:

- EULAs typically **forbid reverse engineering / decompilation**.
- Copying **icons, assets, exact UI chrome, docs text, proprietary names** can still create IP issues.
- **Ideas and categories** (node-based real-time media, texture ops, channel ops) are fine to implement independently.
- **Best practice for company IP hygiene:** clean-room design, original branding, original operator names, public docs + independent implementation only.

**Recommended company policy:**

1. Do **not** decompile TouchDesigner.  
2. Do **not** copy Derivative assets, icons, or UI pixel-for-pixel.  
3. Do study **public** documentation, tutorials, and general node-tool literature.  
4. Build **original** UI and product identity inspired by the *category*, not a skin of TD.  
5. Keep an internal note: “inspired by node-based real-time media tools (TouchDesigner, Houdini, Nuke, Max, vvvv, Notch).”

---

## 6. Strategic Options for the Company

### Option A — License TouchDesigner commercially
Fastest path to results. Build shows/products *on top of* TD.

### Option B — Domain-specific TD-class tool (recommended for building)
Build only what your company needs first (e.g. LED pipeline, generative brand tool, installation mapper). Expand later.

### Option C — Engine-backed hybrid
Use Unreal / Unity / Godot / custom Vulkan for rendering; add your own node graph layer.

### Option D — Full long-term platform competitor
Only with multi-year runway, senior graphics engineers, tech artists, live-systems QA.

**For this repo / initiative:** Option B + modern stack (C++ / Vulkan / Python), phased to grow toward Option D capability over time.

---

## 7. Competitive / Related Landscape (for learning, not copying)

| Tool | Notes |
|------|--------|
| **TouchDesigner** | Reference category leader for real-time media nodes |
| **Notch** | Real-time motion graphics, more closed ecosystem |
| **vvvv / VL** | Node-based, strong in interactive / .NET world |
| **Max/MSP + Jitter** | Audio-first node world + video |
| **Houdini** | Procedural node king (offline / FX; not the same real-time media niche) |
| **Nuke** | Compositor node graph (film, not live show primary) |
| **Unreal Blueprints + Niagara** | Game engine real-time; different UX, huge power |
| **Godot** | Open engine; can host custom tools |
| **openFrameworks / Cinder** | Code-first creative coding |
| **Processing / p5** | Education / sketching, not TD-class |

---

## 8. Risk Register

| Risk | Impact | Mitigation |
|------|--------|------------|
| Scope explosion (“all operators”) | Project death | Strict phase gates; operator quotas per phase |
| Weak cook model | Wrong results / bad FPS | Spec + tests before operator flood |
| Junior-only team | Unstable real-time core | Hire/contract senior graphics systems people |
| GPU driver / multi-monitor hell | Field failures | Hardware test matrix early |
| Python GIL / cook blocking | Frame drops | Isolate scripting; budgets; async where safe |
| Codec / SDK license traps | Legal/business issues | Track FFmpeg, NDI, etc. license terms |
| Agent hallucination in multi-agent builds | Broken architecture | Single source-of-truth roadmap + contracts (see Roadmap MD) |
| Trying to match TD UI exactly | Wasted time + IP risk | Original UI design system |

---

## 9. Success Criteria (Company-Level)

A successful internal TD-class platform should eventually allow the company to:

1. Author real-time networks visually  
2. Process live video/textures on GPU  
3. Drive parameters with channels + Python  
4. Output to windows / displays / network video  
5. Package reusable components  
6. Run stable for long sessions (installations / shows)  
7. Extend with custom operators  

**Year-1 success is NOT “full TD.”**  
**Year-1 success IS:** a stable core + enough operators to ship *one* real company workflow end-to-end.

---

## 10. Document Map

| File | Contents |
|------|----------|
| `01-TouchDesigner-Research-And-Feasibility.md` | This file — research, feasibility, concepts |
| `02-BUILD-ROADMAP-A-to-Z.md` | Complete step-by-step build plan: stack, phases, ops, agent rules, checklists |

---

## 11. Sources / Grounding (public)

- Derivative product positioning and public docs (derivative.ca / docs.derivative.ca)
- Public knowledge of operator families (TOP/CHOP/SOP/MAT/DAT/COMP)
- Public C++ custom operator / GLSL TOP documentation patterns
- Industry practice for real-time node tools (Vulkan + tool UI + Python embed)
- Cost/timeline estimates are **order-of-magnitude engineering judgments**, not quotes from Derivative

---

## 12. One-Page Verdict

> **Yes — we can build a TouchDesigner-class private company platform.**  
> **No — we should not try to swallow the entire TD surface area at once.**  
> **Do — implement a modern cook engine, typed operators, GPU pipeline, Python, and original UI in strict phases.**  
> **Do not — reverse engineer TD or require pixel-identical UI.**  
> **Next document — follow `02-BUILD-ROADMAP-A-to-Z.md` as the single execution bible for humans and agents.**
