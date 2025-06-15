#include "../utils/includes.h"

void InitImGui(HWND window, ID3D11Device* device, ID3D11DeviceContext* context)
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(device, context);
}

void BeginGUIFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void RenderGUI()
{
    static bool firstFrame = true;
    if (firstFrame)
    {
        ImGui::SetNextWindowSize(ImVec2(400, 300)); // initial width, height

        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.3f, 0.15f, 0.0f, 1.0f); // Dark orange
        colors[ImGuiCol_Button] = ImVec4(0.5f, 0.25f, 0.0f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.6f, 0.3f, 0.0f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.7f, 0.35f, 0.0f, 1.0f);
        colors[ImGuiCol_Text] = ImVec4(1.0f, 0.9f, 0.8f, 1.0f); // Slightly off-white
        colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);

        firstFrame = false;
    }

    // Lobby struc
    struct Lobby {
        std::string name;
        std::string version;
        bool hasPassword = false;
        int ping = 0;
    };

    // Globals
    static std::vector<Lobby> lobbies;
    static int selectedRow = -1;

    static bool showCreateLobby = false;
    static char lobbyNameBuf[64] = "";
    static char lobbyPasswordBuf[32] = "";
    static bool requirePassword = false;
    static const char* versions[] = { "Y2S4", "Y5S3", "Y3S2" };
    static int selectedVersion = 0;
    static bool showPasswordPopup = false;
    static char enteredPassword[32] = "";

    ImGui::Begin("WillyNet DEV BUILD");

    if (ImGui::BeginTabBar("Tabs"))
    {
        // WNet TAB
        if (ImGui::BeginTabItem("WNet"))
        {
            if (selectedRow >= lobbies.size()) {
                selectedRow = -1;
            }

            static char searchBuf[64] = "";
            ImGui::InputText("##Search", searchBuf, IM_ARRAYSIZE(searchBuf));
            ImGui::SameLine();
            if (ImGui::Button("Search")) {
                // TODO: implement filter
            }
            ImGui::SameLine();
            if (ImGui::Button("Create")) {
                showCreateLobby = true;
                ImGui::OpenPopup("Create Lobby");
            }
            ImGui::SameLine();

            bool canConnect = (selectedRow != -1);
            if (!canConnect)
                ImGui::BeginDisabled();

            if (ImGui::Button("Connect")) {
                if (selectedRow != -1 && selectedRow < lobbies.size()) {
                    if (lobbies[selectedRow].hasPassword) {
                        showPasswordPopup = true;
                        ImGui::OpenPopup("Enter Password");
                    }
                    else {
                        printf("Connecting to lobby: %s (no password)\n", lobbies[selectedRow].name.c_str());
                    }
                }
            }

            if (!canConnect)
                ImGui::EndDisabled();

            // CREATE LOBBY POPUP
            if (ImGui::BeginPopupModal("Create Lobby", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Create a New Custom Game Lobby");
                ImGui::InputText("Lobby Name", lobbyNameBuf, IM_ARRAYSIZE(lobbyNameBuf));
                ImGui::Combo("Version", &selectedVersion, versions, IM_ARRAYSIZE(versions));
                ImGui::Checkbox("Requires Password", &requirePassword);
                if (requirePassword)
                    ImGui::InputText("Password", lobbyPasswordBuf, IM_ARRAYSIZE(lobbyPasswordBuf), ImGuiInputTextFlags_Password);

                if (ImGui::Button("Create")) {
                    Lobby newLobby;
                    newLobby.name = lobbyNameBuf;
                    newLobby.version = versions[selectedVersion];
                    newLobby.hasPassword = requirePassword;
                    newLobby.ping = rand() % 100 + 30;

                    lobbies.push_back(newLobby);

                    // Reset form
                    // GREEN [BLAME] Changed strcpy to strcpy_s for safety
                    strcpy_s(lobbyNameBuf, "");
                    strcpy_s(lobbyPasswordBuf, "");
                    requirePassword = false;
                    selectedVersion = 0;

                    ImGui::CloseCurrentPopup();
                    showCreateLobby = false;
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                    showCreateLobby = false;
                }
                ImGui::EndPopup();
            }

            // SERVER LIST TABLE
            if (ImGui::BeginTable("ServerList", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("PW", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableSetupColumn("Ping", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableHeadersRow();

                for (int i = 0; i < lobbies.size(); ++i)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    bool isSelected = (selectedRow == i);
                    std::string label = lobbies[i].name + "##row_" + std::to_string(i);

                    if (ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                        selectedRow = i;
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", lobbies[i].version.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", lobbies[i].hasPassword ? "X" : "");

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%dms", lobbies[i].ping);
                }

                ImGui::EndTable();
            }

            // ENTER PASSWORD POPUP
            if (ImGui::BeginPopupModal("Enter Password", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Lobby requires a password:");
                ImGui::InputText("Password", enteredPassword, IM_ARRAYSIZE(enteredPassword), ImGuiInputTextFlags_Password);

                if (ImGui::Button("Connect")) {
                    printf("Entered password: %s\n", enteredPassword);
                    if (selectedRow != -1 && selectedRow < lobbies.size()) {
                        printf("Connecting to password-protected lobby: %s\n", lobbies[selectedRow].name.c_str());
                    }

                    ImGui::CloseCurrentPopup();
                    showPasswordPopup = false;
                    enteredPassword[0] = '\0';
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                    showPasswordPopup = false;
                    enteredPassword[0] = '\0';
                }

                ImGui::EndPopup();
            }

            ImGui::EndTabItem();
        }

        // Other Tabs
        if (ImGui::BeginTabItem("Settings"))
        {
            ImGui::Text("This is tab 2");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("About"))
        {
            ImGui::Text("Version 1.0.0");
            ImGui::Text("Developed by YourName");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Help"))
        {
            ImGui::Text("For help, visit our website or contact support.");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Credits"))
        {
            ImGui::BulletText("Developer 1");
            ImGui::BulletText("Developer 2");
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    if (!ImGui::IsPopupOpen("Enter Password") && showPasswordPopup) {
        showPasswordPopup = false;
        enteredPassword[0] = '\0';
    }

    ImGui::End();
}

void EndGUIFrame(ID3D11DeviceContext* context, ID3D11RenderTargetView* target)
{
    ImGui::Render();
    context->OMSetRenderTargets(1, &target, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
