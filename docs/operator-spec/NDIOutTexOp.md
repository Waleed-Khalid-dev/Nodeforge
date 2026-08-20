# Operator Specification: NDIOutTexOp

## Family
TexOp (TOP)

## Description
Broadcasts texture frames over LAN as an NDI video source for remote media servers, switchers, or secondary workstations.

## Inputs
- `input` (`PinType::Tex`): Texture to broadcast.

## Outputs
- `output` (`PinType::Tex`): Pass-through of input texture.

## Parameters
- `stream_name` (`string`, default: `"NodeForge_NDI"`): Name advertised on the LAN.
- `active` (`bool`, default: `true`): Broadcasting active toggle.

## Cook Semantics
1. Reads input texture.
2. Stages frame data into NDI transmission buffer.
3. Broadcasts frame packet across local network.
