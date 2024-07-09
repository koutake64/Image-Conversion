#include "Utils.h"
#include <string>

std::wstring GetSafeFileName(const std::wstring& fileName)
{
    std::wstring safeFileName = fileName;
    const wchar_t* invalidChars = L"<>:\"/\\|?*";
    for (wchar_t& ch : safeFileName)
    {
        if (wcschr(invalidChars, ch))
        {
            ch = L'_';
        }
    }
    return safeFileName;
}

std::wstring AddFileExtensionIfMissing(const std::wstring& filePath, const std::wstring& extension)
{
    if (filePath.length() >= extension.length()) 
    {
        if (filePath.compare(filePath.length() - extension.length(), extension.length(), extension) == 0) 
        {
            return filePath;
        }
    }
    return filePath + extension;
}
