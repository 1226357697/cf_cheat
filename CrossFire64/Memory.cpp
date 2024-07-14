#include "Memory.h"
#include "Memory.hpp"

Memory::Memory()
{
}

Memory::~Memory()
{
  destory();
}

bool Memory::init()
{
  if(isInited())
    return true;
  hackDll = LoadLibraryA("DLL.dll");
  return isInited();
}

void Memory::destory()
{
  if (!isInited())  
    return ;
  
  FreeLibrary(hackDll);
}

bool Memory::attach(int pid)
{
  detach();
  pid_ = pid;
  targetProcess_ = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, pid_);
  return true;
}

void Memory::detach()
{
  if (!isAttach())
    return;

  targetProcess_.close();
  pid_ = 0;
}
