#pragma once

#include <string>
#include <functional>

#include "imgui.h"
#include "imgui_stdlib.h"

#define ALERT_WINDOW_NAME "Alert"
#define ALERT_WINDOW_WIDTH 500
#define ALERT_WINDOW_HEIGHT 200
#define ALERT_WINDOW_POS_X 200
#define ALERT_WINDOW_POS_Y 200

class AlertWindow {
private:
    bool &showAlertWindow;
    std::string &alertMessage;

public:
    AlertWindow(
        bool &_showAlertWindow,
        std::string &_alertMessage
    ):
        showAlertWindow(_showAlertWindow),
        alertMessage(_alertMessage) {}

    void render() {
        // window color
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.85f, 0.55f, 0.55f, 1.00f));

        // Setup
        ImGui::SetNextWindowSize(ImVec2(ALERT_WINDOW_WIDTH, ALERT_WINDOW_HEIGHT));
        ImGui::SetNextWindowPos(ImVec2(ALERT_WINDOW_POS_X, ALERT_WINDOW_POS_Y));
        ImGui::Begin(ALERT_WINDOW_NAME, &showAlertWindow);

        // Text
        ImGui::SetCursorPosX(ALERT_WINDOW_WIDTH * 0.5f - ImGui::CalcTextSize(alertMessage.c_str()).x * 0.5f);
        ImGui::SetCursorPosY(ALERT_WINDOW_HEIGHT * 0.5f - ImGui::CalcTextSize(alertMessage.c_str()).y * 0.8f);
        ImGui::Text(alertMessage.c_str());
        
        ImGui::Spacing();
        ImGui::Spacing();

        // Button
        ImGui::SetCursorPosX(ALERT_WINDOW_WIDTH * 0.5f - ImGui::CalcTextSize(alertMessage.c_str()).x * 0.5f);
        if (ImGui::Button("OK", ImVec2(ImGui::CalcTextSize(alertMessage.c_str()).x, 40.0f))) { 
           showAlertWindow = false;
        }

        ImGui::PopStyleColor();

        ImGui::SetWindowFocus();
        ImGui::End();
    }
};