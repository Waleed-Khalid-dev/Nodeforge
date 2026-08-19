#include "ChanToDataOp.h"
#include <cstdio>
#include <algorithm>

namespace nf {

ChanToDataOp::ChanToDataOp(NodeId id, const std::string& name)
    : DataOp(id, name, "ChanToDataOp") {
    m_inPin = AddInputPin("input", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Data);

    SetParam("include_index", true);
    SetParam("format", std::string("%.4f"));
    SetParam("max_rows", static_cast<int32_t>(1000));
}

bool ChanToDataOp::Cook(const CookContext& /*context*/) {
    if (!m_inPin || !m_inPin->GetValue().Is<ChannelBuffer>()) {
        SetOutputTable(DataTable{});
        return true;
    }

    const auto& buf = m_inPin->GetValue().Get<ChannelBuffer>();
    if (buf.IsEmpty()) {
        SetOutputTable(DataTable{});
        return true;
    }

    bool includeIndex = GetParam("include_index").Is<bool>() ? GetParam("include_index").Get<bool>() : true;
    std::string formatStr = GetParam("format").Is<std::string>() ? GetParam("format").Get<std::string>() : "%.4f";
    int32_t maxRowsParam = GetParam("max_rows").Is<int32_t>() ? GetParam("max_rows").Get<int32_t>() : 1000;

    std::vector<std::string> headers;
    if (includeIndex) {
        headers.push_back("sample");
    }
    for (const auto& name : buf.GetChannelNames()) {
        headers.push_back(name);
    }

    DataTable outTable(headers, 0);
    size_t numSamples = std::min(buf.GetSampleCount(), static_cast<size_t>(std::max(1, maxRowsParam)));
    char fmtBuf[64];

    for (size_t s = 0; s < numSamples; ++s) {
        std::vector<std::string> rowData;
        if (includeIndex) {
            rowData.push_back(std::to_string(s));
        }
        for (size_t c = 0; c < buf.GetChannelCount(); ++c) {
            float val = buf.GetSample(c, s);
            snprintf(fmtBuf, sizeof(fmtBuf), formatStr.c_str(), val);
            rowData.push_back(std::string(fmtBuf));
        }
        outTable.AppendRow(rowData);
    }

    SetOutputTable(outTable);
    return true;
}

} // namespace nf
