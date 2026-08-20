#pragma once

#include "TexOp.h"

namespace nf {

class VideoDeviceInTexOp : public TexOp {
public:
    VideoDeviceInTexOp(NodeId id, const std::string& name);
    bool Cook(const CookContext& context) override;
};

} // namespace nf
