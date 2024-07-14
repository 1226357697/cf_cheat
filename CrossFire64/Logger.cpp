#include "logger.h"
#include <stdarg.h>

void Logger::info(std::string_view fmt, ...)
{
  va_list ap;
  char buffer[0x1000];

  va_start(ap, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt.data(), ap);
  puts(buffer);
  va_end(ap);
}
