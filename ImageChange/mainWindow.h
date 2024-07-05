#pragma once

#include <windows.h>

#define ID_CONVERT_BUTTON 1001  // ƒ{ƒ^ƒ“ID‚Ì’è‹`

class MainWindow {
public:
    MainWindow(HINSTANCE hInstance, int nCmdShow);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HWND hwnd;
    static const wchar_t CLASS_NAME[];
    HINSTANCE hInstance;
    void CreateButtons();
    void OnButtonClicked(WPARAM wParam);
};
