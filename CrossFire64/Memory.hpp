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

private:
	int pid_ = 0;
	Handle targetProcess_ = NULL;
	HMODULE hackDll = NULL;
};


