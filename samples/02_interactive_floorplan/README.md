# Neo Realms — Interactive Walkable Floor Plan 3D

**Project:** `samples/02_interactive_floorplan/interactive_floorplan.nfp`  
**Target Hardware:** Interactive Touch Display / LED Floor Surface / Overhead LiDAR Sensor  
**Primary Engine Features:** `OSCInChanOp`, `MathChanOp`, `FilterChanOp`, `GridGeomOp`, `SphereGeomOp`, `TransformGeomOp`, `MergeGeomOp`, `PhongMatOp`, `CameraComp`, `LightComp`, `GeometryComp`, `RenderTexOp`.

---

## 1. Overview

This project showcases Neo Realms' walkable interactive floor plan and hologram architecture. Real-time LiDAR or camera sensor data arriving over Open Sound Control (OSC) dynamically updates the position of avatars and interactive ripple hotspots across a procedurally generated 3D architectural floor mesh rendered with Vulkan 1.3 dynamic rasterization.

```
      [ LiDAR / Tracking Camera ]
                   │
                   │ OSC UDP (:8000)
                   ▼
          ┌─────────────────┐
          │  OSCInChanOp    │ (/tracking/*)
          └────────┬────────┘
                   │
          ┌────────▼────────┐
          │   MathChanOp    │ (Remap bounds)
          └────────┬────────┘
                   │
          ┌────────▼────────┐
          │  FilterChanOp   │ (Smoothing lag 0.15s)
          └────────┬────────┘
                   │ (Expression translate)
  ┌────────────┐   │   ┌───────────────┐
  │GridGeomOp  │   └──►│TransformGeomOp│
  │(Floor Mesh)│       └───┬───────────┘
  └─────┬──────┘           │ (Avatar Sphere)
        │     ┌────────────┘
        ▼     ▼
     ┌──────────────┐
     │ MergeGeomOp  │◄─── [PhongMatOp]
     └──────┬───────┘
            ▼
     ┌──────────────┐
     │ GeometryComp │
     └──────┬───────┘
            │
            ├─────────────── [CameraComp]
            ├─────────────── [LightComp]
            ▼
     ┌──────────────┐
     │ RenderTexOp  │ (1920x1080 GPU Dynamic Raster)
     └──────┬───────┘
            ▼
     ┌──────────────┐
     │ToWindowTexOp │ (Interactive Presentation)
     └──────────────┘
```

---

## 2. Sensor Tracking Protocol (OSC Specification)

The network listens on UDP Port `8000` for the following address patterns:

| OSC Address | Data Type | Range | Description |
|-------------|-----------|-------|-------------|
| `/tracking/pos_x` | Float | `[-1.0, 1.0]` | Normalized X coordinate of the visitor centroid |
| `/tracking/pos_y` | Float | `[-1.0, 1.0]` | Normalized Y coordinate of the visitor centroid |
| `/tracking/active` | Int / Bool | `0 or 1` | Presence detection trigger |

---

## 3. Testing with OSC Emulators

You can test the tracking live using any standard OSC generator or Python script:
```python
from pythonosc import udp_client
client = udp_client.SimpleUDPClient("127.0.0.1", 8000)
client.send_message("/tracking/pos_x", 0.45)
client.send_message("/tracking/pos_y", -0.2)
```
