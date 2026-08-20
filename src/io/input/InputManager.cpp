#include "InputManager.h"
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace nf {

static std::string ToLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

InputManager& InputManager::Instance() {
    static InputManager s_instance;
    return s_instance;
}

InputManager::InputManager() {
    ResetState();
}

InputManager::~InputManager() = default;

void InputManager::ResetState() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_mouseState = MouseState{};
    m_prevX = 0.5f;
    m_prevY = 0.5f;
    m_keyStates.clear();
    m_prevKeyStates.clear();
    m_keyPulses.clear();
    m_mockKeyOverrides.clear();
    m_hasMockMouse = false;
}

int InputManager::KeyNameToVirtualKey(const std::string& keyName) const {
    std::string k = ToLower(keyName);
#ifdef _WIN32
    if (k == "space") return VK_SPACE;
    if (k == "enter" || k == "return") return VK_RETURN;
    if (k == "escape" || k == "esc") return VK_ESCAPE;
    if (k == "tab") return VK_TAB;
    if (k == "backspace") return VK_BACK;
    if (k == "left") return VK_LEFT;
    if (k == "right") return VK_RIGHT;
    if (k == "up") return VK_UP;
    if (k == "down") return VK_DOWN;
    if (k == "ctrl" || k == "control") return VK_CONTROL;
    if (k == "shift") return VK_SHIFT;
    if (k == "alt" || k == "menu") return VK_MENU;
    if (k == "meta" || k == "super" || k == "win") return VK_LWIN;
    if (k.size() == 1) {
        char c = k[0];
        if (c >= 'a' && c <= 'z') return 'A' + (c - 'a');
        if (c >= '0' && c <= '9') return '0' + (c - '0');
    }
    if (k.rfind("f", 0) == 0 && k.size() > 1) {
        int fNum = std::atoi(k.c_str() + 1);
        if (fNum >= 1 && fNum <= 12) return VK_F1 + (fNum - 1);
    }
#endif
    return 0;
}

void InputManager::Update(float windowWidth, float windowHeight) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Update mouse state
    if (!m_hasMockMouse) {
#ifdef _WIN32
        POINT pt;
        if (GetCursorPos(&pt)) {
            HWND hWnd = GetForegroundWindow();
            if (hWnd) {
                POINT clientPt = pt;
                ScreenToClient(hWnd, &clientPt);
                RECT rc;
                GetClientRect(hWnd, &rc);
                float w = static_cast<float>(rc.right - rc.left);
                float h = static_cast<float>(rc.bottom - rc.top);
                if (w > 0 && h > 0) {
                    m_mouseState.rawPixelX = static_cast<float>(clientPt.x);
                    m_mouseState.rawPixelY = static_cast<float>(clientPt.y);
                    m_mouseState.x = std::clamp(m_mouseState.rawPixelX / w, 0.0f, 1.0f);
                    m_mouseState.y = std::clamp(m_mouseState.rawPixelY / h, 0.0f, 1.0f);
                    m_mouseState.hover = (clientPt.x >= 0 && clientPt.x <= rc.right && clientPt.y >= 0 && clientPt.y <= rc.bottom);
                }
            } else {
                m_mouseState.rawPixelX = static_cast<float>(pt.x);
                m_mouseState.rawPixelY = static_cast<float>(pt.y);
                m_mouseState.x = std::clamp(m_mouseState.rawPixelX / std::max(windowWidth, 1.0f), 0.0f, 1.0f);
                m_mouseState.y = std::clamp(m_mouseState.rawPixelY / std::max(windowHeight, 1.0f), 0.0f, 1.0f);
            }
            m_mouseState.leftButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
            m_mouseState.rightButton = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
            m_mouseState.middleButton = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
        }
#endif
    }

    m_mouseState.dx = m_mouseState.x - m_prevX;
    m_mouseState.dy = m_mouseState.y - m_prevY;
    m_prevX = m_mouseState.x;
    m_prevY = m_mouseState.y;

    // Update keys
    m_prevKeyStates = m_keyStates;
    for (const auto& [name, mockVal] : m_mockKeyOverrides) {
        m_keyStates[name] = mockVal;
    }

    // Check common queried keys
    for (auto& [keyName, isDown] : m_keyStates) {
        if (m_mockKeyOverrides.find(keyName) == m_mockKeyOverrides.end()) {
#ifdef _WIN32
            int vk = KeyNameToVirtualKey(keyName);
            if (vk != 0) {
                isDown = (GetAsyncKeyState(vk) & 0x8000) != 0;
            }
#endif
        }
        bool prev = m_prevKeyStates[keyName];
        m_keyPulses[keyName] = (isDown && !prev);
    }
}

MouseState InputManager::GetMouseState([[maybe_unused]] int scope) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_mouseState;
}

float InputManager::GetMouseX(bool normalized) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return normalized ? m_mouseState.x : m_mouseState.rawPixelX;
}

float InputManager::GetMouseY(bool normalized) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return normalized ? m_mouseState.y : m_mouseState.rawPixelY;
}

float InputManager::GetMouseDeltaX() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_mouseState.dx;
}

float InputManager::GetMouseDeltaY() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_mouseState.dy;
}

bool InputManager::GetMouseButton(int button) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (button == 0) return m_mouseState.leftButton;
    if (button == 1) return m_mouseState.rightButton;
    if (button == 2) return m_mouseState.middleButton;
    return false;
}

float InputManager::GetMouseWheelX() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_mouseState.wheelX;
}

float InputManager::GetMouseWheelY() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_mouseState.wheelY;
}

bool InputManager::IsKeyDown(const std::string& keyName) const {
    std::string k = ToLower(keyName);
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_keyStates.find(k);
    if (it != m_keyStates.end()) return it->second;

#ifdef _WIN32
    int vk = KeyNameToVirtualKey(k);
    if (vk != 0) {
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    }
#endif
    return false;
}

bool InputManager::IsKeyPulse(const std::string& keyName) const {
    std::string k = ToLower(keyName);
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_keyPulses.find(k);
    if (it != m_keyPulses.end()) return it->second;
    return false;
}

bool InputManager::IsCtrlDown() const {
    return IsKeyDown("ctrl");
}

bool InputManager::IsShiftDown() const {
    return IsKeyDown("shift");
}

bool InputManager::IsAltDown() const {
    return IsKeyDown("alt");
}

bool InputManager::IsMetaDown() const {
    return IsKeyDown("meta");
}

void InputManager::InjectMockMouse(float x, float y, bool left, bool right, bool middle, float wheelX, float wheelY) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_hasMockMouse = true;
    m_mouseState.x = x;
    m_mouseState.y = y;
    m_mouseState.rawPixelX = x * 1920.0f;
    m_mouseState.rawPixelY = y * 1080.0f;
    m_mouseState.leftButton = left;
    m_mouseState.rightButton = right;
    m_mouseState.middleButton = middle;
    m_mouseState.wheelX = wheelX;
    m_mouseState.wheelY = wheelY;
    m_mouseState.hover = true;
}

void InputManager::InjectMockKey(const std::string& keyName, bool down) {
    std::string k = ToLower(keyName);
    std::lock_guard<std::mutex> lock(m_mutex);
    bool prev = m_keyStates[k];
    m_mockKeyOverrides[k] = down;
    m_keyStates[k] = down;
    m_keyPulses[k] = (down && !prev);
}

} // namespace nf
