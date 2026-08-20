# Operator Specification: SpoutOutTexOp

## Family
TexOp (TOP)

## Description
Publishes an input texture to the Windows Spout2 inter-process texture sharing registry for zero-copy consumption by external applications.

## Inputs
- `input` (`PinType::Tex`): Texture to broadcast.

## Outputs
- `output` (`PinType::Tex`): Pass-through of input texture.

## Parameters
- `sender_name` (`string`, default: `"NodeForge_Output"`): Unique Spout sender identification string.
- `active` (`bool`, default: `true`): Transmission active toggle.

## Cook Semantics
1. Retrieves input texture from upstream node.
2. Exports or registers shared DXGI handle into Spout shared memory registry.
3. Sets output pin as pass-through.
