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

	inline void read(std::ptrdiff_t address, void* buffer, size_t size)
	{
		ReadProcessMemory(targetProcess_, (PVOID)address, buffer, size, NULL);
	}
private:
	int pid_ = 0;
	Handle targetProcess_ = NULL;
	HMODULE hackDll = NULL;
};


