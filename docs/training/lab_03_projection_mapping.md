# Workshop Lab 3: Multi-Projector Alignment & Softedge Warping

**Module:** 4 | **Objective:** Calibrate a dual-projector edge-blended setup on an uneven physical surface.

---

## 1. Step-by-Step Build Instructions

1. **Create Source Video Stream:**
   - Create a `MovieFileInTexOp` or `NoiseTexOp` as the primary video source.
2. **Split Video into Dual Warp Channels:**
   - Create two `WarpBlendTexOp` nodes: name them `warp_left` and `warp_right`.
   - Wire the source texture output into the `input` pin of both warp nodes.
3. **Configure Overlap & Blend Margins:**
   - For `warp_left`: set `screen_index` = `0`, `blend_edge` = `Right` (1), `blend_width` = `0.15`, `blend_gamma` = `2.2`.
   - For `warp_right`: set `screen_index` = `1`, `blend_edge` = `Left` (0), `blend_width` = `0.15`, `blend_gamma` = `2.2`.
4. **Attach Physical Output Displays:**
   - Create two `ProjectorOutTexOp` nodes.
   - Wire `warp_left.output` -> `projector1.input` (Display Index 1).
   - Wire `warp_right.output` -> `projector2.input` (Display Index 2).
5. **On-Site Calibration Mode:**
   - Press `F11` to trigger fullscreen presentation mode.
   - Turn on `show_grid = true` on both warp nodes.
   - Using your mouse in the live Viewer, drag the corner and interior Bezier control vertices until physical architectural vertical lines align straight on both projectors.
   - Turn off `show_grid` and observe the seamless single-canvas image across both projectors.
