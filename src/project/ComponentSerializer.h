#pragma once

#include "../operators/comp/ContainerComp.h"
#include <string>
#include <glm/glm.hpp>

namespace nf::project {

class ComponentSerializer {
public:
    // Exports a ContainerComp subnetwork to a standalone .nfc file
    static bool ExportComponent(const std::string& filePath, ContainerComp* comp, std::string* outError = nullptr);

    // Imports a .nfc file as a new ContainerComp node inside the targetGraph
    static ContainerComp* ImportComponent(const std::string& filePath, Graph* targetGraph, const glm::vec2& spawnPos, std::string* outError = nullptr);
};

} // namespace nf::project
