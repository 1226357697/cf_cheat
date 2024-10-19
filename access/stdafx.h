#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "ntdll.h"
#include <stdio.h>
#include <tlhelp32.h>

#include "syscall.h"


BOOL SetupSyscalls();
NTSTATUS DoSyscall(SYSCALL syscall, PVOID args);