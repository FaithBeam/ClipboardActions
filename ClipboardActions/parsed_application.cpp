#include "parsed_application.hpp"

std::wstring parsed_application::get_full_args() const
{
	if (this->include_app_path_in_args)
	{
		return this->app_path + L" " + this->args;
	}
	else
	{
		return this->args;
	}
}

std::wstring parsed_application::get_command(const std::wstring& clipboard) const
{
	return std::regex_replace(this->get_full_args(), std::wregex(L"\\{URL\\}"), clipboard);
}
