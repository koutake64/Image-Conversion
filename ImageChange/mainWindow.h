#pragma once

#include <windows.h>
#include "ImageConverter.h"
#include "AudioConverter.h"  // 追加

#define ID_CONVERT_BUTTON 1001  // ボタンIDの定義
#define ID_AUDIO_CONVERT_BUTTON 1003  // 音源変換ボタンのID定義

class MainWindow
{
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
