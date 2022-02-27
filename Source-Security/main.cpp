#define _CRT_SECURE_NO_WARNINGS

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <d3dx9.h>

#include "junkcode.h"
#include "OfuscateWorker.h"
#include "FileWorker.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <tchar.h>

// GLOBAL VARIABLES NEEDED FOR PROPER EXECUTION

//OFUSCATION
bool JunkCode_enabled = false;
int junk_counter = 0;
//int junk_count = 0;
int O_length = 10, O_frequency = 4;

//FILE HANDLING
string Files[100];
int count_files;
string main_file;
int header_count = 0;
char FilePath[] = "C:\\Users\\karan bamal\\source\\repos\\Major-Test-App\\Major-Test-App";

char complileCom[] = "g++ $main_file_here";

//ENCRYPT STRING
bool EncryptStrings_enabled = false;
int encrypted_strings_count = 0;

bool Antidebug_enabled = false;
int AntiDebug_technique_number = 0;
bool anti_debug_included = false;

//STATUS
int run_status = 0;
float progress = 0.0f;
int progress_precentage = 0;
string debug_messages;

// UI
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

static void MenuBar()
{
    ImGui::MenuItem("Main menu", NULL, false, false);
    if (ImGui::MenuItem("does nothing", "Ctrl+O")) {}

    ImGui::Separator();
    if (ImGui::BeginMenu("Options"))
    {
        static bool enabled = true;
        ImGui::MenuItem("Enabled", "", &enabled);
        ImGui::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Colors"))
    {
        float sz = ImGui::GetTextLineHeight();
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
            ImGui::Dummy(ImVec2(sz, sz));
            ImGui::SameLine();
            ImGui::MenuItem(name);
        }
        ImGui::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (ImGui::BeginMenu("Options")) // <-- Append!
    {
        static bool b = true;
        ImGui::Checkbox("SomeOption", &b);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true)) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}


static void AppMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Main"))
        {
            MenuBar();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Credits"))
        {
            if (ImGui::MenuItem("Karan Bamal", "abankalarm")) {}
            //if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();

#define OpenUrl(x)  system(std::string("start " + (string)x).c_str());

            if (ImGui::MenuItem("Linkedin", "https://www.linkedin.com/in/karanbamal/")) { OpenUrl("https://www.linkedin.com/in/karanbamal/");}
            if (ImGui::MenuItem("Twitter", "https://twitter.com/KaranBamal")) { OpenUrl("https://twitter.com/KaranBamal") }
            if (ImGui::MenuItem("Gmail", "Bamalkaranbamal@gmail.com")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}



int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("A3-OffSec v0.1"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
    //LATER: change to WS_POPUP and add menu bar with close minimize stuff
 
    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //ImVec4 clear_color = ImVec4(6, 57, 112, 1.00f);


    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();


        if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

        bool* p_open = NULL;
        AppMainMenuBar();

        // 1. Our main window
        {
            static bool no_titlebar = true;
            static bool no_scrollbar = false;
            static bool no_menu = true;
            static bool no_move = true;
            static bool no_resize = true;
            static bool no_collapse = true;
            static bool no_close = true;
            static bool no_nav = false;
            static bool no_background = false;
            static bool no_bring_to_front = false;
            static bool unsaved_document = false;

            ImGuiWindowFlags window_flags = 0;
            if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
            if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
            if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
            if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
            if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
            if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
            if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
            if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
            if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
            if (no_close)           p_open = NULL;

            static float f = 0.0f;
            static int counter = 0;
            ImGui::SetNextWindowSize(ImVec2((float)500, (float)500), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2((float)0, (float)19), ImGuiCond_Once);
            ImGui::Begin("Main control", p_open, window_flags);

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Anti-Tampering, Anti-Debugging and Anti-Disassembly for c/c++ code");
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Encrpyt all strings");
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Add Anti-debugging code");
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Add junk code(in assembly)");
            ImGui::Separator();
            ImGui::Text("");
            ImGui::Text("Select Architecture"); ImGui::SameLine(); HelpMarker(
                "Both 32 bit and 64 bit architecture are supported");

            static int Is32Bit = 1;
            ImGui::RadioButton("x86", &Is32Bit, 1); ImGui::SameLine();
            ImGui::RadioButton("x86_64", &Is32Bit, 0);
            if (!Is32Bit) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), " Junk code is not supported in 64 bit");
            }
            ImGui::Text("");

            
            static bool check = false;
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("Anti-Debug"))
                {
                    ImGui::Checkbox("Enable Anti-Debug code generation", &Antidebug_enabled);
                    ImGui::Text("Please select any of the following techniques you want to use \n to protect your code from debugging after compilation.");

                    
                    ImGui::RadioButton("antidebugpresent", &AntiDebug_technique_number, 0);
                    ImGui::RadioButton("antidebugPEB", &AntiDebug_technique_number, 1);
                    ImGui::RadioButton("CheckRemoteDebuggerPresent", &AntiDebug_technique_number, 2);
                    ImGui::RadioButton("CheckNtGlobalFlag", &AntiDebug_technique_number, 3);
                    ImGui::RadioButton("CheckNtQueryInformationProcess (32 bit ONLY)", &AntiDebug_technique_number, 4);

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Encrypt Strings"))
                {
                    ImGui::Checkbox("Enable encryption of all strings", &EncryptStrings_enabled);
                    ImGui::Text("");
                    ImGui::Text("Theres no need to specify a key for encryption,\nas a very secure key is generated at runtime.");
                    ImGui::Text("More information"); ImGui::SameLine();
                    HelpMarker("Encryption of all strings in the project are done with a compile time generated random XOR key. \n\nThis not only makes it very secure but also makes the key \n tied to the the specific build and unique for every single user.");
                    ImGui::Text("");
                    ImGui::Text("");
                    ImGui::Text("");
                    ImGui::Text("");
                    ImGui::Text("");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Ofuscator"))
                {
                    ImGui::Checkbox("Ofuscate the application using junk assembly code", &JunkCode_enabled);
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Please make sure your compiler supports inline assembly for 32 bit");
                    
                    ImGui::Text("Specify how frequent assembly is added - every N th lines");
                    ImGui::SliderInt("frequency", &O_frequency, 1, 20);
                    ImGui::Text("");
                    ImGui::Text("Specify how long each sequence should be");
                    ImGui::SliderInt("length", &O_length, 5, 20);
                    ImGui::Text("More information"); ImGui::SameLine();
                    ImGui::Text("");
                    ImGui::Text("");
                    //HelpMarker("Encryption of all strings in the project are done with a compile time generated random XOR key. \n\nThis not only makes it very secure but also makes the key \n tied to the the specific build and unique for every single user.");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::Text("");
 
            ImGui::Separator();
            //ImGui::Text("");
            
;
            //ImGui::OpenPopup("commit?");
            // Always center this window when appearing
            ImGui::Text("Click below to make the changes to the project ");
                if (ImGui::Button("Commit desired changes / execute"))
                    ImGui::OpenPopup("Commit confirm");
                // Always center this window when appearing
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                if (ImGui::BeginPopupModal("Commit confirm", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Make sure you have backed up your files.\nThis operation cannot be undone!\n\n");
                    ImGui::Separator();

                    //static int unused_i = 0;
                    //ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

                    static bool dont_ask_me_next_time = false;
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
                    ImGui::PopStyleVar();

                    if (ImGui::Button("Proceed", ImVec2(120, 0))) { 
                        
                    
                        // Commit run code
                        //
                        //
                        //
                        //
                        //


                        ImGui::CloseCurrentPopup(); 
                        run_status = 1;
                       
                        for (int i = 0; i < 1; i++) {
                        if (Antidebug_enabled) {
                            debug_messages += "[+] Moving anti-debug library\n";

                            char NPath[MAX_PATH];
                            GetCurrentDirectoryA(MAX_PATH, NPath);
                            
                            char* full_src;
                            char src[] = "\\antidebug.h";
                            full_src = (char*)malloc(strlen(NPath) + strlen(src) + 1);
                            strcpy(full_src, NPath);
                            strcat(full_src, src);

                            char* full_text;
                            
                            full_text = (char*)malloc(strlen(full_src) + strlen(src) + 1);
                            strcpy(full_text, FilePath);
                            strcat(full_text, src);
                            
                            std::cout << "\n\n full    " << full_src << " \n      " << full_text << "\n\n";
                            debug_messages += "Copying into " + std::string(full_text) + "\n";
                            int copying_code = copy_file(full_src, full_text, false);
                            if (copying_code != 0) {
                                debug_messages += "[-] Copying code Failed, error code: \n";
                                if (copying_code == 1) {
                                    debug_messages += "[-] Original debug code not accessible\n";
                                }
                                else
                                {
                                    debug_messages += "[-] Cannot write to project file\n";
                                }

                            }
                            debug_messages += "[+] Adding anti-debug functionality\n";
                            addStringToTop(main_file, "#include \"antidebug.h\"", header_count);
                            

                            if (AntiDebug_technique_number == 0) {
                                _add_antidebug("antidebugpresent(true);", anti_debug_included, main_file);
                            }
                            else if (AntiDebug_technique_number == 1) {
                                _add_antidebug("antidebugPEB(true);", anti_debug_included, main_file);
                            }
                            else if(AntiDebug_technique_number == 2) {
                                _add_antidebug("CheckRemoteDebuggerPresent(true);", anti_debug_included, main_file);
                            }
                            else if(AntiDebug_technique_number == 3) {
                                _add_antidebug("CheckNtGlobalFlag(true);", anti_debug_included, main_file);
                            }
                            else if (AntiDebug_technique_number == 4) {
                                if (Is32Bit == true) {
                                    _add_antidebug("CheckNtQueryInformationProcess(true);", anti_debug_included, main_file);
                                }
                                else
                                {
                                    debug_messages += "[-] Wrong architecture for anti-debug mode chosen\n";
                                }
                            }
                            else {
                                debug_messages += "[-] Value error in choosing Anti-Debug mode\n";
                            }

                        }

                        for (int i = 0; i < 100; i++) {
                            if (strlen(Files[i].c_str()) > 2){

                                if (EncryptStrings_enabled) {

                                    debug_messages += "[+] Moving code into the project file for compile time string encryption\n";
                                    

                                    char NPath[MAX_PATH];
                                    GetCurrentDirectoryA(MAX_PATH, NPath);

                                    char* full_src;
                                    char src[] = "\\encryptstring.h";
                                    full_src = (char*)malloc(strlen(NPath) + strlen(src) + 1);
                                    strcpy(full_src, NPath);
                                    strcat(full_src, src);

                                    char* full_text;

                                    full_text = (char*)malloc(strlen(full_src) + strlen(src) + 1);
                                    strcpy(full_text, FilePath);
                                    strcat(full_text, src);

                                    std::cout << "\n\n full    " << full_src << " \n      " << full_text << "\n\n";
                                    debug_messages += "Copying into " + std::string(full_text) + "\n";
                                    int copying_code = copy_file(full_src, full_text, false);
                                    if (copying_code != 0) {
                                        debug_messages += "[-] Copying code Failed, error code: \n";
                                        if (copying_code == 1) {
                                            debug_messages += "[-] Original debug code not accessible\n";
                                        }
                                        else
                                        {
                                            debug_messages += "[-] Cannot write to project file\n";
                                        }

                                    }
                                    addStringToTop(Files[i], "#include \"encryptstring.h\"", header_count);
                                    addTextToVector(Files[i], encrypted_strings_count);
                                    debug_messages += "[+] Changing string structure\n";
                                    //_add_ofuscation(O_frequency, O_length, junk_counter, Files[i]);
                                }
                                progress += (1 / (2* (float)count_files));
                                
                                if (JunkCode_enabled) {
                                    debug_messages += "[+] Adding assembly instructions for ofuscation\n";
                                    _add_ofuscation(O_frequency, O_length, junk_counter, Files[i]);
                                }
                                progress += 1 / (2 * (float)count_files);
                            }
                            else
                            {
                                debug_messages += "[+] Completed traversing all files\n";
                                break;
                            }
                        }
                        }
                    
                        run_status = 0;
                         //
                         //
                         //
                         //
                         //

                    }   
                    
                    ImGui::SetItemDefaultFocus();
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                    ImGui::EndPopup();
                }

                ImGui::TextWrapped("Click below to compile (compiler has to be configured for individual use cases) ");
                ImGui::Text("Run this only after commiting changes");
                ImGui::InputText("", complileCom, 300);

                ImGui::SameLine();
                if (ImGui::Button("Compile")) {
                    ImGui::OpenPopup("confirm Compilation");

                }
                // Always center this window when appearing
                //ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                if (ImGui::BeginPopupModal("confirm Compilation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Make sure you have set up compiler options\nThis operation cannot be undone!\n\n");
                    ImGui::Separator();

                    //static int unused_i = 0;
                    //ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

                    static bool dont_ask_me_next_time = false;
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
                    ImGui::PopStyleVar();

                    if (ImGui::Button("Proceed", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); system(complileCom); }
                    ImGui::SetItemDefaultFocus();
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                    ImGui::EndPopup();
                }



            ImGui::End();
        }

        // 2. Compiler Congig window
        
        {
            static bool no_titlebar = false;
            static bool no_scrollbar = false;
            static bool no_menu = true;
            static bool no_move = false;
            static bool no_resize = false;
            static bool no_collapse = false;
            static bool no_close = true;
            static bool no_nav = false;
            static bool no_background = false;
            static bool no_bring_to_front = false;
            static bool unsaved_document = false;

            ImGuiWindowFlags window_flags = 0;
            if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
            if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
            if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
            if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
            if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
            if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
            if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
            if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
            if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
            if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

            static float f = 0.0f;
            static int counter = 0;
            ImGui::SetNextWindowSize(ImVec2((float)500, (float)281 -40), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2((float)0, (float)519), ImGuiCond_Once);
            //ImGui::SetNextWindowSize(ImVec2((float)300, (float)500));
            ImGui::Begin("Debug trace", p_open, window_flags);
            //ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Please choose the command you want to run for compilation");
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "In case of any errors, they will be displayed below");
            ImGui::Text("Debug messages for troubleshooting: ");
            ImGui::TextWrapped((debug_messages.c_str()));
            ImGui::End();
        }

        // 3. File Manager Window
        {
            static bool no_titlebar = false;
            static bool no_scrollbar = false;
            static bool no_menu = true;
            static bool no_move = false;
            static bool no_resize = false;
            static bool no_collapse = false;
            static bool no_close = true;
            static bool no_nav = false;
            static bool no_background = false;
            static bool no_bring_to_front = false;
            static bool unsaved_document = false;

            ImGuiWindowFlags window_flags = 0;
            if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
            if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
            if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
            if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
            if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
            if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
            if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
            if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
            if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
            if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

            ImGui::SetNextWindowSize(ImVec2((float)(1280-516), (float)(250)), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2((float)500, (float)19), ImGuiCond_Once);
            ImGui::Begin("Project Configuration", &show_another_window, window_flags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Please enter the full path(folder) of the C/C++ project");
            
            
            ImGui::InputText("", FilePath, 300);

            vector<string> g1;
            int file_array_index;
            ImGui::SameLine();
            if (ImGui::Button("Search")) {
                file_array_index = 0;
                g1 = getFilesInFolder(FilePath);
                Files->clear();

            }

            for (auto i = g1.begin(); i != g1.end(); ++i)
            {
                string fileName = *i;
                if (checkFileExtension(fileName))
                {
                    Files[file_array_index] = fileName;
                    file_array_index++;
                    std::cout << Files[0];
                }
            }


            ImGui::Text("The list of all .cpp, .hpp and .h files detected below.");
            ImGui::Text("Recheck your path if the files are not visible.");
            
            static int item_current_idx = 0; // Here we store our selection data as an index.
            if (ImGui::BeginListBox("Files detected"))
            {
                for (int n = 0; n < IM_ARRAYSIZE(Files); n++)
                {
                    const bool is_selected = (item_current_idx == n);
                    if (ImGui::Selectable(Files[n].c_str(), is_selected))
                        item_current_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        main_file = Files[n];
                        ImGui::SetItemDefaultFocus();

                        if (main_file.length() > 5) {
                            std::string compilecom_local = "g++ " + main_file;
                            strcpy(complileCom, compilecom_local.c_str());
                        }
                }
                ImGui::EndListBox();
            }
            ImGui::Text("Main project file:"); ImGui::SameLine();
            ImGui::Text(main_file.c_str());

            //complileCom = compilecom_local.c_str();
            ImGui::End();
        }

        // 4. Status
        {
            static bool no_titlebar = false;
            static bool no_scrollbar = false;
            static bool no_menu = true;
            static bool no_move = false;
            static bool no_resize = false;
            static bool no_collapse = false;
            static bool no_close = true;
            static bool no_nav = false;
            static bool no_background = false;
            static bool no_bring_to_front = false;
            static bool unsaved_document = false;

            ImGuiWindowFlags window_flags = 0;
            if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
            if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
            if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
            if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
            if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
            if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
            if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
            if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
            if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
            if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

            ImGui::SetNextWindowSize(ImVec2((float)(1280 - 516), (float)(800-250-54)), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2((float)500, (float)269), ImGuiCond_Once);
            ImGui::Begin("Status", &show_another_window, window_flags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Status: "); ImGui::SameLine();
            if (!run_status) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stopped");
            }
            else
            {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Running");
            }
            ImGui::Text("");

            static float progress_dir = 1.0f;
            ImGui::Text("Progress bar");
            /*if (!run_status) {
                
                progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
                if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
                if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
                            }*/
            ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); ImGui::Text("Waiting to run");
            ImGui::Text(to_string(int(progress*100)).c_str());
            ImGui::Text("");
            count_files = 0;
            for (int i = 0; i < 100; i++) {
                if (strlen(Files[i].c_str()) > 2)
                    count_files++;
                else
                {
                    break;
                }
            }
            ImGui::Text("");
            ImGui::Text("Total files: "); ImGui::SameLine();
            ImGui::Text((std::to_string(count_files).c_str()));
            ImGui::Text("");

            ImGui::Text("Anti-Debug inclusion status: "); ImGui::SameLine();
            string inclusionstatus = "False";
            if (anti_debug_included) {
                inclusionstatus = "True";
            }
            ImGui::Text((inclusionstatus.c_str()));

            ImGui::Text("Important function inclusion count: "); ImGui::SameLine();
            ImGui::Text((std::to_string(header_count).c_str()));
            
            ImGui::Text("Encrypted strings count: "); ImGui::SameLine();
            ImGui::Text((std::to_string(encrypted_strings_count).c_str()));

            ImGui::Text("Ofucated code addition count: "); ImGui::SameLine();
            ImGui::Text((std::to_string(junk_counter).c_str()));
            ImGui::Text("");
            ImGui::Text("");


            ImGui::Text("");
            ImGui::Text("");
            ImGui::Text("");
            ImGui::Text("Stats: Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);// Create a window called "Hello, world!" and append into it.

            ImGui::End();
        }
        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f), (int)(clear_color.y*clear_color.w*255.0f), (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;

}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

