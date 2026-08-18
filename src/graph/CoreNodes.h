#pragma once

#include "NodeRegistry.h"

namespace nf {

/// Registers all built-in core operator types into the given registry.
void RegisterCoreNodes(NodeRegistry& registry);

} // namespace nf
