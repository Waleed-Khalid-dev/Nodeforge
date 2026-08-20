# Operator Specification: SpoutInTexOp

## Family
TexOp (TOP)

## Description
Receives shared real-time GPU textures from other Windows visual applications (Resolume, Notch, Unreal Engine, TouchDesigner, OBS) with zero memory copy.

## Inputs
- None

## Outputs
- `output` (`PinType::Tex`): Received shared texture.

## Parameters
- `sender_name` (`string`, default: `""`): Name of the Spout sender to connect to.
- `auto_connect` (`bool`, default: `true`): Automatically connect to the active sender if name is empty.

## Cook Semantics
1. Resolves DXGI shared handle from Spout sender registry.
2. Imports external memory into Vulkan using `VK_KHR_external_memory_win32`.
3. Binds texture directly to node output pin.
