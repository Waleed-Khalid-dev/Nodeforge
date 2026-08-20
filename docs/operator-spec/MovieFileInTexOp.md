# Operator Specification: MovieFileInTexOp

## Family
TexOp (TOP)

## Description
Reads and decodes video files (MP4, H.264, ProRes, WebM, AVI) asynchronously on a dedicated decoding background worker thread and uploads frames directly to a `gpu::Texture2D`.

## Inputs
- None

## Outputs
- `output` (`PinType::Tex`): Decoded video frame texture.

## Parameters
- `file_path` (`string`): Absolute or relative file path to the video asset.
- `play` (`bool`, default: `true`): Playback state toggle.
- `speed` (`float`, default: `1.0`): Playback speed multiplier (negative values play reverse).
- `loop_mode` (`int32_t`, default: `0`): 0: Loop, 1: Once, 2: Ping-Pong.
- `index_type` (`int32_t`, default: `0`): 0: Sequential Time, 1: Explicit Frame Index, 2: Normalized (0-1).
- `frame_index` (`int32_t`, default: `0`): Target frame when index_type is explicit.
- `reload` (`pulse`): Flushes frame cache and re-opens stream.

## Cook Semantics
1. If stream is not open or file changed, asynchronously open video reader.
2. Calculate target presentation timestamp from timeline or parameter override.
3. Fetch nearest decoded frame from ring buffer; trigger background read for subsequent frames.
4. Upload RGBA pixel buffer to `gpu::Texture2D` via staging buffer and present to output pin.
