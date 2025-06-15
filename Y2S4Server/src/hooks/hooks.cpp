#include "../utils/includes.h"

// Global DirectX/ImGui variables
Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
        {
            pDevice->GetImmediateContext(&pContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
            if (pBackBuffer)
            {
                pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
                pBackBuffer->Release();
            }
            else
            {
                return oPresent(pSwapChain, SyncInterval, Flags);
            }

            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            InitImGui(window, pDevice, pContext);

            init = true;
        }

        else
            return oPresent(pSwapChain, SyncInterval, Flags);
    }



    BeginGUIFrame();
    RenderGUI();
    EndGUIFrame(pContext, mainRenderTargetView);

    return oPresent(pSwapChain, SyncInterval, Flags);
    // GREEN [BLAME] Fixed missing closing curly brace
}