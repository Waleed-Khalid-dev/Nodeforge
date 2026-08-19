#pragma once

#include "DataOp.h"
#include <thread>
#include <atomic>
#include <mutex>

namespace nf {

class WebDataOp : public DataOp {
public:
    WebDataOp(NodeId id, const std::string& name);
    ~WebDataOp() override;

    bool Cook(const CookContext& context) override;
    void FetchAsync();

private:
    void WorkerFunc(std::string url, int method, std::string body, std::string headers);

    std::thread m_workerThread;
    std::atomic<bool> m_isFetching{false};
    std::atomic<bool> m_hasNewResponse{false};

    std::mutex m_responseMutex;
    int m_responseStatus = 0;
    std::string m_responseBody;

    std::string m_lastFetchedUrl;
};

} // namespace nf
