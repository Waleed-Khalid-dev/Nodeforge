#include "ParameterGroup.h"
#include <algorithm>

namespace nf {

ParameterGroup::ParameterGroup(Node* owner)
    : m_ownerNode(owner) {
}

void ParameterGroup::SetOwner(Node* owner) {
    m_ownerNode = owner;
    for (auto& param : m_parameters) {
        param->SetOwnerNode(owner);
    }
}

Parameter* ParameterGroup::Add(const ParamMetadata& meta) {
    auto param = std::make_unique<Parameter>(meta, m_ownerNode);
    Parameter* ptr = param.get();
    m_nameMap[meta.name] = ptr;
    m_parameters.push_back(std::move(param));
    return ptr;
}

Parameter* ParameterGroup::AddFloat(const std::string& name, const std::string& label, float defaultVal, float minVal, float maxVal, bool hasLimits, const std::string& page) {
    ParamMetadata meta;
    meta.name = name;
    meta.label = label;
    meta.page = page;
    meta.type = ParamType::Float;
    meta.defaultValue = PinValue(defaultVal);
    meta.minLimit = PinValue(minVal);
    meta.maxLimit = PinValue(maxVal);
    meta.hasLimits = hasLimits;
    return Add(meta);
}

Parameter* ParameterGroup::AddInt(const std::string& name, const std::string& label, int32_t defaultVal, int32_t minVal, int32_t maxVal, bool hasLimits, const std::string& page) {
    ParamMetadata meta;
    meta.name = name;
    meta.label = label;
    meta.page = page;
    meta.type = ParamType::Int;
    meta.defaultValue = PinValue(defaultVal);
    meta.minLimit = PinValue(minVal);
    meta.maxLimit = PinValue(maxVal);
    meta.hasLimits = hasLimits;
    return Add(meta);
}

Parameter* ParameterGroup::AddBool(const std::string& name, const std::string& label, bool defaultVal, const std::string& page) {
    ParamMetadata meta;
    meta.name = name;
    meta.label = label;
    meta.page = page;
    meta.type = ParamType::Bool;
    meta.defaultValue = PinValue(defaultVal);
    return Add(meta);
}

Parameter* ParameterGroup::AddString(const std::string& name, const std::string& label, const std::string& defaultVal, const std::string& page) {
    ParamMetadata meta;
    meta.name = name;
    meta.label = label;
    meta.page = page;
    meta.type = ParamType::String;
    meta.defaultValue = PinValue(defaultVal);
    return Add(meta);
}

Parameter* ParameterGroup::AddVec2(const std::string& name, const std::string& label, const glm::vec2& defaultVal, const std::string& page) {
    ParamMetadata meta;
    meta.name = name;
    meta.label = label;
    meta.page = page;
    meta.type = ParamType::Vec2;
    meta.defaultValue = PinValue(defaultVal);
    return Add(meta);
}

Parameter* ParameterGroup::AddVec3(const std::string& name, const std::string& label, const glm::vec3& defaultVal, const std::string& page) {
    ParamMetadata meta;
    meta.name = name;
    meta.label = label;
    meta.page = page;
    meta.type = ParamType::Vec3;
    meta.defaultValue = PinValue(defaultVal);
    return Add(meta);
}

Parameter* ParameterGroup::AddVec4(const std::string& name, const std::string& label, const glm::vec4& defaultVal, const std::string& page) {
    ParamMetadata meta;
    meta.name = name;
    meta.label = label;
    meta.page = page;
    meta.type = ParamType::Vec4;
    meta.defaultValue = PinValue(defaultVal);
    return Add(meta);
}

Parameter* ParameterGroup::AddMenu(const std::string& name, const std::string& label, int32_t defaultIndex, const std::vector<std::string>& items, const std::string& page) {
    ParamMetadata meta;
    meta.name = name;
    meta.label = label;
    meta.page = page;
    meta.type = ParamType::Menu;
    meta.defaultValue = PinValue(defaultIndex);
    meta.menuItems = items;
    return Add(meta);
}

Parameter* ParameterGroup::Get(const std::string& name) const {
    auto it = m_nameMap.find(name);
    if (it != m_nameMap.end()) return it->second;
    return nullptr;
}

bool ParameterGroup::Has(const std::string& name) const {
    return m_nameMap.find(name) != m_nameMap.end();
}

std::vector<std::string> ParameterGroup::GetPages() const {
    std::vector<std::string> pages;
    for (const auto& param : m_parameters) {
        if (std::find(pages.begin(), pages.end(), param->GetPage()) == pages.end()) {
            pages.push_back(param->GetPage());
        }
    }
    return pages;
}

std::vector<Parameter*> ParameterGroup::GetParametersByPage(const std::string& page) const {
    std::vector<Parameter*> result;
    for (const auto& param : m_parameters) {
        if (param->GetPage() == page) {
            result.push_back(param.get());
        }
    }
    return result;
}

void ParameterGroup::Clear() {
    m_nameMap.clear();
    m_parameters.clear();
}

} // namespace nf
