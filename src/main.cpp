// Project3.cpp : Defines the entry point for the application.
//

// we need commctrl v6 for LoadIconMetric()
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

#include "framework.h"
#include "main.h"
#include <shellapi.h>
#include <CommCtrl.h>
#include <WinUser.h>
#include <ShellScalingApi.h>

#include "clipboard_handler.hpp"
#include "gui_helpers.hpp"
#include "parsed_apps.hpp"
#include "string_helpers.hpp"
#include <commdlg.h>
#include <shlobj_core.h>
#include "config_functions.hpp"
#include "registry_functions.hpp"
#include <filesystem>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE h_inst; // current instance
WCHAR sz_title[MAX_LOADSTRING]; // The title bar text
WCHAR sz_window_class[MAX_LOADSTRING]; // the main window class name
HWND main_h_wnd;
bool checked = true;
bool startup_status;

UINT constexpr wmapp_notifycallback = WM_APP + 1;
UINT constexpr wmapp_hideflyout = WM_APP + 2;

class __declspec(uuid("9D0B8B92-4E1C-488e-A1E1-2331AFCE2CB5")) app_uuid;


// Forward declarations of functions included in this code module:
ATOM my_register_class(HINSTANCE h_instance);
BOOL init_instance(HINSTANCE, int);
LRESULT CALLBACK wnd_proc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK settings(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ const HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	read_config();
	startup_status = get_startup_status();

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, sz_title, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PROJECT3, sz_window_class, MAX_LOADSTRING);
	my_register_class(hInstance);

	// Perform application initialization:
	if (!init_instance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	if (!AddClipboardFormatListener(main_h_wnd))
	{
		const auto err = GetLastError();
		std::cout << err;
	}

	const HACCEL h_accel_table = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT3));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, h_accel_table, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return static_cast<int>(msg.wParam);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM my_register_class(const HINSTANCE h_instance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wnd_proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = h_instance;
	wcex.hIcon = LoadIcon(h_instance, MAKEINTRESOURCE(IDI_PROJECT3));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROJECT3);
	wcex.lpszClassName = sz_window_class;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL init_instance(const HINSTANCE h_instance, int)
{
	h_inst = h_instance; // Store instance handle in our global variable

	main_h_wnd = CreateWindowW(sz_window_class, sz_title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, h_instance, nullptr);

    auto err = GetLastError();
	if (!main_h_wnd)
	{
		return FALSE;
	}

	ShowWindow(main_h_wnd, SW_HIDE);
	UpdateWindow(main_h_wnd);

	return TRUE;
}

BOOL add_notification_icon(const HWND h_wnd)
{
	NOTIFYICONDATA nid{};
	nid.cbSize = sizeof nid;
	nid.hWnd = h_wnd;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP | NIF_GUID;
	nid.guidItem = __uuidof(app_uuid);
	nid.uCallbackMessage = wmapp_notifycallback;
	LoadIconMetric(h_inst, MAKEINTRESOURCE(IDI_PROJECT3), LIM_SMALL, &nid.hIcon);
	LoadString(h_inst, IDS_TOOLTIP, nid.szTip, ARRAYSIZE(nid.szTip));
	Shell_NotifyIcon(NIM_ADD, &nid);

	// NOTIFYICON_VERSION_4 is prefered
	nid.uVersion = NOTIFYICON_VERSION_4;
	return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

void show_context_menu(const HWND h_wnd, const POINT pt)
{
	if (const HMENU h_menu = LoadMenu(h_inst, MAKEINTRESOURCE(IDR_MENU1)))
	{
		if (const HMENU h_sub_menu = GetSubMenu(h_menu, 0))
		{
			// our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
			SetForegroundWindow(h_wnd);

			// set the notifications menu item checked or not
			MENUITEMINFO mi = { 0 };
			mi.cbSize = sizeof(MENUITEMINFO);
			mi.fMask = MIIM_STATE;
			mi.fState = notifications ? MF_CHECKED : MF_UNCHECKED;
			SetMenuItemInfo(h_sub_menu, IDM_NOTIFICATIONS, FALSE, &mi);

			// set the startup menu item check or not
			mi = { 0 };
			mi.cbSize = sizeof(MENUITEMINFO);
			mi.fMask = MIIM_STATE;
			mi.fState = startup_status ? MF_CHECKED : MF_UNCHECKED;
			SetMenuItemInfo(h_sub_menu, IDM_RUN_AT_STARTUP, FALSE, &mi);

			// respect menu drop alignment
			UINT u_flags = TPM_RIGHTBUTTON;
			if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
			{
				u_flags |= TPM_RIGHTALIGN;
			}
			else
			{
				u_flags |= TPM_LEFTALIGN;
			}

			TrackPopupMenuEx(h_sub_menu, u_flags, pt.x, pt.y, h_wnd, nullptr);
		}
		DestroyMenu(h_menu);
	}
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK wnd_proc(const HWND h_wnd, const UINT message, const WPARAM w_param, const LPARAM l_param)
{
	switch (message)
	{
	case WM_CREATE:
		// add the notification icon
		if (!add_notification_icon(h_wnd))
		{
			MessageBox(h_wnd,
				L"Please read the ReadMe.txt file for troubleshooting",
				L"Error adding icon", MB_OK);
			return -1;
		}
		break;

	case WM_COMMAND:
	{
		// Parse the menu selections:
		switch (LOWORD(w_param))
		{
		case IDM_EXIT:
			DestroyWindow(h_wnd);
			break;
		case IDM_SETTINGS:
			DialogBox(h_inst, MAKEINTRESOURCE(IDD_FORMVIEW), h_wnd, settings);
			break;
		case IDM_NOTIFICATIONS:
		{
			notifications = !notifications;
			update_notification(notifications, get_config_path());
			break;
		}
		case IDM_RUN_AT_STARTUP:
		{
			if (startup_status)
			{
				unset_startup();
			}
			else
			{
				set_startup();
			}
			startup_status = !startup_status;

			break;
		}
		case IDM_OPEN_APP_DIRECTORY:
		{
			const std::basic_string<wchar_t> path = GetStringFromWindowsApi<TCHAR>(
				[](TCHAR* buffer, const int size)
				{
					return GetModuleFileName(nullptr, buffer, size);
				});
			auto base_dir = std::regex_replace(path, std::wregex(LR"(^(.*)\\\w+.exe$)"), L"$1");
			ShellExecuteW(nullptr, L"open", base_dir.c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
			break;
		}
		default:
			return DefWindowProc(h_wnd, message, w_param, l_param);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(h_wnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(h_wnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case wmapp_notifycallback:
		switch (LOWORD(l_param))
		{
		case WM_CONTEXTMENU:
		{
			POINT const pt = { LOWORD(w_param), HIWORD(w_param) };
			show_context_menu(h_wnd, pt);
		}
		break;
		default:;
		}
		break;
	case WM_CLIPBOARDUPDATE:
		on_clipboard_change();
		break;
	default:
		return DefWindowProc(h_wnd, message, w_param, l_param);
	}
	return 0;
}

// Message handler for settingsbox.
INT_PTR CALLBACK settings(const HWND h_dlg, const UINT message, const WPARAM w_param, const LPARAM l_param)
{
	int wm_id, wm_event;

	UNREFERENCED_PARAMETER(l_param);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		const HWND h_wnd_list = GetDlgItem(h_dlg, IDC_PROFILE_LIST);
		for (size_t i = 0; i < parsed_apps.size(); i++)
		{
			ListBox_AddString(h_wnd_list, parsed_apps.at(i)->app_name.c_str());
		}
		SetFocus(h_wnd_list);
		return TRUE;
	}
	case WM_CLOSE:
		EndDialog(h_dlg, LOWORD(w_param));
		return TRUE;
	case WM_COMMAND:
		wm_id = LOWORD(w_param);
		wm_event = HIWORD(w_param);

		if (wm_id == IDC_ADD_BUTTON)
		{
			if (wm_event == BN_CLICKED)
			{
				// get text from edits
				const wchar_t* name = get_edit_text(h_dlg, IDC_NAME_EDIT);
				if (wcscmp(name, L"") == 0)
				{
					break;
				}
				std::wstring w_name(name);

				// if the to be added name exists in the parsed_apps, don't add it and return
				auto is_name_same = [w_name](parsed_application*& p) { return p->app_name == w_name; };
				if (const auto it = std::ranges::find_if(parsed_apps, is_name_same); it != parsed_apps.end())
				{
					std::wstring wss = L"Profile " + w_name + L" already exists";
					MessageBox(h_dlg, wss.c_str(), L"Error adding profile", MB_OK);
					break;
				}

				// add name to profiles listbox
				const HWND h_wnd_profiles_lb = GetDlgItem(h_dlg, IDC_PROFILE_LIST);
				ListBox_AddString(h_wnd_profiles_lb, name);

				const wchar_t* path = get_edit_text(h_dlg, IDC_PATH_EDIT);
				const wchar_t* work_dir = get_edit_text(h_dlg, IDC_WORK_DIR_EDIT);
				const wchar_t* args = get_edit_text(h_dlg, IDC_ARGS_EDIT);

				const auto p = new parsed_application{
					w_name, path, work_dir, args
				};

				// get rx from regex listbox
				const HWND h_wnd_rx_lb = GetDlgItem(h_dlg, IDC_REGEXES_LIST);
				const LRESULT num_rx = ListBox_GetCount(h_wnd_rx_lb);
				for (LRESULT i = 0; i < num_rx; i++)
				{
					const wchar_t* rx = get_listbox_item(h_wnd_rx_lb, i);
					p->regexes.emplace_back(rx);
					p->regex_patterns.push_back(rx);
				}

				parsed_apps.push_back(p);

				Button_Enable(GetDlgItem(h_dlg, IDC_SAVE_BUTTON), TRUE);
			}
		}

		else if (wm_id == IDC_PATH_PICKER_BUTTON)
		{
			OPENFILENAME ofn;
			wchar_t sz_file[260];
			ZeroMemory(&ofn, sizeof ofn);
			ofn.lStructSize = sizeof ofn;
			ofn.hwndOwner = h_dlg;
			ofn.lpstrFile = sz_file;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof sz_file;
			ofn.lpstrFilter = L"All\0*.*\0Executables (*.exe)\0*.exe\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
			if (GetOpenFileName(&ofn) == TRUE)
			{
				HWND path_h_wnd = GetDlgItem(h_dlg, IDC_PATH_EDIT);
				Edit_SetText(path_h_wnd, ofn.lpstrFile);

				// add the parent directory if the work dir edit is empty
				HWND work_dir_h_wnd = GetDlgItem(h_dlg, IDC_WORK_DIR_EDIT);
				if (int work_dir_len = Edit_GetTextLength(work_dir_h_wnd); work_dir_len < 1)
				{
					if (std::filesystem::path p(ofn.lpstrFile); p.has_parent_path())
					{
						Edit_SetText(work_dir_h_wnd, p.parent_path().c_str());
					}
				}
			}
		}

		else if (wm_id == IDC_WORK_DIR_PICKER_BUTTON)
		{
			IFileDialog* pfd;
			if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
			{
				DWORD dw_options;
				if (SUCCEEDED(pfd->GetOptions(&dw_options)))
				{
					pfd->SetOptions(dw_options | FOS_PICKFOLDERS);
				}
				if (SUCCEEDED(pfd->Show(NULL)))
				{
					IShellItem* psi;
					if (SUCCEEDED(pfd->GetResult(&psi)))
					{
						if (wchar_t* sz_file[1]; SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, sz_file)))
						{
							HWND work_dir_h_wnd = GetDlgItem(h_dlg, IDC_WORK_DIR_EDIT);
							Edit_SetText(work_dir_h_wnd, sz_file[0]);
						}
						psi->Release();
					}
				}
				pfd->Release();
			}
		}

		else if (wm_id == IDC_CLOSE_BUTTON)
		{
			if (wm_event == BN_CLICKED)
			{
				EndDialog(h_dlg, LOWORD(w_param));
				return TRUE;
			}
		}

		else if (wm_id == IDC_REMOVE_BUTTON)
		{
			if (wm_event == BN_CLICKED)
			{
				const HWND h_wnd_profiles_lb = GetDlgItem(h_dlg, IDC_PROFILE_LIST);

				if (const LRESULT idx = ListBox_GetCurSel(h_wnd_profiles_lb); idx > -1)
				{
					ListBox_DeleteString(h_wnd_profiles_lb, idx);
					delete parsed_apps.at(idx);
					parsed_apps.erase(parsed_apps.begin() + idx);

					Button_Enable(GetDlgItem(h_dlg, IDC_SAVE_BUTTON), TRUE);
				}
			}
		}

		else if (wm_id == IDC_PROFILE_LIST)
		{
			if (wm_event == LBN_SELCHANGE)
			{
				const wchar_t* txt = get_selected_listbox_item(h_dlg, IDC_PROFILE_LIST);
				if (txt == nullptr)
				{
					break;
				}
				const parsed_application* profile = get_profile(txt);
				// aren't I supposed to delete this? Get heap exception here.
				// delete[] txt;

				// update text edits with profile information
				Edit_SetText(GetDlgItem(h_dlg, IDC_NAME_EDIT), profile->app_name.c_str());
				Edit_SetText(GetDlgItem(h_dlg, IDC_PATH_EDIT), profile->app_path.c_str());
				Edit_SetText(GetDlgItem(h_dlg, IDC_WORK_DIR_EDIT), profile->cur_dir.c_str());
				Edit_SetText(GetDlgItem(h_dlg, IDC_ARGS_EDIT), profile->args.c_str());

				// reset regex listbox
				const HWND rx_lb_h_wnd = GetDlgItem(h_dlg, IDC_REGEXES_LIST);
				ListBox_ResetContent(rx_lb_h_wnd);

				// fill regex listbox
				for (size_t i = 0; i < profile->regex_patterns.size(); i++)
				{
					ListBox_AddString(rx_lb_h_wnd, profile->regex_patterns[i].c_str());
				}

				// update full command edit
				Edit_SetText(GetDlgItem(h_dlg, IDC_COMMAND_EDIT), profile->get_full_args().c_str());

				//Button_SetCheck(GetDlgItem(h_dlg, IDC_APP_PATH_FIRST_CHECK), profile->include_app_path_in_args);
			}
		}

		/*else if (wm_id == IDC_APP_PATH_FIRST_CHECK)
		{
			if (wm_event == BN_CLICKED)
			{
				auto check_status = Button_GetCheck(GetDlgItem(h_dlg, IDC_APP_PATH_FIRST_CHECK));
				parsed_application* p = get_current_selected_profile(h_dlg, IDC_PROFILE_LIST);
				if (p == nullptr)
				{
					break;
				}
				p->include_app_path_in_args = check_status;
				Button_Enable(GetDlgItem(h_dlg, IDC_SAVE_BUTTON), TRUE);
				Edit_SetText(GetDlgItem(h_dlg, IDC_COMMAND_EDIT), p->get_full_args().c_str());
			}
		}*/

		else if (wm_id == IDC_REGEXES_LIST)
		{
			if (wm_event == LBN_SELCHANGE)
			{
				const HWND lb_rx_h_wnd = GetDlgItem(h_dlg, IDC_REGEXES_LIST);
				const LRESULT idx = ListBox_GetCurSel(lb_rx_h_wnd);
				const LRESULT len = ListBox_GetTextLen(lb_rx_h_wnd, idx);
				if (len < 1)
				{
					break;
				}
				const auto txt = new wchar_t[len];
				ListBox_GetText(lb_rx_h_wnd, idx, txt);

				Edit_SetText(GetDlgItem(h_dlg, IDC_REGEX_EDIT), txt);
			}
		}

		else if (wm_id == IDC_REGEX_ADD_BUTTON)
		{
			if (wm_event == BN_CLICKED)
			{
				HWND profiles_lb_h_wnd = GetDlgItem(h_dlg, IDC_PROFILE_LIST);
				int profile_idx = ListBox_GetCurSel(profiles_lb_h_wnd);
				if (profile_idx < 0)
				{
					break;
				}

				const HWND rx_edit_h_wnd = GetDlgItem(h_dlg, IDC_REGEX_EDIT);
				const LRESULT len = Edit_GetTextLength(rx_edit_h_wnd);
				if (len < 1)
				{
					break;
				}

				const auto txt = new wchar_t[len];
				Edit_GetText(rx_edit_h_wnd, txt, len + 1);

				// if the to be added rx exists in the parsed_apps, don't add it and return
				const HWND rx_lb_h_wnd = GetDlgItem(h_dlg, IDC_REGEXES_LIST);
				for (int i = 0; i < ListBox_GetCount(rx_lb_h_wnd); i++)
				{
					const int rx_len = ListBox_GetTextLen(rx_lb_h_wnd, i);
					const auto rx_txt = new wchar_t[rx_len];
					ListBox_GetText(rx_lb_h_wnd, i, rx_txt);
					if (wcscmp(txt, rx_txt) == 0)
					{
						std::wstringstream wss;
						wss << L"Regex " << txt << " already exists";
						MessageBox(h_dlg,
							wss.str().c_str(),
							L"Error adding regex", MB_OK);
						return FALSE;
					}
				}

				// add string to regex list
				ListBox_AddString(rx_lb_h_wnd, txt);

				// add string to profile
				int name_len = ListBox_GetTextLen(profiles_lb_h_wnd, profile_idx);
				const auto p_name = new wchar_t[name_len];
				ListBox_GetText(profiles_lb_h_wnd, profile_idx, p_name);
				const auto p = get_profile(p_name);
				p->regexes.emplace_back(txt);
				p->regex_patterns.push_back(txt);

				Button_Enable(GetDlgItem(h_dlg, IDC_SAVE_BUTTON), TRUE);
			}
		}

		else if (wm_id == IDC_REGEX_REMOVE_BUTTON)
		{
			if (wm_event == BN_CLICKED)
			{
				HWND rx_lb_h_wnd = GetDlgItem(h_dlg, IDC_REGEXES_LIST);
				const int rx_idx = ListBox_GetCurSel(rx_lb_h_wnd);
				if (rx_idx < 0)
				{
					break;
				}
				int rx_len = ListBox_GetTextLen(rx_lb_h_wnd, rx_idx);
				auto rx_txt = new wchar_t[rx_len];
				ListBox_GetText(rx_lb_h_wnd, rx_idx, rx_txt);
				ListBox_DeleteString(rx_lb_h_wnd, rx_idx);

				HWND prof_lb_h_wnd = GetDlgItem(h_dlg, IDC_PROFILE_LIST);
				const int p_idx = ListBox_GetCurSel(prof_lb_h_wnd);
				if (p_idx < 0)
				{
					break;
				}

				int p_len = ListBox_GetTextLen(prof_lb_h_wnd, p_idx);
				auto p_name = new wchar_t[p_len];
				ListBox_GetText(prof_lb_h_wnd, p_idx, p_name);
				auto prof = get_profile(p_name);
				for (size_t i = 0; i < prof->regex_patterns.size(); i++)
				{
					if (prof->regex_patterns[i] == rx_txt)
					{
						prof->regex_patterns.erase(prof->regex_patterns.begin() + i);
						prof->regexes.erase(prof->regexes.begin() + i);
						Button_Enable(GetDlgItem(h_dlg, IDC_SAVE_BUTTON), TRUE);
						return FALSE;
					}
				}
			}
		}

		else if (wm_id == IDC_REGEX_UPDATE_BUTTON)
		{
			HWND prof_lb_w_wnd = GetDlgItem(h_dlg, IDC_PROFILE_LIST);
			HWND rx_lb_w_wnd = GetDlgItem(h_dlg, IDC_REGEXES_LIST);
			int prof_idx, rx_idx;
			prof_idx = ListBox_GetCurSel(prof_lb_w_wnd);
			if (prof_idx < 0)
			{
				break;
			}
			rx_idx = ListBox_GetCurSel(rx_lb_w_wnd);
			if (rx_idx < 0)
			{
				break;
			}
			int cur_rx_len = ListBox_GetTextLen(rx_lb_w_wnd, rx_idx);
			auto cur_rx_txt = new wchar_t[cur_rx_len];
			ListBox_GetText(rx_lb_w_wnd, rx_idx, cur_rx_txt);

			HWND rx_edit_h_wnd = GetDlgItem(h_dlg, IDC_REGEX_EDIT);
			const int rx_len = Edit_GetTextLength(rx_edit_h_wnd);
			if (rx_len < 1)
			{
				break;
			}
			auto rx_txt = new wchar_t[rx_len];
			Edit_GetText(rx_edit_h_wnd, rx_txt, rx_len + 1);
			ListBox_DeleteString(rx_lb_w_wnd, rx_idx);
			ListBox_InsertString(rx_lb_w_wnd, rx_idx, rx_txt);
			int profile_len = ListBox_GetTextLen(prof_lb_w_wnd, prof_idx);
			auto profile_txt = new wchar_t[profile_len];
			ListBox_GetText(prof_lb_w_wnd, prof_idx, profile_txt);
			auto profile = get_profile(profile_txt);

			for (size_t i = 0; i < profile->regex_patterns.size(); i++)
			{
				if (profile->regex_patterns.at(i) == cur_rx_txt)
				{
					profile->regex_patterns.at(i) = rx_txt;
					profile->regexes.at(i) = std::wregex(rx_txt);
				}
			}

			Button_Enable(GetDlgItem(h_dlg, IDC_SAVE_BUTTON), TRUE);
		}

		else if (wm_id == IDC_SAVE_BUTTON)
		{
			if (wm_event == BN_CLICKED)
			{
				write_config(parsed_apps, notifications, get_config_path());
				Button_Enable(GetDlgItem(h_dlg, IDC_SAVE_BUTTON), FALSE);
			}
		}
		break;
	default:;
	}
	return FALSE;
}
