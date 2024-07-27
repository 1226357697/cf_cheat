#pragma once
#include <string_view>
#include <d3dx9.h>
#include <array>
#include <cstddef>
#include <string>
#include <utility>
#include "Memory.hpp"


enum  BoneIndex :int
{
	head = 6,
	neck = 5,
	shoulder_r = 15,
	shoulder_l = 8,

	elbow_r = 16,
	elbow_l = 9,

	hand_r = 17,
	hand_l = 10,

	spine = 14,
	hip = 0,

	hip_r = 25,
	hip_l = 21,

	shank_r = 26,
	shank_l = 22,

	foot_r = 27,
	foot_l = 23,
};

constexpr int BoneCount = 29;
constexpr int player_count = 16;
const D3DXVECTOR3 invalidWorldPos{ -1,-1,-1 };
const D3DXVECTOR2 invalidScreenPos{ -1,-1};


struct ViewAngle
{
	float yaw; // º½Ïò½Ç
	float pitch; // ¸©Ñö½Ç
};

class Game
{
public:
  Game();
  ~Game();
  bool init();
  inline HWND window()const {return gameWindow_;}
  inline int pid() {return pid_;}

	bool waitStart();
	const char* WindowTitle();
	const char* WindowClassName();
	std::pair<int, int> getWindowSize();

	float GetDistance3D(const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2);
	float GetDistance2D(const D3DXVECTOR2& pos1, const D3DXVECTOR2& pos2);
	bool WorldToScreen(const D3DXVECTOR3& worldPos, D3DXVECTOR2& screenPos);

	bool validPlayerIndex(int index);
	int getLocalPlayerIndex();
	bool validPlayer(std::ptrdiff_t player);
	std::ptrdiff_t getPlayer(int index);

	D3DXVECTOR3 getFOVPos();

	// Player
	bool playerHasC4(std::ptrdiff_t player);
	int getPlayerTeam(std::ptrdiff_t player) ;
	int getPlayerHP(std::ptrdiff_t player) ;
	void getPlayerBones(std::ptrdiff_t player, std::array<D3DXVECTOR3, BoneCount>& bones) ;
	std::string getPlayerName(std::ptrdiff_t player);
	bool getPlayerAngle(std::ptrdiff_t player, ViewAngle& agnle);
	bool setLocalPlayerAngle(const ViewAngle& agnle);
	std::string getPlayerWeaponName(std::ptrdiff_t player);

  
private:  
  int pid_ = 0;
  HWND gameWindow_ = NULL;
	std::ptrdiff_t crossfireModule_ = NULL;
	std::ptrdiff_t cshell_x64Module_ = NULL;
  Memory mm_;

	std::ptrdiff_t CLTClientShell = 0;
	std::ptrdiff_t CAIBotModePlayer = 0;
};

