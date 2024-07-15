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
  std::pair<Vec2, Vec2> CalcPlayerBoneRect(const std::array<D3DXVECTOR2, BoneCount>& screenBonePos);

private:
  Game& game_;
};



