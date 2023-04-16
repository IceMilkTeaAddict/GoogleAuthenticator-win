
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include "googleAuth.h"
#include "imgui/imgui.h"
#include <d3d9.h>
#include "IMGUI/imgui_impl_win32.h"
#include "IMGUI/imgui_impl_dx9.h"
#include "imgui/imgui_stdlib.h"
// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct accdata
{
    std::string acc;
    std::string code;
    std::string password;
};

string midstr(string oldstr, string startstr, string endstr)
{
    string re = ".*?" + startstr + "(.*?)" + endstr + ".*?";
    regex pattern(re);
    smatch results;
    if (regex_match(oldstr, results, pattern))
    {
        return results[1];
    }
    else
    {
        return oldstr;
    }

}


std::vector<accdata> ac;
accdata a{ "TEST" ,"XEBSSIVQOWIFFQ5X" ,""};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    fstream f("acc.txt");
    accdata tmp;
   
    string line;
    while (getline(f, line))
    {
       
        tmp.acc = midstr(line,"<acc>","<otp>");
        tmp.code = midstr(line, "<otp>", "</otp>");
        ac.emplace_back(tmp);

    }
    f.close();

       
  
    char name[20]{};
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL,L"TEST", NULL};
    ::RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, L"OTP", WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, 600, 600, NULL, NULL, wc.hInstance, NULL);

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



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;




    ImFont* Font3 = io.Fonts->AddFontDefault();

    //ImGui::StyleColorsLight();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);
    ImVec4 clear_color = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {

        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        static bool sett = false;
        if (!sett)
        {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            sett = 1;

        }

      
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("##windows10001", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
            // ImGui::Separator();

            ImGui::Text("Name:"); ImGui::SameLine(120); ImGui::PushItemWidth(120); ImGui::InputText("##001" ,&a.acc);
            ImGui::Text("OTP-Code:");  ImGui::SameLine(120); ImGui::PushItemWidth(120); ImGui::InputText("##002", &a.code);
            if (ImGui::Button("save_acc.txt"))
            {
                std::fstream f;
                f.open("acc.txt", std::ios::out);
                if (f.is_open())
                {
                    for (const auto& it : ac)
                    {
                        std::string s = "<acc>" + it.acc + + "<otp>" + it.code + "</otp>";
                        f << s << std::endl;
                    }


                    f.close();
                }


            }
            ImGui::SameLine(200);
            if (ImGui::Button("[ ADD ]"))
            {
                              
                ac.emplace_back(a);

            }

            /*---------------------------------------------------------*/
            for ( auto&it :ac )
            {
                std::string s = it.code;
                
                it.password = GAget(s); //GET
                  
            }

            ImGui::BeginChild("##AAA01"); {
                ImGui::Columns(3, "AAA02"); 
                ImGui::Separator();
                ImGui::SetColumnWidth(0, 250);
                ImGui::SetColumnWidth(1, 150);
                ImGui::SetColumnWidth(2, 150);
                ImGui::Text("Name"); ImGui::NextColumn();
                ImGui::Text("Code"); ImGui::NextColumn();
                ImGui::Text("OTP"); ImGui::NextColumn();

                ImGui::Separator();

                static int selected = -1;
                for (const auto &it : ac)
                {
                    selected++;                  
                    ImGui::Text("%s", it.acc.c_str()); ImGui::NextColumn();
                    ImGui::Text("%s", it.code.c_str()); ImGui::NextColumn();
                    ImGui::Text("%s", it.password.c_str()); ImGui::NextColumn();
                }ImGui::Columns(1);
                selected = -1;
                ImGui::Separator();
            }ImGui::EndChild();



            ImGui::End();
        



        // Rendering
        Sleep(10);

        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * 255.0f), (int)(clear_color.y * 255.0f), (int)(clear_color.z * 255.0f), (int)(clear_color.w * 255.0f));
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
        {
            ResetDevice();
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

  
    return 0;
}


bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_LBUTTONDOWN:


        break;

    case WM_RBUTTONUP:
      
        break;
    case WM_MOUSEMOVE:

        break;
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        if (wParam == SIZE_MINIMIZED)
        {
            /*            ToTray(hWnd);
                        ShowWindow(hWnd,SW_HIDE);     */
        }
        return 0;
    case WM_USER:
    {



    }
    break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:

        ::PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

