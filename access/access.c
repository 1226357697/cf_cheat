#include "stdafx.h"
#include "access.h"



/*** Process ***/
NTSTATUS NTAPI NtOpenProcessHook(PHANDLE processHandle, ACCESS_MASK desiredAccess, POBJECT_ATTRIBUTES objectAttributes, PCLIENT_ID clientId) {

	NTOPENPROCESS_ARGS args;
	args.ProcessHandle = processHandle;
	args.DesiredAccess = desiredAccess;
	args.ObjectAttributes = objectAttributes;
	args.ClientId = clientId;
	return DoSyscall(SyscallNtOpenProcess, &args);
}

NTSTATUS NTAPI NtSuspendProcessHook(HANDLE processHandle) {
	NTSUSPENDPROCESS_ARGS args;
	args.ProcessHandle = processHandle;
	return DoSyscall(SyscallNtSuspendProcess, &args);
}

NTSTATUS NTAPI NtResumeProcessHook(HANDLE processHandle) {
	NTRESUMEPROCESS_ARGS args;
	args.ProcessHandle = processHandle;
	return DoSyscall(SyscallNtResumeProcess, &args);
}

NTSTATUS NTAPI NtQuerySystemInformationExHook(SYSTEM_INFORMATION_CLASS systemInformationClass, PVOID inputBuffer, ULONG inputBufferLength, PVOID systemInformation, ULONG systemInformationLength, PULONG returnLength) {
	NTQUERYSYSTEMINFORMATIONEX_ARGS args;
	args.SystemInformationClass = systemInformationClass;
	args.InputBuffer = inputBuffer;
	args.InputBufferLength = inputBufferLength;
	args.SystemInformation = systemInformation;
	args.SystemInformationLength = systemInformationLength;
	args.ReturnLength = returnLength;
	return DoSyscall(SyscallNtQuerySystemInformationEx, &args);
}

NTSTATUS NTAPI NtQueryInformationProcessHook(HANDLE processHandle, PROCESSINFOCLASS processInformationClass, PVOID processInformation, ULONG processInformationLength, PULONG returnLength) {
	NTQUERYINFORMATIONPROCESS_ARGS args;
	args.ProcessHandle = processHandle;
	args.ProcessInformationClass = processInformationClass;
	args.ProcessInformation = processInformation;
	args.ProcessInformationLength = processInformationLength;
	args.ReturnLength = returnLength;
	return DoSyscall(SyscallNtQueryInformationProcess, &args);
}

NTSTATUS NTAPI NtSetInformationProcessHook(HANDLE processHandle, PROCESSINFOCLASS processInformationClass, PVOID processInformation, ULONG processInformationLength) {
	NTQUERYINFORMATIONPROCESS_ARGS args;
	args.ProcessHandle = processHandle;
	args.ProcessInformationClass = processInformationClass;
	args.ProcessInformation = processInformation;
	args.ProcessInformationLength = processInformationLength;
	return DoSyscall(SyscallNtSetInformationProcess, &args);
}

NTSTATUS NTAPI NtFlushInstructionCacheHook(HANDLE processHandle, PVOID baseAddress, ULONG numberOfBytesToFlush) {
	NTFLUSHINSTRUCTIONCACHE_ARGS args;
	args.ProcessHandle = processHandle;
	args.BaseAddress = baseAddress;
	args.NumberOfBytesToFlush = numberOfBytesToFlush;
	return DoSyscall(SyscallNtFlushInstructionCache, &args);
}

NTSTATUS NTAPI NtCloseHook(HANDLE handle) {
	return ERROR_SUCCESS;
}

/*** Memory ***/
NTSTATUS NTAPI NtAllocateVirtualMemoryHook(HANDLE processHandle, PVOID* baseAddress, SIZE_T zeroBits, PSIZE_T regionSize, ULONG allocationType, ULONG protect) {

	NTALLOCATEVIRTUALMEMORY_ARGS args;
	args.ProcessHandle = processHandle;
	args.BaseAddress = baseAddress;
	args.ZeroBits = zeroBits;
	args.RegionSize = regionSize;
	args.AllocationType = allocationType;
	args.Protect = protect;
	return DoSyscall(SyscallNtAllocateVirtualMemory, &args);
}

NTSTATUS NTAPI NtFlushVirtualMemoryHook(HANDLE processHandle, PVOID* baseAddress, PSIZE_T regionSize, PIO_STATUS_BLOCK ioStatus) {

	NTFLUSHVIRTUALMEMORY_ARGS args;
	args.ProcessHandle = processHandle;
	args.BaseAddress = baseAddress;
	args.RegionSize = regionSize;
	args.IoStatus = ioStatus;
	return DoSyscall(SyscallNtFlushVirtualMemory, &args);
}

NTSTATUS NTAPI NtFreeVirtualMemoryHook(HANDLE processHandle, PVOID* baseAddress, PSIZE_T regionSize, ULONG freeType) {
	NTFREEVIRTUALMEMORY_ARGS args;
	args.ProcessHandle = processHandle;
	args.BaseAddress = baseAddress;
	args.RegionSize = regionSize;
	args.FreeType = freeType;
	return DoSyscall(SyscallNtFreeVirtualMemory, &args);
}

NTSTATUS NTAPI NtLockVirtualMemoryHook(HANDLE processHandle, PVOID* baseAddress, PSIZE_T regionSize, ULONG lockOption) {
	NTLOCKVIRTUALMEMORY_ARGS args;
	args.ProcessHandle = processHandle;
	args.BaseAddress = baseAddress;
	args.RegionSize = regionSize;
	args.LockOption = lockOption;
	return DoSyscall(SyscallNtLockVirtualMemory, &args);
}

NTSTATUS NTAPI NtUnlockVirtualMemoryHook(HANDLE processHandle, PVOID* baseAddress, PSIZE_T regionSize, ULONG lockOption) {
	NTUNLOCKVIRTUALMEMORY_ARGS args;
	args.ProcessHandle = processHandle;
	args.BaseAddress = baseAddress;
	args.RegionSize = regionSize;
	args.LockOption = lockOption;
	return DoSyscall(SyscallNtUnlockVirtualMemory, &args);
}

NTSTATUS NTAPI NtProtectVirtualMemoryHook(HANDLE processHandle, PVOID* baseAddress, PSIZE_T regionSize, ULONG newAccessProtection, PULONG oldAccessProtection) {
	NTPROTECTVIRTUALMEMORY_ARGS args;
	args.ProcessHandle = processHandle;
	args.BaseAddress = baseAddress;
	args.RegionSize = regionSize;
	args.NewAccessProtection = newAccessProtection;
	args.OldAccessProtection = oldAccessProtection;
	return DoSyscall(SyscallNtProtectVirtualMemory, &args);
}

NTSTATUS NTAPI NtReadVirtualMemoryHook(HANDLE processHandle, PVOID baseAddress, PVOID buffer, SIZE_T numberOfBytesToRead, PSIZE_T numberOfBytesRead) {
	NTREADVIRTUALMEMORY_ARGS args;
	args.ProcessHandle = processHandle;
	args.BaseAddress = baseAddress;
	args.Buffer = buffer;
	args.NumberOfBytesToRead = numberOfBytesToRead;
	args.NumberOfBytesRead = numberOfBytesRead;
	return DoSyscall(SyscallNtReadVirtualMemory, &args);
}

NTSTATUS NTAPI NtWriteVirtualMemoryHook(HANDLE processHandle, PVOID baseAddress, PVOID buffer, SIZE_T numberOfBytesToWrite, PSIZE_T numberOfBytesWritten) {

	NTWRITEVIRTUALMEMORY_ARGS args;
	args.ProcessHandle = processHandle;
	args.BaseAddress = baseAddress;
	args.Buffer = buffer;
	args.NumberOfBytesToWrite = numberOfBytesToWrite;
	args.NumberOfBytesWritten = numberOfBytesWritten;
	return DoSyscall(SyscallNtWriteVirtualMemory, &args);
}

NTSTATUS NTAPI NtQueryVirtualMemoryHook(HANDLE processHandle, PVOID baseAddress, MEMORY_INFORMATION_CLASS memoryInformationClass, PVOID memoryInformation, SIZE_T memoryInformationLength, PSIZE_T returnLength) {

	NTQUERYVIRTUALMEMORY_ARGS args;
	args.ProcessHandle = processHandle;
	args.BaseAddress = baseAddress;
	args.MemoryInformationClass = memoryInformationClass;
	args.MemoryInformation = memoryInformation;
	args.MemoryInformationLength = memoryInformationLength;
	args.ReturnLength = returnLength;
	return DoSyscall(SyscallNtQueryVirtualMemory, &args);
}

/*** Thread ***/
NTSTATUS NTAPI NtOpenThreadHook(PHANDLE threadHandle, ACCESS_MASK accessMask, POBJECT_ATTRIBUTES objectAttributes, PCLIENT_ID clientId) {
	NTOPENTHREAD_ARGS args;
	args.ThreadHandle = threadHandle;
	args.AccessMask = accessMask;
	args.ObjectAttributes = objectAttributes;
	args.ClientId = clientId;
	return DoSyscall(SyscallNtOpenThread, &args);
}

NTSTATUS NTAPI NtQueryInformationThreadHook(HANDLE threadHandle, THREADINFOCLASS threadInformationClass, PVOID threadInformation, ULONG threadInformationLength, PULONG returnLength) {

	NTQUERYINFORMATIONTHREAD_ARGS args;
	args.ThreadHandle = threadHandle;
	args.ThreadInformationClass = threadInformationClass;
	args.ThreadInformation = threadInformation;
	args.ThreadInformationLength = threadInformationLength;
	args.ReturnLength = returnLength;
	return DoSyscall(SyscallNtQueryInformationThread, &args);
}

NTSTATUS NTAPI NtSetInformationThreadHook(HANDLE threadHandle, THREADINFOCLASS threadInformationClass, PVOID threadInformation, ULONG threadInformationLength) {

	NTSETINFORMATIONTHREAD_ARGS args;
	args.ThreadHandle = threadHandle;
	args.ThreadInformationClass = threadInformationClass;
	args.ThreadInformation = threadInformation;
	args.ThreadInformationLength = threadInformationLength;
	return DoSyscall(SyscallNtSetInformationThread, &args);
}

NTSTATUS NTAPI NtGetContextThreadHook(HANDLE threadHandle, PCONTEXT context) {
	NTGETCONTEXTTHREAD_ARGS args;
	args.ThreadHandle = threadHandle;
	args.Context = context;
	return DoSyscall(SyscallNtGetContextThread, &args);
}

NTSTATUS NTAPI NtSetContextThreadHook(HANDLE threadHandle, PCONTEXT context) {

	NTSETCONTEXTTHREAD_ARGS args;
	args.ThreadHandle = threadHandle;
	args.Context = context;
	return DoSyscall(SyscallNtSetContextThread, &args);
}

NTSTATUS NTAPI NtResumeThreadHook(HANDLE threadHandle, PULONG suspendCount) {

	NTRESUMETHREAD_ARGS args;
	args.ThreadHandle = threadHandle;
	args.SuspendCount = suspendCount;
	return DoSyscall(SyscallNtResumeThread, &args);
}

NTSTATUS NTAPI NtSuspendThreadHook(HANDLE threadHandle, PULONG previousSuspendCount) {
	NTSUSPENDTHREAD_ARGS args;
	args.ThreadHandle = threadHandle;
	args.PreviousSuspendCount = previousSuspendCount;
	return DoSyscall(SyscallNtSuspendThread, &args);
}

/*** Sync ***/
NTSTATUS NTAPI NtWaitForSingleObjectHook(HANDLE handle, BOOLEAN alertable, PLARGE_INTEGER timeout) {

	NTWAITFORSINGLEOBJECT_ARGS args;
	args.Handle = handle;
	args.Alertable = alertable;
	args.Timeout = timeout;
	return DoSyscall(SyscallNtWaitForSingleObject, &args);
}

bool kernel_initialize()
{
	if (!SetupSyscalls()) 
		return false;

	return true;
}

void kernel_destory()
{

}

HANDLE kernel_open_process(int pid)
{
	HANDLE process = NULL;
	CLIENT_ID client = {0};
	client.UniqueProcess = LongToHandle(pid);

	OBJECT_ATTRIBUTES attr;
	InitializeObjectAttributes(&attr, NULL, 0,0,0);
	if (!NT_SUCCESS(NtOpenProcessHook(&process, GENERIC_ALL, &attr, &client)))
	{
		return NULL;
	}
	return process;
}

bool kernel_mm_read(HANDLE process, intptr_t address, void* buffer, size_t size, size_t* read_bytes)
{
	return NT_SUCCESS(NtReadVirtualMemoryHook(process, (PVOID)address, buffer, size, read_bytes));
}

bool kernel_mm_write(HANDLE process, intptr_t address, const void* buffer, size_t size, size_t* read_bytes)
{
	return NT_SUCCESS(NtWriteVirtualMemoryHook(process, (PVOID)address, (PVOID)buffer, size, read_bytes));
}

bool kernel_mm_write_forc(HANDLE process, intptr_t address, const void* buffer, size_t size, size_t* read_bytes)
{
	bool ret = false;
	ULONG old_process;
	if (kernel_mm_protect(process, address, size, PAGE_EXECUTE_READWRITE, &old_process))
	{
		ret = kernel_mm_write(process, address, buffer, size, read_bytes);
		kernel_mm_protect(process, address, size, old_process, &old_process);
	}
	return ret;
}

bool kernel_mm_protect(HANDLE process, intptr_t address, size_t size, ULONG protect, PULONG old_protect)
{
	PVOID base_address = (PVOID)address;
	SIZE_T base_size = size;
	return NT_SUCCESS(NtProtectVirtualMemoryHook(process, &base_address, &base_size, protect, old_protect));
}

intptr_t kernel_mm_alloc(HANDLE process, intptr_t address, size_t size, ULONG protect)
{
	PVOID base_address = (PVOID)address;
	SIZE_T region_size = size;
	if (!NT_SUCCESS(NtAllocateVirtualMemoryHook(process, &base_address, 0, &region_size, MEM_COMMIT, protect)))
	{
		return 0;
	}
	return (intptr_t)base_address;
}

void kernel_mm_free(HANDLE process, intptr_t address)
{
	PVOID base_address = (PVOID)address;
	SIZE_T region_size = 0;
	NtFreeVirtualMemoryHook(process, &base_address, &region_size, MEM_RELEASE);
}

void kernel_close(HANDLE process)
{
	NtCloseHook(process);
}
