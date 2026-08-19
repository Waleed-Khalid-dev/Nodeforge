#pragma once

#include "../EditorContext.h"
#include <string>
#include <vector>

namespace nf::ui {

struct LogEntry {
    enum class Level { Info, Warn, Error };
    Level level;
    std::string text;
};

class ConsolePanel {
public:
    explicit ConsolePanel(EditorContext* ctx);
    void Render();

    void AddLog(LogEntry::Level level, const std::string& msg);
    void Clear();

private:
    EditorContext* m_ctx;
    std::vector<LogEntry> m_logs;
    char m_inputBuffer[512]{ 0 };
    std::vector<std::string> m_history;
    int m_historyPos = -1;
    bool m_autoScroll = true;
};

} // namespace nf::ui
