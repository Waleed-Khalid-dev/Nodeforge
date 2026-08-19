#pragma once

#include "commands/ICommand.h"
#include <vector>
#include <memory>

namespace nf::ui {

class UndoManager {
public:
    explicit UndoManager(size_t maxHistory = 100);

    void ExecuteCommand(std::unique_ptr<ICommand> command);
    bool Undo();
    bool Redo();
    bool CanUndo() const;
    bool CanRedo() const;
    void Clear();

    size_t GetUndoCount() const { return m_undoStack.size(); }
    size_t GetRedoCount() const { return m_redoStack.size(); }

private:
    size_t m_maxHistory;
    std::vector<std::unique_ptr<ICommand>> m_undoStack;
    std::vector<std::unique_ptr<ICommand>> m_redoStack;
};

} // namespace nf::ui
