#include "UndoManager.h"

namespace nf::ui {

UndoManager::UndoManager(size_t maxHistory)
    : m_maxHistory(maxHistory) {
}

void UndoManager::ExecuteCommand(std::unique_ptr<ICommand> command) {
    if (!command) return;

    command->Execute();

    // Check if we can merge with the previous command (e.g. continuous slider drags or node movement)
    if (!m_undoStack.empty() && m_undoStack.back()->CanMergeWith(command.get())) {
        m_undoStack.back()->MergeWith(command.get());
        m_redoStack.clear();
        return;
    }

    m_undoStack.push_back(std::move(command));
    if (m_undoStack.size() > m_maxHistory) {
        m_undoStack.erase(m_undoStack.begin());
    }

    m_redoStack.clear();
}

bool UndoManager::Undo() {
    if (m_undoStack.empty()) return false;

    auto cmd = std::move(m_undoStack.back());
    m_undoStack.pop_back();

    cmd->Undo();
    m_redoStack.push_back(std::move(cmd));
    return true;
}

bool UndoManager::Redo() {
    if (m_redoStack.empty()) return false;

    auto cmd = std::move(m_redoStack.back());
    m_redoStack.pop_back();

    cmd->Redo();
    m_undoStack.push_back(std::move(cmd));
    return true;
}

bool UndoManager::CanUndo() const {
    return !m_undoStack.empty();
}

bool UndoManager::CanRedo() const {
    return !m_redoStack.empty();
}

void UndoManager::Clear() {
    m_undoStack.clear();
    m_redoStack.clear();
}

} // namespace nf::ui
