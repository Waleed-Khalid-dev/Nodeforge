#include "ParamCommands.h"
#include "../EditorContext.h"

namespace nf::ui {

SetParamValueCommand::SetParamValueCommand(EditorContext* ctx, NodeId nodeId, const std::string& paramName, const PinValue& oldValue, const PinValue& newValue)
    : m_ctx(ctx), m_nodeId(nodeId), m_paramName(paramName), m_oldValue(oldValue), m_newValue(newValue) {
}

void SetParamValueCommand::Execute() {
    if (!m_ctx || !m_ctx->GetGraph()) return;
    Node* n = m_ctx->GetGraph()->GetNode(m_nodeId);
    if (n) {
        Parameter* p = n->GetParams().Get(m_paramName);
        if (p) {
            p->SetValue(m_newValue);
        }
    }
}

void SetParamValueCommand::Undo() {
    if (!m_ctx || !m_ctx->GetGraph()) return;
    Node* n = m_ctx->GetGraph()->GetNode(m_nodeId);
    if (n) {
        Parameter* p = n->GetParams().Get(m_paramName);
        if (p) {
            p->SetValue(m_oldValue);
        }
    }
}

bool SetParamValueCommand::CanMergeWith(const ICommand* other) const {
    const auto* o = dynamic_cast<const SetParamValueCommand*>(other);
    return o != nullptr && o->m_ctx == m_ctx && o->m_nodeId == m_nodeId && o->m_paramName == m_paramName;
}

void SetParamValueCommand::MergeWith(const ICommand* other) {
    const auto* o = dynamic_cast<const SetParamValueCommand*>(other);
    if (o) {
        m_newValue = o->m_newValue;
    }
}

SetParamExpressionCommand::SetParamExpressionCommand(EditorContext* ctx, NodeId nodeId, const std::string& paramName, const std::string& oldExpr, const std::string& newExpr)
    : m_ctx(ctx), m_nodeId(nodeId), m_paramName(paramName), m_oldExpr(oldExpr), m_newExpr(newExpr) {
}

void SetParamExpressionCommand::Execute() {
    if (!m_ctx || !m_ctx->GetGraph()) return;
    Node* n = m_ctx->GetGraph()->GetNode(m_nodeId);
    if (n) {
        Parameter* p = n->GetParams().Get(m_paramName);
        if (p) {
            if (m_newExpr.empty()) {
                p->SetMode(ParamMode::Constant);
            } else {
                p->SetExpression(m_newExpr);
            }
        }
    }
}

void SetParamExpressionCommand::Undo() {
    if (!m_ctx || !m_ctx->GetGraph()) return;
    Node* n = m_ctx->GetGraph()->GetNode(m_nodeId);
    if (n) {
        Parameter* p = n->GetParams().Get(m_paramName);
        if (p) {
            if (m_oldExpr.empty()) {
                p->SetMode(ParamMode::Constant);
            } else {
                p->SetExpression(m_oldExpr);
            }
        }
    }
}

} // namespace nf::ui
