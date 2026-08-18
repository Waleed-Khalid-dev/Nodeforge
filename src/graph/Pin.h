#pragma once

#include "PinValue.h"
#include <string>
#include <vector>

namespace nf {

class Node;

enum class PinDirection {
    Input,
    Output
};

enum class PinType {
    Tex,   // Texture Operator payload (Texture2D)
    Chan,  // Channel Operator payload (ChannelBuffer)
    Geom,  // Geometry Operator payload (MeshHandle)
    Mat,   // Material Operator payload (Shader/Material)
    Data,  // Data Operator payload (JSON/Table)
    Any    // Polymorphic / generic
};

class Pin {
public:
    Pin(Node* owner, const std::string& name, PinDirection direction, PinType type, const PinValue& defaultValue = PinValue{});

    Node* GetNode() const { return m_owner; }
    const std::string& GetName() const { return m_name; }
    PinDirection GetDirection() const { return m_direction; }
    PinType GetType() const { return m_type; }

    bool IsInput() const { return m_direction == PinDirection::Input; }
    bool IsOutput() const { return m_direction == PinDirection::Output; }

    const PinValue& GetValue() const;
    void SetValue(const PinValue& value);

    bool IsConnected() const;
    const std::vector<Pin*>& GetConnections() const { return m_connections; }
    Pin* GetConnectedSource() const;

    bool CanConnectTo(const Pin* target) const;
    void AddConnection(Pin* other);
    void RemoveConnection(Pin* other);

private:
    Node* m_owner = nullptr;
    std::string m_name;
    PinDirection m_direction;
    PinType m_type;
    PinValue m_value;
    PinValue m_defaultValue;
    std::vector<Pin*> m_connections;
};

} // namespace nf
