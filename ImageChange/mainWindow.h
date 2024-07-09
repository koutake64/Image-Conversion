#pragma once

#include <windows.h>
#include "ImageConverter.h"
#include "AudioConverter.h"  // �ǉ�

#define ID_CONVERT_BUTTON 1001  // �{�^��ID�̒�`
#define ID_AUDIO_CONVERT_BUTTON 1003  // �����ϊ��{�^����ID��`

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
