#include "FileUtil.h"
#include <windows.h>
#include <string>
#include <sstream>
#include "StringHelper.h"

namespace {

    // 文字列を置換
    std::wstring Replace(
        const std::wstring& input,
        std::wstring         pattern,
        std::wstring         replace)
    {
        std::wstring result = input;
        auto pos = result.find(pattern);

        while (pos != std::wstring::npos)
        {
            result.replace(pos, pattern.length(), replace);
            pos = result.find(pattern, pos + replace.length());
        }

        return result;
    }

} // namespace


static void DebugLog(const wchar_t* fmt, ...)
{
    wchar_t buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vswprintf_s(buf, _countof(buf), fmt, ap);
    va_end(ap);
    OutputDebugStringW(buf);
    OutputDebugStringW(L"\n");
}


void DebugCheckPathAndPermissions(const wchar_t* path)
{
    DebugLog(L"--- DebugCheckPathAndPermissions START ---");
    DebugLog(L"Original path: [%s]", path);

    // '/' を '\' に変換
    std::wstring norm = path;
    for (wchar_t& c : norm) if (c == L'/') c = L'\\';
    DebugLog(L"Normalized path: [%s]", norm.c_str());

    // 現在の作業ディレクトリも出力
    wchar_t cwd[MAX_PATH] = {};
    DWORD cwdLen = GetCurrentDirectoryW(_countof(cwd), cwd);
    if (cwdLen == 0) DebugLog(L"GetCurrentDirectoryW failed. err=%u", GetLastError());
    else DebugLog(L"CurrentDirectory: [%s]", cwd);

    // フルパス化
    wchar_t full[MAX_PATH] = {};
    DWORD ret = GetFullPathNameW(norm.c_str(), _countof(full), full, nullptr);
    if (ret == 0) {
        DebugLog(L"GetFullPathNameW failed. GetLastError=%u", GetLastError());
    }
    else if (ret >= _countof(full)) {
        DebugLog(L"GetFullPathNameW truncated. ret=%u", ret);
    }
    else {
        DebugLog(L"FullPathName: [%s]", full);
    }

    // 属性チェック
    DWORD attr = GetFileAttributesW(full);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        DebugLog(L"GetFileAttributesW -> INVALID_FILE_ATTRIBUTES, GetLastError=%u", GetLastError());
    }
    else {
        DebugLog(L"GetFileAttributesW -> 0x%08X (isDir=%s)", attr, (attr & FILE_ATTRIBUTE_DIRECTORY) ? L"YES" : L"NO");
    }

    // CreateFile で開けるか試す（読み取り専用で試す）
    HANDLE h = CreateFileW(full, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        DebugLog(L"CreateFileW failed. GetLastError=%u", GetLastError());
    }
    else {
        DebugLog(L"CreateFileW succeeded.");
        CloseHandle(h);
    }

    DebugLog(L"--- DebugCheckPathAndPermissions END ---");
}

bool SearchFilePath(const char* filename, std::string& result) {
    if (filename == nullptr)
    {
        return false;
    }

    if (strcmp(filename, " ") == 0 || strcmp(filename, "") == 0)
    {
        return false;
    }

    char exePath[520] = {};
    GetModuleFileNameA(nullptr, exePath, 520);
    exePath[519] = '\0'; // null終端化.
    PathRemoveFileSpecA(exePath);

    char dstPath[520] = {};

    strcpy_s(dstPath, filename);
    if (PathFileExistsA(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    sprintf_s(dstPath, "..\\%s", filename);
    if (PathFileExistsA(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    sprintf_s(dstPath, "..\\..\\%s", filename);
    if (PathFileExistsA(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    sprintf_s(dstPath, "\\res\\%s", filename);
    if (PathFileExistsA(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    sprintf_s(dstPath, "%s\\%s", exePath, filename);
    if (PathFileExistsA(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    sprintf_s(dstPath, "%s\\..\\%s", exePath, filename);
    if (PathFileExistsA(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    sprintf_s(dstPath, "%s\\..\\..\\%s", exePath, filename);
    if (PathFileExistsA(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    sprintf_s(dstPath, "%s\\res\\%s", exePath, filename);
    if (PathFileExistsA(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    return false;

}


bool SearchFilePathW(const wchar_t* filename, std::wstring& result)
{
    if (filename == nullptr)
    {
        return false;
    }

    if (wcscmp(filename, L" ") == 0 || wcscmp(filename, L"") == 0)
    {
        return false;
    }

    wchar_t exePath[520] = {};
    GetModuleFileNameW(nullptr, exePath, 520);
    exePath[519] = L'\0'; // null終端化.
    PathRemoveFileSpecW(exePath);

    wchar_t dstPath[520] = {};

    wcscpy_s(dstPath, filename);
    if (PathFileExistsW(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    swprintf_s(dstPath, L"..\\%s", filename);
    if (PathFileExistsW(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    swprintf_s(dstPath, L"..\\..\\%s", filename);
    if (PathFileExistsW(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    swprintf_s(dstPath, L"\\res\\%s", filename);
    if (PathFileExistsW(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    swprintf_s(dstPath, L"%s\\%s", exePath, filename);
    if (PathFileExistsW(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    swprintf_s(dstPath, L"%s\\..\\%s", exePath, filename);
    if (PathFileExistsW(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    swprintf_s(dstPath, L"%s\\..\\..\\%s", exePath, filename);
    if (PathFileExistsW(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    swprintf_s(dstPath, L"%s\\res\\%s", exePath, filename);
    if (PathFileExistsW(dstPath) == TRUE)
    {
        result = dstPath;
        return true;
    }

    return false;
}

std::wstring NormalizePath(const std::wstring& path)
{
    std::wstring result = ReplaceW(path, L"\\", L"/");
    // . と / 以外の先頭を見つけてそれ以降を取り出す。
    size_t start = result.find_first_not_of(L"./");
    if (start != std::wstring::npos)
    {
        return result.substr(start);
    }

    return result;
}


std::wstring RemoveDirectoryPathW(const std::wstring& path)
{
    auto temp = Replace(path, L"\\", L"/");
    auto pos = temp.rfind(L'/');

    if (pos != std::wstring::npos)
    {
        return temp.substr(pos + 1);
    }

    return path;
}

std::string TrimBefore(const std::string& path, const std::string& target)
{
    std::string temp = Replace(path, "\\", "/");
    size_t pos = temp.find(target);
    if (pos != std::string::npos) {
        return temp.substr(pos); 
    }
    return temp;
}

std::string RemoveDirectoryPath(const std::string& path)
{
    auto temp = Replace(path, "\\", "/");
    auto pos = temp.rfind('/');

    if (pos != std::string::npos)
    {
        return temp.substr(pos + 1);
    }

    return path;
}

std::string GetDirectoryPath(const char* filePath)
{
    std::string path = Replace(filePath, "\\", "/");
    size_t idx = path.find_last_of("/");
    if (idx != std::wstring::npos)
    {
        return path.substr(0, idx + 1);
    }

    return std::string();
}


std::wstring GetDirectoryPathW(const wchar_t* filePath)
{
    std::wstring path = Replace(filePath, L"\\", L"/");
    size_t idx = path.find_last_of(L"/");
    if (idx != std::wstring::npos)
    {
        return path.substr(0, idx + 1);
    }

    return std::wstring();
}


std::wstring GetExtension(const std::wstring& filename)
{
    size_t dot = filename.find_last_of(L'.');
    if (dot == std::wstring::npos)
    {
        return L"";
    }
    return filename.substr(dot);
}

std::wstring ConvFileExtension(const std::wstring& filename,const std::wstring& newExtension)
{
    size_t dot = filename.find_last_of(L'.');
    //  拡張子がない場合はそのまま返す
    if (dot == std::wstring::npos)
    {
        return filename;
    }

    return filename.substr(0, dot) + newExtension;

}

std::string RemoveExtension(const std::string& filename)
{
    auto lastSlash = filename.find_last_of("/\\");
    auto dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos && (lastSlash == std::string::npos || dotPos > lastSlash)) {
        return filename.substr(0, dotPos);
    }
    return filename;
}

std::wstring RemoveExtensionW(const std::wstring& filename)
{
    auto lastSlash = filename.find_last_of(L"/\\");
    auto dotPos = filename.find_last_of(L'.');
    if (dotPos != std::wstring::npos && (lastSlash == std::wstring::npos || dotPos > lastSlash)) {
        return filename.substr(0, dotPos);
    }
    return filename;
}

std::wstring GetFilePathW(const wchar_t* filename)
{
    std::wstring findPath;
    if (!SearchFilePathW(filename, findPath))
    {
        ELOG("Error : GetFilePath - not found path");
        return L"";
    }
    // ファイル名であることをチェック.
    {
        if (PathIsDirectoryW(findPath.c_str()) != FALSE)
        {
            ELOG("Error : GetFilePath - path isnt directry");
            return L"";
        }
    }

    return findPath;
}

std::string GetFilePath(const char* filename)
{
    std::string findPath;
    if (!SearchFilePath(filename, findPath))
    {
        ELOG("Error : GetFilePath - not found path");
        return "";
    }
    // ファイル名であることをチェック.
    {
        auto pathW = StringToWString(findPath);
        if (PathIsDirectory(pathW.c_str()) != FALSE)
        {
            ELOG("Error : GetFilePath - path isnt directry");
            return "";
        }
    }

    return findPath;
}

std::wstring GetFileNameW(const wchar_t* path)
{
    std::wstring result;
    result = RemoveDirectoryPathW(path);
    result = RemoveExtensionW(result);
    return result;
}

std::string GetFileName(const wchar_t* path)
{
    std::string result;
    std::string p = WStringToString(path);
    result = RemoveDirectoryPath(p);
    result = RemoveExtension(result);
    return result;
}

std::wstring GetFolderAndFileNameW(const wchar_t* path)
{
    auto temp = ReplaceW(path, L"\\", L"/");
    auto lastPos = temp.rfind(L'/');
    if (lastPos == std::wstring::npos)
    {
        return temp; // 区切り文字がない場合はそのまま
    }
    auto secondLastPos = temp.rfind(L'/', lastPos - 1);
    if (secondLastPos == std::wstring::npos)
    {
        return temp; // '/' が1つしかない場合はそのまま
    }
    return temp.substr(secondLastPos + 1);
}

std::string Replace(const std::string& input, const std::string& oldStr, const std::string& newStr)
{
    std::string result = input;
    auto pos = result.find(oldStr);

    while (pos != std::string::npos)
    {
        result.replace(pos, oldStr.length(), newStr);
        pos = result.find(oldStr, pos + newStr.length());
    }
    return result;
}
std::wstring ReplaceW(
    const std::wstring& input,
    const std::wstring& oldStr,
    const std::wstring& newStr) {
    if (oldStr.empty()) return input;

    return Replace(input, oldStr, newStr);
}
