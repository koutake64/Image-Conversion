#pragma once

#include <windows.h>
#include <string>
#include <thread>

#define ID_VIDEO_CONVERT_BUTTON 2001  // ボタンIDの定義
#define ID_VIDEO_BACK_BUTTON 2002     // 戻るボタンのID定義

class VideoConverter {
public:
    VideoConverter(HWND parent);
    void Show();
    void SetText(const std::wstring& text);  // テキストを設定する関数

private:
    HWND hwnd;
    HWND parent;
    static const wchar_t CLASS_NAME[];
    std::thread conversionThread;
    std::wstring text;  // 表示するテキストを格納するメンバ変数

    void StartConversion(const wchar_t* inputFilePath, const wchar_t* outputFilePath, const std::wstring& extension);
    void ConvertVideo(const wchar_t* inputFilePath, const wchar_t* outputFilePath, const std::wstring& extension);
    void CreateButtons();
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
