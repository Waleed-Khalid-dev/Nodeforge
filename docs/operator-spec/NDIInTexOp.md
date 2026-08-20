# Operator Specification: NDIInTexOp

## Family
TexOp (TOP)

## Description
Receives broadcast video streams over local area network (LAN) using NewTek Network Device Interface (NDI).

## Inputs
- None

## Outputs
- `output` (`PinType::Tex`): Received network video frame texture.

## Parameters
- `source_name` (`string`, default: `""`): Target NDI source name.
- `bandwidth` (`int32_t`, default: `0`): 0: Highest quality, 1: Lowest / Preview.
- `active` (`bool`, default: `true`): Reception active toggle.

## Cook Semantics
1. Discovers and binds to the specified NDI network stream.
2. Ingests network packet frames into thread-safe frame receiver.
3. Uploads latest complete frame to `gpu::Texture2D`.
