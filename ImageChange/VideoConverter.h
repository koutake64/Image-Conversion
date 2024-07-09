#pragma once

#include <windows.h>
#include <string>
#include <thread>

#define ID_VIDEO_CONVERT_BUTTON 2001  // �{�^��ID�̒�`
#define ID_VIDEO_BACK_BUTTON 2002     // �߂�{�^����ID��`

class VideoConverter {
public:
    VideoConverter(HWND parent);
    void Show();
    void SetText(const std::wstring& text);  // �e�L�X�g��ݒ肷��֐�

private:
    HWND hwnd;
    HWND parent;
    static const wchar_t CLASS_NAME[];
    std::thread conversionThread;
    std::wstring text;  // �\������e�L�X�g���i�[���郁���o�ϐ�

    void StartConversion(const wchar_t* inputFilePath, const wchar_t* outputFilePath, const std::wstring& extension);
    void ConvertVideo(const wchar_t* inputFilePath, const wchar_t* outputFilePath, const std::wstring& extension);
    void CreateButtons();
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
