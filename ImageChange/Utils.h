#pragma once
#include <string>

// ���S�ȃt�@�C�������擾����֐�
std::wstring GetSafeFileName(const std::wstring& fileName);

// �g���q��ǉ�����֐�
std::wstring AddFileExtensionIfMissing(const std::wstring& filePath, const std::wstring& extension);