#pragma once

#include "ICommand.h"
#include "../../param/Parameter.h"
#include "../../graph/Graph.h"

namespace nf::ui {

class EditorContext;

class SetParamValueCommand : public ICommand {
public:
    SetParamValueCommand(EditorContext* ctx, NodeId nodeId, const std::string& paramName, const PinValue& oldValue, const PinValue& newValue);
    virtual void Execute() override;
    virtual void Undo() override;
    virtual std::string GetName() const override { return "Set Parameter: " + m_paramName; }
    virtual bool CanMergeWith(const ICommand* other) const override;
    virtual void MergeWith(const ICommand* other) override;

private:
    EditorContext* m_ctx;
    NodeId m_nodeId;
    std::string m_paramName;
    PinValue m_oldValue;
    PinValue m_newValue;
};

class SetParamExpressionCommand : public ICommand {
public:
    SetParamExpressionCommand(EditorContext* ctx, NodeId nodeId, const std::string& paramName, const std::string& oldExpr, const std::string& newExpr);
    virtual void Execute() override;
    virtual void Undo() override;
    virtual std::string GetName() const override { return "Set Expression: " + m_paramName; }

private:
    EditorContext* m_ctx;
    NodeId m_nodeId;
    std::string m_paramName;
    std::string m_oldExpr;
    std::string m_newExpr;
};

} // namespace nf::ui
