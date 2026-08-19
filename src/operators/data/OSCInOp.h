#pragma once

#include "DataOp.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <deque>

namespace nf {

struct OSCMessage {
    std::string timestamp;
    std::string address;
    std::string args;
};

class OSCInOp : public DataOp {
public:
    OSCInOp(NodeId id, const std::string& name);
    ~OSCInOp() override;

    bool Cook(const CookContext& context) override;

private:
    void StartListener(int port);
    void StopListener();
    void ListenerThreadFunc(int port);

    std::thread m_listenerThread;
    std::atomic<bool> m_running{false};
    std::atomic<int> m_currentPort{0};

    std::mutex m_queueMutex;
    std::deque<OSCMessage> m_messageQueue;
};

} // namespace nf
