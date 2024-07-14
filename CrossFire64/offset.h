#pragma once
#include <cstddef>

namespace global_offset {
  // CLTClientShell
  constexpr static std::ptrdiff_t local_player_index = 0x28a;
  constexpr static std::ptrdiff_t player_start = 0x290;
  constexpr static std::ptrdiff_t player_size = 0x00001050;

  // CPlayer
  constexpr static std::ptrdiff_t model_instance = 0x0;
  constexpr static std::ptrdiff_t player_team = 0x9;
  constexpr static std::ptrdiff_t player_name = 0xA;
  constexpr static std::ptrdiff_t player_CharacFx = 0x18;
  constexpr static std::ptrdiff_t player_hp = 0x44;

  // CharacFx
  constexpr static std::ptrdiff_t player_x = 0x170;
  constexpr static std::ptrdiff_t player_y = 0x174;
  constexpr static std::ptrdiff_t player_z = 0x178;

  // ModelInstance
  constexpr static std::ptrdiff_t bone_count = 0x233;
  constexpr static std::ptrdiff_t bone_matrix = 0x28f8;

}