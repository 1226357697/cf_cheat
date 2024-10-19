#pragma once
#include <stdbool.h>
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif // _cpluscplus


bool kernel_initialize();

void kernel_destory();

HANDLE kernel_open_process(int pid);

void kernel_close(HANDLE process);

bool kernel_mm_read(HANDLE process, intptr_t address, void* buffer, size_t size, size_t* read_bytes);

bool kernel_mm_write(HANDLE process, intptr_t address, const void* buffer, size_t size, size_t* write_bytes);

bool kernel_mm_write_forc(HANDLE process, intptr_t address, const void* buffer, size_t size, size_t* write_bytes);

bool kernel_mm_protect(HANDLE process, intptr_t address, size_t size, ULONG protect, PULONG old_protect);

intptr_t kernel_mm_alloc(HANDLE process, intptr_t address, size_t size, ULONG protect);

void kernel_mm_free(HANDLE process, intptr_t address);




#ifdef __cplusplus
}
#endif // _cpluscplus