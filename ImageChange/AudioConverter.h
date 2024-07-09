#pragma once

#include <windows.h>
#include <string>
#include "resource.h"

#define ID_AUDIO_CONVERT_BUTTON 1003  // 音源変換ボタンのID定義
#define ID_AUDIO_BACK_BUTTON 1004     // 戻るボタンのID定義

class AudioConverter
{
public:
    AudioConverter(HWND parent);
    void Show();

private:
    HWND hwnd;
    HWND parent;
    static const wchar_t CLASS_NAME[];
    void ConvertAudio(const wchar_t* inputFilePath, const wchar_t* outputFilePath, const std::wstring& extension);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void CreateButtons();  // ボタンを作成するメソッドの宣言
};
