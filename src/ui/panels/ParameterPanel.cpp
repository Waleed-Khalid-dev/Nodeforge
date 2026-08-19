#include "ParameterPanel.h"
#include "../EditorTheme.h"
#include "../commands/ParamCommands.h"
#include <imgui.h>
#include <cstring>
#include <sstream>

namespace nf::ui {

ParameterPanel::ParameterPanel(EditorContext* ctx)
    : m_ctx(ctx) {
}

void ParameterPanel::Render() {
    ImGui::Begin("Parameters");

    Node* node = m_ctx->GetActiveNode();
    if (!node) {
        ImGui::TextDisabled("No node selected");
        ImGui::End();
        return;
    }

    // Node Header Section
    ImVec4 famCol = EditorTheme::GetFamilyColor(node->GetFamily());
    ImGui::PushStyleColor(ImGuiCol_Text, famCol);
    ImGui::Text("%s", node->GetTypeName().c_str());
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::TextDisabled("(ID: %llu)", node->GetId());

    // Editable Node Name
    std::strncpy(m_nameBuffer, node->GetName().c_str(), sizeof(m_nameBuffer) - 1);
    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::InputText("##NodeName", m_nameBuffer, sizeof(m_nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (std::strlen(m_nameBuffer) > 0) {
            node->SetName(m_nameBuffer);
        }
    }

    ImGui::Separator();

    // Group parameters by pages
    auto pages = node->GetParams().GetPages();
    if (pages.empty()) {
        pages.push_back("Parameters");
    }

    if (ImGui::BeginTabBar("ParamPages")) {
        for (const auto& page : pages) {
            if (ImGui::BeginTabItem(page.c_str())) {
                ImGui::Spacing();
                for (auto* param : node->GetParams().GetParametersByPage(page)) {
                    RenderParameterRow(node, param);
                }
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void ParameterPanel::RenderParameterRow(Node* node, Parameter* param) {
    if (!param) return;

    ImGui::PushID(param->GetName().c_str());

    // Mode Toggle Button: [C] (Constant), [E] (Expression), [B] (Bound Channel)
    bool isExpr = (param->GetMode() == ParamMode::Expression);
    bool isBound = (param->GetMode() == ParamMode::BoundChannel);

    if (isBound) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.95f, 0.60f, 0.10f, 1.00f)); // Amber/Gold
    } else if (isExpr) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.30f, 0.50f, 0.85f, 1.00f)); // Blue
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.20f, 0.25f, 1.00f)); // Dark gray
    }

    const char* btnLabel = isBound ? "B" : (isExpr ? "E" : "C");
    if (ImGui::Button(btnLabel, ImVec2(22.0f, 0.0f))) {
        if (isBound) {
            param->ClearBoundChannel();
        } else if (isExpr) {
            m_ctx->GetUndoManager().ExecuteCommand(
                std::make_unique<SetParamExpressionCommand>(m_ctx, node->GetId(), param->GetName(), param->GetExpression(), "")
            );
        } else {
            m_ctx->GetUndoManager().ExecuteCommand(
                std::make_unique<SetParamExpressionCommand>(m_ctx, node->GetId(), param->GetName(), "", "0.0")
            );
        }
    }
    ImGui::PopStyleColor();

    if (ImGui::IsItemHovered()) {
        if (isBound) ImGui::SetTooltip("Bound Channel Mode: Bound to '%s.%s'. Click to unbind.", param->GetBoundNodeName().c_str(), param->GetBoundChannelName().c_str());
        else if (isExpr) ImGui::SetTooltip("Expression Mode: Click to switch to Constant");
        else ImGui::SetTooltip("Constant Mode: Click to switch to Expression (or drag a channel here to bind)");
    }

    ImGui::SameLine();
    ImGui::Text("%s", param->GetLabel().c_str());

    // Drag-and-drop target to bind channel
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NF_CHANNEL_BIND")) {
            const char* payloadStr = static_cast<const char*>(payload->Data);
            std::string s(payloadStr);
            size_t slashPos = s.find('/');
            if (slashPos != std::string::npos) {
                std::string boundNode = s.substr(0, slashPos);
                std::string boundChan = s.substr(slashPos + 1);
                param->SetBoundChannel(boundNode, boundChan);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine(130.0f);
    ImGui::SetNextItemWidth(-1.0f);

    if (isBound) {
        // Bound Channel Display
        std::string boundStr = "-> " + param->GetBoundNodeName() + "." + param->GetBoundChannelName();
        ImGui::TextColored(ImVec4(0.95f, 0.65f, 0.15f, 1.0f), "%s (%.3f)", boundStr.c_str(), param->AsFloat());
    } else if (isExpr) {
        // Expression Input Field
        std::strncpy(m_exprBuffer, param->GetExpression().c_str(), sizeof(m_exprBuffer) - 1);
        if (ImGui::InputText("##ExprInput", m_exprBuffer, sizeof(m_exprBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            m_ctx->GetUndoManager().ExecuteCommand(
                std::make_unique<SetParamExpressionCommand>(m_ctx, node->GetId(), param->GetName(), param->GetExpression(), m_exprBuffer)
            );
        }
    } else {
        // Constant Value Inputs
        const PinValue& val = param->GetValue();
        const auto& meta = param->GetMetadata();
        PinValue oldVal = val;
        bool changed = false;
        PinValue newVal = val;

        if (val.Is<float>()) {
            float f = val.Get<float>();
            float minF = meta.minLimit.Is<float>() ? meta.minLimit.Get<float>() : -1000.0f;
            float maxF = meta.maxLimit.Is<float>() ? meta.maxLimit.Get<float>() : 1000.0f;
            if (meta.hasLimits) {
                changed = ImGui::SliderFloat("##val", &f, minF, maxF, "%.3f");
            } else {
                changed = ImGui::DragFloat("##val", &f, 0.05f, minF, maxF, "%.3f");
            }
            if (changed) newVal = PinValue(f);
        } else if (val.Is<int32_t>()) {
            int32_t i = val.Get<int32_t>();
            int32_t minI = meta.minLimit.Is<int32_t>() ? meta.minLimit.Get<int32_t>() : -1000;
            int32_t maxI = meta.maxLimit.Is<int32_t>() ? meta.maxLimit.Get<int32_t>() : 1000;
            if (meta.hasLimits) {
                changed = ImGui::SliderInt("##val", &i, minI, maxI);
            } else {
                changed = ImGui::DragInt("##val", &i, 1.0f, minI, maxI);
            }
            if (changed) newVal = PinValue(i);
        } else if (val.Is<bool>()) {
            bool b = val.Get<bool>();
            changed = ImGui::Checkbox("##val", &b);
            if (changed) newVal = PinValue(b);
        } else if (val.Is<glm::vec2>()) {
            glm::vec2 v2 = val.Get<glm::vec2>();
            changed = ImGui::DragFloat2("##val", &v2.x, 0.02f);
            if (changed) newVal = PinValue(v2);
        } else if (val.Is<glm::vec3>()) {
            glm::vec3 v3 = val.Get<glm::vec3>();
            changed = ImGui::DragFloat3("##val", &v3.x, 0.02f);
            if (changed) newVal = PinValue(v3);
        } else if (val.Is<glm::vec4>()) {
            glm::vec4 v4 = val.Get<glm::vec4>();
            if (param->GetName() == "color") {
                changed = ImGui::ColorEdit4("##val", &v4.x);
            } else {
                changed = ImGui::DragFloat4("##val", &v4.x, 0.02f);
            }
            if (changed) newVal = PinValue(v4);
        } else if (val.Is<std::string>()) {
            std::string s = val.Get<std::string>();
            std::strncpy(m_strBuffer, s.c_str(), sizeof(m_strBuffer) - 1);
            if (ImGui::InputText("##val", m_strBuffer, sizeof(m_strBuffer))) {
                newVal = PinValue(std::string(m_strBuffer));
                changed = true;
            }
        }

        if (changed) {
            m_ctx->GetUndoManager().ExecuteCommand(
                std::make_unique<SetParamValueCommand>(m_ctx, node->GetId(), param->GetName(), oldVal, newVal)
            );
        }
    }

    ImGui::PopID();
}

} // namespace nf::ui
