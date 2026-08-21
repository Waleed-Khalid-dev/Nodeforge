#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class AudioSpatializerChanOp : public Node {
public:
    AudioSpatializerChanOp(NodeId id, const std::string& name);
    ~AudioSpatializerChanOp() override = default;

    bool Cook(const CookContext& context) override;

    glm::vec3 GetEmitterPosition() const;
    glm::vec3 GetListenerPosition() const;
    float GetDistanceAttenuation() const;

private:
    Pin* m_inAudioPin = nullptr;
    Pin* m_inPosPin = nullptr;
    Pin* m_outAudioPin = nullptr;
    ChannelBuffer m_outBuffer;
};

} // namespace nf
