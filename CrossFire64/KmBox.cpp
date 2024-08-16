#include "KmBox.h"
#include "KmBox/kmboxNet.h"

bool KmBox::connect(const char* ip, const char* port, const char* uuid)
{
  return kmNet_init(const_cast<char*>(ip), const_cast<char*>(port), const_cast<char*>(uuid)) == 0;
}

void KmBox::mov_mouse(int x, int y)
{
  kmNet_mouse_move_auto(x, y, 400);
}
