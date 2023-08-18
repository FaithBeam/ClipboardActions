#include "registry_functions.hpp"

void set_startup()
{
    const std::basic_string<wchar_t> path = GetStringFromWindowsApi<TCHAR>([](TCHAR* buffer, const int size)
    {
        return GetModuleFileName(nullptr, buffer, size);
    });

    HKEY k{};
    LSTATUS open_result = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0, KEY_ALL_ACCESS, &k);
    LSTATUS result = RegSetKeyValueW(k, nullptr, L"ClipboardActions", REG_SZ, path.c_str(), (path.size() + 1) * sizeof(wchar_t));
    RegCloseKey(k);
}

void unset_startup()
{
    HKEY k;
    LSTATUS result = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0, KEY_ALL_ACCESS, &k);
    LSTATUS del_result = RegDeleteValueW(k, L"ClipboardActions");
    RegCloseKey(k);
}

bool get_startup_status()
{
    HKEY k;
    LSTATUS open_result = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\", 0, KEY_READ, &k);
    LSTATUS val_result = RegGetValueW(k, nullptr, L"ClipboardActions", RRF_RT_REG_SZ, nullptr, nullptr, nullptr);
    RegCloseKey(k);
    return val_result == ERROR_SUCCESS;
}