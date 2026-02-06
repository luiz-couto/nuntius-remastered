#include "client.h"
#include "imgui_helper.h"
#include "login_window.h"
#include "chat_window.h"
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
    std::string username = "";

    ActionMapT actionMap = {
        {MessageType::CONNECT_ACK, [&showLoginWindow, &showChatWindow]() { 
            showLoginWindow = false;
            std::println("connected to the server!");
            showChatWindow = true;
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
    
    std::vector<std::string> usernames = {"cat", "dog"};
    std::vector<std::string> messages = {"luiz: hello!", "ana: heeeey!"};
    ChatWindow *chat = new ChatWindow(showChatWindow, usernames, messages, [&client](std::string msg) {
        client->sendMessage(msg);
    });

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

        // ---------------------------------------------------
        uiHelper::render(clear_color);
    }

    uiHelper::cleanUpIMGUI(hwnd, wc);
    return 0;
}
