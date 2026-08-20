#pragma once

#include <string>

namespace nf {

class Graph;

namespace ui {

class EditorContext;

class ProfilerPanel {
public:
    explicit ProfilerPanel(EditorContext* ctx = nullptr);
    ~ProfilerPanel() = default;

    void Render(bool* pOpen = nullptr);

    bool IsOpen() const { return m_isOpen; }
    void SetOpen(bool open) { m_isOpen = open; }

private:
    EditorContext* m_ctx{nullptr};
    bool m_isOpen{false};
    char m_searchFilter[128]{""};
    int m_sortColumn{2}; // 0=Name, 1=Type, 2=LastCpuMs, 3=AvgCpuMs, 4=TotalCooks
    bool m_sortAscending{false};
};

} // namespace ui
} // namespace nf
