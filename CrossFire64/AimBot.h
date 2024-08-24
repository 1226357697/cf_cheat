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
  ~AimBot();

  void setSmooth(float smooth);
  void setHotKey(int vkey);
  void setBoneIndex(int boneIndex);
  void setType(MenuConfig::AimBotType t);
  void aimbot(const FrameContext& frame_ctx);

  void setPID_p(float p);
  void setPID_i(float i);
  void setPID_d(float d);

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

