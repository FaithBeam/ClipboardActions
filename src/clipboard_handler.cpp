#include "clipboard_handler.hpp"
#include "parsed_apps.hpp"
#include "toast_handler.hpp"

void on_clipboard_change()
{
	if (!OpenClipboard(nullptr))
	{
		return;
	}

	const HANDLE h_data = GetClipboardData(CF_TEXT);
	if (h_data == nullptr)
	{
		CloseClipboard();
		return;
	}

	const auto psz_text = static_cast<char *>(GlobalLock(h_data));
	if (psz_text == nullptr)
	{
		CloseClipboard();
		return;
	}
	const std::string text_str(psz_text);
	const std::wstring text(text_str.begin(), text_str.end());

	start_player(text);

	GlobalUnlock(h_data);

	CloseClipboard();
}

void start_player(const std::wstring clipboard)
{
	std::wsmatch m;
	for (const auto &parsed_app : parsed_apps)
	{
		if (!parsed_app->enabled)
		{
			continue;
		}

		for (auto &rx : parsed_app->regexes)
		{
			std::regex_search(clipboard, m, rx);

			if (!m.empty())
			{
				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				ZeroMemory(&si, sizeof si);
				si.cb = sizeof si;
				ZeroMemory(&pi, sizeof pi);

				std::wstring wargs = parsed_app->get_command(clipboard);
				const LPCWSTR sw = wargs.c_str();

				if (notifications)
				{
					raise_info_toast(wargs);
				}

				if (!CreateProcessW(
						parsed_app->app_path.c_str(),
						const_cast<LPWSTR>(sw),
						nullptr,
						nullptr,
						FALSE,
						0,
						nullptr,
						parsed_app->cur_dir.c_str(),
						&si,
						&pi))
				{
					const auto le = GetLastError();
					std::cout << "OUCH!\n"
							  << le << std::endl;
				}

				HANDLE phan;
				if (!RegisterWaitForSingleObject(&phan, pi.hProcess, wait_or_timer_callback, nullptr, INFINITE, WT_EXECUTEONLYONCE))
				{
					std::cout << "OH NO!" << std::endl;
				}

				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);

				break;
			}
		}
	}
}

void CALLBACK wait_or_timer_callback(PVOID, BOOLEAN) {}