#pragma once

#include <Windows.h>
#include <string>
#include <regex>
#include <iostream>
#include <string>

void on_clipboard_change();
void start_player(const std::wstring);
void CALLBACK wait_or_timer_callback(PVOID, BOOLEAN);