#pragma once

namespace nf::ui {

class EditorContext;

class PerformanceHUD {
public:
    explicit PerformanceHUD(EditorContext* ctx = nullptr);
    ~PerformanceHUD() = default;

    void Render(bool* pOpen = nullptr);

    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }
    void Toggle() { m_visible = !m_visible; }

private:
    EditorContext* m_ctx{nullptr};
    bool m_visible{false};
};

} // namespace nf::ui
