#include "logitech_mouse.h"
#include <assert.h>
#include <windows.h>
#include <winternl.h>
#pragma comment(lib, "ntdll.lib")


typedef struct {
	char button;
	char x;
	char y;
	char wheel;
	char unk1;
} MOUSE_IO;

static HANDLE g_input;
static IO_STATUS_BLOCK g_io;

BOOL g_found_mouse;

static BOOL callmouse(MOUSE_IO* buffer)
{
	IO_STATUS_BLOCK block;
	return NtDeviceIoControlFile(g_input, 0, 0, 0, &block, 0x2a2010, buffer, sizeof(MOUSE_IO), 0, 0) == 0L;
}

static NTSTATUS device_initialize(PCWSTR device_name)
{
	UNICODE_STRING name;
	OBJECT_ATTRIBUTES attr;

	RtlInitUnicodeString(&name, device_name);
	InitializeObjectAttributes(&attr, &name, 0, NULL, NULL);

	NTSTATUS status = NtCreateFile(&g_input, GENERIC_WRITE | SYNCHRONIZE, &attr, &g_io, 0,
		FILE_ATTRIBUTE_NORMAL, 0, 3, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, 0, 0);

	return status;
}

bool logi_mouse_open(void)
{
	NTSTATUS status = 0;
	wchar_t buffer[256] = {L'\0'};
	if (g_input == 0) 
	{
		wcscpy(buffer, L"\\??\\ROOT#SYSTEM#0000#{1abc05c0-c378-41b9-9cef-df1aba82b015}");
		for (int i = 0;i < 10; ++i)
		{
			buffer[19] = i + '0' ;
			status = device_initialize(buffer);
			if (NT_SUCCESS(status))
			{

				g_found_mouse = 1;
				break;
			}
		}
	}
	return status == 0;
}


void logi_mouse_close(void)
{
	if (g_input != 0) {
		NtClose(g_input);
		g_input = 0;
	}
}

bool logi_mouse_down(int key)
{
	MOUSE_IO io = { 0 };
	io.button = key;
	return callmouse(&io);
}

bool logi_mouse_up()
{
	MOUSE_IO io = { 0 };
	return callmouse(&io);
}

bool logi_mouse_click(int key)
{
	return logi_mouse_down(key) && logi_mouse_up();
}

bool logi_mouse_scroll(char dis)
{
	MOUSE_IO io = { 0 };
	io.wheel = -dis;
	return callmouse(&io);
}

bool logi_mouse_move_rel(int x, int y)
{
	MOUSE_IO io;
	memset(&io, 0, sizeof(io));
	int nx = abs(x) / SCHAR_MAX;
	int ny = abs(y) / SCHAR_MAX;
	if (nx > 0 || ny > 0)
	{
		bool sx = x < 0;
		bool sy = y < 0;
		x -= (sx ? -SCHAR_MAX : SCHAR_MAX) * nx;
		y -= (sy ? -SCHAR_MAX : SCHAR_MAX) * ny;
		do
		{
			memset(&io, 0, sizeof(io));
			if (nx > 0)
				io.x = sx ? -SCHAR_MAX : SCHAR_MAX;
			if (ny > 0)
				io.y = sy ? -SCHAR_MAX : SCHAR_MAX;
			if (!callmouse(&io))
				return false;

			if (nx > 0) nx--;
			if (ny > 0) ny--;
		} while (nx > 0 || ny > 0);
	}
	io.x = x;
	io.y = y;
	return callmouse(&io);
}