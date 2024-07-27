#include "util.h"
#include <tlhelp32.h>


std::string util::random_string()
{
	srand((unsigned int)time((time_t*)0));
	std::string str = xorstr_("QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890");
	std::string newstr;
	int pos;
	while (newstr.size() != 32) {
		pos = ((rand() % (str.size() + 1)));
		newstr += str.substr(pos, 1);
	}
	return newstr;
}

int util::get_process_id(std::string_view name)
{
	PROCESSENTRY32 processentry;
	const Handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));

	if (!snapshot_handle)
		return 0;

	processentry.dwSize = sizeof(MODULEENTRY32);

	while (Process32Next(snapshot_handle, &processentry) == TRUE)
	{
		if (name.compare(processentry.szExeFile) == NULL)
		{
			return (int)processentry.th32ProcessID;
		}
	}
	return 0;
}

HMODULE util::get_module_base_x64(DWORD processID, const char* module_name)
{
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	HMODULE moduleBaseAddr = NULL;

	Handle hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
	if (!hSnapshot) {
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

	return moduleBaseAddr;
}

std::string util::CharToUtf8(const std::string& asciiStr) 
{

	int wideCharLength = MultiByteToWideChar(CP_ACP, 0, asciiStr.c_str(), -1, NULL, 0);
	if (wideCharLength == 0) 
		return "";

	std::wstring wideStr(wideCharLength, 0);
	MultiByteToWideChar(CP_ACP, 0, asciiStr.c_str(), -1, &wideStr[0], wideCharLength);

	int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, NULL, 0, NULL, NULL);
	if (utf8Length == 0)
		return "";

	std::string utf8Str(utf8Length, 0);
	WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], utf8Length, NULL, NULL);

	return utf8Str;
}