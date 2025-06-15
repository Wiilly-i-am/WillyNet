#pragma once
#include <Windows.h>
#include <d3d11.h>

//ImGui hook WndProc handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void InitImGui(HWND window, ID3D11Device* device, ID3D11DeviceContext* context);

void BeginGUIFrame();

void RenderGUI();

void EndGUIFrame(ID3D11DeviceContext* context, ID3D11RenderTargetView* target);