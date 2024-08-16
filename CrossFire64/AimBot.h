#pragma once
#include "FrameConetxt.h"
#include "Game.h"
#include "menu_config.hpp"
#include "KmBox.h"
#include "PIDController.h"

class AimBot
{
public:
  AimBot(Game& game);

  void setSmooth(float smooth);
  void setHotKey(int vkey);
  void setBoneIndex(int boneIndex);
  void setType(MenuConfig::AimBotType t);
  void aimbot(const FrameContext& frame_ctx);

  bool connectKmBox(const char* ip, const char* port, const char* uuid);
private:
  PIDController pidctrlX_;
  PIDController pidctrlY_;
  Game& game_;
  KmBox kb_;
  MenuConfig::AimBotType type_;
  int boneIndex_;
  int vkey_;
  float smooth_;
};

