#pragma once
#include <string_view>

class Logger
{
public:
  static void info(std::string_view fmt, ...);

private:

};
