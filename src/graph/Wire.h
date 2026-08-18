#pragma once

#include <cstdint>

namespace nf {

class Pin;

using WireId = uint64_t;

class Wire {
public:
    Wire(WireId id, Pin* fromPin, Pin* toPin);

    WireId GetId() const { return m_id; }
    Pin* GetFromPin() const { return m_fromPin; }
    Pin* GetToPin() const { return m_toPin; }

private:
    WireId m_id;
    Pin* m_fromPin = nullptr;
    Pin* m_toPin = nullptr;
};

} // namespace nf
