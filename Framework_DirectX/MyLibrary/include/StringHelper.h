#pragma once
#include <windows.h>
#include <string>
#include <vector>

//  文字列変換用関数
///////////////////////

inline std::string WStringToString(const std::wstring& wstr)
 {
    if (wstr.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

    return strTo;
}

inline std::wstring StringToWString(const std::string& str)
{
    if (str.empty()) return std::wstring();

    int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

    return wstrTo;
}

inline std::vector<std::wstring> SplitW(const std::wstring& input, const std::wstring& delimiter)
{
    std::vector<std::wstring> tokens;
    if (delimiter.empty()) return { input };

    size_t start = 0;
    size_t end = input.find(delimiter);

    while (end != std::wstring::npos)
    {
        tokens.push_back(input.substr(start, end - start));
        start = end + delimiter.length();
        end = input.find(delimiter, start);
    }

    tokens.push_back(input.substr(start));

    return tokens;
}