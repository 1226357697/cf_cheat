#pragma once
#include "Game.h"
#include <utility>
#include "OS-ImGui/OS-ImGui.h"
#include "Radar.h"
#include "AimBot.h"
#include "FrameConetxt.h"

class Render 
{
public:
  Render(Game& game);

  // Show menu
  inline void operator()() {run();};

private:
  void run();
  void DrawMenu();

  void PlayerESP();
  bool calcPlayerBox(const APawn& pawn, Rect& rt);
  bool CalcPlayerBoneRect(const std::array<D3DXVECTOR2, BoneCount>& screenBonePos, std::pair<Vec2, Vec2>& Rect);
  void DrawHealthBar(std::ptrdiff_t Sign, float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size, bool Horizontal);
  void RadarSetting(Radar& Radar);
  void resetFrameContext();
  void addRanderRectText(const Rect& rect, int index, const std::string& txt);
  void drawPayerBone(const APawn& pawn);

  void updateFPS();
private:
  Game& game_;
  FrameContext frame_ctx_;
  AimBot aimbot_;
  uint64_t render_frame_count_ = 0; 
  float fps_ = 0;
};



