#include "OSCOutOp.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace nf {

OSCOutOp::OSCOutOp(NodeId id, const std::string& name)
    : DataOp(id, name, "OSCOutOp") {
    m_inPin = AddInputPin("input", PinType::Data);
    m_outPin = AddOutputPin("output", PinType::Data);

    SetParam("ip_address", std::string("127.0.0.1"));
    SetParam("port", static_cast<int32_t>(7001));
    SetParam("address", std::string("/nodeforge/trigger"));
    SetParam("value", 1.0f);
    SetParam("send_on_cook", false);
    SetParam("pulse_send", false);
}

void OSCOutOp::SendPacket(const std::string& address, const std::string& payload) {
    std::string ip = GetParam("ip_address").Is<std::string>() ? GetParam("ip_address").Get<std::string>() : "127.0.0.1";
    int32_t port = GetParam("port").Is<int32_t>() ? GetParam("port").Get<int32_t>() : 7001;

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock != INVALID_SOCKET) {
        sockaddr_in destAddr{};
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(static_cast<u_short>(port));
        inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr);

        std::string packet = address + '\0' + payload;
        sendto(sock, packet.c_str(), static_cast<int>(packet.size()), 0, (sockaddr*)&destAddr, sizeof(destAddr));
        closesocket(sock);
    }
    WSACleanup();
#endif
}

bool OSCOutOp::Cook(const CookContext& /*context*/) {
    const DataTable* inTable = GetInputTable(0);
    bool sendOnCook = GetParam("send_on_cook").Is<bool>() ? GetParam("send_on_cook").Get<bool>() : false;

    if (sendOnCook) {
        std::string address = GetParam("address").Is<std::string>() ? GetParam("address").Get<std::string>() : "/trigger";
        float val = GetParam("value").Is<float>() ? GetParam("value").Get<float>() : 1.0f;
        SendPacket(address, std::to_string(val));
    }

    SetOutputTable(inTable ? *inTable : DataTable{});
    return true;
}

} // namespace nf
