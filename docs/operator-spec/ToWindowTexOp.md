# Operator Specification: ToWindowTexOp

**Family:** `TexOp` (Texture Operator)  
**Name:** `ToWindow`  
**Description:** Output display sink presenting the input texture directly to the GLFW Vulkan swapchain window.

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Tex`): Texture stream to present on screen.

### Output Pins
- `output` (`PinType::Tex`): Pass-through output.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `fit` | `bool` | `true` | Scale image to fit the window dimensions preserving aspect ratio |
| `enable` | `bool` | `true` | Enable/disable window presentation |

---

## Cook Behavior
1. Checks for active GLFW window surface in `CookContext`.
2. Transitions swapchain image to `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL`.
3. Blits/renders the input texture into the current swapchain frame.
4. Passes the texture along to `output`.
