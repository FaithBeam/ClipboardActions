#pragma once
#include <string>
#include <Windows.h>


std::string utf8_encode(const std::wstring&);
std::wstring utf8_decode(const std::string&);
// https://stackoverflow.com/a/54491532
template <typename TChar, typename TStringGetterFunc>
std::basic_string<TChar> GetStringFromWindowsApi(TStringGetterFunc stringGetter, int initialSize = 0)
{
    if (initialSize <= 0)
    {
        initialSize = MAX_PATH;
    }

    std::basic_string<TChar> result(initialSize, 0);
    for (;;)
    {
        auto length = stringGetter(&result[0], result.length());
        if (length == 0)
        {
            return std::basic_string<TChar>();
        }

        if (length < result.length() - 1)
        {
            result.resize(length);
            result.shrink_to_fit();
            return result;
        }

        result.resize(result.length() * 2);
    }
}