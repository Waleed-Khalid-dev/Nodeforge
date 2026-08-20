#pragma once

#include "../EditorContext.h"
#include <imgui.h>

namespace nf::ui {

class ViewerPanel {
public:
    explicit ViewerPanel(EditorContext* ctx);
    void Render();

private:
    void RenderTexViewer(Node* node);
    void RenderChanViewer(Node* node);
    void RenderDataViewer(Node* node);
    void RenderGeomViewer(Node* node);

    EditorContext* m_ctx;
    int m_channelMode = 0; // 0: RGBA, 1: RGB, 2: R, 3: G, 4: B, 5: A
    bool m_fitToWindow = true;
    float m_viewerZoom = 1.0f;

    // 3D Orbit Viewport Camera
    float m_orbitYaw = 45.0f;
    float m_orbitPitch = 30.0f;
    float m_orbitDistance = 4.0f;
    bool m_showGrid = true;
    bool m_showWireframe = true;
    bool m_showNormals = false;
};

} // namespace nf::ui
