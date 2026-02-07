#pragma once

#include <string>
#include <functional>
#include <vector>
#include <format>
#include <print>

#include "imgui.h"
#include "imgui_stdlib.h"

#define PRIVATE_CHAT_WINDOW_NAME " | Private Chat"
#define CHAT_WINDOW_WIDTH 800
#define CHAT_WINDOW_HEIGHT 500
#define CHAT_WINDOW_POS_X 300
#define CHAT_WINDOW_POS_Y 300

class PrivateChatWindow {
private:
    bool &showPrivateChatWindow;
    std::vector<std::string> &messages;
    std::string &selectedUser;
    std::function<void(std::string message)> onClickSendButton;

    std::string inputMessage;
    bool refocusInput = true;

public:
    PrivateChatWindow(bool &_showPrivateChatWindow, std::vector<std::string> &_messages, std::string &_selectedUser, std::function<void(std::string message)> _onClickSendButton):
        showPrivateChatWindow(_showPrivateChatWindow), messages(_messages), selectedUser(_selectedUser), onClickSendButton(_onClickSendButton) {}

    void onClickSend() {
        if (!inputMessage.empty()) {
            onClickSendButton(inputMessage);
            inputMessage = "";
            refocusInput = true;
        }
    }
    
    void render() {
        std::string windowName = selectedUser + PRIVATE_CHAT_WINDOW_NAME;

        // Setup
        ImGui::SetNextWindowSize(ImVec2(CHAT_WINDOW_WIDTH, CHAT_WINDOW_HEIGHT));
        ImGui::SetNextWindowPos(ImVec2(CHAT_WINDOW_POS_X, CHAT_WINDOW_POS_Y));
        ImGui::Begin(windowName.c_str(), &showPrivateChatWindow);

        // Messages Right Panel
        ImGui::SameLine();
        ImGui::BeginChild("RightPanel", ImVec2(0, 0), false); // fixed width, full height - border true
        
        ImGui::BeginChild("MessagesPanel", ImVec2(0, 400), true); // fixed width, full height - border true
        ImGui::TextDisabled("MESSAGES");
        ImGui::Separator();

        for (int i=0; i<messages.size(); i++) {
            ImGui::TextWrapped(messages[i].c_str());
        }

        ImGui::EndChild();

        ImGui::Spacing();

        // Input and Send Button
        float sendButtonWidth = ImGui::CalcTextSize("SEND").x + ImGui::GetStyle().FramePadding.x * 2;
        float inputWidth = ImGui::GetContentRegionAvail().x - sendButtonWidth - ImGui::GetStyle().ItemSpacing.x;

        ImGui::SetNextItemWidth(inputWidth);

        if (refocusInput) {
            ImGui::SetKeyboardFocusHere();
            refocusInput = false;
        }

        if (ImGui::InputText("##inputMessage", &inputMessage, ImGuiInputTextFlags_EnterReturnsTrue)) {
            onClickSend();
        }

        ImGui::SameLine();
        if (ImGui::Button("SEND")) {
            onClickSend();
        }

        ImGui::EndChild();
    
        ImGui::End();
    }
};