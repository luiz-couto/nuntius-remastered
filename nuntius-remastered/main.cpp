#include "client.h"
#include "imgui_helper.h"

// Main code
int main() {

    float main_scale = uiHelper::getMainScale();
    WNDCLASSEXW wc = uiHelper::createWindowClass();
    HWND hwnd = uiHelper::createWindowHandler(wc, main_scale);

    uiHelper::initIMGUI(main_scale, wc, hwnd);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 custom_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    char buff[256] = "";
    bool show_text_name = false;
    float r = 10.0f;
    std::string helloMessage = "Hello! Enter your username to join the chat:";

    // Main loop
    bool done = false;

    ActionMapT actionMap = {
        {MessageType::CONNECT_ACK, [&buff]() { 
            std::string msg = "Connected!";
            strcpy_s(buff, sizeof(buff), msg.c_str());
        }}
    };
    
    Client *client = new Client(actionMap);
    client->init();

    while (!done)
    {
        uiHelper::STATE state = uiHelper::initFrame(done);
        if (state == uiHelper::STATE::BREAK) break;
        if (state == uiHelper::STATE::CONTINUE) continue;

        // ---------------------------------------------------
        ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(100, 100));
        ImGui::Begin("Hello, world!", &show_demo_window, ImGuiWindowFlags_NoCollapse);                          // Create a window called "Hello, world!" and append into it.
        
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - ImGui::CalcTextSize("Hello, pleae enter your name:").x * 0.5f); // Centered button (approximately, because of rounding and style)
        
        ImGui::Text("Hello! please enter your name:");
        ImGui::InputText("##Name", buff, sizeof(buff));
        
        ImGui::SameLine();
        if (ImGui::Button("Click!")) {
            client->connectToServer(buff);
            show_text_name = !show_text_name;
        }

        if (show_text_name) ImGui::Text("Hello %s!", buff);

        ImGui::Checkbox("Check1", &show_text_name);      // Edit bools storing our window open/close state
        
        ImGui::SliderFloat("Circle radius", &r, 5.0f, 20.0f);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 center = ImVec2(pos.x + 100, pos.y + 150);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        //draw_list->AddCircleFilled(center, r, IM_COL32(255, 0, 0, 255));
        draw_list->AddCircleFilled(center, r, ImGui::ColorConvertFloat4ToU32(custom_color));

        ImGui::ColorEdit4("Color Picker", (float*)&custom_color); // Edit 4 floats representing a color

        ImGui::End();

        // ---------------------------------------------------
        uiHelper::render(clear_color);
    }

    uiHelper::cleanUpIMGUI(hwnd, wc);
    return 0;
}
