#pragma once
#include <string>
#include <cstdlib>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "../gui/gui.h"
#include "../hooks/hooks.h"
#include "kiero.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#pragma comment(lib, "Ws2_32.lib")

extern std::string sdp_string; // its somewhere else dumbass, extern it from the header 

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;