#pragma once
#include <string_view>
#include <array>
#include <cstddef>
#include <string>
#include <utility>
#include <d3d9.h>
#include <d3dx9math.h>
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

struct KnifeData
{
	 float marks;
	 float tap_distance;
	 float tap_range ;
	 float attack_distance;
	 float attack_range;
	 float attack_speed ;
	 float secondary_distance;
	 float secondary_range;
	 float movement_speed;
};

struct ViewAngle
{
	float yaw; // º½Ïò½Ç
	float pitch; // ¸©Ñö½Ç
};


class IntersectQuery {
public:
	IntersectQuery() {
		m_Flags = 0;
		m_FilterFn = 0;
		m_PolyFilterFn = 0;
		m_FilterActualIntersectFn = 0;
		m_pActualIntersectUserData = NULL;
		m_pUserData = NULL;
	}
	D3DXVECTOR3 m_From;
	D3DXVECTOR3 m_To;
	uint32_t m_Flags;
	void* m_FilterFn;
	__int64 m_FilterActualIntersectFn;
	__int64 m_PolyFilterFn;
	void* m_pUserData;
	void* m_pActualIntersectUserData;
};

struct IntersectInfo
{
	D3DXVECTOR3 vImpactPos;
	char spacer00[32];
	__int64 hObjImpact;
	unsigned long unk1;
	unsigned long unk2;
	unsigned long unk3;
};

typedef bool(WINAPIV* IntersectSegment_t)(const IntersectQuery& iQuery, IntersectInfo* pInfo);

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
	bool update();

	float GetDistance3D(const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2);
	float GetDistance2D(const D3DXVECTOR2& pos1, const D3DXVECTOR2& pos2);
	bool WorldToScreen(const D3DXVECTOR3& worldPos, D3DXVECTOR2& screenPos);

	bool validPlayerIndex(int index);
	int getLocalPlayerIndex();
	bool validPlayer(std::ptrdiff_t player);
	std::ptrdiff_t getPlayer(int index);

	D3DXVECTOR3 getFOVPos();

	bool isVisible(D3DXVECTOR3 from, D3DXVECTOR3 to);

	// Player
	bool playerHasC4(std::ptrdiff_t player);
	int getPlayerTeam(std::ptrdiff_t player) ;
	int getPlayerHP(std::ptrdiff_t player) ;
	void getPlayerBones(std::ptrdiff_t player, std::array<D3DXVECTOR3, BoneCount>& bones) ;
	std::string getPlayerName(std::ptrdiff_t player);
	bool getPlayerAngle(std::ptrdiff_t player, ViewAngle& agnle);
	bool setLocalPlayerAngle(const ViewAngle& agnle);
	std::string getPlayerWeaponName(std::ptrdiff_t player);

	// HOOK Knife
  void hookKnifeData(bool hook);
	bool setKnifeData(const KnifeData& data);

private:  
  int pid_ = 0;
  HWND gameWindow_ = NULL;
	std::ptrdiff_t crossfireModule_ = NULL;
	std::ptrdiff_t cshell_x64Module_ = NULL;
  Memory mm_;

	IntersectSegment_t  IntersectSegment_;
	std::ptrdiff_t CLTClientShell = 0;
	std::ptrdiff_t CAIBotModePlayer = 0;

	bool is_hooked_knife = false;
};

