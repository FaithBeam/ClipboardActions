#include "toast_handler.hpp"

class __declspec(uuid("9D0B8B92-4E1C-488e-A1E1-2331AFCE2CB5")) app_uuid;

BOOL raise_info_toast(std::wstring msg)
{
	NOTIFYICONDATA nid = { sizeof nid };
	nid.uFlags = NIF_INFO | NIF_GUID;
	nid.guidItem = __uuidof(app_uuid);
	nid.dwInfoFlags = NIIF_NONE | NIIF_RESPECT_QUIET_TIME;
	nid.uTimeout = 5;
	std::wstring title(L"Clipboard Actions");
	std::ranges::copy(title, nid.szInfoTitle);
	std::ranges::copy(msg, nid.szInfo);
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}