#pragma once

#include "TexOp.h"
#include "../../media/VideoDecoder.h"

namespace nf {

class MovieFileInTexOp : public TexOp {
public:
    MovieFileInTexOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;

    VideoDecoder& GetDecoder() { return m_decoder; }

private:
    VideoDecoder m_decoder;
    std::string m_loadedPath;
};

} // namespace nf
