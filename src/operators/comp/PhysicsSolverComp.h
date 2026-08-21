#pragma once

#include "../../graph/Node.h"
#include "../../core/ChannelBuffer.h"
#include "../../physics/PhysicsWorld.h"

namespace nf {

class PhysicsSolverComp : public Node {
public:
    PhysicsSolverComp(NodeId id, const std::string& name);
    ~PhysicsSolverComp() override = default;

    bool Cook(const CookContext& context) override;

    PhysicsWorld& GetWorld() { return m_world; }
    const PhysicsWorld& GetWorld() const { return m_world; }

private:
    Pin* m_inBodiesPin = nullptr;
    Pin* m_inForcesPin = nullptr;
    Pin* m_outCompPin = nullptr;
    Pin* m_outEventsPin = nullptr;
    PhysicsWorld m_world;
    ChannelBuffer m_eventsBuffer;
    bool m_initialized = false;
};

} // namespace nf
