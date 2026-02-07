#pragma once

#include <string>
#include <functional>
#include <vector>
#include <format>
#include <print>

#include "imgui.h"
#include "imgui_stdlib.h"

#define CHAT_WINDOW_NAME "Nuntius Remastered"
#define CHAT_WINDOW_WIDTH 800
#define CHAT_WINDOW_HEIGHT 500
#define CHAT_WINDOW_POS_X 100
#define CHAT_WINDOW_POS_Y 100

#define ONLINE_STATUS_COLOR ImVec4(0.55f, 0.85f, 0.55f, 1.00f)

class ChatWindow {
private:
    bool &showChatWindow;
    std::vector<std::string> &usernames;
    std::vector<std::string> &messages;
    std::function<void(std::string message)> onClickSendButton;
    std::string inputMessage;
    bool refocusInput = true;

public:
    ChatWindow(bool &_showChatWindow, std::vector<std::string> &_usernames, std::vector<std::string> &_messages, std::function<void(std::string message)> _onClickSendButton):
        showChatWindow(_showChatWindow), usernames(_usernames), messages(_messages), onClickSendButton(_onClickSendButton) {}

    void onClickSend() {
        if (!inputMessage.empty()) {
            onClickSendButton(inputMessage);
            inputMessage = "";
            refocusInput = true;
        }
    }
    
    void render() {
        // Setup
        ImGui::SetNextWindowSize(ImVec2(CHAT_WINDOW_WIDTH, CHAT_WINDOW_HEIGHT));
        ImGui::SetNextWindowPos(ImVec2(CHAT_WINDOW_POS_X, CHAT_WINDOW_POS_Y));
        ImGui::Begin(CHAT_WINDOW_NAME, nullptr, ImGuiWindowFlags_NoCollapse);

        // Users Left Panel
        ImGui::BeginChild("UsersPanel", ImVec2(200, 0), true); // fixed width, full height - border true
        ImGui::TextDisabled("ONLINE");
        ImGui::Separator();

        for (int i=0; i<usernames.size(); i++) {
            ImGui::Selectable(usernames[i].c_str(), false);

            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Private Message")) {

                }
                ImGui::EndPopup();
            }

            ImGui::SameLine();
            ImGui::TextColored(ONLINE_STATUS_COLOR, "●");
        }

        ImGui::EndChild();

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