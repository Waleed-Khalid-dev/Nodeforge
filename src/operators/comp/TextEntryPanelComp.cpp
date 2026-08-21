#include "TextEntryPanelComp.h"

namespace nf {

TextEntryPanelComp::TextEntryPanelComp(NodeId id, const std::string& name)
    : Node(id, name, "TextEntryPanelComp") {
    m_inTextPin = AddInputPin("in_text", PinType::Data);
    m_outCompPin = AddOutputPin("output", PinType::Any);
    m_outDataPin = AddOutputPin("out_data", PinType::Data);

    SetParam("label", std::string("Text Input"));
    SetParam("text", std::string(""));
    SetParam("read_only", false);

    m_outTable.Resize(1, 1);
    m_outTable.SetCell(0, 0, "");
}

const std::string& TextEntryPanelComp::GetText() const {
    static const std::string empty;
    return GetParam("text").Is<std::string>() ? GetParam("text").Get<std::string>() : empty;
}

void TextEntryPanelComp::SetText(const std::string& text) {
    SetParam("text", text);
}

bool TextEntryPanelComp::Cook(const CookContext& /*context*/) {
    if (m_inTextPin && m_inTextPin->GetValue().Is<DataTable>()) {
        const auto& inT = m_inTextPin->GetValue().Get<DataTable>();
        if (inT.GetRowCount() > 0 && inT.GetColumnCount() > 0) {
            SetParam("text", inT.GetCell(0, 0));
        }
    }

    const std::string& txt = GetText();
    m_outTable.Resize(1, 1);
    m_outTable.SetCell(0, 0, txt);

    m_outDataPin->SetValue(PinValue(m_outTable));
    return true;
}

} // namespace nf
