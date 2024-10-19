#pragma once
#include <cstddef>
#include "../access/access.h"

class Memory
{
public:
	Memory();
	~Memory();

	bool init();
	void destory();

	bool attach(int pid);
	void detach();
	inline bool isAttach(){ return targetProcess_ != NULL;};
	
	template<typename RET>
	inline RET read(std::ptrdiff_t address)
	{
		SIZE_T size = 0;
		RET ret;
		
		if (!kernel_mm_read(targetProcess_, address, &ret, sizeof(ret), &size))
			return {};

		return ret;
	}

	template<typename T>
	inline bool write(std::ptrdiff_t address, const T* value)
	{
		return kernel_mm_write(targetProcess_, address, value, sizeof(T), NULL) == TRUE;
	}

	inline void read(std::ptrdiff_t address, void* buffer, size_t size)
	{
		kernel_mm_read(targetProcess_, address, buffer, size, NULL);
	}

	inline void write(std::ptrdiff_t address, void* buffer, size_t size)
	{
		kernel_mm_write(targetProcess_, address, buffer, size, NULL);
	}

	template<typename T>
	inline bool forc_write(std::ptrdiff_t address, const T* value)
	{
		return kernel_mm_write_forc(targetProcess_,  address, value, sizeof(T), NULL);
	}

	inline bool forc_write(std::ptrdiff_t address, const void* value, size_t size)
	{

		return kernel_mm_write_forc(targetProcess_, address, value, size, NULL);
	}

private:
	int pid_ = 0;
	HANDLE targetProcess_ = NULL;
};


