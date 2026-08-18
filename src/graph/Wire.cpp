#include "Wire.h"
#include "Pin.h"

namespace nf {

Wire::Wire(WireId id, Pin* fromPin, Pin* toPin)
    : m_id(id), m_fromPin(fromPin), m_toPin(toPin) {
}

} // namespace nf
