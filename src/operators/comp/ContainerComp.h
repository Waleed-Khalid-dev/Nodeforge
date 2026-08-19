#pragma once

#include "../../graph/Node.h"
#include "../../graph/Graph.h"
#include <memory>
#include <vector>
#include <string>

namespace nf {

class ContainerComp : public Node {
public:
    ContainerComp(NodeId id, const std::string& name);
    ~ContainerComp() override = default;

    Graph* GetInnerGraph() const { return m_innerGraph.get(); }
    void SetInnerGraph(std::unique_ptr<Graph> graph);

    // Synchronizes boundary input/output pins on this container from InOp/OutOp inside the inner graph
    void SynchronizeBoundaryPins();

    // Cook propagates data across the boundary into and out of the subnetwork
    bool Cook(const CookContext& context) override;

private:
    std::unique_ptr<Graph> m_innerGraph;
};

} // namespace nf
