#pragma once
#include "Game.h"
#include <utility>
#include "OS-ImGui/OS-ImGui.h"

class Render 
{
public:
  Render(Game& game);

  // Show menu
  inline void operator()() {run();};

private:
  void run();
  void PlayerESP();
  bool CalcPlayerBoneRect(const std::array<D3DXVECTOR2, BoneCount>& screenBonePos, std::pair<Vec2, Vec2>& Rect);
  void DrawHealthBar(std::ptrdiff_t Sign, float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size, bool Horizontal);

private:
  Game& game_;
};



