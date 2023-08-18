#pragma once

#include <fstream>
#include "parsed_application.hpp"

void read_config();
void write_config(const std::vector<parsed_application*>& apps, bool notifications, const std::wstring& dst);
void update_notification(bool notifications, const std::wstring& dst);
std::wstring get_config_path();