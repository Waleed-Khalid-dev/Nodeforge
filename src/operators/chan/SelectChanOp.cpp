#include "SelectChanOp.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace nf {

SelectChanOp::SelectChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "SelectChanOp") {
    AddInputPin("input", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("channel_names", std::string("*"));
    SetParam("rename_to", std::string(""));
}

static bool MatchPattern(const std::string& pattern, const std::string& text) {
    if (pattern == "*" || pattern == text) return true;

    // Convert wildcard pattern to regex
    std::string regexStr;
    for (char c : pattern) {
        if (c == '*') regexStr += ".*";
        else if (c == '?') regexStr += ".";
        else if (c == '.' || c == '[' || c == ']' || c == '(' || c == ')' || c == '+' || c == '^' || c == '$') {
            regexStr += '\\';
            regexStr += c;
        } else {
            regexStr += c;
        }
    }
    try {
        std::regex re(regexStr, std::regex::icase);
        return std::regex_match(text, re);
    } catch (...) {
        return false;
    }
}

bool SelectChanOp::Cook(const CookContext& /*context*/) {
    const ChannelBuffer* inBuf = GetInputBuffer(0);
    if (!inBuf || inBuf->IsEmpty()) {
        SetOutputBuffer(ChannelBuffer{});
        return true;
    }

    std::string patternStr = GetParam("channel_names").Is<std::string>() ? GetParam("channel_names").Get<std::string>() : "*";
    std::string renameStr = GetParam("rename_to").Is<std::string>() ? GetParam("rename_to").Get<std::string>() : "";

    std::vector<std::string> patterns;
    std::stringstream pss(patternStr);
    std::string pitem;
    while (pss >> pitem) {
        patterns.push_back(pitem);
    }
    if (patterns.empty()) patterns.push_back("*");

    std::vector<std::string> renameList;
    std::stringstream rss(renameStr);
    std::string ritem;
    while (rss >> ritem) {
        renameList.push_back(ritem);
    }

    std::vector<std::string> selectedNames;
    std::vector<size_t> selectedIndices;

    for (size_t c = 0; c < inBuf->GetChannelCount(); ++c) {
        const std::string& chanName = inBuf->GetChannelNames()[c];
        for (const auto& pat : patterns) {
            if (MatchPattern(pat, chanName)) {
                selectedIndices.push_back(c);
                size_t outIdx = selectedNames.size();
                std::string finalName = (outIdx < renameList.size()) ? renameList[outIdx] : chanName;
                selectedNames.push_back(finalName);
                break;
            }
        }
    }

    if (selectedNames.empty()) {
        SetOutputBuffer(ChannelBuffer{});
        return true;
    }

    size_t sampleCount = inBuf->GetSampleCount();
    ChannelBuffer outBuf(selectedNames, sampleCount, inBuf->GetSampleRate(), inBuf->GetStartSample());

    for (size_t i = 0; i < selectedIndices.size(); ++i) {
        const float* src = inBuf->GetChannelData(selectedIndices[i]);
        float* dst = outBuf.GetChannelData(i);
        if (src && dst) {
            std::memcpy(dst, src, sampleCount * sizeof(float));
        }
    }

    SetOutputBuffer(outBuf);
    return true;
}

} // namespace nf
