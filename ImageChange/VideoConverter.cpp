#include "VideoConverter.h"
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
#include <thread>

const wchar_t VideoConverter::CLASS_NAME[] = L"VideoConverterClass";
const wchar_t* FFmpegPath2 = L"C:\\Users\\tk060\\Desktop\\Take-labo-Application\\ImageChange\\bin\\ffmpeg.exe"; // FFmpeg�̃p�X���w��

VideoConverter::VideoConverter(HWND parent) {
    this->parent = parent;

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"����ϊ�",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        parent,
        nullptr,
        GetModuleHandle(NULL),
        this
    );

    if (hwnd == nullptr) {
        return;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);  // �E�B���h�E�̍X�V��ǉ�

    CreateButtons();  // �{�^���̍쐬���Ăяo��
}

void VideoConverter::Show() {
    ShowWindow(hwnd, SW_SHOW);
}

void VideoConverter::SetText(const std::wstring& newText) {
    text = newText;
    InvalidateRect(hwnd, nullptr, TRUE);  // �E�B���h�E�̍ĕ`���v��
}

void VideoConverter::CreateButtons() {
    CreateWindow(
        L"BUTTON",
        L"�t�@�C���I��",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        10,
        150,
        30,
        hwnd,
        (HMENU)ID_VIDEO_CONVERT_BUTTON,
        GetModuleHandle(NULL),
        nullptr
    );

    CreateWindow(
        L"BUTTON",
        L"�߂�",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        50,
        150,
        30,
        hwnd,
        (HMENU)ID_VIDEO_BACK_BUTTON,
        GetModuleHandle(NULL),
        nullptr
    );
}

void VideoConverter::StartConversion(const wchar_t* inputFilePath, const wchar_t* outputFilePath, const std::wstring& extension) {
    conversionThread = std::thread(&VideoConverter::ConvertVideo, this, inputFilePath, outputFilePath, extension);
    conversionThread.detach(); // �X���b�h�𕪗�
}

void VideoConverter::ConvertVideo(const wchar_t* inputFilePath, const wchar_t* outputFilePath, const std::wstring& extension) {
    std::wstring safeOutputFilePath = AddFileExtensionIfMissing(outputFilePath, extension);

    std::wstring command = L"\"";
    command += FFmpegPath2;
    command += L"\" -i \"";
    command += inputFilePath;
    command += L"\" \"";
    command += safeOutputFilePath;
    command += L"\"";

    // ���̓t�@�C���p�X�Əo�̓t�@�C���p�X�̊m�F�̂��߃��O�ɏo��
   /* std::wofstream debugLog(L"C:\\Users\\tk060\\Desktop\\debug_log.txt", std::ios::app);
    debugLog << L"Input File Path: " << inputFilePath << std::endl;
    debugLog << L"Output File Path: " << safeOutputFilePath << std::endl;
    debugLog << L"Command: " << command << std::endl;
    debugLog.close();*/

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    // �p�C�v���쐬���ĕW���o�͂ƕW���G���[�o�͂��L���v�`��
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

    if (result) {
        WaitForSingleObject(pi.hProcess, INFINITE);

        // �W���o�͂ƕW���G���[�o�͂�ǂݎ��
        std::ostringstream ss;
        DWORD bytesRead;
        CHAR buffer[4096];
        while (ReadFile(hStdOutputRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead != 0) {
            ss.write(buffer, bytesRead);
        }
        while (ReadFile(hStdErrorRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead != 0) {
            ss.write(buffer, bytesRead);
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hStdOutputRead);
        CloseHandle(hStdErrorRead);

        std::string output = ss.str();

        // UTF-8����UTF-16�ɕϊ�
        int utf16Size = MultiByteToWideChar(CP_UTF8, 0, output.c_str(), -1, NULL, 0);
        std::wstring utf16Output(utf16Size, 0);
        MultiByteToWideChar(CP_UTF8, 0, output.c_str(), -1, &utf16Output[0], utf16Size);

        // ���O���t�@�C���ɕۑ�
        /*std::wofstream logFile(L"C:\\Users\\tk060\\Desktop\\ffmpeg_output.log");
        logFile << utf16Output;
        logFile.close();*/

       // MessageBox(hwnd, utf16Output.c_str(), L"FFmpeg Output", MB_OK);
    }
    else {
        MessageBox(hwnd, L"FFmpeg�̎��s�Ɏ��s���܂���", L"�G���[", MB_OK);
    }
}

LRESULT CALLBACK VideoConverter::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    VideoConverter* pThis = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (VideoConverter*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->hwnd = hwnd;
    }
    else {
        pThis = (VideoConverter*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        switch (uMsg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_VIDEO_CONVERT_BUTTON) {
                // ����ϊ��������Ăяo��
                OPENFILENAME ofn = { 0 };
                wchar_t szFile[260];
                ZeroMemory(&szFile, sizeof(szFile));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = L"Video Files\0*.mp4;*.avi;*.mov;*.mkv;*.mp3\0";
                ofn.nFilterIndex = 1;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn)) {
                    std::wofstream debugLog(L"C:\\Users\\tk060\\Desktop\\debug_log.txt", std::ios::app);
                    debugLog << L"Selected Input File Path: " << szFile << std::endl;
                    debugLog.close();

                    wchar_t saveFile[260];
                    ZeroMemory(&saveFile, sizeof(saveFile));
                    OPENFILENAME ofnSave = { 0 };
                    ofnSave.lStructSize = sizeof(ofnSave);
                    ofnSave.hwndOwner = hwnd;
                    ofnSave.lpstrFile = saveFile;
                    ofnSave.nMaxFile = sizeof(saveFile);
                    ofnSave.lpstrFilter = L"MP4\0*.mp4\0AVI\0*.avi\0MOV\0*.mov\0MKV\0*.mkv\0MP3\0*.mp3\0";
                    ofnSave.nFilterIndex = 1;
                    ofnSave.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

                    if (GetSaveFileName(&ofnSave)) {
                        std::wofstream debugLogSave(L"C:\\Users\\tk060\\Desktop\\debug_log.txt", std::ios::app);
                        debugLogSave << L"Selected Output File Path: " << saveFile << std::endl;
                        debugLogSave.close();

                        // �t�B���^�̃C���f�b�N�X�Ɋ�Â��Ċg���q��ݒ�
                        std::wstring extension;
                        switch (ofnSave.nFilterIndex) {
                        case 1: extension = L".mp4"; break;
                        case 2: extension = L".avi"; break;
                        case 3: extension = L".mov"; break;
                        case 4: extension = L".mkv"; break;
                        case 5: extension = L".mp3"; break;
                        default: extension = L".mp4"; break;
                        }
                        pThis->StartConversion(ofn.lpstrFile, saveFile, extension);
                    }
                }
            }
            else if (LOWORD(wParam) == ID_VIDEO_BACK_BUTTON) {
                ShowWindow(pThis->hwnd, SW_HIDE);  // ���݂̃E�B���h�E���\���ɂ���
                ShowWindow(pThis->parent, SW_SHOW);  // ���C���E�B���h�E��\������
            }
            break;

        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc;
            hdc = BeginPaint(hwnd, &ps);
            if (!pThis->text.empty()) {  // �e�L�X�g���ݒ肳��Ă���ꍇ�̂ݕ`��
                RECT rect = { 270, 10, 530, 500 };  // �`�悷��̈��ݒ�
                DrawText(hdc, pThis->text.c_str(), -1, &rect, DT_LEFT | DT_WORDBREAK);  // �e�L�X�g���w�肵�����W�ɕ`��
            }
            EndPaint(hwnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
