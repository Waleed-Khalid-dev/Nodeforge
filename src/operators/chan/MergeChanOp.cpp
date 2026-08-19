#include "MergeChanOp.h"
#include <unordered_set>
#include <algorithm>

namespace nf {

MergeChanOp::MergeChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "MergeChanOp") {
    AddInputPin("input1", PinType::Chan);
    AddInputPin("input2", PinType::Chan);
    AddInputPin("input3", PinType::Chan);
    AddInputPin("input4", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("duplicate_names", static_cast<int32_t>(1)); // 0: Replace, 1: Append Suffix
}

bool MergeChanOp::Cook(const CookContext& /*context*/) {
    int32_t dupMode = GetParam("duplicate_names").Is<int32_t>() ? GetParam("duplicate_names").Get<int32_t>() : 1;

    std::vector<std::string> mergedNames;
    std::vector<const float*> sourcePointers;
    std::vector<size_t> sourceLengths;
    size_t maxSampleCount = 0;
    float sampleRate = 60.0f;
    int64_t startSample = 0;

    std::unordered_set<std::string> existingNames;

    for (size_t pinIdx = 0; pinIdx < 4; ++pinIdx) {
        const ChannelBuffer* inBuf = GetInputBuffer(pinIdx);
        if (!inBuf || inBuf->IsEmpty()) continue;

        if (maxSampleCount == 0) {
            maxSampleCount = inBuf->GetSampleCount();
            sampleRate = inBuf->GetSampleRate();
            startSample = inBuf->GetStartSample();
        } else {
            maxSampleCount = std::max(maxSampleCount, inBuf->GetSampleCount());
        }

        for (size_t c = 0; c < inBuf->GetChannelCount(); ++c) {
            std::string name = inBuf->GetChannelNames()[c];
            if (existingNames.count(name)) {
                if (dupMode == 0) {
                    // Replace existing
                    int existingIdx = -1;
                    for (size_t i = 0; i < mergedNames.size(); ++i) {
                        if (mergedNames[i] == name) {
                            existingIdx = static_cast<int>(i);
                            break;
                        }
                    }
                    if (existingIdx >= 0) {
                        sourcePointers[existingIdx] = inBuf->GetChannelData(c);
                        sourceLengths[existingIdx] = inBuf->GetSampleCount();
                        continue;
                    }
                } else {
                    // Append suffix
                    int suffix = 1;
                    std::string newName = name + "_" + std::to_string(suffix);
                    while (existingNames.count(newName)) {
                        suffix++;
                        newName = name + "_" + std::to_string(suffix);
                    }
                    name = newName;
                }
            }

            existingNames.insert(name);
            mergedNames.push_back(name);
            sourcePointers.push_back(inBuf->GetChannelData(c));
            sourceLengths.push_back(inBuf->GetSampleCount());
        }
    }

    if (mergedNames.empty() || maxSampleCount == 0) {
        SetOutputBuffer(ChannelBuffer{});
        return true;
    }

    ChannelBuffer outBuf(mergedNames, maxSampleCount, sampleRate, startSample);

    for (size_t i = 0; i < mergedNames.size(); ++i) {
        float* dst = outBuf.GetChannelData(i);
        const float* src = sourcePointers[i];
        size_t srcLen = sourceLengths[i];
        if (dst && src) {
            size_t copyCount = std::min(srcLen, maxSampleCount);
            std::memcpy(dst, src, copyCount * sizeof(float));
            // Pad remaining with last sample
            if (copyCount < maxSampleCount && copyCount > 0) {
                float lastVal = src[copyCount - 1];
                std::fill(dst + copyCount, dst + maxSampleCount, lastVal);
            }
        }
    }

    SetOutputBuffer(outBuf);
    return true;
}

} // namespace nf
