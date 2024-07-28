#pragma once
#include <cstddef>
#include "NativeHandle.h"

class Memory
{
public:
	Memory();
	~Memory();

	bool init();
	void destory();
	inline bool isInited() const { return hackDll != NULL;};

	bool attach(int pid);
	void detach();
	inline bool isAttach(){ return targetProcess_;};
	
	template<typename RET>
	inline RET read(std::ptrdiff_t address)
	{
		RET ret;
		ReadProcessMemory(targetProcess_, (PVOID)address, &ret, sizeof(ret), NULL);
		return ret;
	}

	template<typename T>
	inline bool write(std::ptrdiff_t address, const T* value)
	{
		return WriteProcessMemory(targetProcess_, (PVOID)address, value, sizeof(T), NULL) == TRUE;
	}

	inline void read(std::ptrdiff_t address, void* buffer, size_t size)
	{
		ReadProcessMemory(targetProcess_, (PVOID)address, buffer, size, NULL);
	}

	inline void write(std::ptrdiff_t address, void* buffer, size_t size)
	{
		WriteProcessMemory(targetProcess_, (PVOID)address, buffer, size, NULL);
	}

	template<typename T>
	inline bool forc_write(std::ptrdiff_t address, const T* value)
	{
		bool ret = false;
		DWORD oldProetct = 0;
		VirtualProtectEx(targetProcess_, (LPVOID)address, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProetct);
		ret = WriteProcessMemory(targetProcess_, (PVOID)address, value, sizeof(T), NULL) == TRUE;
		VirtualProtectEx(targetProcess_, (LPVOID)address, sizeof(T), oldProetct, &oldProetct);
		return ret;
	}

	inline bool forc_write(std::ptrdiff_t address, const void* value, size_t size)
	{
		bool ret = false;
		DWORD oldProetct = 0;
		VirtualProtectEx(targetProcess_, (LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProetct);
		ret = WriteProcessMemory(targetProcess_, (PVOID)address, value, size, NULL) == TRUE;
		VirtualProtectEx(targetProcess_, (LPVOID)address, size, oldProetct, &oldProetct);
		return ret;
	}

private:
	int pid_ = 0;
	Handle targetProcess_ = NULL;
	HMODULE hackDll = NULL;
};


