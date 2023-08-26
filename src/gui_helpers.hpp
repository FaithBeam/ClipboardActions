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
void fill_listbox_with_string(const HWND, const std::string &);
wchar_t *string_to_wchar_ptr(std::string);
void fill_edit_with_string(const HWND, std::string);
wchar_t *get_edit_text(const HWND, const UINT);
std::tuple<Profile *, int> get_current_selected_profile(const HWND, const int);