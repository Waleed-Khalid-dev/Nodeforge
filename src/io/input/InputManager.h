#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <cstdint>

namespace nf {

struct MouseState {
    float x = 0.5f;           // Normalized 0.0 - 1.0
    float y = 0.5f;           // Normalized 0.0 - 1.0
    float rawPixelX = 0.0f;
    float rawPixelY = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    bool leftButton = false;
    bool rightButton = false;
    bool middleButton = false;
    float wheelX = 0.0f;
    float wheelY = 0.0f;
    bool hover = true;
};

class InputManager {
public:
    static InputManager& Instance();

    InputManager();
    ~InputManager();

    // Called once per frame by the engine
    void Update(float windowWidth = 1920.0f, float windowHeight = 1080.0f);

    // Mouse Inspection
    MouseState GetMouseState(int scope = 0) const; // 0=Active Window, 1=Projector, 2=Global Desktop
    float GetMouseX(bool normalized = true) const;
    float GetMouseY(bool normalized = true) const;
    float GetMouseDeltaX() const;
    float GetMouseDeltaY() const;
    bool GetMouseButton(int button) const; // 0=left, 1=right, 2=middle
    float GetMouseWheelX() const;
    float GetMouseWheelY() const;

    // Keyboard Inspection
    bool IsKeyDown(const std::string& keyName) const;
    bool IsKeyPulse(const std::string& keyName) const;
    bool IsCtrlDown() const;
    bool IsShiftDown() const;
    bool IsAltDown() const;
    bool IsMetaDown() const;

    // Testing simulation
    void InjectMockMouse(float x, float y, bool left = false, bool right = false, bool middle = false, float wheelX = 0.0f, float wheelY = 0.0f);
    void InjectMockKey(const std::string& keyName, bool down);
    void ResetState();

private:
    int KeyNameToVirtualKey(const std::string& keyName) const;

    mutable std::mutex m_mutex;
    MouseState m_mouseState;
    float m_prevX = 0.5f;
    float m_prevY = 0.5f;

    std::unordered_map<std::string, bool> m_keyStates;
    std::unordered_map<std::string, bool> m_prevKeyStates;
    std::unordered_map<std::string, bool> m_keyPulses;
    std::unordered_map<std::string, bool> m_mockKeyOverrides;
    bool m_hasMockMouse = false;
};

} // namespace nf
