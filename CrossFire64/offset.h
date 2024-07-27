#pragma once
#include <cstddef>

namespace crossfire_offset {
  constexpr static std::ptrdiff_t CLTClientShell = 0x32001C8;
  constexpr static std::ptrdiff_t CAIBotModePlayer = 0x346EE38;

  // D3D Engine
  constexpr static std::ptrdiff_t d3d_viewport = 0x57D0E0;
  constexpr static std::ptrdiff_t d3d_Projection_matrix = d3d_viewport - 0x40;
  constexpr static std::ptrdiff_t d3d_view_matrix = d3d_viewport - 0x40 - 0x40;

  // CAIBotModePlayer
  constexpr static std::ptrdiff_t local_angle_yaw = 0x86c;
  constexpr static std::ptrdiff_t local_angle_pitch = local_angle_yaw - 4;


  // CLTClientShell
  constexpr static std::ptrdiff_t local_player_index = 0x28a;
  constexpr static std::ptrdiff_t player_start = 0x290;
  constexpr static std::ptrdiff_t player_size = 0x000010B0;

  // CPlayer
  constexpr static std::ptrdiff_t model_instance = 0x0;
  constexpr static std::ptrdiff_t player_team = 0x9;
  constexpr static std::ptrdiff_t player_name = 0xA;
  constexpr static std::ptrdiff_t player_CharacFx = 0x18;
  constexpr static std::ptrdiff_t player_HasC4 = 0x28;
  constexpr static std::ptrdiff_t player_hp = 0x44;

  // CharacFx
  constexpr static std::ptrdiff_t player_yaw = 0x160;
  constexpr static std::ptrdiff_t player_pitch = player_yaw +4;
  constexpr static std::ptrdiff_t player_x = 0x170;
  constexpr static std::ptrdiff_t player_y = 0x174;
  constexpr static std::ptrdiff_t player_z = 0x178;
  constexpr static std::ptrdiff_t player_weapon = 0x113F8;
  constexpr static std::ptrdiff_t player_weapon_id = 0x17A94;

  // Weapon
  constexpr static std::ptrdiff_t weapon_name = 0xe;


  // ModelInstance
  constexpr static std::ptrdiff_t bone_matrix = 0x2910;

}