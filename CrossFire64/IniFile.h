#pragma once
#include <string>
#include <string_view>


class IniFile
{
public:
  IniFile(const std::string& path);
  std::string readString(std::string_view section, std::string_view key, const std::string& default_value = "");
  int readInt(std::string_view section, std::string_view key, int default_value = 0);
  bool readBool(std::string_view section, std::string_view key, bool default_value = false);
  float readFloat(std::string_view section, std::string_view key, float default_value = 0.0f);

  bool writeString(std::string_view section, std::string_view key, const std::string& value);
  bool writeInt(std::string_view section, std::string_view key, int value);
  bool writeBool(std::string_view section, std::string_view key, bool value);
  bool writeFloat(std::string_view section, std::string_view key, float value);
  
private:
  std::string  path_;
};

