#include "MainWindow.h"
#include "ImageConverter.h"
#include "AudioConverter.h"
#include "VideoConverter.h"
#include "resource.h"

const wchar_t MainWindow::CLASS_NAME[] = L"MainWindowClass";

MainWindow::MainWindow(HINSTANCE hInstance, int nCmdShow) 
{
    this->hInstance = hInstance;

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));  // アイコンの設定

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Take's Labo Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,
        nullptr,
        hInstance,
        this
    );

    if (hwnd == nullptr) 
    {
        return;
    }

    ShowWindow(hwnd, nCmdShow);
    CreateButtons();
}

//===== ボタン作成 =====
void MainWindow::CreateButtons() 
{
    //--- 画像変換
    CreateWindow(
        L"BUTTON",
        L"画像変換",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        10,
        150,
        30,
        hwnd,
        (HMENU)ID_CONVERT_BUTTON,
        hInstance,
        nullptr
    );

    //--- 音源変換
    CreateWindow(
        L"BUTTON",
        L"音源変換",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        50,
        150,
        30,
        hwnd,
        (HMENU)ID_AUDIO_CONVERT_BUTTON,
        hInstance,
        nullptr
    );

    CreateWindow(
        L"BUTTON",
        L"動画変換",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        90,
        150,
        30,
        hwnd,
        (HMENU)ID_VIDEO_CONVERT_BUTTON,
        hInstance,
        nullptr
    );

}

void MainWindow::OnButtonClicked(WPARAM wParam) 
{
    switch (LOWORD(wParam))
    {
    case ID_CONVERT_BUTTON:
    {
        ImageConverter* converter = new ImageConverter(hwnd);  // new で作成しポインタを保持
        converter->Show();
        ShowWindow(hwnd, SW_HIDE);  // メインウィンドウを非表示にする
    }
    break;

    case ID_AUDIO_CONVERT_BUTTON:  // 追加
    {
        AudioConverter* converter = new AudioConverter(hwnd);
        converter->Show();
        ShowWindow(hwnd, SW_HIDE);  // メインウィンドウを非表示にする
    }
    break;

    case ID_VIDEO_CONVERT_BUTTON:
    {
        VideoConverter* converter = new VideoConverter(hwnd);
        converter->SetText(L"設定した名前が反映されないバグ 戻るを押すと正常に動画が生成されます。");
        converter->Show();
        ShowWindow(hwnd, SW_HIDE);
    }
    }
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    MainWindow* pThis = nullptr;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (MainWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->hwnd = hwnd;
    }
    else 
    {
        pThis = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) 
    {
        switch (uMsg)
        {
        case WM_COMMAND:
            pThis->OnButtonClicked(wParam);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    else
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
