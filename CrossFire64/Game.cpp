#include "Game.h"
#include "util.h"
#include "offset.h"
#include <assert.h>
#include <cmath>
#include "xorstr.hpp"
#include <stdexcept>

static const char* s_GameProcessName = NULL;
static const char* s_GameWindowTitleName = NULL;
static const char* s_GameWindowClassName = NULL;

Game::Game()
{
	s_GameProcessName = ("crossfire.exe");
	s_GameWindowTitleName = ("´©Ô½»ðÏß");
	s_GameWindowClassName = ("CrossFire");
}

Game::~Game()
{
}

bool Game::init()
{
  pid_ = util::get_process_id(s_GameProcessName);
  gameWindow_ = FindWindowA(s_GameWindowClassName, s_GameWindowTitleName);
	if(pid_ == 0 || gameWindow_ == NULL)
		return false;

	if(!mm_.init() || !mm_.attach(pid_))
		return false;

	crossfireModule_ = (std::ptrdiff_t)util::get_module_base_x64(pid_, xorstr_("crossfire.exe"));
	cshell_x64Module_ = (std::ptrdiff_t)util::get_module_base_x64(pid_, xorstr_("cshell_x64.dll"));
	if (crossfireModule_ == 0 || cshell_x64Module_ == 0)
		return false;

	CLTClientShell = mm_.read<std::ptrdiff_t>(cshell_x64Module_ + crossfire_offset::CLTClientShell);
	if (CLTClientShell == 0)
		return false;

	CAIBotModePlayer = mm_.read<std::ptrdiff_t>(cshell_x64Module_ + crossfire_offset::CAIBotModePlayer);
	if (CAIBotModePlayer == 0)
		return false;

	//float a = 600.0f;
	//mm_.forc_write(cshell_x64Module_ + 0X710, &a);

	//int b = 0;
	//mm_.forc_write(cshell_x64Module_ + 0x700, &b);

  return true;
}

bool Game::waitStart()
{
	while(FindWindowA(s_GameWindowClassName, s_GameWindowTitleName) == NULL)
		Sleep(300);
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
	return false;
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
	std::ptrdiff_t CharacFx = mm_.read<ptrdiff_t>(player + crossfire_offset::player_CharacFx);
	if (CharacFx == 0)
		return false;

	std::ptrdiff_t model_instance = mm_.read<ptrdiff_t>(player + crossfire_offset::model_instance);
	if (model_instance == 0)
		return false;

	return true;
}

std::ptrdiff_t Game::getPlayer(int index)
{
	assert(index >= 0 && index < player_count);
	return CLTClientShell + crossfire_offset::player_start + index * crossfire_offset::player_size;
}

D3DXVECTOR3 Game::getFOVPos()
{
	D3DXMATRIX View = mm_.read<D3DXMATRIX>(crossfireModule_ + crossfire_offset::d3d_view_matrix);
	D3DXMatrixInverse(&View, 0, &View);
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
}

std::string Game::getPlayerName(std::ptrdiff_t player)
{
	std::string name;
	name.resize(14);

	mm_.read(player + crossfire_offset::player_name, name.data(), name.size());
	return util::CharToUtf8(name);
}

bool Game::getPlayerAngle(std::ptrdiff_t player, ViewAngle& agnle)
{
	std::ptrdiff_t CharacFx = mm_.read<ptrdiff_t>(player + crossfire_offset::player_CharacFx);
	if (CharacFx == 0)
		return false;

	agnle.yaw = mm_.read<float>(CharacFx + crossfire_offset::player_yaw);
	agnle.pitch = mm_.read<float>(CharacFx + crossfire_offset::player_pitch);
	return true;
}

bool Game::setLocalPlayerAngle(const ViewAngle& agnle)
{
	mm_.write(CAIBotModePlayer + crossfire_offset::local_angle_yaw, &agnle.yaw);
	mm_.write(CAIBotModePlayer + crossfire_offset::local_angle_pitch, &agnle.pitch);
	return false;
}

std::string Game::getPlayerWeaponName(std::ptrdiff_t player)
{
	std::string name;
	std::ptrdiff_t CharacFx = mm_.read<ptrdiff_t>(player + crossfire_offset::player_CharacFx);
	if (CharacFx == 0)
		return "";

	char buffer[33] = {'\0'};
	std::ptrdiff_t Weapon = mm_.read<ptrdiff_t>(CharacFx + crossfire_offset::player_weapon);

	mm_.read(Weapon + crossfire_offset::weapon_name, buffer, sizeof(buffer) -1);
	return util::CharToUtf8(buffer);
}
