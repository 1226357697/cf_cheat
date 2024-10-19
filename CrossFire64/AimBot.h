#pragma once
#include "Game.h"
#include "FrameConetxt.h"
#include "menu_config.h"
#include "PIDController.h"

class AimBot
{
public:
  AimBot(Game& game);
  ~AimBot();

  void setSmooth(float smooth);
  void setHotKey(int vkey);
  void setBoneIndex(int boneIndex);
  void setType(MenuConfig_::AimBotType t);
  void setPolicy(MenuConfig_::AimBotPolicy policy);
  void aimbot(const FrameContext& frame_ctx);

  void setPID_p(float p);
  void setPID_i(float i);
  void setPID_d(float d);

private:
  PIDController pidctrlX_;
  PIDController pidctrlY_;
  Game& game_;
  MenuConfig_::AimBotType type_;
  MenuConfig_::AimBotPolicy policy_;
  int boneIndex_;
  int vkey_;
  float smooth_;
};

