#include "WireCommands.h"
#include "../EditorContext.h"

namespace nf::ui {

ConnectWireCommand::ConnectWireCommand(EditorContext* ctx, Pin* fromOutPin, Pin* toInPin)
    : m_ctx(ctx) {
    if (fromOutPin && fromOutPin->GetNode()) {
        m_fromNodeId = fromOutPin->GetNode()->GetId();
        m_fromPinName = fromOutPin->GetName();
    }
    if (toInPin && toInPin->GetNode()) {
        m_toNodeId = toInPin->GetNode()->GetId();
        m_toPinName = toInPin->GetName();
    }
}

void ConnectWireCommand::Execute() {
    if (!m_ctx || !m_ctx->GetGraph()) return;
    Graph* g = m_ctx->GetGraph();

    Node* fromNode = g->GetNode(m_fromNodeId);
    Node* toNode = g->GetNode(m_toNodeId);
    if (!fromNode || !toNode) return;

    Pin* fromPin = fromNode->GetOutputPin(m_fromPinName);
    Pin* toPin = toNode->GetInputPin(m_toPinName);
    if (!fromPin || !toPin) return;

    Wire* w = g->Connect(fromPin, toPin);
    if (w) {
        m_wireId = w->GetId();
    }
}

void ConnectWireCommand::Undo() {
    if (!m_ctx || !m_ctx->GetGraph()) return;
    if (m_wireId != 0) {
        m_ctx->GetGraph()->Disconnect(m_wireId);
    }
}

DisconnectWireCommand::DisconnectWireCommand(EditorContext* ctx, Pin* toInPin)
    : m_ctx(ctx) {
    if (toInPin && toInPin->GetNode()) {
        m_toNodeId = toInPin->GetNode()->GetId();
        m_toPinName = toInPin->GetName();

        if (toInPin->IsConnected()) {
            Pin* fromPin = toInPin->GetConnectedSource();
            if (fromPin && fromPin->GetNode()) {
                m_fromNodeId = fromPin->GetNode()->GetId();
                m_fromPinName = fromPin->GetName();
            }
        }
    }
}

void DisconnectWireCommand::Execute() {
    if (!m_ctx || !m_ctx->GetGraph()) return;
    Node* toNode = m_ctx->GetGraph()->GetNode(m_toNodeId);
    if (!toNode) return;

    Pin* toPin = toNode->GetInputPin(m_toPinName);
    if (toPin) {
        m_ctx->GetGraph()->Disconnect(toPin);
    }
}

void DisconnectWireCommand::Undo() {
    if (!m_ctx || !m_ctx->GetGraph() || m_fromNodeId == 0 || m_toNodeId == 0) return;
    Node* fromNode = m_ctx->GetGraph()->GetNode(m_fromNodeId);
    Node* toNode = m_ctx->GetGraph()->GetNode(m_toNodeId);
    if (!fromNode || !toNode) return;

    Pin* fromPin = fromNode->GetOutputPin(m_fromPinName);
    Pin* toPin = toNode->GetInputPin(m_toPinName);
    if (fromPin && toPin) {
        m_ctx->GetGraph()->Connect(fromPin, toPin);
    }
}

} // namespace nf::ui
