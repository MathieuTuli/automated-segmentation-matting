#include "modals.hpp"

int ErrorModal(const char *title, const char *err) {
    // ImGui::SetNextItemOpen(true, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
            ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(200.f, 100.f), ImGuiCond_Always);
    ImGuiWindowFlags popup_flags = 0;
    popup_flags |= ImGuiWindowFlags_NoNav;
    popup_flags |= ImGuiWindowFlags_NoResize;
    int ret = 0;
    if (ImGui::BeginPopupModal(title, NULL, popup_flags)) {
        ImGui::BeginChild(
                "error message", ImVec2(ImGui::GetContentRegionAvail().x * 0.95f, 30.f),
                ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

        ImGui::Text("something went wrong %s", err);
        ImGui::EndChild();
        ImGui::Separator();
        if (ImGui::Button("o.k.")) {
            ImGui::CloseCurrentPopup();
            ret = 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("report")) {
            ImGui::CloseCurrentPopup();
            ret = 2;
        }
        ImGui::EndPopup();
    }
    return ret;
}
