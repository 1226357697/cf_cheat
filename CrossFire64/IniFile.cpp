#include "IniFile.h"

IniFile::IniFile(const std::string& path)
:path_(path)
{
}

std::string IniFile::readString(std::string_view section, std::string_view key, const std::string& default_value)
{
  char buffer[0x1000] = {'\0'};
  if (GetPrivateProfileStringA(section.data(), key.data(), "", buffer, sizeof(buffer), path_.data()) == 0)
  {
    return default_value;
  }
  return std::string(buffer);
}

int IniFile::readInt(std::string_view section, std::string_view key, int default_value)
{
  int value = (int)GetPrivateProfileIntA(section.data(), key.data(), default_value, path_.data());
  return value;
}


bool IniFile::readBool(std::string_view section, std::string_view key, bool default_value)
{
  int value = (int)GetPrivateProfileIntA(section.data(), key.data(), default_value, path_.data());
  return value != 0;
}

float IniFile::readFloat(std::string_view section, std::string_view key, float default_value)
{
  char buffer[0x1000];
  if(GetPrivateProfileStringA(section.data(), key.data(), "", buffer, sizeof(buffer), path_.data()) == 0)
    return default_value;

  return std::stof(buffer);
}

bool IniFile::writeString(std::string_view section, std::string_view key, const std::string& value)
{
  return WritePrivateProfileStringA(section.data(), key.data(), value.c_str(), path_.data()) == TRUE;
}

bool IniFile::writeInt(std::string_view section, std::string_view key, int value)
{
  std::string v(std::to_string(value));
  return WritePrivateProfileStringA(section.data(), key.data(), v.c_str(), path_.data()) == TRUE;
}

bool IniFile::writeBool(std::string_view section, std::string_view key, bool value)
{
  std::string v(std::to_string(value));
  return WritePrivateProfileStringA(section.data(), key.data(), v.c_str(), path_.data()) == TRUE;
}

bool IniFile::writeFloat(std::string_view section, std::string_view key, float value)
{
  std::string v(std::to_string(value));
  return WritePrivateProfileStringA(section.data(), key.data(), v.c_str(), path_.data()) == TRUE;
}
