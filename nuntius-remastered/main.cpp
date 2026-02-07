#include "client.h"
#include "imgui_helper.h"
#include "login_window.h"
#include "chat_window.h"
#include "private_chat_window.h"
#include <print>
#include <format>

// Main code
int main() {

    float main_scale = uiHelper::getMainScale();
    WNDCLASSEXW wc = uiHelper::createWindowClass();
    HWND hwnd = uiHelper::createWindowHandler(wc, main_scale);

    uiHelper::initIMGUI(main_scale, wc, hwnd);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;

    bool showLoginWindow = true;
    bool showChatWindow = false;
    bool showPrivateChatWindow = false;

    std::string username = "";
    std::vector<std::string> messages = {};
    std::vector<std::string> usernames = {};
    std::string selectedUserForPrivate = "";

    ActionMapT actionMap = {
        {MessageType::CONNECT_ACK, [&showLoginWindow, &showChatWindow](SOCKET socket, Header &header) { 
            showLoginWindow = false;
            std::println("connected to the server!");
            showChatWindow = true;
        }},
        {MessageType::SERVER_GROUP_MESSAGE, [&messages](SOCKET socket, Header &header) {
            ServerGroupMessagePayload payload;
            readServerGroupMessage(socket, header, payload);
            std::println("received group message from {}: {}", payload.username, payload.message);
            messages.push_back(payload.username + ": " + payload.message);
        }},
        {MessageType::USERS_LIST_UPDATE, [&usernames](SOCKET socket, Header &header) {
            UsersListUpdatePayload payload;
            readUsersListUpdateMessage(socket, header, payload);
            std::print("received users list update: {}\n", payload.usernames);
            usernames = payload.usernames;
        }}
    };

    Client *client = new Client(actionMap);
    client->init();

    std::function<void()> onClickLogin = [&client, &username]() {
        try {
            client->connectToServer(username);
        } catch (const FatalServerException &err) {
            std::println("unable to connect with the server!");
        }
    };
    LoginWindow *login = new LoginWindow(showLoginWindow, username, onClickLogin);
    
    
    ChatWindow *chat = new ChatWindow(showChatWindow, usernames, messages, [&client](std::string msg) {
        client->sendMessage(msg);
    }, [&showPrivateChatWindow, &selectedUserForPrivate, &username](std::string usernameFromList) {
        if (usernameFromList != username) {
            showPrivateChatWindow = true;
            selectedUserForPrivate = usernameFromList;
        }
    });

    std::vector<std::string> pmessages = {"Private messages!"};
    PrivateChatWindow *privateChat = new PrivateChatWindow(showPrivateChatWindow, pmessages, selectedUserForPrivate, [](std::string msg) {});

    // showMainWindow
    // showAlertWindow
    // showPrivateWindow

    // string[] mainConversation
    // map<string: username, string[]> privateConversations
    // string[] usersArray

    // username
    // messageInput

    while (!done)
    {
        uiHelper::STATE state = uiHelper::initFrame(done);
        if (state == uiHelper::STATE::BREAK) break;
        if (state == uiHelper::STATE::CONTINUE) continue;

        // ---------------------------------------------------
        if (showLoginWindow) login->render();
        if (showChatWindow) chat->render();
        if (showPrivateChatWindow) privateChat->render();

        // ---------------------------------------------------
        uiHelper::render(clear_color);
    }

    uiHelper::cleanUpIMGUI(hwnd, wc);
    return 0;
}
