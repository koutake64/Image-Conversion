#pragma once
#include <string>

// 安全なファイル名を取得する関数
std::wstring GetSafeFileName(const std::wstring& fileName);

// 拡張子を追加する関数
std::wstring AddFileExtensionIfMissing(const std::wstring& filePath, const std::wstring& extension);