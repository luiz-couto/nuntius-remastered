#pragma once

#include <string>
#include <functional>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_helper.h"

#define LOGIN_WINDOW_NAME "Login"
#define LOGIN_WINDOW_WIDTH 800
#define LOGIN_WINDOW_HEIGHT 500
#define LOGIN_WINDOW_POS_X 20
#define LOGIN_WINDOW_POS_Y 20

#define LOGIN_MESSAGE "Hello! Enter your username to start chatting:"
#define TITLE "Nuntius Remastered"

class LoginWindow {
private:
    bool &showLoginWindow;
    std::string &username;
    std::function<void()> onClickButton;
    bool refocusInput = true;

public:
    LoginWindow(bool &_showLoginWindow, std::string &_username, std::function<void()> _onClickButton):
        showLoginWindow(_showLoginWindow), username(_username), onClickButton(_onClickButton) {}

    void render() {
        // Setup
        ImGui::SetNextWindowSize(ImVec2(LOGIN_WINDOW_WIDTH, LOGIN_WINDOW_HEIGHT));
        ImGui::SetNextWindowPos(ImVec2(LOGIN_WINDOW_POS_X, LOGIN_WINDOW_POS_Y));
        ImGui::Begin(LOGIN_WINDOW_NAME, nullptr, ImGuiWindowFlags_NoCollapse);

        // Title
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        if (uiHelper::g_titleFont) {
            ImGui::PushFont(uiHelper::g_titleFont);
            const char* title = TITLE;
            ImGui::SetCursorPosX(LOGIN_WINDOW_WIDTH * 0.5f - ImGui::CalcTextSize(title).x * 0.5f);
            ImGui::Text(title);
            ImGui::PopFont();
        } else {
            ImGui::SetWindowFontScale(2.5f);
            const char* title = TITLE;
            ImGui::SetCursorPosX(LOGIN_WINDOW_WIDTH * 0.5f - ImGui::CalcTextSize(title).x * 0.5f);
            ImGui::Text(title);
            ImGui::SetWindowFontScale(1.0f);
        }
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        // Text
        ImGui::SetCursorPosX(LOGIN_WINDOW_WIDTH * 0.5f - ImGui::CalcTextSize(LOGIN_MESSAGE).x * 0.5f);
        ImGui::Text(LOGIN_MESSAGE);
        
        // Input
        ImGui::SetCursorPosX(LOGIN_WINDOW_WIDTH * 0.5f - ImGui::CalcTextSize(LOGIN_MESSAGE).x * 0.5f);
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(LOGIN_MESSAGE).x);
        
        if (refocusInput) {
            ImGui::SetKeyboardFocusHere();
            refocusInput = false;
        }

        if (ImGui::InputText("##username", &username, ImGuiInputTextFlags_EnterReturnsTrue)) {
            onClickButton();
            refocusInput = true;
        }

        // Button
        ImGui::SetCursorPosX(LOGIN_WINDOW_WIDTH * 0.5f - ImGui::CalcTextSize(LOGIN_MESSAGE).x * 0.5f);
        if (ImGui::Button("Login", ImVec2(ImGui::CalcTextSize(LOGIN_MESSAGE).x, 40.0f))) { 
            onClickButton();
            refocusInput = true;
        }

        ImGui::End();
    }
};