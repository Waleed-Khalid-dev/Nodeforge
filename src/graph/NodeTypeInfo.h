#pragma once

#include "Pin.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace nf {

class Node;
using NodeId = uint64_t;

enum class NodeFamily {
    TexOp,   // 2D GPU Textures (TOP)
    ChanOp,  // Numeric / Audio Channels (CHOP)
    GeomOp,  // 3D Geometry (SOP)
    MatOp,   // Materials & Shaders (MAT)
    DataOp,  // JSON / Tables (DAT)
    Comp     // Container / Composition (COMP)
};

struct PinDef {
    std::string name;
    PinDirection direction;
    PinType type;
    PinValue defaultValue = PinValue{};
};

struct NodeTypeInfo {
    std::string typeName;
    NodeFamily family;
    std::string category;
    std::string description;
    std::vector<PinDef> defaultPins;
    std::function<std::unique_ptr<Node>(NodeId id, const std::string& name)> factory;
};

} // namespace nf
