#include "PinValue.h"

namespace nf {

std::string PinValue::GetTypeName() const {
    return std::visit([](const auto& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            return "Empty";
        } else if constexpr (std::is_same_v<T, float>) {
            return "Float";
        } else if constexpr (std::is_same_v<T, int32_t>) {
            return "Int";
        } else if constexpr (std::is_same_v<T, bool>) {
            return "Bool";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "String";
        } else if constexpr (std::is_same_v<T, glm::vec2>) {
            return "Vec2";
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            return "Vec3";
        } else if constexpr (std::is_same_v<T, glm::vec4>) {
            return "Vec4";
        } else if constexpr (std::is_same_v<T, ChannelBuffer>) {
            return "ChannelBuffer";
        } else if constexpr (std::is_same_v<T, DataTable>) {
            return "DataTable";
        } else if constexpr (std::is_same_v<T, std::shared_ptr<gpu::Texture2D>>) {
            return "Texture2D";
        } else {
            return "Unknown";
        }
    }, m_value);
}

} // namespace nf
