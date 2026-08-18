#include "CoreNodes.h"
#include "../operators/chan/ConstantChanOp.h"
#include "../operators/chan/MathChanOp.h"
#include "../operators/tex/ConstantTexOp.h"
#include "../operators/tex/TransformTexOp.h"

namespace nf {

void RegisterCoreNodes(NodeRegistry& registry) {
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

    registry.RegisterNodeType<ConstantTexOp>(
        "ConstantTexOp",
        NodeFamily::TexOp,
        "Generators",
        "Generates a 2D solid color GPU texture."
    );

    registry.RegisterNodeType<TransformTexOp>(
        "TransformTexOp",
        NodeFamily::TexOp,
        "Transform",
        "Applies 2D geometric transformation to an input texture."
    );
}

} // namespace nf
