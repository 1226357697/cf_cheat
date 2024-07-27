#pragma once
#include "FrameConetxt.h"
#include "Game.h"
#include "menu_config.hpp"

class AimBot
{
public:
  AimBot(Game& game);

  void setHotKey(int vkey);
  void setBoneIndex(int boneIndex);
  void aimbot(const FrameContext& frame_ctx);

private:
  Game& game_;
  int boneIndex_;
  int vkey_;
};

