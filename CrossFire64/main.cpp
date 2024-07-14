#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include "Logger.h"
#include <TlHelp32.h>
#define xor_a(x)  x
#include "dwmapi.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <array>
#include "offset.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_win32.h"
#include "IMGUI/imgui_impl_dx9.h"


//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
//{
//  return 0;
//}

HWND gameWnd;
int gamePId;
HANDLE gameHandle;
HMODULE kernelHack;
ULONG64 crossfireBase;
ULONG64 cshell_x64Base;

std::string random_string() 
{
	srand((unsigned int)time((time_t*)0));
	std::string str = xor_a("QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890");
	std::string newstr;
	int pos;
	while (newstr.size() != 32) {
		pos = ((rand() % (str.size() + 1)));
		newstr += str.substr(pos, 1);
	}
	return newstr;
}

struct HandleDisposer
{
	using pointer = HANDLE;
	void operator()(HANDLE handle) const
	{
		if (handle != NULL || handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
		}
	}
};

using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;
std::uint32_t get_process_id(std::string_view process_name)
{
	PROCESSENTRY32 processentry;
	const unique_handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));

	if (snapshot_handle.get() == INVALID_HANDLE_VALUE)
		return NULL;

	processentry.dwSize = sizeof(MODULEENTRY32);

	while (Process32Next(snapshot_handle.get(), &processentry) == TRUE)
	{
		if (process_name.compare(processentry.szExeFile) == NULL)
		{
			return processentry.th32ProcessID;
		}
	}
	return NULL;
}

static ULONG64 get_module_base_x64(DWORD processID, const char* module_name)
{
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	HMODULE moduleBaseAddr = NULL;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	if (Module32First(hSnapshot, &me32)) {
		do {
			if (strcmp(me32.szModule, module_name) == 0) {
				moduleBaseAddr = me32.hModule;
				break;
			}
		} while (Module32Next(hSnapshot, &me32));
	}

	CloseHandle(hSnapshot);
	return (ULONG64)moduleBaseAddr;
}


LONG WindowWidth = 0;
LONG WindowHeight = 0;
HWND OverlayWnd = NULL;
WNDCLASSEX OverlayWndClass;
const char* className = "12312312";
const char * wndName = "14444";
LONG GameWindowWidth = 0;
LONG GameWindowHeight = 0;
LONG GameWindowLeft = 0;
LONG GameWindowRight = 0;
LONG GameWindowTop = 0;
LONG GameWindowBottom = 0;

namespace DirectX9Interface {
	IDirect3D9Ex* Direct3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParams = { NULL };
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message) {
	case WM_DESTROY:
		if (DirectX9Interface::pDevice != NULL) {
			DirectX9Interface::pDevice->EndScene();
			DirectX9Interface::pDevice->Release();
		}
		if (DirectX9Interface::Direct3D9 != NULL) {
			DirectX9Interface::Direct3D9->Release();
		}
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (DirectX9Interface::pDevice != NULL && wParam != SIZE_MINIMIZED) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			DirectX9Interface::pParams.BackBufferWidth = LOWORD(lParam);
			DirectX9Interface::pParams.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void SetupWindow()
{
	OverlayWndClass = {
		sizeof(WNDCLASSEX), 0, 
		WinProc, 0, 0, 
		 reinterpret_cast<HINSTANCE>(GetWindowLongA(gameWnd, (-6))) , 
		 LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, 
		className, LoadIcon(nullptr, IDI_APPLICATION)
	};

	RegisterClassEx(&OverlayWndClass);
	if (gameWnd) {
		static RECT TempRect = { NULL };
		static POINT TempPoint;
		GetClientRect(gameWnd, &TempRect);
		ClientToScreen(gameWnd, &TempPoint);
		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		GameWindowWidth = TempRect.right;
		GameWindowHeight = TempRect.bottom;
	}

	RECT TempRect;
	GetWindowRect(gameWnd, &TempRect);
	GameWindowWidth = TempRect.right - TempRect.left;
	GameWindowHeight = TempRect.bottom - TempRect.top;
	GameWindowLeft = TempRect.left;
	GameWindowRight = TempRect.right;
	GameWindowTop = TempRect.top;
	GameWindowBottom = TempRect.bottom;

	OverlayWnd = CreateWindowEx(NULL, className, className, WS_POPUP | WS_VISIBLE, GameWindowLeft, GameWindowTop, WindowWidth, WindowHeight, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(OverlayWnd, &DirectX9Interface::Margin);
	SetWindowLong(OverlayWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	//SetWindowLong(OverlayWnd, GWL_EXSTYLE, WS_EX_LAYERED);
	SetWindowPos(OverlayWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	ShowWindow(OverlayWnd, SW_SHOW);
	UpdateWindow(OverlayWnd);
}


void LoadFonts()
{
	ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();  // 添加默认字体

	// 添加中文字体
	static const ImWchar ranges[] =
	{
			0x0020, 0x00FF, // Basic Latin
			0x4e00, 0x9FAF, // CJK Ideograms
			0,
	};
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simhei.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simhei.ttf", 16.0f, NULL, ranges);
}
bool DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::Direct3D9))) {
		return false;
	}

	D3DPRESENT_PARAMETERS& Params = DirectX9Interface::pParams;
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = OverlayWnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = GameWindowWidth;
	Params.BackBufferHeight = GameWindowHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if (FAILED(DirectX9Interface::Direct3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, OverlayWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9Interface::pDevice))) {
		DirectX9Interface::Direct3D9->Release();
		return false;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


	ImGui_ImplWin32_Init(OverlayWnd);
	ImGui_ImplDX9_Init(DirectX9Interface::pDevice);
	DirectX9Interface::Direct3D9->Release();
	LoadFonts();
	return true;
}

int InitializeStyle = 0;
bool MenuShow = true;

void TerCleanMenu() {

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


}


void load_styles()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	{

		colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(1.000f, 0.000f, 0.365f, 0.818f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 0.000f, 0.365f, 0.818f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_Button] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);;
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 0.000f, 0.365f, 0.818f);
		colors[ImGuiCol_Header] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.93f, 0.65f, 0.14f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.000f, 0.365f, 0.818f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.000f, 0.365f, 0.818f);;
		colors[ImGuiCol_Tab] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.040f, 0.298f, 0.008f, 1.000f);
		colors[ImGuiCol_TabActive] = ImVec4(1.000f, 0.000f, 0.365f, 0.818f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);


		InitializeStyle = 1;
	}

}

int InitializeMenus;
void TerInitMenu() {

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (!InitializeStyle)
		load_styles();

	ImGui::GetIO().MouseDrawCursor = MenuShow;
}


void TerFinishMenu() {

	//ImGui::End();
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

}

bool bESP = true;
D3DXVECTOR3 pos{ -949.197998 ,-2354.241211 , -106.0632935 };
D3DXVECTOR3 Gpos;
bool test = false;
void TerDrawMenu(IDirect3DDevice9*) {


	ImGui::SetNextWindowSize({ 280.f,370.f });
	ImGui::Begin(xor_a("[CFBR] External FRCHEATS"), 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);


	if (ImGui::CollapsingHeader(xor_a("ESP"))) {
		ImGui::SetCursorPos({ 10.f,80.f });
		//ImGui::Checkbox(xor_a("ESP"), &bESP);
		if(ImGui::Button("esp"))
		{
			bESP = !bESP;
		}


		ImGui::InputFloat("x", &pos.x);
		ImGui::InputFloat("y", &pos.y);
		ImGui::InputFloat("z", &pos.z);

		if(ImGui::Button("Test"))
		{
			Gpos = pos;
			test = true;
		}

		if (ImGui::Button("Cancel"))
		{
			test = false;
		}


	}



	ImGui::End();
}

//---------------------------------------------------------------------------------------------------------------------------------------------
template<typename RET>
RET Read(std::ptrdiff_t address)
{
	RET ret;
	ReadProcessMemory(gameHandle, (PVOID)address, &ret, sizeof(ret),NULL);
	return ret;
}

void Read(std::ptrdiff_t address, void* buffer, size_t size)
{
	ReadProcessMemory(gameHandle, (PVOID)address, buffer, size, NULL);
}

bool WorldToScreen(D3DXVECTOR3* InOut)
{
	D3DXMATRIX View = Read<D3DXMATRIX>(0x14056E9A0 - 0x80);
	D3DXMATRIX Projection = Read<D3DXMATRIX>(0x14056E9A0 -0x40);


	D3DVIEWPORT9 viewport = Read<D3DVIEWPORT9>(0x14056E9A0);

	D3DXVECTOR3 vScreen;
	D3DXVECTOR3 PlayerPos(InOut->x, InOut->y, InOut->z);

	D3DXMATRIX world = D3DXMATRIX(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	D3DXVec3Project(&vScreen, &PlayerPos, &viewport, &Projection, &View, &world);
	if (vScreen.z >= 0.0f && vScreen.z <= 1.0f) {
		*InOut = vScreen;
		return true;
	}
	return false;
}


enum  BoneIndex:int
{
	head = 6,
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
D3DXVECTOR3 invalidPos {-1,-1,-1};

#include <vector>
std::vector<int>  BoneIndexRightLeg{ foot_r , shank_r , hip_r ,hip };
std::vector<int>  BoneIndexLeftLeg{ foot_l , shank_l , hip_l, hip };
std::vector<int>  BoneIndexSpine{ hip , spine };
std::vector<int>  BoneIndexRightArm{ hand_r , elbow_r, shoulder_r, spine };
std::vector<int>  BoneIndexLeftArm{ hand_l , elbow_l, shoulder_l, spine };
std::vector<int>  BoneIndexHead{ spine , head };

void DrawBonePart(std::array<D3DXVECTOR3, BoneCount>& BonePos, const std::vector<int>& BonePartIndex)
{
	assert(BonePartIndex.size() % 2 == 0);
	D3DXVECTOR3 prevPosint;
	for (size_t i = 0; i < BonePartIndex.size(); i++)
	{
		D3DXVECTOR3 pos = BonePos[BonePartIndex[i]];
		if (i == 0)
		{
			prevPosint = pos;
			continue;
		}


		ImGui::GetOverlayDrawList()->AddLine(
		ImVec2(prevPosint.x, prevPosint.y),
		ImVec2(pos.x, pos.y),
		ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)));
		prevPosint = pos;
	}
}

void PlayerESP()
{
	std::array<D3DXVECTOR3, BoneCount> BonePos;
	std::array<ImVec2, 15>  BonePosArray;
	if(bESP)
	{
		std::ptrdiff_t CLTClientShell = Read<std::ptrdiff_t>(cshell_x64Base + 0x300B0E8);
		int local_player_index = Read<uint16_t>(CLTClientShell + global_offset::local_player_index);
		std::ptrdiff_t local_player = CLTClientShell + global_offset::player_start + local_player_index * global_offset::player_size;
		uint8_t local_player_team = Read<uint8_t>(local_player + global_offset::player_team);
		for(int i = 0; i < 16; ++i)
		{
			char player_name[14];
			std::ptrdiff_t cplayer = CLTClientShell + global_offset::player_start + i * global_offset::player_size;
			uint8_t player_team = Read<uint8_t>(cplayer + global_offset::player_team);
			int player_hp = Read<int>(cplayer + global_offset::player_hp);
			Read(cplayer + global_offset::player_name, player_name, sizeof(player_name));

			if(i == local_player_index)
				continue;

			if(local_player_team == player_team)
				continue;

			if(player_hp == 0)
				continue;
				

			std::ptrdiff_t CharacFx = Read<ptrdiff_t>(cplayer + global_offset::player_CharacFx);
			if(CharacFx == 0)
				continue;

			std::ptrdiff_t model_instance = Read<ptrdiff_t>(cplayer + global_offset::model_instance);
			if (model_instance == 0)
				continue;

			bool canDrawBone = true;
			int boneCount = Read<int>(model_instance + global_offset::bone_count);
			ptrdiff_t boneMatrixArray = Read<ptrdiff_t>(model_instance + global_offset::bone_matrix);
			for (int j = 0; j < BoneCount; ++j)
			{
				D3DXMATRIX matrix = Read<D3DXMATRIX>(boneMatrixArray + j * sizeof(D3DXMATRIX));
				D3DXVECTOR3 bonePos {matrix._14, matrix._24, matrix._34};

				if (WorldToScreen(&bonePos))
				{
					BonePos[j] = bonePos;
					/*char buffer[20];
					sprintf(buffer, "%d", j);
					ImGui::GetForegroundDrawList()->AddText(ImVec2(bonePos.x, bonePos.y), 
					ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)), buffer);*/
				}
				else
				{
					BonePos[j] = invalidPos;
					canDrawBone = false;
				}
			}


			// draw bone
			if(canDrawBone)
			{
				DrawBonePart(BonePos, BoneIndexRightLeg);
				DrawBonePart(BonePos, BoneIndexLeftLeg);
				DrawBonePart(BonePos, BoneIndexSpine);
				DrawBonePart(BonePos, BoneIndexRightArm);
				DrawBonePart(BonePos, BoneIndexLeftArm);
				DrawBonePart(BonePos, BoneIndexHead);

				const D3DXVECTOR3& pos = BonePos[foot_l];
				ImGui::GetOverlayDrawList()->AddText({ pos.x,pos .y}, ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)), player_name);
			}

			constexpr float Padding = 10;
			//draw rect
		/*	float top_x = 0;
			float top_y = 0;
			float bottom_x = 0;
			float bottom_y = 0;
			bool init = false;
			for(int i = 0; i< BonePos.size(); ++i)
			{
				D3DXVECTOR3& pos = BonePos[i];

				if (pos == invalidPos)
					continue;
				if(!init)
				{
					top_x = bottom_x = pos.x;
					top_y = bottom_y = pos.y;
					init = true;
					continue;
				}

				if (pos.x > top_x)
					top_x = pos.x;

				if (pos.y > top_y)
					top_y = pos.y;

				if (pos.x < bottom_x)
					bottom_x = pos.x;

				if (pos.y < bottom_y)
					bottom_y = pos.y;

			}

			ImGui::GetOverlayDrawList()->AddRect(
				ImVec2(top_x + Padding, top_y + Padding),
				ImVec2(bottom_x - Padding, bottom_y - Padding),
				ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)));*/


#if 0
			/*playerPos = Read<D3DXVECTOR3>(CharacFx + global_offset::player_x);
			if (WorldToScreen(&playerPos))
			{
				ImGui::GetForegroundDrawList()->AddText(ImVec2(playerPos.x, playerPos.y), ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)), "T");
			}*/

			D3DXVECTOR3 playerPos;
			playerPos = Read<D3DXVECTOR3>(CharacFx + 0x17c);
			if (WorldToScreen(&playerPos))
			{
				ImGui::GetForegroundDrawList()->AddText(ImVec2(playerPos.x, playerPos.y), ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)), "T");
			}

			playerPos = Read<D3DXVECTOR3>(CharacFx + 0x1a4);
			if (WorldToScreen(&playerPos))
			{
				ImGui::GetForegroundDrawList()->AddText(ImVec2(playerPos.x, playerPos.y), ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)), "T");
			}

			playerPos = Read<D3DXVECTOR3>(CharacFx + 0x1f4);
			if (WorldToScreen(&playerPos))
			{
				ImGui::GetForegroundDrawList()->AddText(ImVec2(playerPos.x, playerPos.y), ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)), "T");
			}

			playerPos = Read<D3DXVECTOR3>(CharacFx + 0x224);
			if (WorldToScreen(&playerPos))
			{
				ImGui::GetForegroundDrawList()->AddText(ImVec2(playerPos.x, playerPos.y), ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)), "T");
			}

#endif // 0

	

			//D3DXVECTOR3 playerPos = Read<D3DXVECTOR3>(CharacFx + global_offset::player_x);
			//D3DXVECTOR3 playerPos = Read<D3DXVECTOR3>(CharacFx + 0x17c);
			//D3DXVECTOR3 playerPos = Read<D3DXVECTOR3>(CharacFx + 0x1a4);
			//D3DXVECTOR3 playerPos = Read<D3DXVECTOR3>(CharacFx + 0x1f4);
			//D3DXVECTOR3 playerPos = Read<D3DXVECTOR3>(CharacFx + 0x224);
			//playerPos.x = Read<float>(CharacFx + global_offset::player_x);
			//playerPos.y = Read<float>(CharacFx + global_offset::player_y);
			//playerPos.z = Read<float>(CharacFx + global_offset::player_z);
		/*	if (WorldToScreen(&playerPos))
			{
				ImGui::GetForegroundDrawList()->AddText(ImVec2(playerPos.x, playerPos.y), ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)), "TEST");
			}*/
			
		}

	}
	

	if(test)
	{
		D3DXVECTOR3 tPos = Gpos;
		if(WorldToScreen(&tPos))
		{
			ImGui::GetForegroundDrawList()->AddText(ImVec2( tPos.x,tPos.y ), ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)), "TEST");
		}
	}
}


IDirect3DDevice9* m_TerDevice;
void TerRenderMenu(IDirect3DDevice9* TerDevice) {

	if (TerDevice != m_TerDevice) {

		if (InitializeMenus) TerCleanMenu();
			m_TerDevice = TerDevice;
	}

	if (GetAsyncKeyState(VK_INSERT) & 1)
		MenuShow = !MenuShow;

	if (TerDevice) {

		TerInitMenu();

		//if (MenuShow)
		//	TerDrawMenu(TerDevice);

		PlayerESP();
		TerFinishMenu();

	}
}

void Render() {


	TerRenderMenu(DirectX9Interface::pDevice);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_ZENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

	DirectX9Interface::pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (DirectX9Interface::pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		DirectX9Interface::pDevice->EndScene();
	}

	HRESULT result = DirectX9Interface::pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && DirectX9Interface::pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}


void MainLoop() {
	static RECT OldRect;
	ZeroMemory(&DirectX9Interface::Message, sizeof(MSG));

	while (DirectX9Interface::Message.message != WM_QUIT) {
		if (PeekMessage(&DirectX9Interface::Message, OverlayWnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&DirectX9Interface::Message);
			DispatchMessage(&DirectX9Interface::Message);
		}
		HWND ForegroundWindow = GetForegroundWindow();
		if (ForegroundWindow == gameWnd) 
		{
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos(OverlayWnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT TempRect;
		POINT TempPoint;
		ZeroMemory(&TempRect, sizeof(RECT));
		ZeroMemory(&TempPoint, sizeof(POINT));

		GetClientRect(gameWnd, &TempRect);
		ClientToScreen(gameWnd, &TempPoint);

		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		//ImGuiIO& io = ImGui::GetIO();
		////io.ImeWindowHandle = gameWnd;

		//POINT TempPoint2;
		//GetCursorPos(&TempPoint2);
		//io.MousePos.x = TempPoint2.x - TempPoint.x;
		//io.MousePos.y = TempPoint2.y - TempPoint.y;
		//if (GetAsyncKeyState(VK_LBUTTON)) {
		//	static int i = 0;

		//	//Logger::info("GetAsyncKeyState %d\n", i);
		//	io.MouseDown[0] = true;
		//	io.MouseClicked[0] = true;
		//	io.MouseClickedPos[0].x = io.MousePos.x;
		//	io.MouseClickedPos[0].x = io.MousePos.y;
		//}
		//else {
		//	io.MouseDown[0] = false;
		//}

		if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
			OldRect = TempRect;
			GameWindowWidth = TempRect.right;
			GameWindowHeight = TempRect.bottom;
			DirectX9Interface::pParams.BackBufferWidth = GameWindowWidth;
			DirectX9Interface::pParams.BackBufferHeight = GameWindowHeight;
			SetWindowPos(OverlayWnd, (HWND)0, TempPoint.x, TempPoint.y, GameWindowWidth, GameWindowHeight, SWP_NOREDRAW);
			DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		}

		Render();

}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (DirectX9Interface::pDevice != NULL) {
		DirectX9Interface::pDevice->EndScene();
		DirectX9Interface::pDevice->Release();
	}
	if (DirectX9Interface::Direct3D9 != NULL) {
		DirectX9Interface::Direct3D9->Release();
	}
	DestroyWindow(OverlayWnd);
	UnregisterClass(OverlayWndClass.lpszClassName, OverlayWndClass.hInstance);
}


int main() 
{
	
	char name[14] { 0xC9, 0xA2, 0xC8, 0xC8, 0xD2, 0xB2, 0xBA, 0xC3, 0xCC, 0xAB, 0xB9, 0xFD, 0x00 };

#if 0
	std::thread randomTitleThd([]() {
		while (true) {
			SetConsoleTitleA(random_string().c_str());
			Sleep(300);
		}
		});
	randomTitleThd.detach();
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);


	//while (true)
//{
//	if (gameWnd = FindWindowA("CrossFire", "穿越火线"))
//		break;
//	Logger::info("未找到游戏窗口");
//	Sleep(300);
//}
#endif
	kernelHack = LoadLibraryA("DLL.dll");

	gamePId = get_process_id("crossfire.exe");
	gameHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, gamePId);
	gameWnd = FindWindowA("CrossFire", "穿越火线");
	crossfireBase = get_module_base_x64(gamePId, "crossfire.exe");
	cshell_x64Base = get_module_base_x64(gamePId, "cshell_x64.dll");


	SetupWindow();
	DirectXInit();
	while (true)
	{
		MainLoop();
	}


  getchar();
  return 0;
}