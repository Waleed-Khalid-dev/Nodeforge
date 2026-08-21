#include "CoreNodes.h"
#include "../operators/chan/ConstantChanOp.h"
#include "../operators/chan/TimeChanOp.h"
#include "../operators/chan/LFOChanOp.h"
#include "../operators/chan/NoiseChanOp.h"
#include "../operators/chan/MathChanOp.h"
#include "../operators/chan/FilterChanOp.h"
#include "../operators/chan/MergeChanOp.h"
#include "../operators/chan/SelectChanOp.h"
#include "../operators/chan/TrailChanOp.h"
#include "../operators/chan/AudioFileInChanOp.h"
#include "../operators/chan/ChanToTexOp.h"
#include "../operators/chan/TexToChanOp.h"
#include "../operators/tex/NullTexOp.h"
#include "../operators/tex/ConstantTexOp.h"
#include "../operators/tex/NoiseTexOp.h"
#include "../operators/tex/LoadImageTexOp.h"
#include "../operators/tex/TransformTexOp.h"
#include "../operators/tex/CompositeTexOp.h"
#include "../operators/tex/BlurTexOp.h"
#include "../operators/tex/LevelTexOp.h"
#include "../operators/tex/ResolutionTexOp.h"
#include "../operators/tex/ToWindowTexOp.h"
#include "../operators/comp/ContainerComp.h"
#include "../operators/comp/InOp.h"
#include "../operators/comp/OutOp.h"
#include "../operators/data/TextDataOp.h"
#include "../operators/data/TableDataOp.h"
#include "../operators/data/ScriptDataOp.h"
#include "../operators/data/JSONDataOp.h"
#include "../operators/data/WebDataOp.h"
#include "../operators/data/OSCInOp.h"
#include "../operators/data/OSCOutOp.h"
#include "../operators/data/SelectDataOp.h"
#include "../operators/data/MergeDataOp.h"
#include "../operators/data/ChanToDataOp.h"
#include "../operators/data/DataToChanOp.h"
#include "../operators/geom/GridGeomOp.h"
#include "../operators/geom/SphereGeomOp.h"
#include "../operators/geom/BoxGeomOp.h"
#include "../operators/geom/TorusGeomOp.h"
#include "../operators/geom/CylinderGeomOp.h"
#include "../operators/geom/TransformGeomOp.h"
#include "../operators/geom/MergeGeomOp.h"
#include "../operators/geom/NoiseDeformGeomOp.h"
#include "../operators/geom/NormalsGeomOp.h"
#include "../operators/geom/ChanToGeomOp.h"
#include "../operators/mat/ConstantMatOp.h"
#include "../operators/mat/PhongMatOp.h"
#include "../operators/mat/GLSLMatOp.h"
#include "../operators/comp/CameraComp.h"
#include "../operators/comp/LightComp.h"
#include "../operators/comp/GeometryComp.h"
#include "../operators/tex/RenderTexOp.h"
#include "../operators/tex/MovieFileInTexOp.h"
#include "../operators/tex/VideoDeviceInTexOp.h"
#include "../operators/tex/SpoutInTexOp.h"
#include "../operators/tex/SpoutOutTexOp.h"
#include "../operators/tex/NDIInTexOp.h"
#include "../operators/tex/NDIOutTexOp.h"
#include "../operators/tex/ProjectorOutTexOp.h"
#include "../operators/tex/WarpBlendTexOp.h"
#include "../operators/chan/MIDIInChanOp.h"
#include "../operators/chan/MIDIOutChanOp.h"
#include "../operators/chan/OSCInChanOp.h"
#include "../operators/chan/OSCOutChanOp.h"
#include "../operators/data/SerialDataOp.h"
#include "../operators/chan/DMXInChanOp.h"
#include "../operators/chan/DMXOutChanOp.h"
#include "../operators/chan/MouseInChanOp.h"
#include "../operators/chan/KeyboardInChanOp.h"
#include "../operators/geom/ParticleEmitterGeomOp.h"
#include "../operators/geom/ParticleForceGeomOp.h"
#include "../operators/geom/ParticleAttractorGeomOp.h"
#include "../operators/mat/ParticleMatOp.h"

namespace nf {

void RegisterCoreNodes(NodeRegistry& registry) {
    // ChanOps
    registry.RegisterNodeType<ConstantChanOp>(
        "ConstantChanOp",
        NodeFamily::ChanOp,
        "Generators",
        "Generates constant multi-channel numeric streams."
    );

    registry.RegisterNodeType<TimeChanOp>(
        "TimeChanOp",
        NodeFamily::ChanOp,
        "Generators",
        "Generates continuous timeline, clock, frame index, and playback progress channels."
    );

    registry.RegisterNodeType<LFOChanOp>(
        "LFOChanOp",
        NodeFamily::ChanOp,
        "Generators",
        "Low-frequency oscillator generating periodic waveforms (Sine, Tri, Saw, Square, Pulse)."
    );

    registry.RegisterNodeType<NoiseChanOp>(
        "NoiseChanOp",
        NodeFamily::ChanOp,
        "Generators",
        "Procedural 1D Perlin / Simplex smooth noise channel generator."
    );

    registry.RegisterNodeType<MathChanOp>(
        "MathChanOp",
        NodeFamily::ChanOp,
        "Math",
        "Performs arithmetic, trigonometric, combine, and range remapping operations on channels."
    );

    registry.RegisterNodeType<FilterChanOp>(
        "FilterChanOp",
        NodeFamily::ChanOp,
        "Filter",
        "Applies lag, spring dynamics, damping, and moving average smoothing to channels."
    );

    registry.RegisterNodeType<MergeChanOp>(
        "MergeChanOp",
        NodeFamily::ChanOp,
        "Combine",
        "Merges multiple channel sets into a single unified multi-channel buffer."
    );

    registry.RegisterNodeType<SelectChanOp>(
        "SelectChanOp",
        NodeFamily::ChanOp,
        "Utility",
        "Filters, selects, reorders, and renames channels using pattern matching."
    );

    registry.RegisterNodeType<TrailChanOp>(
        "TrailChanOp",
        NodeFamily::ChanOp,
        "Utility",
        "Records a continuous sliding history ring buffer of time-sliced samples."
    );

    registry.RegisterNodeType<AudioFileInChanOp>(
        "AudioFileInChanOp",
        NodeFamily::ChanOp,
        "File I/O",
        "Decodes audio files (WAV, PCM) into streaming multi-channel audio buffers."
    );

    registry.RegisterNodeType<TexToChanOp>(
        "TexToChanOp",
        NodeFamily::ChanOp,
        "Interop",
        "Extracts pixel data from a GPU Texture2D into CPU RGBA channel buffers."
    );

    // TexOps
    registry.RegisterNodeType<ChanToTexOp>(
        "ChanToTexOp",
        NodeFamily::TexOp,
        "Interop",
        "Converts numeric multi-channel buffer data into a 1D/2D GPU Texture2D."
    );

    registry.RegisterNodeType<NullTexOp>(
        "NullTexOp",
        NodeFamily::TexOp,
        "Utility",
        "Pass-through alias texture operator."
    );

    registry.RegisterNodeType<ConstantTexOp>(
        "ConstantTexOp",
        NodeFamily::TexOp,
        "Generators",
        "Generates a 2D solid color GPU texture."
    );

    registry.RegisterNodeType<NoiseTexOp>(
        "NoiseTexOp",
        NodeFamily::TexOp,
        "Generators",
        "Procedural 2D Perlin / Simplex / FBM noise compute shader."
    );

    registry.RegisterNodeType<LoadImageTexOp>(
        "LoadImageTexOp",
        NodeFamily::TexOp,
        "File I/O",
        "Loads still image files (PNG, JPG, BMP) to GPU texture."
    );

    registry.RegisterNodeType<TransformTexOp>(
        "TransformTexOp",
        NodeFamily::TexOp,
        "Transform",
        "Applies 2D geometric affine transformation to an input texture."
    );

    registry.RegisterNodeType<CompositeTexOp>(
        "CompositeTexOp",
        NodeFamily::TexOp,
        "Composite",
        "Blends two input textures with configurable layer modes."
    );

    registry.RegisterNodeType<BlurTexOp>(
        "BlurTexOp",
        NodeFamily::TexOp,
        "Filter",
        "Two-pass separable Gaussian blur filter."
    );

    registry.RegisterNodeType<LevelTexOp>(
        "LevelTexOp",
        NodeFamily::TexOp,
        "Color",
        "Photometric adjustments (black/white point, brightness, contrast, gamma)."
    );

    registry.RegisterNodeType<ResolutionTexOp>(
        "ResolutionTexOp",
        NodeFamily::TexOp,
        "Transform",
        "Resizes and resamples textures with aspect-ratio fitting."
    );

    registry.RegisterNodeType<ToWindowTexOp>(
        "ToWindowTexOp",
        NodeFamily::TexOp,
        "Presentation",
        "Presents the texture directly to the GLFW Vulkan swapchain."
    );

    // Comp
    registry.RegisterNodeType<ContainerComp>(
        "ContainerComp",
        NodeFamily::Comp,
        "Container",
        "Subnetwork container operator encapsulating an inner node graph."
    );

    registry.RegisterNodeType<InOp>(
        "InOp",
        NodeFamily::Comp,
        "Boundary",
        "Boundary input operator exposing a parent container input pin."
    );

    registry.RegisterNodeType<OutOp>(
        "OutOp",
        NodeFamily::Comp,
        "Boundary",
        "Boundary output operator exposing a parent container output pin."
    );

    // DataOps (DAT)
    registry.RegisterNodeType<TextDataOp>(
        "TextDataOp",
        NodeFamily::DataOp,
        "Text",
        "Stores, loads, and edits raw multiline UTF-8 text strings."
    );

    registry.RegisterNodeType<TableDataOp>(
        "TableDataOp",
        NodeFamily::DataOp,
        "Table",
        "Generates, imports, and manages 2D tabular CSV/TSV data."
    );

    registry.RegisterNodeType<ScriptDataOp>(
        "ScriptDataOp",
        NodeFamily::DataOp,
        "Scripting",
        "Executes embedded Python code on cook and pulse triggers."
    );

    registry.RegisterNodeType<JSONDataOp>(
        "JSONDataOp",
        NodeFamily::DataOp,
        "Parsing",
        "Parses JSON documents and performs JSON Pointer queries."
    );

    registry.RegisterNodeType<WebDataOp>(
        "WebDataOp",
        NodeFamily::DataOp,
        "Network",
        "Performs non-blocking asynchronous HTTP REST requests."
    );

    registry.RegisterNodeType<OSCInOp>(
        "OSCInOp",
        NodeFamily::DataOp,
        "Network",
        "Receives UDP Open Sound Control packets over local network."
    );

    registry.RegisterNodeType<OSCOutOp>(
        "OSCOutOp",
        NodeFamily::DataOp,
        "Network",
        "Transmits UDP Open Sound Control packets to remote endpoints."
    );

    registry.RegisterNodeType<SelectDataOp>(
        "SelectDataOp",
        NodeFamily::DataOp,
        "Utility",
        "Filters and slices specific rows and columns from tables."
    );

    registry.RegisterNodeType<MergeDataOp>(
        "MergeDataOp",
        NodeFamily::DataOp,
        "Combine",
        "Concatenates tables horizontally or vertically with header alignment."
    );

    registry.RegisterNodeType<ChanToDataOp>(
        "ChanToDataOp",
        NodeFamily::DataOp,
        "Interop",
        "Converts numeric channel buffers into tabular sample tracks."
    );

    registry.RegisterNodeType<DataToChanOp>(
        "DataToChanOp",
        NodeFamily::ChanOp,
        "Interop",
        "Extracts numeric table columns/rows into channel buffers."
    );

    // GeomOps (SOP)
    registry.RegisterNodeType<GridGeomOp>(
        "GridGeomOp",
        NodeFamily::GeomOp,
        "Primitives",
        "Generates a planar 2D grid/plane mesh in 3D space."
    );

    registry.RegisterNodeType<SphereGeomOp>(
        "SphereGeomOp",
        NodeFamily::GeomOp,
        "Primitives",
        "Generates a 3D UV sphere mesh with configurable rings and segments."
    );

    registry.RegisterNodeType<BoxGeomOp>(
        "BoxGeomOp",
        NodeFamily::GeomOp,
        "Primitives",
        "Generates a 3D cuboid/cube mesh with subdivisions and per-face UVs."
    );

    registry.RegisterNodeType<TorusGeomOp>(
        "TorusGeomOp",
        NodeFamily::GeomOp,
        "Primitives",
        "Generates a 3D donut/toroid mesh."
    );

    registry.RegisterNodeType<CylinderGeomOp>(
        "CylinderGeomOp",
        NodeFamily::GeomOp,
        "Primitives",
        "Generates a 3D cylinder, cone, or tube mesh."
    );

    registry.RegisterNodeType<TransformGeomOp>(
        "TransformGeomOp",
        NodeFamily::GeomOp,
        "Modify",
        "Applies 3D translation, rotation, scaling, and pivot offsets to meshes."
    );

    registry.RegisterNodeType<MergeGeomOp>(
        "MergeGeomOp",
        NodeFamily::GeomOp,
        "Combine",
        "Combines multiple geometry streams into a single consolidated mesh."
    );

    registry.RegisterNodeType<NoiseDeformGeomOp>(
        "NoiseDeformGeomOp",
        NodeFamily::GeomOp,
        "Modify",
        "Displaces 3D mesh vertices using Simplex/Perlin noise."
    );

    registry.RegisterNodeType<NormalsGeomOp>(
        "NormalsGeomOp",
        NodeFamily::GeomOp,
        "Modify",
        "Computes smooth, faceted, or inverted normals and tangent vectors."
    );

    registry.RegisterNodeType<ChanToGeomOp>(
        "ChanToGeomOp",
        NodeFamily::GeomOp,
        "Interop",
        "Converts channel buffers into 3D point clouds, line strips, or meshes."
    );

    // MatOps (MAT)
    registry.RegisterNodeType<ConstantMatOp>(
        "ConstantMatOp",
        NodeFamily::MatOp,
        "Materials",
        "Unlit flat-color or wireframe material."
    );

    registry.RegisterNodeType<PhongMatOp>(
        "PhongMatOp",
        NodeFamily::MatOp,
        "Materials",
        "Blinn-Phong lit material with specular highlights, ambient, and normal maps."
    );

    registry.RegisterNodeType<GLSLMatOp>(
        "GLSLMatOp",
        NodeFamily::MatOp,
        "Custom",
        "Custom GLSL vertex and fragment shader material with live hot-reloading."
    );

    // Scene Components (COMP)
    registry.RegisterNodeType<CameraComp>(
        "CameraComp",
        NodeFamily::Comp,
        "3D Scene",
        "3D virtual camera with perspective/orthographic view-projection matrices."
    );

    registry.RegisterNodeType<LightComp>(
        "LightComp",
        NodeFamily::Comp,
        "3D Scene",
        "3D light source (Directional, Point, Ambient) with color and attenuation."
    );

    registry.RegisterNodeType<GeometryComp>(
        "GeometryComp",
        NodeFamily::Comp,
        "3D Scene",
        "Scene object linking 3D mesh, material shader, transform, and instancing."
    );

    // RenderTexOp (TOP)
    registry.RegisterNodeType<RenderTexOp>(
        "RenderTexOp",
        NodeFamily::TexOp,
        "Render",
        "Renders a 3D scene composed of geometry, camera, and lights into a 2D texture."
    );

    // Media I/O (TOP)
    registry.RegisterNodeType<MovieFileInTexOp>(
        "MovieFileInTexOp",
        NodeFamily::TexOp,
        "Input",
        "Asynchronous video file playback with speed, loop, and scrub control."
    );

    registry.RegisterNodeType<VideoDeviceInTexOp>(
        "VideoDeviceInTexOp",
        NodeFamily::TexOp,
        "Input",
        "Captures live video streams from USB webcams and HDMI capture cards."
    );

    registry.RegisterNodeType<SpoutInTexOp>(
        "SpoutInTexOp",
        NodeFamily::TexOp,
        "Interop",
        "Receives shared real-time GPU textures from other Windows applications."
    );

    registry.RegisterNodeType<SpoutOutTexOp>(
        "SpoutOutTexOp",
        NodeFamily::TexOp,
        "Interop",
        "Publishes real-time GPU textures to the Windows Spout2 texture sharing registry."
    );

    registry.RegisterNodeType<NDIInTexOp>(
        "NDIInTexOp",
        NodeFamily::TexOp,
        "Interop",
        "Receives broadcast network video streams over LAN via NDI protocol."
    );

    registry.RegisterNodeType<NDIOutTexOp>(
        "NDIOutTexOp",
        NodeFamily::TexOp,
        "Interop",
        "Broadcasts real-time video frames across LAN as an NDI network video source."
    );

    // Projection Mapping & Multi-Output (TOP)
    registry.RegisterNodeType<ProjectorOutTexOp>(
        "ProjectorOutTexOp",
        NodeFamily::TexOp,
        "Output",
        "Presents video/generative textures directly to designated physical projectors."
    );

    registry.RegisterNodeType<WarpBlendTexOp>(
        "WarpBlendTexOp",
        NodeFamily::TexOp,
        "Projection",
        "2D Bezier grid warping, perspective corner-pin, and softedge gamma blending."
    );

    // Protocols & Show Control (Phase 11)
    registry.RegisterNodeType<MIDIInChanOp>(
        "MIDIInChanOp",
        NodeFamily::ChanOp,
        "Protocols",
        "Ingests live MIDI CC, note triggers, pitchbend, and aftertouch into control channels."
    );

    registry.RegisterNodeType<MIDIOutChanOp>(
        "MIDIOutChanOp",
        NodeFamily::ChanOp,
        "Protocols",
        "Converts channel triggers and values into MIDI events transmitted to hardware."
    );

    registry.RegisterNodeType<OSCInChanOp>(
        "OSCInChanOp",
        NodeFamily::ChanOp,
        "Protocols",
        "Decodes live UDP Open Sound Control (OSC) messages directly into real-time channels."
    );

    registry.RegisterNodeType<OSCOutChanOp>(
        "OSCOutChanOp",
        NodeFamily::ChanOp,
        "Protocols",
        "Broadcasts control channels as Open Sound Control (OSC) packets over UDP."
    );

    registry.RegisterNodeType<SerialDataOp>(
        "SerialDataOp",
        NodeFamily::DataOp,
        "Protocols",
        "Bidirectional asynchronous COM port communication with microcontrollers and sensors."
    );

    registry.RegisterNodeType<DMXInChanOp>(
        "DMXInChanOp",
        NodeFamily::ChanOp,
        "Protocols",
        "Receives 512-channel lighting universes over Art-Net 4 (Port 6454)."
    );

    registry.RegisterNodeType<DMXOutChanOp>(
        "DMXOutChanOp",
        NodeFamily::ChanOp,
        "Protocols",
        "Transmits 512-channel DMX lighting frames via Art-Net 4 UDP broadcast/unicast."
    );

    registry.RegisterNodeType<MouseInChanOp>(
        "MouseInChanOp",
        NodeFamily::ChanOp,
        "Input",
        "Interactive mouse cursor positions, motion deltas, button states, and wheel channels."
    );

    registry.RegisterNodeType<KeyboardInChanOp>(
        "KeyboardInChanOp",
        NodeFamily::ChanOp,
        "Input",
        "Interactive keyboard key states, momentary pulse triggers, and modifier keys."
    );

    // Particle Subsystem (Phase 15 Epic 15.1)
    registry.RegisterNodeType<ParticleEmitterGeomOp>(
        "ParticleEmitterGeomOp",
        NodeFamily::GeomOp,
        "Particles",
        "GPU compute particle emitter generating high-density particle streams from points, volumes, and meshes."
    );

    registry.RegisterNodeType<ParticleForceGeomOp>(
        "ParticleForceGeomOp",
        NodeFamily::GeomOp,
        "Particles",
        "Applies directional gravity, viscous drag, 3D curl turbulence noise, and vortex forces to particles."
    );

    registry.RegisterNodeType<ParticleAttractorGeomOp>(
        "ParticleAttractorGeomOp",
        NodeFamily::GeomOp,
        "Particles",
        "Dynamic 3D point attractors and repulsors driven by OSC/gesture and channel streams."
    );

    registry.RegisterNodeType<ParticleMatOp>(
        "ParticleMatOp",
        NodeFamily::MatOp,
        "Particles",
        "High-performance point-sprite and billboard quad particle material with additive/alpha blending."
    );
}

} // namespace nf
