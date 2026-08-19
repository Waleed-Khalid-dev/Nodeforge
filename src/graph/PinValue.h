#pragma once

#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <glm/glm.hpp>

#include "../core/ChannelBuffer.h"

namespace gpu {
    class Texture2D;
}

namespace nf {

/// Type-safe data payload passed across node pins.
class PinValue {
public:
    using VariantType = std::variant<
        std::monostate,
        float,
        int32_t,
        bool,
        std::string,
        glm::vec2,
        glm::vec3,
        glm::vec4,
        ChannelBuffer,
        std::shared_ptr<gpu::Texture2D>
    >;

    PinValue() = default;

    template <typename T>
    PinValue(T&& val) : m_value(std::forward<T>(val)) {}

    bool IsEmpty() const {
        return std::holds_alternative<std::monostate>(m_value);
    }

    template <typename T>
    bool Is() const {
        return std::holds_alternative<T>(m_value);
    }

    template <typename T>
    const T& Get() const {
        return std::get<T>(m_value);
    }

    template <typename T>
    T* GetIf() {
        return std::get_if<T>(&m_value);
    }

    template <typename T>
    const T* GetIf() const {
        return std::get_if<T>(&m_value);
    }

    template <typename T>
    void Set(const T& val) {
        m_value = val;
    }

    void Clear() {
        m_value = std::monostate{};
    }

    std::string GetTypeName() const;

    const VariantType& GetRawVariant() const { return m_value; }

private:
    VariantType m_value;
};

} // namespace nf
