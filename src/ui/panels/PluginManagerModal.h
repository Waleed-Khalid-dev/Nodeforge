#pragma once

namespace nf::ui {

class EditorContext;

class PluginManagerModal {
public:
    explicit PluginManagerModal(EditorContext* ctx = nullptr);
    ~PluginManagerModal() = default;

    void Render(bool* pOpen = nullptr);

    bool IsOpen() const { return m_isOpen; }
    void SetOpen(bool open) { m_isOpen = open; }

private:
    EditorContext* m_ctx{nullptr};
    bool m_isOpen{false};
    char m_newSearchPathBuffer[256]{""};
    char m_statusMessage[256]{""};
};

} // namespace nf::ui
