# NodeForge Master Quick-Reference & Operator Cheat Sheet

**NodeForge Core Reference** | **Version 1.0 (2026)** | **Neo Realms Interactive**

---

## 1. Keyboard Shortcuts & Canvas Navigation

| Action | Shortcut | Description |
|--------|----------|-------------|
| **Pan Canvas** | `Middle Mouse Drag` or `Alt + Left Drag` | Infinite canvas navigation |
| **Zoom Canvas** | `Mouse Wheel` or `Alt + Right Drag` | 0.2x to 2.5x continuous zoom |
| **OP Create Menu** | `Tab` or `Double Left Click` | Fuzzy search operator palette |
| **Multi-Select** | `Shift + Left Drag` / `Ctrl + Click` | Box marquee selection or additive toggle |
| **Delete Nodes** | `Delete` or `Backspace` | Removes selected nodes and attached wires |
| **Undo / Redo** | `Ctrl + Z` / `Ctrl + Y` (or `Ctrl + Shift + Z`) | Infinite command pattern undo/redo stack |
| **Duplicate Node**| `Ctrl + D` | Clones selected node with parameter states |
| **Save Project** | `Ctrl + S` | Serializes `.nfp` JSON v1 project file |
| **Fullscreen Out**| `F11` | Toggles on-site warp & projector fullscreen mode |
| **Performance HUD**| `F3` | Toggles floating engine stats HUD overlay |
| **Cook Profiler** | `Ctrl + P` | Opens dockable per-node cook profiler panel |

---

## 2. Operator Families & Pin Color Standard

```
┌─────────┬──────────────┬───────────────┬──────────────────────────────────────────┐
│ Family  │ Color Badge  │ Pin Type Color│ Primary Role                             │
├─────────┼──────────────┼───────────────┼──────────────────────────────────────────┤
│ TexOp   │ Purple/Cyan  │ Cyan (#00E5FF)│ 2D GPU Texture processing, shaders & I/O │
│ ChanOp  │ Green        │ Green(#00E676)│ 1D/SIMD Channels, audio & show control   │
│ DataOp  │ Magenta      │ Pink (#F50057)│ 2D Data tables, CSV/JSON, Python scripts │
│ GeomOp  │ Blue         │ Blue (#2979FF)│ 3D Meshes, procedural primitives & SOPs  │
│ MatOp   │ Amber        │ Gold (#FFD600)│ Materials, GLSL shaders, camera & lights │
│ Comp    │ Orange       │ Gray (#B0BEC5)│ Container subnetworks, I/O boundary pins │
└─────────┴──────────────┴───────────────┴──────────────────────────────────────────┘
```

---

## 3. Python Expressions & Parameter Binding

NodeForge features an embedded CPython 3 runtime. Any parameter can toggle between `Constant` and `Expression` mode by clicking the `C / E` badge in the Parameter Inspector.

### Standard Python API Patterns

```python
# Access another node's output value
op('lfo1')['chan1']              # Read channel 'chan1' from ChanOp 'lfo1'
op('table1')[0, 1]               # Read cell (row 0, col 1) from DataTable
op('noise1').par.frequency       # Read parameter object

# Built-in Global Timeline Context Variables
me.time                          # Current playback time in seconds (float)
me.frame                         # Current integer timeline frame index
me.fps                           # Target engine framerate (e.g. 60.0)

# Common Dynamic Expressions
op('lfo1')['chan1'] * 5.0        # Modulate amplitude
math.sin(me.time * 2.0) * 10.0   # Trigonometric harmonic animation
[op('pos')['x'], 0.0, op('pos')['y']]  # Vector3 translation array
```

---

## 4. Performance Golden Rules

1. **Keep Compute on the GPU:** Never download textures from the GPU to CPU RAM (`glReadPixels` / `vkCmdCopyImageToBuffer`) inside the per-frame cook path.
2. **Leverage SIMD Channels:** ChanOps process batches of continuous floating-point channels in single CPU cache lines with AVX2 acceleration.
3. **Use TexturePool Leases:** TexOps lease render targets from the zero-allocation `TexturePool`. Re-use standard resolutions (1080p, 4K) to prevent memory allocation spikes.
4. **Monitor Dirty Flags:** NodeForge uses push-dirty propagation with pull-on-demand evaluation. If a node's inputs have not changed, its cook is skipped entirely (0.00 ms CPU/GPU cost).
