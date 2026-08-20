#pragma once

#include "../graph/Node.h"
#include "PluginTypes.h"
#include <memory>

namespace nf {

class PluginNodeProxy : public Node {
public:
    PluginNodeProxy(NodeId id, const std::string& name, const PluginOperatorInfo& opInfo);
    ~PluginNodeProxy() override;

    bool Cook(const CookContext& context) override;

    const PluginOperatorInfo& GetOperatorInfo() const { return m_opInfo; }
    NF_PluginInstanceHandle GetInstanceHandle() const { return m_instanceHandle; }

private:
    void InitPinsAndParams();
    void SyncParamsToPlugin();

    PluginOperatorInfo m_opInfo;
    NF_PluginInstanceHandle m_instanceHandle = nullptr;
};

} // namespace nf
