#include "gui_helpers.hpp"

/**
 * \brief Find a profile with a name
 * \param wc_name The name of the profile to find
 * \return The profile
 */
parsed_application *get_profile(const wchar_t *wc_name)
{
	const std::wstring w_name(wc_name);

	for (const auto i : parsed_apps)
	{
		if (i->app_name == w_name)
		{
			return i;
		}
	}

	return {};
}

std::tuple<parsed_application *, int> get_current_selected_profile(const HWND h_dlg, const int id)
{
	HWND profiles_lb_h_wnd = GetDlgItem(h_dlg, id);
	int profile_idx = ListBox_GetCurSel(profiles_lb_h_wnd);
	if (profile_idx < 0)
	{
		return {};
	}
	int name_len = ListBox_GetTextLen(profiles_lb_h_wnd, profile_idx);
	const auto p_name = new wchar_t[name_len];
	ListBox_GetText(profiles_lb_h_wnd, profile_idx, p_name);
	return {get_profile(p_name), profile_idx};
}

/**
 * \brief Get the text of the selected listbox item
 * \param handle The handle to the dialog containing the listbox
 * \param id_item The id of the listbox
 * \return A pointer of the name in the listbox
 */
wchar_t *get_selected_listbox_item(const HWND handle, const int id_item)
{
	const HWND h_wnd_list = GetDlgItem(handle, id_item);

	if (const LRESULT dw_sel = ListBox_GetCurSel(h_wnd_list); dw_sel > -1)
	{
		return get_listbox_item(h_wnd_list, dw_sel);
	}

	return {};
}

/**
 * \brief Helper function to turn a string to a wchar_t pointer.
 * You MUST call CoTaskMemFree on the returned pointer once done using it.
 * \param str Input string
 * \return A pointer to a wchar_t
 */
wchar_t *string_to_wchar_ptr(const std::string str)
{
	const std::wstring tmp(str.begin(), str.end());
	const size_t size = (tmp.size() + 1) * sizeof(wchar_t);
	const auto ptr = static_cast<wchar_t *>(CoTaskMemAlloc(size));
	CopyMemory(ptr, tmp.c_str(), size);
	return ptr;
}

/**
 * \brief Adds a string to a listbox
 * \param handle Handle to the listbox
 * \param str The string to be added
 * \param idx The index to add the string in the listbox
 */
void fill_listbox_with_string(const HWND handle, const std::string &str)
{
	wchar_t *ptr = string_to_wchar_ptr(str);

	ListBox_AddString(handle, ptr);

	CoTaskMemFree(ptr);
}

/**
 * \brief Get the text of a listbox item
 * \param handle The handle to the listbox
 * \param idx The index of the item to return
 * \return A pointer of the name in the listbox
 */
wchar_t *get_listbox_item(const HWND handle, const LRESULT idx)
{
	const LRESULT len = ListBox_GetTextLen(handle, idx);
	const auto buf = new wchar_t[len];
	ListBox_GetText(handle, idx, buf);
	return buf;
}

void fill_edit_with_string(const HWND handle, const std::string str)
{
	const std::wstring tmp(str.begin(), str.end());
	Edit_SetText(handle, tmp.c_str());
}

wchar_t *get_edit_text(const HWND handle, const UINT edit_id)
{
	const int len = Edit_GetTextLength(GetDlgItem(handle, edit_id));
	const auto txt = new wchar_t[len];
	Edit_GetText(GetDlgItem(handle, edit_id), txt, len + 1);
	return txt;
}
