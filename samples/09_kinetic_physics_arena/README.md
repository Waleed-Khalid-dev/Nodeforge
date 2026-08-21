# Neo Realms — Kinetic Physics Arena & Domino Cascade

**Project:** `samples/09_kinetic_physics_arena/kinetic_physics_arena.nfp`  
**Target Hardware:** Interactive Touchscreen / 3D Projection Mapping / Physical Destruction Show  
**Primary Engine Features:** `PhysicsSolverComp`, `RigidBodyGeomOp`, `ColliderGeomOp`, `PhysicsForceChanOp`, `AudioSpatializerChanOp`, `BoxGeomOp`, `RenderTexOp`, `ToWindowTexOp`.

---

## 1. Overview

This project demonstrates **Real-Time 3D Physics & Rigid Body Dynamics** inside the NodeForge DAG runtime. 

Dynamic rigid blocks drop, bounce, and collide against static ground planes. Dynamic mouse/gesture inputs blast radial impulse shockwaves into the physical structures, while real-time collision impact telemetry automatically triggers 3D spatial audio thuds and visual feedback.

```
┌─────────────────────┐
│      BoxGeomOp      │ (Mesh Shape)
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│   RigidBodyGeomOp   │ (Mass = 5.0, Restitution = 0.6)
└──────────┬──────────┘
           │
           ▼
┌─────────────────────────────┐
│      PhysicsSolverComp      │ (Gravity = -9.81, SubSteps = 4)
└──────────┬──────────────────┘
           │
     ┌─────┴─────────────────────┐
     │                           │
     ▼ (Simulated 3D Mesh)       ▼ (Collision Telemetry)
┌─────────────────────┐   ┌─────────────────────────────┐
│     RenderTexOp     │   │   AudioSpatializerChanOp    │ (3D Impact Thuds)
└─────────────────────┘   └─────────────────────────────┘
```

---

## 2. Interactive Features

- **Dynamic Impulse Blasts:** Trigger radial explosions via `PhysicsForceChanOp`.
- **Collision Feedback:** Collision contacts emit 3D spatial audio impacts mapped across surround sound speakers.
