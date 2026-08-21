# Physics & Rigid Body Dynamics Subsystem Cheat Sheet

**Subsystem:** Real-Time Physics & Rigid Body Dynamics | **Version:** 1.0 (Phase 15 Epic 15.5)

---

## 1. Physics Operator Reference

| Operator | Family | In Pins | Out Pins | Key Roles |
|---|---|---|---|---|
| **`PhysicsSolverComp`** | `Comp` | `in_bodies` (Comp), `in_forces` (Chan) | `output` (Comp), `out_events` (Chan) | Master physics simulation controller & collision telemetry |
| **`RigidBodyGeomOp`** | `GeomOp` | `template_geom` (Geom), `in_force` (Chan) | `output` (Geom) | 6-DOF dynamic/static rigid body mesh transformer |
| **`ColliderGeomOp`** | `GeomOp` | `in_geom` (Geom) | `output` (Geom) | Static ground plane and boundary collision shapes |
| **`PhysicsForceChanOp`** | `ChanOp` | `in_trigger` (Chan) | `out_force` (Chan) | Dynamic spatial forces (Explosion, Wind, Vortex, Attractor) |

---

## 2. Standard Network Wiring Idiom

```
# Interactive Rigid Body Destruction Arena & Collision Audio
BoxGeomOp (Stone Monolith) ──────► RigidBodyGeomOp (Mass = 5.0, Dynamic)
                                            │
                                            ▼ (Simulated 3D Mesh)
GridGeomOp (Ground) ──────────────► ColliderGeomOp (Plane)
                                            │
                                            ▼
MouseInChanOp (Click) ────────────► PhysicsForceChanOp (Radial Blast)
                                            │
                                            ▼
                                  PhysicsSolverComp (Gravity = -9.81)
                                            │
                                            ├─► RenderTexOp (3D Vulkan Render)
                                            │
                                            └─► out_events (Collision Pulses)
                                                    │
                                                    ▼
                                            AudioSpatializerChanOp (3D Impact Thuds)
```
