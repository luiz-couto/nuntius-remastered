#pragma once

#include <string>
#include <functional>

#include "imgui.h"
#include "imgui_stdlib.h"

#define LOGIN_WINDOW_NAME "Login"
#define LOGIN_WINDOW_WIDTH 800
#define LOGIN_WINDOW_HEIGHT 500
#define LOGIN_WINDOW_POS_X 100
#define LOGIN_WINDOW_POS_Y 100

#define LOGIN_MESSAGE "Hello! Enter your username to start chatting:"

class LoginWindow {
private:
    bool &showLoginWindow;
    std::string &username;
    std::function<void()> onClickButton;

public:
    LoginWindow(bool &_showLoginWindow, std::string &_username, std::function<void()> _onClickButton):
        showLoginWindow(_showLoginWindow), username(_username), onClickButton(_onClickButton) {}

    void render() {
        // Setup
        ImGui::SetNextWindowSize(ImVec2(LOGIN_WINDOW_WIDTH, LOGIN_WINDOW_HEIGHT));
        ImGui::SetNextWindowPos(ImVec2(LOGIN_WINDOW_POS_X, LOGIN_WINDOW_POS_Y));
        ImGui::Begin(LOGIN_WINDOW_NAME, nullptr, ImGuiWindowFlags_NoCollapse);

        // Text
        ImGui::SetCursorPosX(LOGIN_WINDOW_WIDTH * 0.5f - ImGui::CalcTextSize(LOGIN_MESSAGE).x * 0.5f);
        ImGui::Text(LOGIN_MESSAGE);
        
        // Input
        ImGui::SetCursorPosX(LOGIN_WINDOW_WIDTH * 0.5f - ImGui::CalcTextSize(LOGIN_MESSAGE).x * 0.5f);
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(LOGIN_MESSAGE).x);
        ImGui::InputText("##username", &username);

        // Button
        ImGui::SetCursorPosX(LOGIN_WINDOW_WIDTH * 0.5f - ImGui::CalcTextSize(LOGIN_MESSAGE).x * 0.5f);
        if (ImGui::Button("Login", ImVec2(ImGui::CalcTextSize(LOGIN_MESSAGE).x, 40.0f))) { 
            onClickButton();
        }

        ImGui::End();
    }
};