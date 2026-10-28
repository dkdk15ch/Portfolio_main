#pragma once
//  安全にポインタの開放をするための関数・マクロ
/////////////////////////////////////////

//  マクロ類
//  privateメンバを削除するときテンプレートではできないため、こっちを使う
#define SAFE_DELETE( x ) { delete ( x ); ( x ) = 0; }
#define SAFE_DELETE_ARRAY( x ) { delete[] ( x ); ( x ) = 0; }

//-----------------------------------------------------------------------------
//      nullptr を考慮して delete 処理を行います.
//-----------------------------------------------------------------------------
template<typename T>
inline void SafeDelete(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete ptr;
        ptr = nullptr;
    }
}

//-----------------------------------------------------------------------------
//      nullptr を考慮して delete[] 処理を行います.
//-----------------------------------------------------------------------------
template<typename T>
inline void SafeDeleteArray(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete[] ptr;
        ptr = nullptr;
    }
}

//-----------------------------------------------------------------------------
//      nullptr を考慮して Release() メソッドを呼び出します.
//-----------------------------------------------------------------------------
template<typename T>
inline void SafeRelease(T*& ptr)
{
    if (ptr != nullptr)
    {
        ptr->Release();
        ptr = nullptr;
    }
}

//-----------------------------------------------------------------------------
//      nullptr を考慮して Term() メソッドを呼び出し, delete 処理を行います.
//-----------------------------------------------------------------------------
template<typename T>
inline void SafeTerm(T*& ptr)
{
    if (ptr != nullptr)
    {
        ptr->Term();
        delete ptr;
        ptr = nullptr;
    }
}
