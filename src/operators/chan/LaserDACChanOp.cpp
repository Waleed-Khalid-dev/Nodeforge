#include "LaserDACChanOp.h"
#include "../../laser/LaserEngine.h"

namespace nf {

LaserDACChanOp::LaserDACChanOp(NodeId id, const std::string& name)
    : Node(id, name, "LaserDACChanOp") {
    m_inPointsPin = AddInputPin("in_points", PinType::Chan);
    m_outTelemetryPin = AddOutputPin("out_telemetry", PinType::Chan);

    SetParam("dac_type", static_cast<int32_t>(2)); // 0: Ether Dream, 1: Helios, 2: Headless Emulated
    SetParam("ip_address", std::string("192.168.1.100"));
    SetParam("safety_shutter", true);
    SetParam("master_brightness", 1.0f);

    m_telemetryBuffer.Resize(4, 1);
    m_telemetryBuffer.SetChannelNames({"buffer_fill", "pps", "dropped", "shutter"});
}

int32_t LaserDACChanOp::GetDACType() const {
    return GetParam("dac_type").Is<int32_t>() ? GetParam("dac_type").Get<int32_t>() : 2;
}

std::string LaserDACChanOp::GetIPAddress() const {
    return GetParam("ip_address").Is<std::string>() ? GetParam("ip_address").Get<std::string>() : "192.168.1.100";
}

bool LaserDACChanOp::IsSafetyShutterOpen() const {
    return GetParam("safety_shutter").Is<bool>() ? GetParam("safety_shutter").Get<bool>() : true;
}

float LaserDACChanOp::GetMasterBrightness() const {
    return GetParam("master_brightness").Is<float>() ? GetParam("master_brightness").Get<float>() : 1.0f;
}

bool LaserDACChanOp::Cook(const CookContext& /*context*/) {
    std::vector<LaserPoint> points;

    if (m_inPointsPin && m_inPointsPin->GetValue().Is<ChannelBuffer>()) {
        const auto& buf = m_inPointsPin->GetValue().Get<ChannelBuffer>();
        uint32_t sampleCount = buf.GetSampleCount();
        points.reserve(sampleCount);

        for (uint32_t i = 0; i < sampleCount; ++i) {
            LaserPoint pt;
            if (buf.GetChannelCount() >= 2) {
                pt.x = LaserPoint::NormalizeCoord(buf.GetChannelData(0)[i]);
                pt.y = LaserPoint::NormalizeCoord(buf.GetChannelData(1)[i]);
            }
            if (buf.GetChannelCount() >= 6) {
                pt.r = buf.GetChannelData(3)[i] * GetMasterBrightness();
                pt.g = buf.GetChannelData(4)[i] * GetMasterBrightness();
                pt.b = buf.GetChannelData(5)[i] * GetMasterBrightness();
            }
            if (buf.GetChannelCount() >= 7) {
                pt.isBlanked = (buf.GetChannelData(6)[i] > 0.5f);
            }
            points.push_back(pt);
        }
    }

    auto tele = LaserEngine::Instance().TransmitPoints(
        points,
        30000,
        IsSafetyShutterOpen(),
        GetDACType()
    );

    m_telemetryBuffer.Resize(4, 1);
    m_telemetryBuffer.SetChannelNames({"buffer_fill", "pps", "dropped", "shutter"});
    m_telemetryBuffer.GetChannelData(0)[0] = tele.bufferFill;
    m_telemetryBuffer.GetChannelData(1)[0] = static_cast<float>(tele.currentPPS);
    m_telemetryBuffer.GetChannelData(2)[0] = static_cast<float>(tele.droppedPoints);
    m_telemetryBuffer.GetChannelData(3)[0] = tele.shutterOpen ? 1.0f : 0.0f;

    m_outTelemetryPin->SetValue(PinValue(m_telemetryBuffer));
    return true;
}

} // namespace nf
