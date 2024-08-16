#pragma once
class KmBox
{
public:
  KmBox() =default;
  bool connect(const char* ip, const char* port, const char* uuid);
  void mov_mouse(int x, int y);
};

