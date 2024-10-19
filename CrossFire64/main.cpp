#include "Render.h"
#include "Logger.h"
#include "xorstr.hpp"
#include "VMProtectSDK.h"
#include "util.h"

static void setupConsole(std::string title)
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w+", stdout);
	freopen_s(&stream, "CONOUT$", "w+", stderr);
	SetConsoleTitleA(title.c_str());
}

static
BOOL EnablePrivilege(LPCTSTR szPrivilege, BOOL fEnable)
{
	BOOL fOk = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, szPrivilege, &tp.Privileges[0].Luid);
		tp.Privileges->Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOk;
}

#if 0
int main()
#else
int WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
#endif
{
	VMProtectBeginUltra(__FUNCSIG__);
	EnablePrivilege(SE_DEBUG_NAME, TRUE);
	setupConsole(util::random_string());

	if (!kernel_initialize())
	{
		Logger::info(xorstr_("failed to attach access"));
		return 1;
	}

	Game crossfire;
	Logger::info(xorstr_("start"));
	Logger::info(xorstr_("find game ..."));
	if (!crossfire.waitStart() || !crossfire.init())
	{
		Logger::info(xorstr_("游戏初始化失败") );
		return 1;
	}
	Logger::info(xorstr_("游戏初始化完毕"));

	try
	{
		MenuConfig.load();
		Gui.AttachAnotherWindow(crossfire.WindowTitle(), crossfire.WindowClassName(), Render(crossfire));
		//Gui.NewWindow("WindowName", Vec2(500, 500), Render(crossfire), true);
	}
	catch (const OSImGui::OSException& e)
	{
		Logger::info(xorstr_("OSImGui Exception: %s"), e.what());
	}
	VMProtectEnd();
  return 0;
}