#pragma once
#include <memory>
#include <string>
#include <windows.h>
#include <windowsx.h>
#include "parsed_application.hpp"
#include "parsed_apps.hpp"

parsed_application *get_profile(const wchar_t *);
wchar_t *get_listbox_item(HWND, LRESULT);
wchar_t *get_selected_listbox_item(HWND, int);
void fill_listbox_with_string(HWND, const std::string &);
wchar_t *string_to_wchar_ptr(std::string);
void fill_edit_with_string(const HWND, std::string);
wchar_t *get_edit_text(const HWND, const UINT);
parsed_application *get_current_selected_profile(HWND, int);