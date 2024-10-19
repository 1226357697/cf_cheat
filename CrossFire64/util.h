#pragma once
#include <cstdio> 
#include <string>
#include <string_view>
#include <time.h>
#include "xorstr.hpp"
#include "NativeHandle.h"

namespace util
{
	std::string random_string();
	int get_process_id(std::string_view name);
	HMODULE get_module_base_x64(DWORD processID, const char* module_name);
	std::string CharToUtf8(const std::string& asciiStr);

	template <typename ...Types>
	inline std::string tiny_format_string(std::string_view format, Types... args)
	{
		char buffer[0x1000];
		std::snprintf(buffer, sizeof(buffer), format.data(), args...);
		return buffer;
	}
};

