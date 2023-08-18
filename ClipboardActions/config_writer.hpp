#pragma once
#include <vector>
#include "parsed_application.hpp"

void write_config(const std::vector<parsed_application*>& apps, bool notifications, const std::string& dst);
void update_notification(bool notifications, const std::string& dst);