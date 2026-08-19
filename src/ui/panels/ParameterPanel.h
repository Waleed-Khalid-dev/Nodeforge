#pragma once

#include "../EditorContext.h"

namespace nf::ui {

class ParameterPanel {
public:
    explicit ParameterPanel(EditorContext* ctx);
    void Render();

private:
    void RenderParameterRow(Node* node, Parameter* param);

    EditorContext* m_ctx;
    char m_nameBuffer[128]{ 0 };
    char m_exprBuffer[512]{ 0 };
    char m_strBuffer[512]{ 0 };
};

} // namespace nf::ui
