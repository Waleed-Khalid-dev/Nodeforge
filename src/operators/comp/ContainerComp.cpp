#include "ContainerComp.h"
#include "InOp.h"
#include "OutOp.h"
#include <algorithm>

namespace nf {

ContainerComp::ContainerComp(NodeId id, const std::string& name)
    : Node(id, name, "ContainerComp"), m_innerGraph(std::make_unique<Graph>()) {
    SynchronizeBoundaryPins();
}

void ContainerComp::SetInnerGraph(std::unique_ptr<Graph> graph) {
    m_innerGraph = std::move(graph);
    if (!m_innerGraph) {
        m_innerGraph = std::make_unique<Graph>();
    }
    SynchronizeBoundaryPins();
}

void ContainerComp::SynchronizeBoundaryPins() {
    if (!m_innerGraph) return;

    for (const auto& [nodeId, node] : m_innerGraph->GetNodes()) {
        if (auto* inOp = dynamic_cast<InOp*>(node.get())) {
            std::string pinName = "in1";
            if (inOp->GetParam("pin_name").Is<std::string>()) {
                pinName = inOp->GetParam("pin_name").Get<std::string>();
            } else {
                pinName = inOp->GetName();
            }

            if (!GetInputPin(pinName)) {
                AddInputPin(pinName, inOp->GetPinPayloadType());
            }
        } else if (auto* outOp = dynamic_cast<OutOp*>(node.get())) {
            std::string pinName = "out1";
            if (outOp->GetParam("pin_name").Is<std::string>()) {
                pinName = outOp->GetParam("pin_name").Get<std::string>();
            } else {
                pinName = outOp->GetName();
            }

            if (!GetOutputPin(pinName)) {
                AddOutputPin(pinName, outOp->GetPinPayloadType());
            }
        }
    }
}

bool ContainerComp::Cook(const CookContext& context) {
    if (!m_innerGraph) return true;

    // 1. Forward parent container inputs to inner InOp nodes
    for (const auto& [nodeId, node] : m_innerGraph->GetNodes()) {
        if (auto* inOp = dynamic_cast<InOp*>(node.get())) {
            std::string pinName = "in1";
            if (inOp->GetParam("pin_name").Is<std::string>()) {
                pinName = inOp->GetParam("pin_name").Get<std::string>();
            } else {
                pinName = inOp->GetName();
            }

            Pin* inPin = GetInputPin(pinName);
            if (inPin && inOp->GetOutputDataPin()) {
                inOp->GetOutputDataPin()->SetValue(inPin->GetValue());
            }
        }
    }

    // 2. Evaluate inner subnetwork graph
    bool success = m_innerGraph->CookAll(context);

    // 3. Forward inner OutOp values to parent container outputs
    for (const auto& [nodeId, node] : m_innerGraph->GetNodes()) {
        if (auto* outOp = dynamic_cast<OutOp*>(node.get())) {
            std::string pinName = "out1";
            if (outOp->GetParam("pin_name").Is<std::string>()) {
                pinName = outOp->GetParam("pin_name").Get<std::string>();
            } else {
                pinName = outOp->GetName();
            }

            Pin* outPin = GetOutputPin(pinName);
            if (outPin && outOp->GetInputDataPin()) {
                outPin->SetValue(outOp->GetInputDataPin()->GetValue());
            }
        }
    }

    return success;
}

} // namespace nf
