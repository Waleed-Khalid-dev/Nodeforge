# Operator Specification: WarpBlendTexOp

## Family
TexOp (TOP)

## Description
Applies 2D Bezier grid warping, perspective homography corner-pinning, S-curve gamma-corrected softedge blending, black-level pedestal compensation, and alignment test patterns for multi-projector mapping installations.

## Inputs
- `input` (`PinType::Tex`): Source content texture to warp and blend.
- `mask` (`PinType::Tex`, optional): Optional custom blend alpha mask.

## Outputs
- `output` (`PinType::Tex`): Warped and edge-blended output texture.

## Parameters
- `grid_rows` (`int32_t`, default: `4`): Number of warp control grid rows (2 to 16).
- `grid_cols` (`int32_t`, default: `4`): Number of warp control grid columns (2 to 16).
- `blend_left` (`float`, default: `0.0`): Left softedge overlap width (0.0 to 0.5).
- `blend_right` (`float`, default: `0.0`): Right softedge overlap width (0.0 to 0.5).
- `blend_top` (`float`, default: `0.0`): Top softedge overlap width (0.0 to 0.5).
- `blend_bottom` (`float`, default: `0.0`): Bottom softedge overlap width (0.0 to 0.5).
- `blend_gamma` (`float`, default: `2.2`): Gamma curve exponent for luminance-correct edge blending.
- `black_level` (`float`, default: `0.0`): Black pedestal lift offset.
- `test_pattern` (`int32_t`, default: `0`): 0: Disabled (Content), 1: Alignment Grid, 2: Crosshair & Circles, 3: SMPTE Color Bars, 4: Checkerboard.
- `calibration_mode` (`bool`, default: `false`): Render interactive control point overlay handles.

## Cook Semantics
1. Evaluates input texture dimensions and warp grid mesh control points.
2. Calculates bicubic Bezier interpolation across grid subdivisions.
3. Renders warped quad mesh with softedge falloff and black-level compensation fragment shader into output texture.
