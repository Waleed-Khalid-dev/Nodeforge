#pragma once

#include "ChanOp.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>

namespace nf {

class OSCInChanOp : public ChanOp {
public:
    OSCInChanOp(NodeId id, const std::string& name);
    ~OSCInChanOp() override;

    bool Cook(const CookContext& context) override;

    void InjectMockOSC(const std::string& address, const std::vector<float>& args);

private:
    void StartListener(int port);
    void StopListener();
    void ListenerThreadFunc(int port);
    void ProcessRawPacket(const uint8_t* data, size_t size);

    std::thread m_listenerThread;
    std::atomic<bool> m_running{false};
    std::atomic<int> m_currentPort{0};

    mutable std::mutex m_valueMutex;
    std::unordered_map<std::string, float> m_channelValues;
};

} // namespace nf
