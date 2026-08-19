#include "DataToChanOp.h"
#include "../../core/DataTable.h"

namespace nf {

DataToChanOp::DataToChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "DataToChanOp") {
    m_inPin = AddInputPin("input", PinType::Data);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("layout", static_cast<int32_t>(0)); // 0: Columns are channels, 1: Rows are channels
    SetParam("sample_rate", 60.0f);
}

bool DataToChanOp::Cook(const CookContext& /*context*/) {
    if (!m_inPin || !m_inPin->GetValue().Is<DataTable>()) {
        SetOutputBuffer(ChannelBuffer{});
        return true;
    }

    const auto& table = m_inPin->GetValue().Get<DataTable>();
    if (table.IsEmpty()) {
        SetOutputBuffer(ChannelBuffer{});
        return true;
    }

    float rate = GetParam("sample_rate").Is<float>() ? GetParam("sample_rate").Get<float>() : 60.0f;
    int32_t layout = GetParam("layout").Is<int32_t>() ? GetParam("layout").Get<int32_t>() : 0;

    if (layout == 0) {
        // Columns are channels
        std::vector<std::string> names = table.GetColumnHeaders();
        size_t sampleCount = table.GetRowCount();
        ChannelBuffer buf(names, sampleCount, rate);

        for (size_t c = 0; c < table.GetColumnCount(); ++c) {
            float* chanData = buf.GetChannelData(c);
            if (chanData) {
                for (size_t r = 0; r < sampleCount; ++r) {
                    chanData[r] = table.GetCellFloat(r, c, 0.0f);
                }
            }
        }
        SetOutputBuffer(buf);
    } else {
        // Rows are channels
        size_t chanCount = table.GetRowCount();
        size_t sampleCount = table.GetColumnCount();
        std::vector<std::string> names;
        for (size_t r = 0; r < chanCount; ++r) {
            names.push_back("chan" + std::to_string(r + 1));
        }
        ChannelBuffer buf(names, sampleCount, rate);

        for (size_t r = 0; r < chanCount; ++r) {
            float* chanData = buf.GetChannelData(r);
            if (chanData) {
                for (size_t c = 0; c < sampleCount; ++c) {
                    chanData[c] = table.GetCellFloat(r, c, 0.0f);
                }
            }
        }
        SetOutputBuffer(buf);
    }

    return true;
}

} // namespace nf
