#include "OSCInOp.h"
#include <chrono>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

namespace nf {

OSCInOp::OSCInOp(NodeId id, const std::string& name)
    : DataOp(id, name, "OSCInOp") {
    m_outPin = AddOutputPin("output", PinType::Data);

    SetParam("port", static_cast<int32_t>(7000));
    SetParam("active", true);
    SetParam("max_rows", static_cast<int32_t>(100));
    SetParam("clear_on_cook", false);

    m_cachedTable.SetColumnHeaders({ "timestamp", "address", "args" });
}

OSCInOp::~OSCInOp() {
    StopListener();
}

void OSCInOp::StartListener(int port) {
    StopListener();
    m_running = true;
    m_currentPort = port;
    m_listenerThread = std::thread(&OSCInOp::ListenerThreadFunc, this, port);
}

void OSCInOp::StopListener() {
    m_running = false;
    if (m_listenerThread.joinable()) {
        m_listenerThread.join();
    }
}

void OSCInOp::ListenerThreadFunc(int port) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return;
    }
    // Set timeout on receive
    DWORD timeout = 200; // ms
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<u_short>(port));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return;
    }

    char buffer[4096];
    while (m_running) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);
        int bytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &clientLen);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::string rawMsg(buffer, bytes);

            // Basic OSC parse: path followed by args
            std::string address = "/osc";
            std::string args = "";
            size_t nullPos = rawMsg.find('\0');
            if (nullPos != std::string::npos) {
                address = rawMsg.substr(0, nullPos);
                if (nullPos + 1 < rawMsg.size()) {
                    args = rawMsg.substr(nullPos + 1);
                }
            } else {
                address = rawMsg;
            }

            auto now = std::chrono::system_clock::now();
            auto timeT = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&timeT), "%H:%M:%S");

            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_messageQueue.push_back({ ss.str(), address, args });
        }
    }

    closesocket(sock);
    WSACleanup();
#endif
}

bool OSCInOp::Cook(const CookContext& /*context*/) {
    bool active = GetParam("active").Is<bool>() ? GetParam("active").Get<bool>() : true;
    int32_t port = GetParam("port").Is<int32_t>() ? GetParam("port").Get<int32_t>() : 7000;
    int32_t maxRows = GetParam("max_rows").Is<int32_t>() ? GetParam("max_rows").Get<int32_t>() : 100;
    bool clearOnCook = GetParam("clear_on_cook").Is<bool>() ? GetParam("clear_on_cook").Get<bool>() : false;

    if (active && (!m_running || m_currentPort != port)) {
        StartListener(port);
    } else if (!active && m_running) {
        StopListener();
    }

    if (clearOnCook) {
        m_cachedTable.Clear();
        m_cachedTable.SetColumnHeaders({ "timestamp", "address", "args" });
    }

    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        while (!m_messageQueue.empty()) {
            const auto& msg = m_messageQueue.front();
            m_cachedTable.AppendRow({ msg.timestamp, msg.address, msg.args });
            m_messageQueue.pop_front();
        }
    }

    while (static_cast<int32_t>(m_cachedTable.GetRowCount()) > maxRows) {
        m_cachedTable.DeleteRow(0);
    }

    SetOutputTable(m_cachedTable);
    return true;
}

} // namespace nf
