#pragma once
#include <string>
#include <time.h>

#include "NativeHandle.h"
#define xor_a(x) x

namespace util
{
	std::string random_string();
	int get_process_id(std::string_view name);
	HMODULE get_module_base_x64(DWORD processID, const char* module_name);
	std::string CharToUtf8(const std::string& asciiStr);
};

