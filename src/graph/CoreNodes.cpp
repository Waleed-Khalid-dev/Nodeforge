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
}

} // namespace nf
