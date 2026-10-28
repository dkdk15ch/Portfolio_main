#pragma once
#include <string>
#include <Shlwapi.h>
#include "Logger.h"
#pragma comment(lib, "shlwapi.lib")

/////////////////////
//  フェイル検索用関数
/////////////////////

bool SearchFilePath(const char* filename, std::string& result);
bool SearchFilePathW(const wchar_t* filename, std::wstring& result);
//  パス正規化
std::wstring NormalizePath(const std::wstring& path);
//  ディレクトリパス取得・除去
std::wstring GetDirectoryPathW(const wchar_t* path);
std::wstring RemoveDirectoryPathW(const std::wstring& path);
std::string GetDirectoryPath(const char* path);
std::string RemoveDirectoryPath(const std::string& path);
std::string TrimBefore(const std::string& path, const std::string& target);

//  拡張子変換
std::wstring GetExtension(const std::wstring& filename);
std::wstring ConvFileExtension(const std::wstring& filename,const std::wstring& newExtension);
std::string RemoveExtension(const std::string& filename);
std::wstring RemoveExtensionW(const std::wstring& filename);
//  パスの取得
std::wstring GetFilePathW(const wchar_t* filename);
std::string GetFilePath(const char* filename);
//  ファイル名のみ
std::wstring GetFileNameW(const wchar_t* path);
std::string GetFileName(const wchar_t* path);
//  上位フォルダ付きのファイル名
std::wstring GetFolderAndFileNameW(const wchar_t* path);

/**
 * @brief           oldStrをnewStrに置き換え
 *                  Assets/Texture/Player/
 *                  Assets/Mesh/Player/ など
 * @param str       文字列（パスなど）
 * @param oldStr    変更部分
 * @param newStr    置き換え文字列
 * @return 
 */
std::string Replace(
    const std::string& input,
    const std::string& oldStr,
    const std::string& newStr);

std::wstring ReplaceW(
    const std::wstring& input,
    const std::wstring& oldStr,
    const std::wstring& newStr);


#if defined(UNICODE) || defined(_UNICODE)
inline bool SearchFilePath(const wchar_t* filename, std::wstring& result)
{
    return SearchFilePathW(filename, result);
}

inline std::wstring RemoveDirectoryPath(const std::wstring& path)
{
    return RemoveDirectoryPathW(path);
}

inline std::wstring GetDirectoryPath(const wchar_t* path)
{
    return GetDirectoryPathW(path);
}
#else
inline bool SearchFilePath(const char* filename, std::string& result)
{
    return SearchFilePathA(filename, result);
}

inline std::string RemoveDirectoryPath(const std::string& path)
{
    return RemoveDirectoryPathA(path);
}

inline std::string GetDirectoryPath(const char* path)
{
    return GetDirectoryPathA(path);
}
#endif//defined(UNICODE) || defined(_UNICODE)

