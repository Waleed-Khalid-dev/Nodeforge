#pragma once

#include "ParamType.h"
#include <functional>

namespace nf {

class Node;

class Parameter {
public:
    Parameter(const ParamMetadata& meta, Node* owner = nullptr);

    const std::string& GetName() const { return m_meta.name; }
    const std::string& GetLabel() const { return m_meta.label; }
    const std::string& GetPage() const { return m_meta.page; }
    ParamType GetType() const { return m_meta.type; }
    const ParamMetadata& GetMetadata() const { return m_meta; }

    ParamMode GetMode() const { return m_mode; }
    void SetMode(ParamMode mode);

    // Get current evaluated or constant value
    const PinValue& GetValue() const;

    // Set static constant value (switches mode to Constant and marks dirty)
    void SetValue(const PinValue& value);

    // Set dynamic expression string (switches mode to Expression and marks dirty)
    const std::string& GetExpression() const { return m_expression; }
    void SetExpression(const std::string& expr);

    // Channel Binding (switches mode to BoundChannel and marks dirty)
    void SetBoundChannel(const std::string& nodeName, const std::string& channelName);
    const std::string& GetBoundNodeName() const { return m_boundNodeName; }
    const std::string& GetBoundChannelName() const { return m_boundChannelName; }
    void ClearBoundChannel();

    // Evaluated cache update (called by engine during cook or channel bind update)
    void SetEvaluatedValue(const PinValue& value) { m_evaluatedValue = value; }
    const PinValue& GetEvaluatedValue() const { return m_evaluatedValue; }
    const PinValue& GetConstantValue() const { return m_constantValue; }

    void SetOnChangeCallback(std::function<void()> cb) { m_onChanged = std::move(cb); }
    void SetOwnerNode(Node* node) { m_ownerNode = node; }
    Node* GetOwnerNode() const { return m_ownerNode; }

    // Helpers
    float AsFloat() const;
    int32_t AsInt() const;
    bool AsBool() const;
    std::string AsString() const;
    glm::vec2 AsVec2() const;
    glm::vec3 AsVec3() const;
    glm::vec4 AsVec4() const;

private:
    PinValue ClampValue(const PinValue& val) const;
    void NotifyChanged();

    ParamMetadata m_meta;
    Node* m_ownerNode = nullptr;
    ParamMode m_mode = ParamMode::Constant;
    PinValue m_constantValue;
    std::string m_expression;
    std::string m_boundNodeName;
    std::string m_boundChannelName;
    PinValue m_evaluatedValue;
    std::function<void()> m_onChanged;
};

} // namespace nf
