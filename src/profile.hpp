#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <regex>
#include <format>

class Profile
{
public:
	std::vector<std::wregex> regexes{};
	std::vector<std::wstring> regex_patterns{};
	std::wstring app_name;
	std::wstring app_path;
	std::wstring cur_dir;
	std::wstring args;
	bool enabled = true;
	bool include_app_path_in_args = true;

	Profile() {}

	Profile(const std::wstring &app_name, std::wstring app_path, std::wstring cur_dir, std::wstring extra_args, bool enabled)
	{
		this->app_name = app_name;
		this->app_path = app_path;
		this->cur_dir = cur_dir;
		this->args = extra_args;
		this->enabled = enabled;
	}

	[[nodiscard]] std::wstring get_full_args() const;

	[[nodiscard]] std::wstring get_command(const std::wsmatch &) const;
};
