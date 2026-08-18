#include "Pin.h"
#include "Node.h"
#include <algorithm>

namespace nf {

Pin::Pin(Node* owner, const std::string& name, PinDirection direction, PinType type, const PinValue& defaultValue)
    : m_owner(owner), m_name(name), m_direction(direction), m_type(type), m_defaultValue(defaultValue), m_value(defaultValue) {
}

const PinValue& Pin::GetValue() const {
    if (m_direction == PinDirection::Input && IsConnected()) {
        Pin* src = GetConnectedSource();
        if (src) {
            return src->GetValue();
        }
    }
    return m_value;
}

void Pin::SetValue(const PinValue& value) {
    m_value = value;
}

bool Pin::IsConnected() const {
    return !m_connections.empty();
}

Pin* Pin::GetConnectedSource() const {
    if (m_direction == PinDirection::Input && !m_connections.empty()) {
        return m_connections.front();
    }
    return nullptr;
}

bool Pin::CanConnectTo(const Pin* target) const {
    if (!target || target == this) return false;
    if (m_owner == target->GetNode()) return false; // No self-connection on same node
    if (m_direction == target->GetDirection()) return false; // Output -> Input only
    if (m_type != PinType::Any && target->GetType() != PinType::Any && m_type != target->GetType()) {
        return false; // Type mismatch
    }
    return true;
}

void Pin::AddConnection(Pin* other) {
    if (!other) return;
    if (m_direction == PinDirection::Input) {
        m_connections.clear(); // Inputs allow single upstream source
        m_connections.push_back(other);
    } else {
        if (std::find(m_connections.begin(), m_connections.end(), other) == m_connections.end()) {
            m_connections.push_back(other);
        }
    }
}

void Pin::RemoveConnection(Pin* other) {
    auto it = std::find(m_connections.begin(), m_connections.end(), other);
    if (it != m_connections.end()) {
        m_connections.erase(it);
    }
}

} // namespace nf
