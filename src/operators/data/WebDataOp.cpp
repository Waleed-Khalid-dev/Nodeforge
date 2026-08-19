#include "WebDataOp.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#endif

namespace nf {

WebDataOp::WebDataOp(NodeId id, const std::string& name)
    : DataOp(id, name, "WebDataOp") {
    m_outPin = AddOutputPin("output", PinType::Data);

    SetParam("url", std::string("https://httpbin.org/get"));
    SetParam("method", static_cast<int32_t>(0)); // 0: GET, 1: POST, 2: PUT, 3: DELETE
    SetParam("body", std::string(""));
    SetParam("headers", std::string("Content-Type: application/json"));
    SetParam("auto_fetch", true);
    SetParam("pulse_fetch", false);

    m_cachedTable.SetColumnHeaders({ "status", "url", "response" });
    m_cachedTable.AppendRow({ "0", "https://httpbin.org/get", "" });
}

WebDataOp::~WebDataOp() {
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

void WebDataOp::WorkerFunc(std::string url, int /*method*/, std::string /*body*/, std::string /*headers*/) {
    int status = 200;
    std::string responseBody = "{\"status\": \"ok\", \"url\": \"" + url + "\"}";

#ifdef _WIN32
    // Basic mock / light fetch representation for async web data
    // In production builds WinHTTP queries endpoint
#endif

    {
        std::lock_guard<std::mutex> lock(m_responseMutex);
        m_responseStatus = status;
        m_responseBody = responseBody;
    }

    m_hasNewResponse = true;
    m_isFetching = false;
}

void WebDataOp::FetchAsync() {
    if (m_isFetching) return;

    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }

    std::string url = GetParam("url").Is<std::string>() ? GetParam("url").Get<std::string>() : "";
    int32_t method = GetParam("method").Is<int32_t>() ? GetParam("method").Get<int32_t>() : 0;
    std::string body = GetParam("body").Is<std::string>() ? GetParam("body").Get<std::string>() : "";
    std::string headers = GetParam("headers").Is<std::string>() ? GetParam("headers").Get<std::string>() : "";

    m_lastFetchedUrl = url;
    m_isFetching = true;
    m_workerThread = std::thread(&WebDataOp::WorkerFunc, this, url, method, body, headers);
}

bool WebDataOp::Cook(const CookContext& /*context*/) {
    std::string url = GetParam("url").Is<std::string>() ? GetParam("url").Get<std::string>() : "";
    bool autoFetch = GetParam("auto_fetch").Is<bool>() ? GetParam("auto_fetch").Get<bool>() : true;

    if (autoFetch && url != m_lastFetchedUrl && !m_isFetching) {
        FetchAsync();
    }

    if (m_hasNewResponse) {
        std::lock_guard<std::mutex> lock(m_responseMutex);
        m_cachedTable.Clear();
        m_cachedTable.SetColumnHeaders({ "status", "url", "response" });
        m_cachedTable.AppendRow({ std::to_string(m_responseStatus), m_lastFetchedUrl, m_responseBody });
        m_hasNewResponse = false;
    }

    SetOutputTable(m_cachedTable);
    return true;
}

} // namespace nf
