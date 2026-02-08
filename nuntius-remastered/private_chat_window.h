#pragma once

#include <string>
#include <functional>
#include <vector>
#include <format>
#include <print>
#include <map>

#include "imgui.h"
#include "imgui_stdlib.h"

#define PRIVATE_CHAT_WINDOW_NAME " | Private Chat"
#define CHAT_WINDOW_WIDTH 800
#define CHAT_WINDOW_HEIGHT 500
#define CHAT_WINDOW_POS_X 20
#define CHAT_WINDOW_POS_Y 600

class PrivateChatWindow {
private:
    bool &showPrivateChatWindow;
    std::string username;
    std::map<std::string, std::vector<ReceivedMessage>> &privateMessages;
    std::string &selectedUser;
    std::function<void(std::string message)> onClickSendButton;

    std::string inputMessage;
    bool refocusInput = true;
    int lastMessagesSize = 0;

public:
    PrivateChatWindow(
        bool &_showPrivateChatWindow,
        std::string _username,
        std::map<std::string, std::vector<ReceivedMessage>> &_privateMessages,
        std::string &_selectedUser, std::function<void(std::string message)> _onClickSendButton
    ):
        showPrivateChatWindow(_showPrivateChatWindow),
        username(_username),
        privateMessages(_privateMessages),
        selectedUser(_selectedUser),
        onClickSendButton(_onClickSendButton) {}

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

        if (selectedUser != "") {
            std::string lastMessageUser = "";
            std::vector<ReceivedMessage> messages = privateMessages[selectedUser];
            for (int i=0; i<messages.size(); i++) {
                std::string fromLabel = messages[i].from;
                if (messages[i].from == username) {
                    fromLabel += " (You)";
                }

                fromLabel += " says:";

                if (lastMessageUser != messages[i].from) {
                    ImGui::TextDisabled(fromLabel.c_str());
                }

                std::string msg = "   " + messages[i].msg;
                ImGui::TextWrapped(msg.c_str());
                lastMessageUser = messages[i].from;

                privateMessages[selectedUser][i].read = true;

                if (i == messages.size() - 1) {
                    if (lastMessagesSize != messages.size()) {
                        ImGui::SetScrollHereY(1.0f);
                        lastMessagesSize = messages.size();
                    }
                }
            }
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