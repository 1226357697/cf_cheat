#include "Game.h"
#include "util.h"
#include "offset.h"
#include <assert.h>
#include <cmath>

Game::~Game()
{
}

bool Game::init(std::string_view processName, std::string_view gameWindowName, std::string_view gameWindowClass)
{
  pid_ = util::get_process_id(processName);
  gameWindow_ = FindWindowA(gameWindowClass.data(), gameWindowName.data());
	if(pid_ == 0 || gameWindow_ == NULL)
		return false;

	if(!mm_.init() || !mm_.attach(pid_))
		return false;

	crossfireModule_ = (std::ptrdiff_t)util::get_module_base_x64(pid_, "crossfire.exe");
	cshell_x64Module_ = (std::ptrdiff_t)util::get_module_base_x64(pid_, "cshell_x64.dll");
	if (crossfireModule_ == 0 || cshell_x64Module_ == 0)
		return false;

	CLTClientShell = mm_.read<std::ptrdiff_t>(cshell_x64Module_ + crossfire_offset::CLTClientShell);
	if (CLTClientShell == 0)
		return false;

  return true;
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
	D3DXMATRIX View = mm_.read<D3DXMATRIX>(0x14056E9A0 - 0x80);
	D3DXMATRIX Projection = mm_.read<D3DXMATRIX>(0x14056E9A0 - 0x40);


	D3DVIEWPORT9 viewport = mm_.read<D3DVIEWPORT9>(0x14056E9A0);

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
