#pragma once

#include "../../graph/Node.h"
#include "../../gpu/Texture.h"

namespace nf {

class ONNXInferenceTexOp : public Node {
public:
    ONNXInferenceTexOp(NodeId id, const std::string& name);
    ~ONNXInferenceTexOp() override = default;

    bool Cook(const CookContext& context) override;

    std::string GetModelPath() const;
    int32_t GetExecutionProvider() const;
    std::string GetInputTensorName() const;
    std::string GetOutputTensorName() const;

private:
    Pin* m_inTexPin = nullptr;
    Pin* m_outTexPin = nullptr;
    std::shared_ptr<gpu::Texture2D> m_outTexture;
};

} // namespace nf
