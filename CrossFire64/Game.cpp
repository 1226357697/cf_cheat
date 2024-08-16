#include "Game.h"
#include "util.h"
#include "offset.h"
#include <assert.h>
#include <cmath>
#include "xorstr.hpp"
#include <stdexcept>
#include "Logger.h"
#include "VMProtectSDK.h"

static char s_GameProcessName[MAX_PATH] = {'\0'};
static char s_GameWindowTitleName[MAX_PATH] = { '\0' };
static char s_GameWindowClassName[MAX_PATH] = { '\0' };

Game::Game()
{
	strcpy(s_GameProcessName, xorstr_("crossfire.exe"));
	strcpy(s_GameWindowTitleName, xorstr_("´©Ô½»ðÏß"));
	strcpy(s_GameWindowClassName, xorstr_("CrossFire"));
}

Game::~Game()
{
}

bool Game::init()
{

	Logger::info(xorstr_("Game Initialize"));
	VMProtectBeginUltra(__FUNCSIG__);
  pid_ = util::get_process_id(s_GameProcessName);
  gameWindow_ = FindWindowA(s_GameWindowClassName, s_GameWindowTitleName);
	if(pid_ == 0 || gameWindow_ == NULL)
		return false;

	if (!mm_.init() || !mm_.attach(pid_))
	{
		return false;
	}

	// patch deleter patch 0xeb
	//const std::ptrdiff_t ACE_CSI64_offset1 = 0x45946 + 0x1000;
	//const std::ptrdiff_t ACE_CSI64_offset2= 0x56D1B + 0x1000;
	//std::ptrdiff_t ACE_CSI64 = (std::ptrdiff_t)util::get_module_base_x64(pid_, xorstr_("ACE-CSI64.dll"));
	//if(ACE_CSI64 == 0)
	//	return false;

	//uint8_t patch_jmp = 0xeb;
	//mm_.forc_write(ACE_CSI64 + ACE_CSI64_offset1, &patch_jmp, sizeof(patch_jmp));
	//mm_.forc_write(ACE_CSI64 + ACE_CSI64_offset2, &patch_jmp, sizeof(patch_jmp));

	crossfireModule_ = (std::ptrdiff_t)util::get_module_base_x64(pid_, xorstr_("crossfire.exe"));
	cshell_x64Module_ = (std::ptrdiff_t)util::get_module_base_x64(pid_, xorstr_("cshell_x64.dll"));
	if (crossfireModule_ == 0 || cshell_x64Module_ == 0)
		return false;

	CLTClientShell = mm_.read<std::ptrdiff_t>(cshell_x64Module_ + crossfire_offset::CLTClientShell);
	if (CLTClientShell == 0)
		return false;

	VMProtectEnd();
  return true;
}

bool Game::update()
{
	CAIBotModePlayer = mm_.read<std::ptrdiff_t>(cshell_x64Module_ + crossfire_offset::CAIBotModePlayer);
	if (CAIBotModePlayer == 0)
		return false;

	return true;
}

bool Game::waitStart()
{
	while(FindWindowA(s_GameWindowClassName, s_GameWindowTitleName) == NULL)
		Sleep(300);
	Logger::info(xorstr_("Find Game"));
	return true;
}

const char* Game::WindowTitle()
{
	return s_GameWindowTitleName;
}

const char* Game::WindowClassName()
{
	return s_GameWindowClassName;
}

std::pair<int, int> Game::getWindowSize()
{
	RECT rc;
	GetClientRect(gameWindow_,&rc);
	return std::make_pair(rc.right - rc.left, rc.bottom - rc.top);
}


float Game::GetDistance3D(const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2)
{
	return sqrt(pow(pos2.x - pos1.x,2) + pow(pos2.y - pos1.y, 2) + pow(pos2.z - pos1.z, 2));
}

float Game::GetDistance2D(const D3DXVECTOR2& pos1, const D3DXVECTOR2& pos2)
{
	return sqrt(pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2));
}

bool Game::WorldToScreen(const D3DXVECTOR3& worldPos, D3DXVECTOR2& screenPos)
{
	VMProtectBeginMutation(__FUNCSIG__);
	D3DXMATRIX View = mm_.read<D3DXMATRIX>(crossfireModule_ + crossfire_offset::d3d_view_matrix);
	D3DXMATRIX Projection = mm_.read<D3DXMATRIX>(crossfireModule_ + crossfire_offset::d3d_Projection_matrix);
	D3DVIEWPORT9 viewport = mm_.read<D3DVIEWPORT9>(crossfireModule_ + crossfire_offset::d3d_viewport);

	D3DXVECTOR3 vScreen;
	D3DXVECTOR3 PlayerPos(worldPos.x, worldPos.y, worldPos.z);

	D3DXMATRIX world = D3DXMATRIX(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	D3DXVec3Project(&vScreen, &PlayerPos, &viewport, &Projection, &View, &world);
	if (vScreen.z >= 0.0f && vScreen.z <= 1.0f) {
		screenPos.x = vScreen.x;
		screenPos.y = vScreen.y;
		return true;
	}
	VMProtectEnd();
	return false;
}

bool Game::validPlayerIndex(int index)
{
	return index >= 0 && index < player_count;
}

int Game::getLocalPlayerIndex()
{
	return (int)mm_.read<uint16_t>(CLTClientShell + crossfire_offset::local_player_index);
}

bool Game::validPlayer(std::ptrdiff_t player)
{
	VMProtectBeginMutation(__FUNCSIG__);
	std::ptrdiff_t CharacFx = mm_.read<ptrdiff_t>(player + crossfire_offset::player_CharacFx);
	if (CharacFx == 0)
		return false;

	std::ptrdiff_t model_instance = mm_.read<ptrdiff_t>(player + crossfire_offset::model_instance);
	if (model_instance == 0)
		return false;

	VMProtectEnd();
	return true;
}

std::ptrdiff_t Game::getPlayer(int index)
{
	assert(index >= 0 && index < player_count);
	return CLTClientShell + crossfire_offset::player_start + index * crossfire_offset::player_size;
}

D3DXVECTOR3 Game::getFOVPos()
{
	VMProtectBeginMutation(__FUNCSIG__);
	D3DXMATRIX View = mm_.read<D3DXMATRIX>(crossfireModule_ + crossfire_offset::d3d_view_matrix);
	D3DXMatrixInverse(&View, 0, &View);
	VMProtectEnd();
	return D3DXVECTOR3(View._41, View._42, View._43);
}

bool Game::playerHasC4(std::ptrdiff_t player)
{
	return mm_.read<int>(player + crossfire_offset::player_HasC4) == 1;
}

int Game::getPlayerTeam(std::ptrdiff_t player)
{
	return (int)mm_.read<uint8_t>(player + crossfire_offset::player_team);
}

int Game::getPlayerHP(std::ptrdiff_t player)
{
	return mm_.read<int>(player + crossfire_offset::player_hp);
}

void Game::getPlayerBones(std::ptrdiff_t player, std::array<D3DXVECTOR3, BoneCount>& bones)
{
	VMProtectBeginMutation(__FUNCSIG__);
	std::ptrdiff_t model_instance = mm_.read<ptrdiff_t>(player + crossfire_offset::model_instance);
	if (model_instance == 0)
		return;

	ptrdiff_t boneMatrixArray = mm_.read<ptrdiff_t>(model_instance + crossfire_offset::bone_matrix);
	for (int i = 0; i < BoneCount; ++i)
	{
		D3DXMATRIX matrix = mm_.read<D3DXMATRIX>(boneMatrixArray + i * sizeof(D3DXMATRIX));
		D3DXVECTOR3 bonePos{ matrix._14, matrix._24, matrix._34 };
		bones[i] = bonePos;
	}
	VMProtectEnd();
}

std::string Game::getPlayerName(std::ptrdiff_t player)
{
	VMProtectBeginMutation(__FUNCSIG__);
	std::string name;
	name.resize(14);

	mm_.read(player + crossfire_offset::player_name, name.data(), name.size());
	VMProtectEnd();
	return util::CharToUtf8(name);
}

bool Game::getPlayerAngle(std::ptrdiff_t player, ViewAngle& agnle)
{
	VMProtectBeginMutation(__FUNCSIG__);
	std::ptrdiff_t CharacFx = mm_.read<ptrdiff_t>(player + crossfire_offset::player_CharacFx);
	if (CharacFx == 0)
		return false;

	agnle.yaw = mm_.read<float>(CharacFx + crossfire_offset::player_yaw);
	agnle.pitch = mm_.read<float>(CharacFx + crossfire_offset::player_pitch);
	VMProtectEnd();
	return true;
}

bool Game::setLocalPlayerAngle(const ViewAngle& agnle)
{
	VMProtectBeginMutation(__FUNCSIG__);
	mm_.write(CAIBotModePlayer + crossfire_offset::local_angle_yaw, &agnle.yaw);
	mm_.write(CAIBotModePlayer + crossfire_offset::local_angle_pitch, &agnle.pitch);
	VMProtectEnd();
	return false;
}

std::string Game::getPlayerWeaponName(std::ptrdiff_t player)
{
	VMProtectBeginMutation(__FUNCSIG__);
	std::string name;
	std::ptrdiff_t CharacFx = mm_.read<ptrdiff_t>(player + crossfire_offset::player_CharacFx);
	if (CharacFx == 0)
		return "";

	char buffer[33] = {'\0'};
	std::ptrdiff_t Weapon = mm_.read<ptrdiff_t>(CharacFx + crossfire_offset::player_weapon);

	mm_.read(Weapon + crossfire_offset::weapon_name, buffer, sizeof(buffer) -1);
	VMProtectEnd();
	return util::CharToUtf8(buffer);
}

void Game::hookKnifeData(bool hook)
{
	VMProtectBeginMutation(__FUNCSIG__);
	// original
	uint8_t original_code_move_speed1[10] = {0xF3, 0x0F, 0x10, 0x87, 0x44, 0x14, 0x00, 0x00, 0xF3, 0x0F};
	uint8_t original_code_move_speed2[10] = {0xF3, 0x0F, 0x10, 0x87, 0x44, 0x14, 0x00, 0x00, 0x48, 0x8B};

	uint8_t original_code_knife_mark1[10] = {0xF3, 0x0F, 0x10, 0xB2, 0x30, 0x0C, 0x00, 0x00, 0x0F, 0x57};
	uint8_t original_code_knife_mark2[10] = {0xF3, 0x0F, 0x10, 0xB3, 0x30, 0x0C, 0x00, 0x00, 0xF3, 0x0F};
	uint8_t original_code_knife_mark3[10] = {0xF3, 0x0F, 0x10, 0xB3, 0x30, 0x0C, 0x00, 0x00, 0xF3, 0x0F};

	uint8_t original_code_tap_distance1[10] = {0xF3, 0x0F, 0x10, 0x83, 0x8C, 0x0F, 0x00, 0x00, 0x48, 0x8B};
	uint8_t original_code_tap_distance2[10] = {0xF3, 0x0F, 0x10, 0x83, 0x8C, 0x0F, 0x00, 0x00, 0x48, 0x8B};
	uint8_t original_code_tap_distance3[10] = {0xF3, 0x0F, 0x10, 0x83, 0x90, 0x0F, 0x00, 0x00, 0x48, 0x8B};
	uint8_t original_code_tap_distance4[10] = {0xF3, 0x0F, 0x10, 0x83, 0x90, 0x0F, 0x00, 0x00, 0x48, 0x8B};

	uint8_t original_code_tap_range1[10] = {0xF3, 0x0F, 0x10, 0x83, 0x9C, 0x0F, 0x00, 0x00, 0x48, 0x8B};
	uint8_t original_code_tap_range2[10] = {0xF3, 0x0F, 0x10, 0x83, 0x9C, 0x0F, 0x00, 0x00, 0x48, 0x8B};
	uint8_t original_code_tap_range3[10] = {0xF3, 0x0F, 0x10, 0x83, 0xA0, 0x0F, 0x00, 0x00, 0x48, 0x8B};
	uint8_t original_code_tap_range4[10] = {0xF3, 0x0F, 0x10, 0x83, 0xA0, 0x0F, 0x00, 0x00, 0x48, 0x8B};

	uint8_t original_code_attack_distance1[10] = {0xF3, 0x41, 0x0F, 0x10, 0x8E, 0xBC, 0x0F, 0x00, 0x00, 0xFF};
	uint8_t original_code_attack_distance2[10] = {0xF3, 0x41, 0x0F, 0x10, 0x8E, 0xBC, 0x0F, 0x00, 0x00, 0x0F};
	uint8_t original_code_attack_distance3[10] = {0xF3, 0x41, 0x0F, 0x10, 0x86, 0xBC, 0x0F, 0x00, 0x00, 0x0F};
	uint8_t original_code_attack_distance4[10] = {0xF3, 0x41, 0x0F, 0x10, 0x86, 0xBC, 0x0F, 0x00, 0x00, 0x0F};

	uint8_t original_code_attack_range1[10] = {0xF3, 0x0F, 0x10, 0x83, 0xCC, 0x0F, 0x00, 0x00, 0x48, 0x8B};
	uint8_t original_code_attack_range2[10] = {0xF3, 0x0F, 0x10, 0x83, 0xCC, 0x0F, 0x00, 0x00, 0x48, 0x8B};

	uint8_t original_code_attack_speed1[10] = {0xF3, 0x0F, 0x10, 0xB6, 0xDC, 0x0F, 0x00, 0x00, 0xE8, 0xB6};
	uint8_t original_code_attack_speed2[10] = {0xF3, 0x0F, 0x10, 0xB6, 0xDC, 0x0F, 0x00, 0x00, 0xE8, 0xAA};

	uint8_t original_code_secondary_distance1[10] = {0xF3, 0x41, 0x0F, 0x10, 0x8E, 0x1C, 0x10, 0x00, 0x00, 0x41};
	uint8_t original_code_secondary_distance2[10] = {0xF3, 0x41, 0x0F, 0x10, 0x8E, 0x1C, 0x10, 0x00, 0x00, 0x0F};
	uint8_t original_code_secondary_distance3[10] = {0xF3, 0x41, 0x0F, 0x10, 0x86, 0x1C, 0x10, 0x00, 0x00, 0x41};

	uint8_t original_code_secondary_range1[10] ={0xF3, 0x0F, 0x10, 0x83, 0x2C, 0x10, 0x00, 0x00, 0x48, 0x8B};
	uint8_t original_code_secondary_range2[10] ={0xF3, 0x0F, 0x10, 0x83, 0x2C, 0x10, 0x00, 0x00, 0x48, 0x8B};

	// HOOK 
	uint8_t hook_code_move_speed1[10] = {0xF3, 0x0F, 0x10, 0x05, 0x64, 0xF0, 0x27, 0xFF, 0xF3, 0x0F};
	uint8_t hook_code_move_speed2[10] = {0xF3, 0x0F, 0x10, 0x05, 0xDA, 0x0B, 0x88, 0xFE, 0x48, 0x8B};

	uint8_t hook_code_knife_mark1[10] = {0xF3, 0x0F, 0x10, 0x35, 0x86, 0xB8, 0x26, 0xFF, 0x0F, 0x57};
	uint8_t hook_code_knife_mark2[10] = {0xF3, 0x0F, 0x10, 0x35, 0xDC, 0xE5, 0x86, 0xFE, 0xF3, 0x0F};
	uint8_t hook_code_knife_mark3[10] = {0xF3, 0x0F, 0x10, 0x35, 0xB0, 0xE5, 0x86, 0xFE, 0xF3, 0x0F};

	uint8_t hook_code_tap_distance1[10] = {0xF3, 0x0F, 0x10, 0x05, 0x21, 0x8B, 0x25, 0xFF, 0x48, 0x8B};
	uint8_t hook_code_tap_distance2[10] = {0xF3, 0x0F, 0x10, 0x05, 0xA4, 0xC2, 0x85, 0xFE, 0x48, 0x8B};
	uint8_t hook_code_tap_distance3[10] = {0xF3, 0x0F, 0x10, 0x05, 0x65, 0x8B, 0x25, 0xFF, 0x48, 0x8B};
	uint8_t hook_code_tap_distance4[10] = {0xF3, 0x0F, 0x10, 0x05, 0xE8, 0xC2, 0x85, 0xFE, 0x48, 0x8B};

	uint8_t hook_code_tap_range1[10] = {0xF3, 0x0F, 0x10, 0x05, 0xFB, 0x85, 0x25, 0xFF, 0x48, 0x8B};
	uint8_t hook_code_tap_range2[10] = {0xF3, 0x0F, 0x10, 0x05, 0x20, 0xBD, 0x85, 0xFE, 0x48, 0x8B};
	uint8_t hook_code_tap_range3[10] = {0xF3, 0x0F, 0x10, 0x05, 0x3F, 0x86, 0x25, 0xFF, 0x48, 0x8B};
	uint8_t hook_code_tap_range4[10] = {0xF3, 0x0F, 0x10, 0x05, 0x64, 0xBD, 0x85, 0xFE, 0x48, 0x8B};

	uint8_t hook_code_attack_distance1[10] = {0xF3, 0x0F, 0x10, 0x0D, 0xD8, 0xA7, 0x26, 0xFF, 0x90, 0xFF};
	uint8_t hook_code_attack_distance2[10] = {0xF3, 0x0F, 0x10, 0x0D, 0xC3, 0xA7, 0x26, 0xFF, 0x90, 0x0F};
	uint8_t hook_code_attack_distance3[10] = {0xF3, 0x0F, 0x10, 0x05, 0x56, 0xDD, 0x86, 0xFE, 0x90, 0x0F};
	uint8_t hook_code_attack_distance4[10] = {0xF3, 0x0F, 0x10, 0x05, 0x4E, 0x70, 0x45, 0xFE, 0x90, 0x0F};

	uint8_t hook_code_attack_range1[10] = {0xF3, 0x0F, 0x10, 0x05, 0x39, 0x87, 0x25, 0xFF, 0x48, 0x8B};
	uint8_t hook_code_attack_range2[10] = {0xF3, 0x0F, 0x10, 0x05, 0x5A, 0xBE, 0x85, 0xFE, 0x48, 0x8B};
	
	uint8_t hook_code_attack_speed1[10] = {0xF3, 0x0F, 0x10, 0x35, 0x0B, 0xBD, 0x27, 0xFF, 0xE8, 0xB6};
	uint8_t hook_code_attack_speed2[10] = {0xF3, 0x0F, 0x10, 0x35, 0xFF, 0xCC, 0x87, 0xFE, 0xE8, 0xAA};

	uint8_t hook_code_secondary_distance1[10] = {0xF3, 0x0F, 0x10, 0x0D, 0x52, 0xA7, 0x26, 0xFF, 0x90, 0x41};
	uint8_t hook_code_secondary_distance2[10] = {0xF3, 0x0F, 0x10, 0x0D, 0x2F, 0xA7, 0x26, 0xFF, 0x90, 0x0F};
	uint8_t hook_code_secondary_distance3[10] = {0xF3, 0x0F, 0x10, 0x05, 0xF1, 0xDC, 0x86, 0xFE, 0x90, 0x41};

	uint8_t hook_code_secondary_range1[10] = {0xF3, 0x0F, 0x10, 0x05, 0x55, 0x87, 0x25, 0xFF, 0x48, 0x8B};
	uint8_t hook_code_secondary_range2[10] = {0xF3, 0x0F, 0x10, 0x05, 0x76, 0xBE, 0x85, 0xFE, 0x48, 0x8B};

	if (hook)
	{
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_marks1, hook_code_knife_mark1, sizeof(hook_code_knife_mark1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_marks2, hook_code_knife_mark2, sizeof(hook_code_knife_mark2));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_marks3, hook_code_knife_mark3, sizeof(hook_code_knife_mark3));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_distance1, hook_code_tap_distance1, sizeof(hook_code_tap_distance1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_distance2, hook_code_tap_distance2, sizeof(hook_code_tap_distance2));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_distance3, hook_code_tap_distance3, sizeof(hook_code_tap_distance3));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_distance4, hook_code_tap_distance4, sizeof(hook_code_tap_distance4));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_range1, hook_code_tap_range1, sizeof(hook_code_tap_range1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_range2, hook_code_tap_range2, sizeof(hook_code_tap_range2));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_range3, hook_code_tap_range3, sizeof(hook_code_tap_range3));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_range4, hook_code_tap_range4, sizeof(hook_code_tap_range4));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_distance1, hook_code_attack_distance1, sizeof(hook_code_attack_distance1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_distance2, hook_code_attack_distance2, sizeof(hook_code_attack_distance2));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_distance3, hook_code_attack_distance3, sizeof(hook_code_attack_distance3));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_distance4, hook_code_attack_distance4, sizeof(hook_code_attack_distance4));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_range1, hook_code_attack_range1, sizeof(hook_code_attack_range1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_range2, hook_code_attack_range2, sizeof(hook_code_attack_range2));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_speed1, hook_code_attack_speed1, sizeof(hook_code_attack_speed1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_speed2, hook_code_attack_speed2, sizeof(hook_code_attack_speed2));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_secondary_distance1, hook_code_secondary_distance1, sizeof(hook_code_secondary_distance1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_secondary_distance2, hook_code_secondary_distance2, sizeof(hook_code_secondary_distance2));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_secondary_distance3, hook_code_secondary_distance3, sizeof(hook_code_secondary_distance3));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_secondary_range1, hook_code_secondary_range1, sizeof(hook_code_secondary_range1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_secondary_range2, hook_code_secondary_range2, sizeof(hook_code_secondary_range2));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_movement_speed1, hook_code_move_speed1, sizeof(hook_code_move_speed1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_movement_speed2, hook_code_move_speed2, sizeof(hook_code_move_speed2));
	}
	else
	{
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_marks1, original_code_knife_mark1, sizeof(original_code_knife_mark1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_marks2, original_code_knife_mark2, sizeof(original_code_knife_mark2));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_marks3, original_code_knife_mark3, sizeof(original_code_knife_mark3));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_distance1, original_code_tap_distance1, sizeof(original_code_tap_distance1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_distance2, original_code_tap_distance2, sizeof(original_code_tap_distance2));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_distance3, original_code_tap_distance3, sizeof(original_code_tap_distance3));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_distance4, original_code_tap_distance4, sizeof(original_code_tap_distance4));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_range1, original_code_tap_range1, sizeof(original_code_tap_range1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_range2, original_code_tap_range2, sizeof(original_code_tap_range2));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_range3, original_code_tap_range3, sizeof(original_code_tap_range3));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_tap_range4, original_code_tap_range4, sizeof(original_code_tap_range4));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_distance1, original_code_attack_distance1, sizeof(original_code_attack_distance1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_distance2, original_code_attack_distance2, sizeof(original_code_attack_distance2));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_distance3, original_code_attack_distance3, sizeof(original_code_attack_distance3));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_distance4, original_code_attack_distance4, sizeof(original_code_attack_distance4));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_range1, original_code_attack_range1, sizeof(original_code_attack_range1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_range2, original_code_attack_range2, sizeof(original_code_attack_range2));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_speed1, original_code_attack_speed1, sizeof(original_code_attack_speed1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_attack_speed2, original_code_attack_speed2, sizeof(original_code_attack_speed2));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_secondary_distance1, original_code_secondary_distance1, sizeof(original_code_secondary_distance1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_secondary_distance2, original_code_secondary_distance2, sizeof(original_code_secondary_distance2));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_secondary_distance3, original_code_secondary_distance3, sizeof(original_code_secondary_distance3));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_secondary_range1, original_code_secondary_range1, sizeof(original_code_secondary_range1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_secondary_range2, original_code_secondary_range2, sizeof(original_code_secondary_range2));

		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_movement_speed1, original_code_move_speed1, sizeof(original_code_move_speed1));
		mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_movement_speed2, original_code_move_speed2, sizeof(original_code_move_speed2));
	}
	is_hooked_knife = hook;
	VMProtectEnd();
}

bool Game::setKnifeData(const KnifeData& data)
{
	VMProtectBeginMutation(__FUNCSIG__);
	if(!is_hooked_knife)
		return false;

	mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_data_marks, &data.marks);
	mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_data_tap_distance, &data.tap_distance);
	mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_data_tap_range, &data.tap_range);
	mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_data_attack_distance, &data.attack_distance);
	mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_data_attack_range, &data.attack_range);
	mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_data_attack_speed, &data.attack_speed);
	mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_data_secondary_distance, &data.secondary_distance);
	mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_data_secondary_range, &data.secondary_range);
	mm_.forc_write(cshell_x64Module_ + crossfire_offset::knife_data_movement_speed, &data.movement_speed);

	VMProtectEnd();
	return true;
}
