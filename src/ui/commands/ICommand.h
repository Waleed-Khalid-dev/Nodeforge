#pragma once

#include <string>
#include <memory>

namespace nf::ui {

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual void Redo() { Execute(); }
    virtual std::string GetName() const = 0;
    virtual bool CanMergeWith(const ICommand* /*other*/) const { return false; }
    virtual void MergeWith(const ICommand* /*other*/) {}
};

} // namespace nf::ui
