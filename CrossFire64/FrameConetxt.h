#pragma once
#include "Game.h"
#include "OS-ImGui/OS-ImGui.h"

struct PlayerBoneData
{
  D3DXVECTOR3 worldPos;
  D3DXVECTOR2 screenPos;

  inline bool inScreen()const { return screenPos != invalidScreenPos; };
  inline bool valid()const { return worldPos != invalidScreenPos; };
};

struct Rect
{
  float x;
  float y;
  float w;
  float h;
};

struct APawn
{ 
  std::ptrdiff_t raw_address;
  bool valid;
  int index;
  uint8_t team;
  ViewAngle viewAngle;
  int hp;
  bool has_c4;
  std::string name;
  std::string weapon_name;
  std::array<PlayerBoneData, BoneCount> Bone;

  std::pair<Rect, bool> box;
};

struct FrameContext
{
  int localPlayerIndex;
  std::array<APawn, player_count> playerArray;
  Vec2 game_window_size;
};