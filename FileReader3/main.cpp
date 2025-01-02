#include <windows.h>
#include "DXManager.h"
#include "Character.h"
#include <tchar.h>
#include <stdio.h>

Character g_character;
DXManager* g_pDxManager = nullptr;

bool InitializeWindow(HINSTANCE hInstance, int nCmdShow, HWND& hwnd);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_KEYDOWN:
        if (g_pDxManager) g_pDxManager->OnKeyDown(wParam);
        break;
    case WM_KEYUP:
        if (g_pDxManager) g_pDxManager->OnKeyUp(wParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,      
    _In_opt_ HINSTANCE hPrevInstance,  
    _In_ LPSTR lpCmdLine,        
    _In_ int nShowCmd             
)
{
    HWND hwnd;

    if (!InitializeWindow(hInstance, nShowCmd, hwnd)) {
        MessageBox(nullptr, _T("Window creation failed!"), _T("Error"),
            MB_OK | MB_ICONERROR);
        return -1;
    }

    g_pDxManager = new DXManager();

    if (!g_pDxManager->Initialize(hwnd)) {
        MessageBox(hwnd, _T("DirectX initialization failed!"), _T("Error"),
            MB_OK | MB_ICONERROR);
        delete g_pDxManager;
        return -1;
    }

    MSG msg = { 0 };
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            float deltaTime = 1.0f / 60.0f;
            g_pDxManager->Update(deltaTime);
            g_pDxManager->Render();
        }
    }

    if (g_pDxManager) {
        delete g_pDxManager;
        g_pDxManager = nullptr;
    }

    return (int)msg.wParam;
}

bool InitializeWindow(HINSTANCE hInstance, int nCmdShow, HWND& hwnd) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      _T("Movable Cube Demo"), NULL };
    RegisterClassEx(&wc);

    hwnd = CreateWindow(wc.lpszClassName, _T("Movable Cube Demo"),
        WS_OVERLAPPEDWINDOW, 100, 100, 1280, 720,
        NULL, NULL, wc.hInstance, NULL);
    if (!hwnd) {
        return false;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    return true;
}