#pragma once

#include "../../graph/Node.h"
#include "../../gpu/Texture.h"
#include "../../gpu/TexturePool.h"
#include <memory>

namespace nf {

class TexOp : public Node {
public:
    TexOp(NodeId id, const std::string& name, const std::string& typeName);
    virtual ~TexOp() = default;

    std::shared_ptr<gpu::Texture2D> GetOutputTexture() const { return m_outputTexture; }
    std::shared_ptr<gpu::Texture2D> GetInputTexture(size_t index = 0) const;

protected:
    void SetOutputTexture(std::shared_ptr<gpu::Texture2D> texture);

    Pin* m_outPin = nullptr;
    std::shared_ptr<gpu::Texture2D> m_outputTexture;
};

} // namespace nf
