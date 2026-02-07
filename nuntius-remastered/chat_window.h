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
#define CHAT_WINDOW_POS_X 20
#define CHAT_WINDOW_POS_Y 20

#define ONLINE_STATUS_COLOR ImVec4(0.55f, 0.85f, 0.55f, 1.00f)

class ChatWindow {
private:
    bool &showChatWindow;
    std::string &username;
    std::vector<std::string> &usernames;
    std::vector<ReceivedMessage> &messages;
    std::map<std::string, std::vector<ReceivedMessage>> &privateMessages;

    std::function<void(std::string message)> onClickSendButton;
    std::function<void(std::string username)> onClickUsername;
    
    std::string inputMessage;
    bool refocusInput = true;
    int lastMessagesSize = 0;

public:
    ChatWindow(
        bool &_showChatWindow,
        std::string &_username,
        std::vector<std::string> &_usernames, 
        std::vector<ReceivedMessage> &_messages,
        std::map<std::string, std::vector<ReceivedMessage>> &_privateMessages,
        std::function<void(std::string message)> _onClickSendButton, 
        std::function<void(std::string username)> _onClickUsername
    ):
        showChatWindow(_showChatWindow),
        username(_username),
        usernames(_usernames), 
        messages(_messages),
        privateMessages(_privateMessages),
        onClickSendButton(_onClickSendButton), 
        onClickUsername(_onClickUsername)  {}

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
            if (ImGui::Selectable(usernames[i].c_str(), false)) {
                onClickUsername(usernames[i]);
            }

            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Private Message")) {
                    onClickUsername(usernames[i]);
                }
                ImGui::EndPopup();
            }

            uint32_t unreadCount = 0;
            for (const auto& msg : privateMessages[usernames[i]]) {
                if (!msg.read) {
                    unreadCount++;
                }
            }
            if (unreadCount != 0) {
                ImGui::SameLine();
                ImGui::TextColored(ONLINE_STATUS_COLOR, std::format("{} ●", unreadCount).c_str());
            }
        }

        ImGui::EndChild();

        // Messages Right Panel
        ImGui::SameLine();
        ImGui::BeginChild("RightPanel", ImVec2(0, 0), false); // fixed width, full height - border true
        
        ImGui::BeginChild("MessagesPanel", ImVec2(0, 400), true); // fixed width, full height - border true
        ImGui::TextDisabled("MESSAGES");
        ImGui::Separator();

        std::string lastMessageUser = "";
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
            
            if (i == messages.size() - 1) {
                if (lastMessagesSize != messages.size()) {
                    ImGui::SetScrollHereY(1.0f);
                    lastMessagesSize = messages.size();
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