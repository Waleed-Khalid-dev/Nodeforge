#pragma once

#include "../graph/PinValue.h"
#include <string>
#include <vector>

namespace nf {

enum class ParamType {
    Float,
    Int,
    Bool,
    String,
    Vec2,
    Vec3,
    Vec4,
    Menu,
    RGBA,
    Filepath
};

enum class ParamMode {
    Constant,    // Static PinValue literal
    Expression   // Evaluated dynamically via Python expression string
};

struct ParamMetadata {
    std::string name;
    std::string label;
    std::string page = "General";
    ParamType type = ParamType::Float;
    PinValue defaultValue = PinValue{ 0.0f };
    PinValue minLimit = PinValue{ 0.0f };
    PinValue maxLimit = PinValue{ 1.0f };
    bool hasLimits = false;
    std::vector<std::string> menuItems;
};

} // namespace nf
