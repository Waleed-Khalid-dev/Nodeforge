#pragma once

#include "../graph/PinValue.h"
#include "../graph/CookContext.h"
#include <string>
#include <memory>

namespace nf {

class Graph;
class Node;

class PythonEngine {
public:
    static PythonEngine& Instance();

    void Initialize();
    void Shutdown();
    bool IsInitialized() const { return m_initialized; }

    void SetActiveGraph(Graph* graph) { m_activeGraph = graph; }
    Graph* GetActiveGraph() const { return m_activeGraph; }

    bool ExecuteString(const std::string& code, std::string* outError = nullptr);
    PinValue EvaluateExpression(const std::string& expression, Node* meNode, const CookContext& context, std::string* outError = nullptr);

private:
    PythonEngine() = default;
    ~PythonEngine();

    bool m_initialized = false;
    Graph* m_activeGraph = nullptr;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace nf
