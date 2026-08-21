#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"

namespace nf {

class AudioEmitterComp : public Node {
public:
    AudioEmitterComp(NodeId id, const std::string& name);
    ~AudioEmitterComp() override = default;

    bool Cook(const CookContext& context) override;

    glm::vec3 GetPosition() const;
    float GetRadius() const;

private:
    Pin* m_inAudioPin = nullptr;
    Pin* m_inGeomPin = nullptr;
    Pin* m_outCompPin = nullptr;
    Pin* m_outAudioPin = nullptr;
};

} // namespace nf
