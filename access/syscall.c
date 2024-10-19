#include "stdafx.h"

PVOID(NTAPI *NtConvertBetweenAuxiliaryCounterAndPerformanceCounter)(ULONG64, PVOID, PVOID, PVOID);

BOOL SetupSyscalls() 
{
	char szntdll[10] = { 'n', 't', 'd', 'l', 'l', '.', 'd', 'l', 'l', '\0' };
	char  szNtConvertBetweenAuxiliaryCounterAndPerformanceCounter[54]= {
		0x4E, 0x74, 0x43, 0x6F, 0x6E, 0x76, 0x65, 0x72, 0x74, 0x42, 0x65, 0x74, 0x77, 0x65, 0x65, 0x6E,
		 0x41, 0x75, 0x78, 0x69, 0x6C, 0x69, 0x61, 0x72, 0x79, 0x43, 0x6F, 0x75, 0x6E, 0x74, 0x65, 0x72,
		 0x41, 0x6E, 0x64, 0x50, 0x65, 0x72, 0x66, 0x6F, 0x72, 0x6D, 0x61, 0x6E, 0x63, 0x65, 0x43, 0x6F,
		 0x75, 0x6E, 0x74, 0x65, 0x72, 0x00
	};
	*(PVOID *)&NtConvertBetweenAuxiliaryCounterAndPerformanceCounter = GetProcAddress(
		GetModuleHandleA(szntdll),
		szNtConvertBetweenAuxiliaryCounterAndPerformanceCounter
	);

	if (!NtConvertBetweenAuxiliaryCounterAndPerformanceCounter) 
		return FALSE;

	return TRUE;
}

NTSTATUS DoSyscall(SYSCALL syscall, PVOID args) 
{
	SYSCALL_DATA data;
	data.Unique = SYSCALL_UNIQUE;
	data.Syscall = syscall;
	data.Arguments = args;

	// NtConvertBetweenAuxiliaryCounterAndPerformanceCounter will dereference this
	PVOID dataPtr = &data;

	INT64 status = 0;
	NtConvertBetweenAuxiliaryCounterAndPerformanceCounter(1, &dataPtr, &status, 0);
	return (NTSTATUS)status;
}