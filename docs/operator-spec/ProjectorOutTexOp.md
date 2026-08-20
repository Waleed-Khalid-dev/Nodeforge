# Operator Specification: ProjectorOutTexOp

## Family
TexOp (TOP)

## Description
Directs a video or generative texture stream directly to a designated physical monitor, LED processor, or projector output window in borderless fullscreen mode.

## Inputs
- `input` (`PinType::Tex`): Texture to display on the physical projector.

## Outputs
- `output` (`PinType::Tex`): Pass-through of input texture.

## Parameters
- `display_index` (`int32_t`, default: `1`): Index of physical monitor/projector output (1 = secondary display).
- `fullscreen` (`bool`, default: `true`): Open borderless fullscreen on target display.
- `vsync` (`bool`, default: `true`): Synchronize presentation with monitor refresh rate.
- `active` (`bool`, default: `true`): Window output active state.
- `title` (`string`, default: `"NodeForge Projector Out"`): OS Window title bar identifier.

## Cook Semantics
1. Validates upstream input texture.
2. Directs `DisplayManager` to ensure output window for `display_index` is created and active.
3. Submits texture frame to the target window's swapchain for hardware presentation.
