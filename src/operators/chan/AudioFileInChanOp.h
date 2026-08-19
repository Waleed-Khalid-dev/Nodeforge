#pragma once

#include "ChanOp.h"
#include <string>
#include <vector>

namespace nf {

class AudioFileInChanOp : public ChanOp {
public:
    AudioFileInChanOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

private:
    bool LoadAudioFile(const std::string& filepath);

    std::string m_loadedPath;
    std::vector<float> m_audioDataL;
    std::vector<float> m_audioDataR;
    float m_fileSampleRate = 44100.0f;
    double m_playCursor = 0.0;
};

} // namespace nf
