#include "registry_functions.hpp"

void set_startup()
{

    std::basic_string<TCHAR> path = GetStringFromWindowsApi<TCHAR>([](TCHAR *buffer, int size)
                                                                   { return ::GetModuleFileName(nullptr, buffer, size); });

    HKEY k{};
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0,
                      KEY_ALL_ACCESS, &k))
    {
        RegSetKeyValueW(k, nullptr, L"ClipboardActions", REG_SZ, path.c_str(),
                        static_cast<DWORD>(path.size() + 1) * sizeof(wchar_t));
        RegCloseKey(k);
    }
}

void unset_startup()
{
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0,
                      KEY_ALL_ACCESS, &k))
    {
        RegDeleteValueW(k, L"ClipboardActions");
        RegCloseKey(k);
    }
}

bool get_startup_status()
{
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0,
                      KEY_READ, &k))
    {
        LSTATUS val_result = RegGetValueW(k, nullptr, L"ClipboardActions", RRF_RT_REG_SZ, nullptr, nullptr, nullptr);
        RegCloseKey(k);
        return val_result == ERROR_SUCCESS;
    }
    return false;
}