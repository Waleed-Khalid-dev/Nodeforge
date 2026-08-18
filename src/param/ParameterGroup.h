#pragma once

#include "Parameter.h"
#include <vector>
#include <unordered_map>
#include <memory>

namespace nf {

class Node;

class ParameterGroup {
public:
    ParameterGroup(Node* owner = nullptr);

    void SetOwner(Node* owner);

    Parameter* Add(const ParamMetadata& meta);

    // Fluent helper creators
    Parameter* AddFloat(const std::string& name, const std::string& label, float defaultVal, float minVal = 0.0f, float maxVal = 1.0f, bool hasLimits = false, const std::string& page = "General");
    Parameter* AddInt(const std::string& name, const std::string& label, int32_t defaultVal, int32_t minVal = 0, int32_t maxVal = 100, bool hasLimits = false, const std::string& page = "General");
    Parameter* AddBool(const std::string& name, const std::string& label, bool defaultVal, const std::string& page = "General");
    Parameter* AddString(const std::string& name, const std::string& label, const std::string& defaultVal, const std::string& page = "General");
    Parameter* AddVec2(const std::string& name, const std::string& label, const glm::vec2& defaultVal, const std::string& page = "General");
    Parameter* AddVec3(const std::string& name, const std::string& label, const glm::vec3& defaultVal, const std::string& page = "General");
    Parameter* AddVec4(const std::string& name, const std::string& label, const glm::vec4& defaultVal, const std::string& page = "General");
    Parameter* AddMenu(const std::string& name, const std::string& label, int32_t defaultIndex, const std::vector<std::string>& items, const std::string& page = "General");

    Parameter* Get(const std::string& name) const;
    bool Has(const std::string& name) const;

    const std::vector<std::unique_ptr<Parameter>>& GetAll() const { return m_parameters; }
    std::vector<std::string> GetPages() const;
    std::vector<Parameter*> GetParametersByPage(const std::string& page) const;

    void Clear();

private:
    Node* m_ownerNode = nullptr;
    std::vector<std::unique_ptr<Parameter>> m_parameters;
    std::unordered_map<std::string, Parameter*> m_nameMap;
};

} // namespace nf
