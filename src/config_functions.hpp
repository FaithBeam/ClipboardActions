#pragma once

#include <fstream>
#include "profile.hpp"

void read_config();
void write_config(const std::vector<Profile *> &apps, bool notifications, const std::wstring &dst);
void update_notification(bool notifications, const std::wstring &dst);
std::wstring get_config_path();