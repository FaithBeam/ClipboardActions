#include "parsed_apps.hpp"
#include "config_functions.hpp"
#include <iostream>
#include "string_helpers.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void read_config()
{
	const auto config_path = get_config_path();

	if (const std::filesystem::path file{config_path}; !exists(file))
	{
		return;
	}

	std::ifstream f(config_path);
	auto data = json::parse(f);

	notifications = data["notifications"];

	for (auto &[_, v] : data["applications"].items())
	{
		auto *p = new parsed_application();
		p->app_name = utf8_decode(v["appName"]);
		p->app_path = utf8_decode(v["executablePath"]);
		p->cur_dir = utf8_decode(v["workingDirectory"]);
		p->args = utf8_decode(v["arguments"]);
		p->include_app_path_in_args = v["includeAppPathInArgs"];

		for (auto &[rk, rv] : v["regexes"].items())
		{
			p->regexes.push_back(std::wregex(utf8_decode(rv)));
			p->regex_patterns.push_back(utf8_decode(rv));
		}

		parsed_apps.push_back(p);
	}
}

void write_config(const std::vector<parsed_application *> &apps, bool notifications_enabled, const std::wstring &dst)
{
	json j;

	j["notifications"] = notifications_enabled;
	j["applications"] = {};

	for (const auto app : apps)
	{
		json a;

		a["appName"] = utf8_encode(app->app_name);
		a["executablePath"] = utf8_encode(app->app_path);
		a["workingDirectory"] = utf8_encode(app->cur_dir);
		a["arguments"] = utf8_encode(app->args);
		a["includeAppPathInArgs"] = app->include_app_path_in_args;
		a["regexes"] = {};

		for (const auto &rx : app->regex_patterns)
		{
			a["regexes"].emplace_back(utf8_encode(rx));
		}

		j["applications"].emplace_back(a);
	}

	std::ofstream o(dst);
	o << std::setw(4) << j << std::endl;
}

void update_notification(bool notifications_enabled, const std::wstring &dst)
{
	if (const std::filesystem::path file{dst}; !exists(file))
	{
		return;
	}
	std::ifstream f(dst);
	json data = json::parse(f);
	data["notifications"] = notifications_enabled;
	std::ofstream o(dst);
	o << std::setw(4) << data << std::endl;
}

std::wstring get_config_path()
{
	const std::basic_string<wchar_t> module_name = GetStringFromWindowsApi<TCHAR>([](TCHAR *buffer, const int size)
																				  { return GetModuleFileName(nullptr, buffer, size); });
	return std::regex_replace(module_name, std::wregex(LR"(^(.*)\\\w+.exe$)"), L"$1\\config.json");
}
