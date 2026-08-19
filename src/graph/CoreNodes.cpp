#include "CoreNodes.h"
#include "../operators/chan/ConstantChanOp.h"
#include "../operators/chan/MathChanOp.h"
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

namespace nf {

void RegisterCoreNodes(NodeRegistry& registry) {
    // ChanOps
    registry.RegisterNodeType<ConstantChanOp>(
        "ConstantChanOp",
        NodeFamily::ChanOp,
        "Generators",
        "Generates constant multi-channel numeric streams."
    );

    registry.RegisterNodeType<MathChanOp>(
        "MathChanOp",
        NodeFamily::ChanOp,
        "Math",
        "Performs arithmetic and trigonometric operations on channels."
    );

    // TexOps
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
}

} // namespace nf
