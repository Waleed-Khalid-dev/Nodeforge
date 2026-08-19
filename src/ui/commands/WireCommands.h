#pragma once

#include "ICommand.h"
#include "../../graph/Graph.h"

namespace nf::ui {

class EditorContext;

class ConnectWireCommand : public ICommand {
public:
    ConnectWireCommand(EditorContext* ctx, Pin* fromOutPin, Pin* toInPin);
    virtual void Execute() override;
    virtual void Undo() override;
    virtual std::string GetName() const override { return "Connect Wire"; }

private:
    EditorContext* m_ctx;
    NodeId m_fromNodeId = 0;
    std::string m_fromPinName;
    NodeId m_toNodeId = 0;
    std::string m_toPinName;
    WireId m_wireId = 0;
};

class DisconnectWireCommand : public ICommand {
public:
    DisconnectWireCommand(EditorContext* ctx, Pin* toInPin);
    virtual void Execute() override;
    virtual void Undo() override;
    virtual std::string GetName() const override { return "Disconnect Wire"; }

private:
    EditorContext* m_ctx;
    NodeId m_fromNodeId = 0;
    std::string m_fromPinName;
    NodeId m_toNodeId = 0;
    std::string m_toPinName;
};

} // namespace nf::ui
