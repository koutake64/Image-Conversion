#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <shlwapi.h>
#include <webp/decode.h>
#include <vector>
#include <fstream>

#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "libwebp.lib")

using namespace Gdiplus;
using namespace std;

#define ID_CONVERT_BUTTON 1001  // �{�^��ID�̒�`
#define ID_BACK_BUTTON 1002     // �߂�{�^����ID��`

class ImageConverter 
{
public:
    ImageConverter(HWND parent);
    void Show();

private:
    HWND hwnd;
    HWND parent;
    static const wchar_t CLASS_NAME[];
    void ConvertImage(const wchar_t* inputFilePath, const wchar_t* outputFilePath, CLSID outputClsid);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void CreateButtons();  // �{�^�����쐬���郁�\�b�h�̐錾
};
