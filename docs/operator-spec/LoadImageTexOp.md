# Operator Specification: LoadImageTexOp

**Family:** `TexOp` (Texture Operator)  
**Name:** `LoadImage`  
**Description:** Loads still image files (PNG, JPG, BMP, TGA) from disk via `stb_image` and uploads them to a GPU texture.

---

## Inputs & Outputs

### Input Pins
- None (Generator / File I/O Node)

### Output Pins
- `output` (`PinType::Tex`): GPU texture holding the decoded image.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `file_path` | `std::string` | `""` | Absolute or relative path to the image file |
| `reload` | `bool` | `false` | Pulse trigger to force reload from disk |

---

## Cook Behavior
1. Checks if the file path has changed or reload pulse is active.
2. Decodes image pixels to 32-bit RGBA using `stbi_load()`.
3. Allocates or acquires a GPU texture matching the image's native resolution.
4. Uploads pixel data using a host-visible VMA staging buffer and copies to GPU optimal memory.
5. Sets the output texture on the output pin.
