#include "ImageConverter.h"
#include <iostream>

const wchar_t ImageConverter::CLASS_NAME[] = L"ImageConverterClass";

ImageConverter::ImageConverter(HWND parent) 
{
    this->parent = parent;

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"画像変換",
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

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);  // ウィンドウの更新を追加

    CreateButtons();  // ボタンの作成を呼び出し
}

void ImageConverter::Show()
{
    ShowWindow(hwnd, SW_SHOW);
}

void ImageConverter::CreateButtons()
{
    CreateWindow(
        L"BUTTON",
        L"ファイル選択",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        10,
        150,
        30,
        hwnd,
        (HMENU)ID_CONVERT_BUTTON,
        GetModuleHandle(NULL),
        nullptr
    );

    CreateWindow(
        L"BUTTON",
        L"戻る",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        50,
        150,
        30,
        hwnd,
        (HMENU)ID_BACK_BUTTON,
        GetModuleHandle(NULL),
        nullptr
    );
}

std::vector<uint8_t> LoadFile(const wchar_t* filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        return {};
    }
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
}

void ImageConverter::ConvertImage(const wchar_t* inputFilePath, const wchar_t* outputFilePath, CLSID outputClsid) 
{
    std::wstring input(inputFilePath);
    if (input.find(L".webp") != std::wstring::npos) 
    {
        auto fileData = LoadFile(inputFilePath);
        if (fileData.empty()) 
        {
            MessageBox(NULL, L"Failed to load WebP image.", L"Error", MB_OK);
            return;
        }

        int width = 0, height = 0;
        uint8_t* data = WebPDecodeRGBA(fileData.data(), fileData.size(), &width, &height);
        if (!data)
        {
            MessageBox(NULL, L"Failed to decode WebP image.", L"Error", MB_OK);
            return;
        }

        Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
        BitmapData bitmapData;
        Gdiplus::Rect rect(0, 0, width, height);
        bitmap->LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);
        memcpy(bitmapData.Scan0, data, width * height * 4);
        bitmap->UnlockBits(&bitmapData);

        if (bitmap->GetLastStatus() == Ok)
        {
            bitmap->Save(outputFilePath, &outputClsid, NULL);
        }
        else 
        {
            MessageBox(NULL, L"Failed to save image.", L"Error", MB_OK);
        }

        delete bitmap;
        std::free(data);  // メモリを解放する
    }
    else
    {
        Image* image = new Image(inputFilePath);
        if (image->GetLastStatus() == Ok) 
        {
            image->Save(outputFilePath, &outputClsid, NULL);
        }
        else 
        {
           MessageBox(NULL, L"Failed to load image.", L"Error", MB_OK);
        }
        delete image;
    }
}

LRESULT CALLBACK ImageConverter::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ImageConverter* pThis = nullptr;

    if (uMsg == WM_NCCREATE) 
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (ImageConverter*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        pThis->hwnd = hwnd;
    }
    else 
    {
        pThis = (ImageConverter*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis)
    {
        switch (uMsg) 
        {
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_CONVERT_BUTTON)
            {
                //MessageBox(hwnd, L"Convert Button Clicked", L"Debug", MB_OK); // デバッグメッセージ
                // ここに画像変換処理を追加
                OPENFILENAME ofn = { 0 };
                wchar_t szFile[260];
                ZeroMemory(&szFile, sizeof(szFile));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = L"Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.webp;*.ico\0";
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
                    ofnSave.lpstrFilter = L"JPEG\0*.jpg\0PNG\0*.png\0BMP\0*.bmp\0WebP\0*.webp\0ICO\0*.ico\0";
                    ofnSave.nFilterIndex = 1;
                    ofnSave.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

                    if (GetSaveFileName(&ofnSave))
                    {
                        CLSID clsid;
                        const wchar_t* extension = nullptr;
                        HRESULT hr = S_OK;
                        if (ofnSave.nFilterIndex == 1) 
                        {
                            hr = CLSIDFromString(L"{557CF401-1A04-11D3-9A73-0000F81EF32E}", &clsid); // JPEG
                            extension = L".jpg";
                        }
                        else if (ofnSave.nFilterIndex == 2) 
                        {
                            hr = CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &clsid); // PNG
                            extension = L".png";
                        }
                        else if (ofnSave.nFilterIndex == 3) 
                        {
                            hr = CLSIDFromString(L"{557CF400-1A04-11D3-9A73-0000F81EF32E}", &clsid); // BMP
                            extension = L".bmp";
                        }
                        else if (ofnSave.nFilterIndex == 4) 
                        {
                            hr = CLSIDFromString(L"{6FDDC324-4E03-4BFE-B185-3D77768DC904}", &clsid); // WebP
                            extension = L".webp";
                        }
                        else if (ofnSave.nFilterIndex == 5) 
                        {
                            hr = CLSIDFromString(L"{557CF401-1A04-11D3-9A73-0000F81EF32E}", &clsid); // ICO
                            extension = L".ico";
                        }

                        if (SUCCEEDED(hr)) 
                        {
                            if (!PathFindExtension(saveFile) || lstrcmp(PathFindExtension(saveFile), L"") == 0)
                            {
                                lstrcat(saveFile, extension);
                            }
                            pThis->ConvertImage(ofn.lpstrFile, ofnSave.lpstrFile, clsid);
                        }
                        else 
                        {
                            MessageBox(hwnd, L"Failed to get CLSID.", L"Error", MB_OK);
                        }
                    }
                }
            }
            else if (LOWORD(wParam) == ID_BACK_BUTTON) 
            {
               // MessageBox(hwnd, L"Back Button Clicked", L"Debug", MB_OK); // デバッグメッセージ
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
