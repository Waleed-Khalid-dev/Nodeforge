#include "Parameter.h"
#include "../graph/Node.h"
#include <algorithm>

namespace nf {

Parameter::Parameter(const ParamMetadata& meta, Node* owner)
    : m_meta(meta), m_ownerNode(owner), m_constantValue(meta.defaultValue), m_evaluatedValue(meta.defaultValue) {
}

void Parameter::SetMode(ParamMode mode) {
    if (m_mode != mode) {
        m_mode = mode;
        NotifyChanged();
    }
}

const PinValue& Parameter::GetValue() const {
    if (m_mode == ParamMode::Expression) {
        return m_evaluatedValue;
    }
    return m_constantValue;
}

void Parameter::SetValue(const PinValue& value) {
    m_constantValue = ClampValue(value);
    m_mode = ParamMode::Constant;
    NotifyChanged();
}

void Parameter::SetExpression(const std::string& expr) {
    m_expression = expr;
    m_mode = ParamMode::Expression;
    NotifyChanged();
}

PinValue Parameter::ClampValue(const PinValue& val) const {
    if (!m_meta.hasLimits) return val;

    if (val.Is<float>() && m_meta.minLimit.Is<float>() && m_meta.maxLimit.Is<float>()) {
        float v = val.Get<float>();
        float minV = m_meta.minLimit.Get<float>();
        float maxV = m_meta.maxLimit.Get<float>();
        return PinValue(std::clamp(v, minV, maxV));
    }

    if (val.Is<int32_t>() && m_meta.minLimit.Is<int32_t>() && m_meta.maxLimit.Is<int32_t>()) {
        int32_t v = val.Get<int32_t>();
        int32_t minV = m_meta.minLimit.Get<int32_t>();
        int32_t maxV = m_meta.maxLimit.Get<int32_t>();
        return PinValue(std::clamp(v, minV, maxV));
    }

    return val;
}

void Parameter::NotifyChanged() {
    if (m_ownerNode) {
        m_ownerNode->MarkDirty();
    }
    if (m_onChanged) {
        m_onChanged();
    }
}

float Parameter::AsFloat() const {
    const auto& v = GetValue();
    if (v.Is<float>()) return v.Get<float>();
    if (v.Is<int32_t>()) return static_cast<float>(v.Get<int32_t>());
    if (v.Is<bool>()) return v.Get<bool>() ? 1.0f : 0.0f;
    if (v.Is<glm::vec2>()) return v.Get<glm::vec2>().x;
    if (v.Is<glm::vec3>()) return v.Get<glm::vec3>().x;
    if (v.Is<glm::vec4>()) return v.Get<glm::vec4>().x;
    return 0.0f;
}

int32_t Parameter::AsInt() const {
    const auto& v = GetValue();
    if (v.Is<int32_t>()) return v.Get<int32_t>();
    if (v.Is<float>()) return static_cast<int32_t>(v.Get<float>());
    if (v.Is<bool>()) return v.Get<bool>() ? 1 : 0;
    return 0;
}

bool Parameter::AsBool() const {
    const auto& v = GetValue();
    if (v.Is<bool>()) return v.Get<bool>();
    if (v.Is<int32_t>()) return v.Get<int32_t>() != 0;
    if (v.Is<float>()) return std::abs(v.Get<float>()) > 1e-6f;
    return false;
}

std::string Parameter::AsString() const {
    const auto& v = GetValue();
    if (v.Is<std::string>()) return v.Get<std::string>();
    if (v.Is<float>()) return std::to_string(v.Get<float>());
    if (v.Is<int32_t>()) return std::to_string(v.Get<int32_t>());
    if (v.Is<bool>()) return v.Get<bool>() ? "true" : "false";
    return "";
}

glm::vec2 Parameter::AsVec2() const {
    const auto& v = GetValue();
    if (v.Is<glm::vec2>()) return v.Get<glm::vec2>();
    return glm::vec2(AsFloat());
}

glm::vec3 Parameter::AsVec3() const {
    const auto& v = GetValue();
    if (v.Is<glm::vec3>()) return v.Get<glm::vec3>();
    return glm::vec3(AsFloat());
}

glm::vec4 Parameter::AsVec4() const {
    const auto& v = GetValue();
    if (v.Is<glm::vec4>()) return v.Get<glm::vec4>();
    return glm::vec4(AsFloat());
}

} // namespace nf
