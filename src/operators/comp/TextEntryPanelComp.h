#pragma once

#include "../../graph/Node.h"
#include "../../core/DataTable.h"

namespace nf {

class TextEntryPanelComp : public Node {
public:
    TextEntryPanelComp(NodeId id, const std::string& name);
    ~TextEntryPanelComp() override = default;

    bool Cook(const CookContext& context) override;

    const std::string& GetText() const;
    void SetText(const std::string& text);

private:
    Pin* m_inTextPin = nullptr;
    Pin* m_outCompPin = nullptr;
    Pin* m_outDataPin = nullptr;
    DataTable m_outTable;
};

} // namespace nf
