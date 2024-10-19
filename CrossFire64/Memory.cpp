#include "Memory.hpp"
#include "Logger.h" 



Memory::Memory()
{
}

Memory::~Memory()
{
  destory();
}

bool Memory::init()
{
  return kernel_initialize();
}

void Memory::destory()
{
  kernel_destory();
}

bool Memory::attach(int pid)
{
  detach();
  pid_ = pid;
  targetProcess_ = kernel_open_process(pid_);
  return isAttach();
}

void Memory::detach()
{
  if (!isAttach())
    return;

  CloseHandle(targetProcess_);
  pid_ = 0;
}
