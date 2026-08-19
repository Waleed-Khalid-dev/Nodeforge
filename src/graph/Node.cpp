#include "Node.h"
#include "NodeRegistry.h"
#include "../python/PythonEngine.h"
#include <spdlog/spdlog.h>

namespace nf {

Node::Node(NodeId id, const std::string& name, const std::string& typeName)
    : m_id(id), m_name(name), m_typeName(typeName), m_params(this) {
}

NodeFamily Node::GetFamily() const {
    const auto* info = NodeRegistry::Instance().GetTypeInfo(m_typeName);
    if (info) return info->family;
    return NodeFamily::DataOp;
}

Pin* Node::AddInputPin(const std::string& name, PinType type, const PinValue& defaultValue) {
    auto pin = std::make_unique<Pin>(this, name, PinDirection::Input, type, defaultValue);
    Pin* ptr = pin.get();
    m_inputPins.push_back(std::move(pin));
    return ptr;
}

Pin* Node::AddOutputPin(const std::string& name, PinType type, const PinValue& defaultValue) {
    auto pin = std::make_unique<Pin>(this, name, PinDirection::Output, type, defaultValue);
    Pin* ptr = pin.get();
    m_outputPins.push_back(std::move(pin));
    return ptr;
}

Pin* Node::GetInputPin(const std::string& name) {
    for (auto& pin : m_inputPins) {
        if (pin->GetName() == name) return pin.get();
    }
    return nullptr;
}

Pin* Node::GetOutputPin(const std::string& name) {
    for (auto& pin : m_outputPins) {
        if (pin->GetName() == name) return pin.get();
    }
    return nullptr;
}

Pin* Node::GetInputPin(size_t index) {
    if (index < m_inputPins.size()) return m_inputPins[index].get();
    return nullptr;
}

Pin* Node::GetOutputPin(size_t index) {
    if (index < m_outputPins.size()) return m_outputPins[index].get();
    return nullptr;
}

void Node::SetParam(const std::string& name, const PinValue& value) {
    Parameter* p = m_params.Get(name);
    if (p) {
        p->SetValue(value);
    } else {
        ParamMetadata meta;
        meta.name = name;
        meta.label = name;
        meta.defaultValue = value;
        if (value.Is<float>()) meta.type = ParamType::Float;
        else if (value.Is<int32_t>()) meta.type = ParamType::Int;
        else if (value.Is<bool>()) meta.type = ParamType::Bool;
        else if (value.Is<std::string>()) meta.type = ParamType::String;
        else if (value.Is<glm::vec2>()) meta.type = ParamType::Vec2;
        else if (value.Is<glm::vec3>()) meta.type = ParamType::Vec3;
        else if (value.Is<glm::vec4>()) meta.type = ParamType::Vec4;
        m_params.Add(meta);
        MarkDirty();
    }
}

const PinValue& Node::GetParam(const std::string& name) const {
    static const PinValue emptyVal{};
    Parameter* p = m_params.Get(name);
    if (p) return p->GetValue();
    return emptyVal;
}

bool Node::HasParam(const std::string& name) const {
    return m_params.Has(name);
}

void Node::RemoveDownstreamNode(Node* node) {
    std::erase(m_downstreamNodes, node);
}

void Node::MarkDirty() {
    if (m_isDirty) return; // Short-circuit redundant propagation
    m_isDirty = true;

    // Propagate downstream directly via cached node pointers
    for (Node* downstream : m_downstreamNodes) {
        if (downstream) {
            downstream->MarkDirty();
        }
    }
}

bool Node::EnsureCooked(const CookContext& context) {
    // If not dirty and already cooked for this exact frame, return cached result
    if (!m_isDirty && m_lastCookFrame == context.frameIndex) {
        return true;
    }

    // Recursively ensure all upstream input dependencies are cooked first
    for (auto& inPin : m_inputPins) {
        if (inPin->IsConnected()) {
            Pin* srcPin = inPin->GetConnectedSource();
            if (srcPin && srcPin->GetNode()) {
                if (!srcPin->GetNode()->EnsureCooked(context)) {
                    spdlog::error("Upstream node '{}' failed to cook for '{}'", srcPin->GetNode()->GetName(), m_name);
                    return false;
                }
            }
        }
    }

    // Evaluate dynamic parameter expressions
    for (const auto& param : m_params.GetAll()) {
        if (param->GetMode() == ParamMode::Expression && !param->GetExpression().empty()) {
            std::string exprErr;
            PinValue evalResult = PythonEngine::Instance().EvaluateExpression(param->GetExpression(), this, context, &exprErr);
            param->SetEvaluatedValue(evalResult);
        }
    }

    // Cook this node
    bool ok = Cook(context);
    if (ok) {
        m_isDirty = false;
        m_lastCookFrame = context.frameIndex;
    } else {
        spdlog::error("Node '{}' (type {}) failed to cook at frame {}", m_name, m_typeName, context.frameIndex);
    }
    return ok;
}

std::vector<Node*> Node::GetUpstreamNodes() const {
    std::vector<Node*> result;
    for (const auto& inPin : m_inputPins) {
        if (inPin->IsConnected()) {
            Pin* src = inPin->GetConnectedSource();
            if (src && src->GetNode()) {
                result.push_back(src->GetNode());
            }
        }
    }
    return result;
}

} // namespace nf
