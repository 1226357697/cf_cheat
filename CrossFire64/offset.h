#pragma once
#include <cstddef>

namespace crossfire_offset {
  constexpr static std::ptrdiff_t CLTClientShell = 0x32053B8;
  constexpr static std::ptrdiff_t CAIBotModePlayer = 0x98;

  // D3D Engine
  constexpr static std::ptrdiff_t d3d_viewport = 0x57D0E0;
  constexpr static std::ptrdiff_t d3d_Projection_matrix = d3d_viewport - 0x40;
  constexpr static std::ptrdiff_t d3d_view_matrix = d3d_viewport - 0x40 - 0x40;

  // Obstacle detection crossfire.exe
  constexpr static std::ptrdiff_t obstacle_detection_call = 0x54940;

  // red name detection crossfire.exe
  constexpr static std::ptrdiff_t red_name_detection = 0x5436C0;

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

  // HOOK KNIFE  cshell_x64.dll  constexpr static std::ptrdiff_t knife_marks = 0x700;
  constexpr static std::ptrdiff_t knife_marks1 = 0xD94E72;
  constexpr static std::ptrdiff_t knife_marks2 = 0x179211C;
  constexpr static std::ptrdiff_t knife_marks3 = 0x1792148;
  constexpr static std::ptrdiff_t knife_tap_distance1 = 0xDA7BE7;
  constexpr static std::ptrdiff_t knife_tap_distance2 = 0x17A4464;
  constexpr static std::ptrdiff_t knife_tap_distance3 = 0xDA7BA3;
  constexpr static std::ptrdiff_t knife_tap_distance4 = 0x17A4420;
  constexpr static std::ptrdiff_t knife_tap_range1 = 0xDA811D;
  constexpr static std::ptrdiff_t knife_tap_range2 = 0x17A49F8;
  constexpr static std::ptrdiff_t knife_tap_range3 = 0xDA80D9;
  constexpr static std::ptrdiff_t knife_tap_range4 = 0x17A49B4;
  constexpr static std::ptrdiff_t knife_attack_distance1 = 0xD95F60;
  constexpr static std::ptrdiff_t knife_attack_distance2 = 0xD95F75;
  constexpr static std::ptrdiff_t knife_attack_distance3 = 0x17929E2;
  constexpr static std::ptrdiff_t knife_attack_distance4 = 0x1BA96EA;
  constexpr static std::ptrdiff_t knife_attack_range1 = 0xDA800F;
  constexpr static std::ptrdiff_t knife_attack_range2 = 0x17A48EE;
  constexpr static std::ptrdiff_t knife_attack_speed1 = 0xD84A4D;
  constexpr static std::ptrdiff_t knife_attack_speed2 = 0x1783A59;
  constexpr static std::ptrdiff_t knife_secondary_distance1 = 0xD96016;
  constexpr static std::ptrdiff_t knife_secondary_distance2 = 0xD96039;
  constexpr static std::ptrdiff_t knife_secondary_distance3 = 0x1792A77;
  constexpr static std::ptrdiff_t knife_secondary_range1 = 0xDA8023;
  constexpr static std::ptrdiff_t knife_secondary_range2 = 0x17A4902;
  constexpr static std::ptrdiff_t knife_movement_speed1 = 0xD81744;
  constexpr static std::ptrdiff_t knife_movement_speed2 = 0x177FBCE;


  // HOOK KNIFE DATA  cshell_x64.dll
  constexpr static std::ptrdiff_t knife_data_marks = 0x700;
  constexpr static std::ptrdiff_t knife_data_tap_distance = 0x710;
  constexpr static std::ptrdiff_t knife_data_tap_range = 0x720;
  constexpr static std::ptrdiff_t knife_data_attack_distance = 0x740;
  constexpr static std::ptrdiff_t knife_data_attack_range = 0x750;
  constexpr static std::ptrdiff_t knife_data_attack_speed = 0x760;
  constexpr static std::ptrdiff_t knife_data_secondary_distance = 0x770;
  constexpr static std::ptrdiff_t knife_data_secondary_range = 0x780;
  constexpr static std::ptrdiff_t knife_data_movement_speed = 0x7B0;

}