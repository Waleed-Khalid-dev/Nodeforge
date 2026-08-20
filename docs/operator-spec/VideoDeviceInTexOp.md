# Operator Specification: VideoDeviceInTexOp

## Family
TexOp (TOP)

## Description
Captures live real-time video streams from USB webcams, capture cards (Elgato, Magewell, Blackmagic, Cam Link), and Virtual Cameras via Windows Media Foundation.

## Inputs
- None

## Outputs
- `output` (`PinType::Tex`): Live camera frame texture.

## Parameters
- `device_index` (`int32_t`, default: `0`): Index of enumerated capture device.
- `resolution` (`vec2`, default: `(1920, 1080)`): Requested capture resolution.
- `framerate` (`float`, default: `60.0`): Target capture frame rate.
- `active` (`bool`, default: `true`): Capture state toggle.

## Cook Semantics
1. Initializes asynchronous Media Foundation capture source reader on selected device index.
2. Background capture callback ingests camera sample into thread-safe texture upload queue.
3. On node cook, binds latest complete video frame to output texture.
