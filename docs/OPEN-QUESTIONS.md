# Open Questions

Resolve these early. Answers guide Phase 0–1 decisions.

| # | Question | Answer | Date |
|---|----------|--------|------|
| 1 | Final product name? | NodeForge (working) | 2026-08-04 |
| 2 | Primary company flagship workflow (exact)? | **Projection mapping on building façades + interactive gesture layer + walkable 1:1 floor-plan projections for brands & real estate** | 2026-08-09 |
| 3 | NDI SDK available? | **YES — download free at ndi.video/developers/sdk/ (current v6.3.2, registration required). SDK only needed during dev — no cost.** | 2026-08-09 |
| 4 | Capture cards needed in year 1? | **YES (planned) — no harm adding support. Blackmagic Decklink recommended. Does not break anything if unused.** | 2026-08-09 |
| 5 | Max output resolution year 1? | **5760×1080 (3× 1080p blended) or 3840×2160 (4K floor plan). RTX 3060 12 GB minimum.** | 2026-08-09 |
| 6 | Python version pin? | **Embed Python 3.11 inside the engine (via CPython embed package). System 3.13.x install is fine alongside it — they do not conflict.** | 2026-08-09 |
| 7 | Pure ImGui forever or Qt shell later? | ImGui first (roadmap default) | 2026-08-04 |
| 8 | Team size / who owns architecture? | **Solo — Waleed Khalid. Part-time. Architecture owner: Waleed Khalid.** | 2026-08-09 |
| 9 | Git remote / monorepo location? | **https://github.com/Waleed-Khalid-dev/Nodeforge (private repo recommended — see note below)** | 2026-08-09 |

---

## Guidance: Where to Find Each Answer

> Answer these before starting Phase 1. They directly affect what we build first.

### Q2 — Primary company flagship workflow (exact)

**What this means:** The exact technical pipeline for a real Neo Realms show, end-to-end.

**Where to find the answer:** Think about your last completed job or your next confirmed booking.

Write it out this specifically:
```
Source: [video file / camera feed / generative graphic]
Canvas size: [e.g. 3840×1080 across 3 outputs]
Number of projectors: [e.g. 3 Christie 20K on a building]
Interactivity: [gesture layer / OSC trigger / none]
Show duration: [e.g. 4 hours continuous]
On-site adjustments: [warp point tweaking / none]
```

This description becomes the Year-1 benchmark graph that Phase 14 must ship.

---

### Q3 — NDI SDK available?

**What this means:** NDI lets you send live video between machines over a LAN network.
Used for: NodeForge on machine A → media server or backup on machine B.

**Where to find the answer:** Visit https://ndi.video/tools/ndi-sdk/ — it is **free to download**.

**Likely answer for Neo Realms:** **YES** — multi-projector shows often use a second machine as a hot backup or feed video to a different zone. Confirm yes/no and download the SDK.

---

### Q4 — Capture cards needed in year 1?

**What this means:** A capture card (Blackmagic Decklink, Magewell) lets you input a live HDMI/SDI camera signal into NodeForge in real time.

**Where to find the answer:** Ask: do your shows use **live cameras**?
- Gesture-interactive walls: possibly (depth camera, not HDMI)
- Pre-rendered file playback only: **No capture card needed Year 1**
- Live audience camera overlay: **Yes, Decklink needed**

**Likely answer for Neo Realms:** Probably **No for Year 1** (you play video files + generative content). Depth sensors for gesture (Azure Kinect / RealSense) are USB and use their own SDK, not capture cards.

---

### Q5 — Max output resolution year 1?

**What this means:** The total pixel canvas across all outputs combined. Determines GPU VRAM requirements.

**Common Neo Realms scenarios:**

| Scenario | Total canvas |
|----------|-------------|
| 2 blended 1080p projectors (building wall) | 3840×1080 |
| 3 blended 1080p projectors | 5760×1080 |
| Single 4K floor-plan projector | 3840×2160 |
| Hologram (4-sided pepper's ghost) | ~4×1080p faces |

**Recommendation:** Target **5760×1080 max** for Year 1. Requires RTX 3060 12GB minimum.

---

### Q6 — Python version pin

**Answer (pre-filled):** **Python 3.11** — most stable CPython embed as of 2026. Avoid 3.12 until pybind11 confirms stable embed support for your build toolchain.

---

### Q8 — Team size / who owns architecture?

**Answer template (fill in):**
```
Total developers: [e.g. 1 / 2 / solo founder]
Architecture owner: [your name or engineer name]
Full-time or part-time?
```

**Why it matters:** Solo = Phase 0 takes 2–3 months. 2 full-time = Phase 0 done in 2 weeks. Affects the milestone timeline in Section 11 of the roadmap.

---

### Q9 — Git remote / monorepo location?

**Options:**
| Option | Cost | Recommended for Neo Realms |
|--------|------|---------------------------|
| GitHub Private Repo | Free | ✅ Yes — works with GitHub Actions CI |
| Azure DevOps | Free tier | OK alternative |
| GitLab | Free | OK alternative |
| Local only | Free | ❌ No — risky, no CI |

**Recommendation:** Create a **private GitHub repo** at `github.com/neo-realms/nodeforge` (or similar). This plugs directly into the CI pipeline in the roadmap (Section 2.1, GitHub Actions).
