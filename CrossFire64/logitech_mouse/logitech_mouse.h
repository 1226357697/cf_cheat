#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


enum LOGITECH_MOUSE_KEY
{
  kMOUSE_KEY_LEFT = 1,
  kMOUSE_KEY_UND = 2,
  kMOUSE_KEY_RIGHT = 3,
  kMOUSE_KEY_MID = 4,
};

bool logi_mouse_open(void);

void logi_mouse_close(void);

bool logi_mouse_down(int key);

bool logi_mouse_up();

bool logi_mouse_click(int key);

bool logi_mouse_scroll(char dis);

bool logi_mouse_move_rel(int x, int y);

#ifdef __cplusplus
}
#endif // __cplusplus