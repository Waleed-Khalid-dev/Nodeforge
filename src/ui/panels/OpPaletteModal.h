#pragma once

#include "../EditorContext.h"
#include "../../graph/NodeRegistry.h"
#include <string>

namespace nf::ui {

class OpPaletteModal {
public:
    explicit OpPaletteModal(EditorContext* ctx);
    void Render();

private:
    EditorContext* m_ctx;
    char m_searchQuery[128]{ 0 };
    int m_selectedFamilyIndex = 0; // 0: ALL, 1: TexOp, 2: ChanOp, 3: GeomOp, 4: MatOp, 5: DataOp, 6: Comp
    int m_highlightedItemIndex = 0;
};

} // namespace nf::ui
