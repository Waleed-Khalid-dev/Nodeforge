#include "RenderTexOp.h"
#include "../comp/GeometryComp.h"
#include "../comp/CameraComp.h"
#include "../comp/LightComp.h"
#include "../geom/GeomOp.h"

namespace nf {

RenderTexOp::RenderTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "RenderTexOp") {
    m_geo1Pin = AddInputPin("geo1", PinType::Any);
    m_geo2Pin = AddInputPin("geo2", PinType::Any);
    m_camPin = AddInputPin("camera", PinType::Any);
    m_lightPin = AddInputPin("light", PinType::Any);
    m_outPin = AddOutputPin("output", PinType::Tex);

    SetParam("resolution", glm::vec2(1920.0f, 1080.0f));
    SetParam("clear_color", glm::vec4(0.05f, 0.05f, 0.08f, 1.0f));
    SetParam("enable_depth", true);
    SetParam("cull_mode", static_cast<int32_t>(1)); // 0: None, 1: Back, 2: Front
}

bool RenderTexOp::Cook(const CookContext& context) {
    glm::vec2 res = GetParam("resolution").Is<glm::vec2>() ? GetParam("resolution").Get<glm::vec2>() : glm::vec2(1920.0f, 1080.0f);
    uint32_t width = static_cast<uint32_t>(std::max(16.0f, res.x));
    uint32_t height = static_cast<uint32_t>(std::max(16.0f, res.y));
    float aspect = static_cast<float>(width) / static_cast<float>(height);

    glm::vec4 clearColor = GetParam("clear_color").Is<glm::vec4>() ? GetParam("clear_color").Get<glm::vec4>() : glm::vec4(0.05f, 0.05f, 0.08f, 1.0f);
    bool enableDepth = GetParam("enable_depth").Is<bool>() ? GetParam("enable_depth").Get<bool>() : true;
    int32_t cullMode = GetParam("cull_mode").Is<int32_t>() ? GetParam("cull_mode").Get<int32_t>() : 1;

    // Evaluate Camera
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0), glm::vec3(0, 1, 0));
    glm::mat4 projMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
    projMatrix[1][1] *= -1.0f;
    glm::vec3 cameraPos(0, 0, 5);

    if (m_camPin && m_camPin->IsConnected()) {
        Pin* src = m_camPin->GetConnectedSource();
        if (src && src->GetNode()) {
            auto* camComp = dynamic_cast<CameraComp*>(src->GetNode());
            if (camComp) {
                viewMatrix = camComp->GetViewMatrix();
                projMatrix = camComp->GetProjectionMatrix(aspect);
                cameraPos = camComp->GetPosition();
            }
        }
    }

    // Evaluate Lights
    std::vector<LightData> lights;
    if (m_lightPin && m_lightPin->IsConnected()) {
        Pin* src = m_lightPin->GetConnectedSource();
        if (src && src->GetNode()) {
            auto* lightComp = dynamic_cast<LightComp*>(src->GetNode());
            if (lightComp) {
                lights.push_back(lightComp->GetLightData());
            }
        }
    }

    // Collect Draw Calls
    std::vector<DrawCall3D> drawCalls;

    auto collectGeoPin = [&](Pin* pin) {
        if (!pin || !pin->IsConnected()) return;
        Pin* src = pin->GetConnectedSource();
        if (!src || !src->GetNode()) return;

        auto* geoComp = dynamic_cast<GeometryComp*>(src->GetNode());
        if (geoComp) {
            drawCalls.push_back(DrawCall3D{
                .geometry = geoComp->GetGeometry(),
                .material = geoComp->GetMaterial(),
                .modelMatrix = geoComp->GetTransformMatrix(),
                .instances = geoComp->GetInstanceTransforms()
            });
            return;
        }

        auto* geomOp = dynamic_cast<GeomOp*>(src->GetNode());
        if (geomOp) {
            drawCalls.push_back(DrawCall3D{
                .geometry = geomOp->GetOutputGeometry(),
                .material = nullptr,
                .modelMatrix = glm::mat4(1.0f)
            });
        }
    };

    collectGeoPin(m_geo1Pin);
    collectGeoPin(m_geo2Pin);

    // Lease output texture from pool if device is available
    if (context.gpuDevice && context.texturePool) {
        if (!m_scenePass) {
            m_scenePass = std::make_unique<Scene3DPass>(context.gpuDevice, context.texturePool);
        }

        auto targetTex = context.texturePool->Acquire(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, context.frameIndex);
        if (targetTex) {
            m_scenePass->Execute(targetTex.get(), viewMatrix, projMatrix, cameraPos, drawCalls, lights, clearColor, enableDepth, cullMode);
            SetOutputTexture(targetTex);
        }
    }

    return true;
}

} // namespace nf
