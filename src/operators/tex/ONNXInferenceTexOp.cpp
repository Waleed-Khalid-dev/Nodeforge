#include "ONNXInferenceTexOp.h"
#include "../../ai/ONNXInferenceEngine.h"

namespace nf {

ONNXInferenceTexOp::ONNXInferenceTexOp(NodeId id, const std::string& name)
    : Node(id, name, "ONNXInferenceTexOp") {
    m_inTexPin = AddInputPin("input", PinType::Tex);
    m_outTexPin = AddOutputPin("output", PinType::Tex);

    SetParam("model_path", std::string(""));
    SetParam("execution_provider", static_cast<int32_t>(0)); // 0: DirectML, 1: CUDA, 2: CPU
    SetParam("input_tensor_name", std::string("input"));
    SetParam("output_tensor_name", std::string("output"));
}

std::string ONNXInferenceTexOp::GetModelPath() const {
    return GetParam("model_path").Is<std::string>() ? GetParam("model_path").Get<std::string>() : "";
}

int32_t ONNXInferenceTexOp::GetExecutionProvider() const {
    return GetParam("execution_provider").Is<int32_t>() ? GetParam("execution_provider").Get<int32_t>() : 0;
}

std::string ONNXInferenceTexOp::GetInputTensorName() const {
    return GetParam("input_tensor_name").Is<std::string>() ? GetParam("input_tensor_name").Get<std::string>() : "input";
}

std::string ONNXInferenceTexOp::GetOutputTensorName() const {
    return GetParam("output_tensor_name").Is<std::string>() ? GetParam("output_tensor_name").Get<std::string>() : "output";
}

bool ONNXInferenceTexOp::Cook(const CookContext& /*context*/) {
    if (m_inTexPin && m_inTexPin->GetValue().Is<std::shared_ptr<gpu::Texture2D>>()) {
        m_outTexture = m_inTexPin->GetValue().Get<std::shared_ptr<gpu::Texture2D>>();
    }

    if (m_outTexture) {
        m_outTexPin->SetValue(PinValue(m_outTexture));
    }
    return true;
}

} // namespace nf
