#pragma once
#include "FileUtil.h"

#if defined(DEBUG) || defined(_DEBUG)

//	デバッグメッセージ用関数
////////////////////////

/**
 * @brief				ログを出力
 * @param[in] format	フォーマットです
 * @param  
 */
inline void OutputLog(const char* format, ...) {
    char buf[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    OutputDebugStringA(buf);
}

#define DELOG(format, ...) \
    OutputLog("%s [file:%s - Line:%d]\n", \
        std::format(format, ##__VA_ARGS__).c_str(), \
        RemoveDirectoryPath(__FILE__).c_str(), \
        __LINE__)

#define ELOG(format, ...) \
    OutputLog("[File : %s, Line : %d] " format "\n", \
        RemoveDirectoryPath(__FILE__).c_str(), \
        __LINE__, \
        ##__VA_ARGS__)

#else

#define DELOG(format, ...) ((void)0)
#define ELOG(format, ...)  ((void)0)

#endif