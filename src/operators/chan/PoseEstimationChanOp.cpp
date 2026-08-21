#include "PoseEstimationChanOp.h"
#include "../../ai/ONNXInferenceEngine.h"

namespace nf {

PoseEstimationChanOp::PoseEstimationChanOp(NodeId id, const std::string& name)
    : Node(id, name, "PoseEstimationChanOp") {
    m_inTexPin = AddInputPin("input", PinType::Tex);
    m_outKeypointsPin = AddOutputPin("out_keypoints", PinType::Chan);
    m_outConfidencePin = AddOutputPin("out_confidence", PinType::Chan);

    SetParam("confidence_threshold", 0.5f);
    SetParam("smoothing", 0.2f);

    m_keypointsBuffer.Resize(34, 1);
    m_confidenceBuffer.Resize(17, 1);

    std::vector<std::string> kpNames;
    kpNames.reserve(34);
    const char* jointNames[17] = {
        "nose", "l_eye", "r_eye", "l_ear", "r_ear",
        "l_shoulder", "r_shoulder", "l_elbow", "r_elbow",
        "l_wrist", "r_wrist", "l_hip", "r_hip",
        "l_knee", "r_knee", "l_ankle", "r_ankle"
    };
    for (int i = 0; i < 17; ++i) {
        kpNames.push_back(std::string(jointNames[i]) + "_x");
        kpNames.push_back(std::string(jointNames[i]) + "_y");
    }
    m_keypointsBuffer.SetChannelNames(kpNames);

    std::vector<std::string> confNames;
    confNames.reserve(17);
    for (int i = 0; i < 17; ++i) {
        confNames.push_back(std::string(jointNames[i]) + "_conf");
    }
    m_confidenceBuffer.SetChannelNames(confNames);
}

float PoseEstimationChanOp::GetConfidenceThreshold() const {
    return GetParam("confidence_threshold").Is<float>() ? GetParam("confidence_threshold").Get<float>() : 0.5f;
}

float PoseEstimationChanOp::GetSmoothing() const {
    return GetParam("smoothing").Is<float>() ? GetParam("smoothing").Get<float>() : 0.2f;
}

bool PoseEstimationChanOp::Cook(const CookContext& context) {
    Pose17 pose = ONNXInferenceEngine::Instance().EstimatePose(static_cast<float>(context.timeSeconds));

    m_keypointsBuffer.Resize(34, 1);
    m_confidenceBuffer.Resize(17, 1);

    for (int i = 0; i < 17; ++i) {
        m_keypointsBuffer.GetChannelData(i * 2 + 0)[0] = pose.keypoints[i].position.x;
        m_keypointsBuffer.GetChannelData(i * 2 + 1)[0] = pose.keypoints[i].position.y;
        m_confidenceBuffer.GetChannelData(i)[0] = pose.keypoints[i].confidence;
    }

    m_outKeypointsPin->SetValue(PinValue(m_keypointsBuffer));
    m_outConfidencePin->SetValue(PinValue(m_confidenceBuffer));
    return true;
}

} // namespace nf
