#pragma once
#include <memory>
#include <string>
#include <windows.h>
#include <windowsx.h>
#include "profile.hpp"
#include "shared_vars.hpp"
#include <tuple>

Profile *get_profile(const wchar_t *);
wchar_t *get_listbox_item(const HWND, const LRESULT);
wchar_t *get_selected_listbox_item(const HWND, int);
wchar_t *get_edit_text(const HWND, const UINT);
std::tuple<Profile *, int> get_current_selected_profile(const HWND, const int);