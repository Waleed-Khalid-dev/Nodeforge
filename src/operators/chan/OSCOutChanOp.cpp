#include "OSCOutChanOp.h"
#include <cmath>
#include <vector>
#include <cstring>
#include <algorithm>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
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
#define SOCKET int
#define INVALID_SOCKET -1
#define closesocket close
#endif

namespace nf {

static uint32_t Swap32(uint32_t val) {
    return ((val >> 24) & 0xFF) |
           ((val >> 8) & 0xFF00) |
           ((val << 8) & 0xFF0000) |
           ((val << 24) & 0xFF000000);
}

OSCOutChanOp::OSCOutChanOp(NodeId id, const std::string& name)
    : ChanOp(id, name, "OSCOutChanOp") {
    AddInputPin("input", PinType::Chan);
    m_outPin = AddOutputPin("output", PinType::Chan);

    SetParam("host", std::string("127.0.0.1"));
    SetParam("port", static_cast<int32_t>(8000));
    SetParam("address_prefix", std::string("/nf"));
    SetParam("bundle_mode", true);
    SetParam("send_on_change_only", false);
    SetParam("active", true);
}

OSCOutChanOp::~OSCOutChanOp() = default;

void OSCOutChanOp::SendOSCMessage(const std::string& host, int port, const std::string& address, float value) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) return;

    sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(static_cast<uint16_t>(port));
    inet_pton(AF_INET, host.empty() ? "127.0.0.1" : host.c_str(), &destAddr.sin_addr);

    // Build OSC packet: [address\0 padded to 4] [typetag ",f\0\0"] [float big-endian]
    std::vector<uint8_t> packet;
    for (char c : address) packet.push_back(static_cast<uint8_t>(c));
    packet.push_back(0);
    while (packet.size() % 4 != 0) packet.push_back(0);

    // Typetag ",f\0\0"
    packet.push_back(',');
    packet.push_back('f');
    packet.push_back(0);
    packet.push_back(0);

    // Float argument
    uint32_t rawFloat;
    std::memcpy(&rawFloat, &value, 4);
    rawFloat = Swap32(rawFloat);
    uint8_t* fPtr = reinterpret_cast<uint8_t*>(&rawFloat);
    packet.insert(packet.end(), fPtr, fPtr + 4);

    sendto(sock, reinterpret_cast<const char*>(packet.data()), static_cast<int>(packet.size()), 0, reinterpret_cast<sockaddr*>(&destAddr), sizeof(destAddr));
    closesocket(sock);
}

bool OSCOutChanOp::Cook(const CookContext& /*context*/) {
    Pin* inPin = GetInputPin("input");
    ChannelBuffer inBuf;
    if (inPin && inPin->IsConnected()) {
        inBuf = inPin->GetValue().Get<ChannelBuffer>();
    }
    SetOutputBuffer(inBuf);

    bool active = true;
    if (GetParam("active").Is<bool>()) {
        active = GetParam("active").Get<bool>();
    }
    if (!active || inBuf.GetChannelCount() == 0) {
        return true;
    }

    std::string host = "127.0.0.1";
    if (GetParam("host").Is<std::string>()) {
        host = GetParam("host").Get<std::string>();
    }

    int32_t port = 8000;
    if (GetParam("port").Is<int32_t>()) {
        port = GetParam("port").Get<int32_t>();
    }

    std::string prefix = "/nf";
    if (GetParam("address_prefix").Is<std::string>()) {
        prefix = GetParam("address_prefix").Get<std::string>();
    }
    if (!prefix.empty() && prefix.back() == '/') prefix.pop_back();

    bool onChangeOnly = false;
    if (GetParam("send_on_change_only").Is<bool>()) {
        onChangeOnly = GetParam("send_on_change_only").Get<bool>();
    }

    for (size_t i = 0; i < inBuf.GetChannelCount(); ++i) {
        const std::string& name = inBuf.GetChannelNames()[i];
        const float* data = inBuf.GetChannelData(i);
        if (!data) continue;
        float currentVal = data[inBuf.GetSampleCount() - 1];
        float prevVal = m_prevValues[name];

        if (!onChangeOnly || std::abs(currentVal - prevVal) > 0.0001f) {
            std::string oscAddress = prefix + "/" + name;
            SendOSCMessage(host, port, oscAddress, currentVal);
            m_prevValues[name] = currentVal;
        }
    }

    return true;
}

} // namespace nf
