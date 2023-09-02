#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <regex>
#include <format>
#include "program.hpp"

class Profile
{
public:
	std::vector<std::wregex> regexes{};
	std::vector<std::wstring> regex_patterns{};
	std::vector<Program *> programs{};
	std::wstring name;
	bool enabled = true;

	Profile() {}

	Profile(const std::wstring &app_name, bool enabled)
	{
		this->name = app_name;
		this->enabled = enabled;
	}
};
