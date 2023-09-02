#include "shared_vars.hpp"
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

	for (auto &[_, v] : data["profiles"].items())
	{
		auto *p = new Profile();
		p->name = utf8_decode(v["name"]);
		p->enabled = v["enabled"];

		for (auto &[_, prog] : v["programs"].items())
		{
			auto *program = new Program();
			program->path = utf8_decode(prog["path"]);
			program->dir = utf8_decode(prog["dir"]);
			program->args = utf8_decode(prog["args"]);
			program->include_app_path_in_args = prog["includeAppPathInArgs"];

			p->programs.emplace_back(program);
		}

		for (auto &[rk, rv] : v["regexes"].items())
		{
			p->regexes.push_back(std::wregex(utf8_decode(rv)));
			p->regex_patterns.push_back(utf8_decode(rv));
		}

		parsed_profiles.push_back(p);
	}
}

void write_config(const std::vector<Profile *> &profiles, bool notifications_enabled, const std::wstring &dst)
{
	json j_config = {
		{"notifications", notifications_enabled},
		{"profiles", nullptr}};

	for (const auto &profile : profiles)
	{
		json j_prof = {
			{"name", utf8_encode(profile->name)},
			{"enabled", profile->enabled},
			{"programs", nullptr}};

		for (const auto &program : profile->programs)
		{
			json j_prog = {
				{"path", utf8_encode(program->path)},
				{"dir", utf8_encode(program->dir)},
				{"args", utf8_encode(program->args)},
				{"includeAppPathInArgs", program->include_app_path_in_args}};

			j_prof["programs"].emplace_back(j_prog);
		}

		j_prof["regexes"] = {};

		for (const auto &rx : profile->regex_patterns)
		{
			j_prof["regexes"].emplace_back(utf8_encode(rx));
		}

		j_config["profiles"].emplace_back(j_prof);
	}

	std::ofstream o(dst);
	o << std::setw(4) << j_config << std::endl;
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
