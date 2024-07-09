#include "AudioConverter.h"
#include "Utils.h"
#include <shlwapi.h>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <windows.h>
#include <string>
#include <sstream>
#include <iostream>
#include <filesystem>

//-- クラス名
const wchar_t AudioConverter::CLASS_NAME[] = L"AudioConverterClass";
//-- FFmpegのパス
const wchar_t* FFmpegPath = L"C:\\Users\\tk060\\Desktop\\Take-labo-Application\\ImageChange\\bin\\ffmpeg.exe"; // FFmpegのパスを指定

// === コンストラクタ ===
AudioConverter::AudioConverter(HWND parent) 
{
    this->parent = parent;

    //--- ウィンドウクラスの設定
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    //--- ウィンドウクラスの登録
    RegisterClass(&wc);

    //--- ウィンドウの作成
    hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"音源変換",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        parent,
        nullptr,
        GetModuleHandle(NULL),
        this
    );

    if (hwnd == nullptr) 
    {
        return;
    }

    ShowWindow(hwnd, SW_SHOW);  // ウィンドウの表示
    UpdateWindow(hwnd);         // ウィンドウの更新を追加

    CreateButtons();            // ボタンの作成を呼び出し
}

//=== ウィンドウ表示方法 ===
void AudioConverter::Show() 
{
    ShowWindow(hwnd, SW_SHOW);
}

// === ボタン作成 ===
void AudioConverter::CreateButtons()
{
    //--- ファイル選択
    CreateWindow(
        L"BUTTON",
        L"ファイル選択",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        10,
        150,
        30,
        hwnd,
        (HMENU)ID_AUDIO_CONVERT_BUTTON,
        GetModuleHandle(NULL),
        nullptr
    );

    //--- 戻る
    CreateWindow(
        L"BUTTON",
        L"戻る",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        50,
        150,
        30,
        hwnd,
        (HMENU)ID_AUDIO_BACK_BUTTON,
        GetModuleHandle(NULL),
        nullptr
    );
}

//=== 音声ファイルを変換処理 ===
void AudioConverter::ConvertAudio(const wchar_t* inputFilePath, const wchar_t* outputFilePath, const std::wstring& extension) 
{
    std::wstring safeOutputFilePath = GetSafeFileName(outputFilePath);
    safeOutputFilePath = AddFileExtensionIfMissing(safeOutputFilePath, extension);

    //--- FFmpegコマンド構築
    std::wstring command = L"\"";
    command += FFmpegPath;
    command += L"\" -i \"";
    command += inputFilePath;
    command += L"\" \"";
    command += safeOutputFilePath;
    command += L"\"";

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    // エラー出力
    HANDLE hStdOutputRead, hStdOutputWrite;
    HANDLE hStdErrorRead, hStdErrorWrite;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    CreatePipe(&hStdOutputRead, &hStdOutputWrite, &sa, 0);
    CreatePipe(&hStdErrorRead, &hStdErrorWrite, &sa, 0);
    SetHandleInformation(hStdOutputRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hStdErrorRead, HANDLE_FLAG_INHERIT, 0);
    si.hStdOutput = hStdOutputWrite;
    si.hStdError = hStdErrorWrite;

    BOOL result = CreateProcessW(NULL, &command[0], NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    CloseHandle(hStdOutputWrite);
    CloseHandle(hStdErrorWrite);

    if (result) 
    {
        WaitForSingleObject(pi.hProcess, INFINITE);

        // 標準出力と標準エラー出力を読み取る
        std::ostringstream ss;
        DWORD bytesRead;
        CHAR buffer[4096];
        while (ReadFile(hStdOutputRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead != 0)
        {
            ss.write(buffer, bytesRead);
        }

        while (ReadFile(hStdErrorRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead != 0)
        {
            ss.write(buffer, bytesRead);
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hStdOutputRead);
        CloseHandle(hStdErrorRead);

        std::string output = ss.str();

        /*
        // UTF-8からUTF-16に変換
        int utf16Size = MultiByteToWideChar(CP_UTF8, 0, output.c_str(), -1, NULL, 0);
        std::wstring utf16Output(utf16Size, 0);
        MultiByteToWideChar(CP_UTF8, 0, output.c_str(), -1, &utf16Output[0], utf16Size);

        // ログをファイルに保存
       std::wofstream logFile(L"C:\\Users\\tk060\\Documents\\Take-labo-Application\\ImageChange\\bin\\ffmpeg_output.log");
        logFile << utf16Output;
        logFile.close();

       MessageBox(hwnd, utf16Output.c_str(), L"FFmpeg Output", MB_OK);
       */
    }
    else
    {
        MessageBox(hwnd, L"FFmpegの実行に失敗しました", L"エラー", MB_OK);
    }
}

LRESULT CALLBACK AudioConverter::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    AudioConverter* pThis = nullptr;

    if (uMsg == WM_NCCREATE) 
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (AudioConverter*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->hwnd = hwnd;
    }
    else
    {
        pThis = (AudioConverter*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) 
    {
        switch (uMsg)
        {
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_AUDIO_CONVERT_BUTTON)
            {
                // 音源変換処理を呼び出し
                OPENFILENAME ofn = { 0 };
                wchar_t szFile[260];
                ZeroMemory(&szFile, sizeof(szFile));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = L"Audio Files\0*.mp3;*.wav;*.aif;*.mp4;*.aac\0";
                ofn.nFilterIndex = 1;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn)) 
                {
                    wchar_t saveFile[260];
                    ZeroMemory(&saveFile, sizeof(saveFile));
                    OPENFILENAME ofnSave = { 0 };
                    ofnSave.lStructSize = sizeof(ofnSave);
                    ofnSave.hwndOwner = hwnd;
                    ofnSave.lpstrFile = saveFile;
                    ofnSave.nMaxFile = sizeof(saveFile);
                    ofnSave.lpstrFilter = L"MP3\0*.mp3\0WAV\0*.wav\0AIF\0*.aif\0MP4\0*.mp4\0AAC\0*.aac\0";
                    ofnSave.nFilterIndex = 1;
                    ofnSave.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

                    if (GetSaveFileName(&ofnSave))
                    {
                        // フィルタのインデックスに基づいて拡張子を設定
                        std::wstring extension;
                        switch (ofnSave.nFilterIndex)
                        {
                        case 1: extension = L".mp3"; break;
                        case 2: extension = L".wav"; break;
                        case 3: extension = L".aif"; break;
                        case 4: extension = L".mp4"; break;
                        case 5: extension = L".aac"; break;
                        default: extension = L".mp3"; break;
                        }
                        pThis->ConvertAudio(ofn.lpstrFile, ofnSave.lpstrFile, extension);
                    }
                }
            }
            else if (LOWORD(wParam) == ID_AUDIO_BACK_BUTTON) 
            {
                ShowWindow(pThis->hwnd, SW_HIDE);  // 現在のウィンドウを非表示にする
                ShowWindow(pThis->parent, SW_SHOW);  // メインウィンドウを表示する
            }
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
