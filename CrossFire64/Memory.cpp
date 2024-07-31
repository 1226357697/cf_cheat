#include "Memory.hpp"
#include "Logger.h" 

static
BOOL EnablePrivilege(LPCTSTR szPrivilege, BOOL fEnable)
{
  BOOL fOk = FALSE;
  HANDLE hToken = NULL;
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    LookupPrivilegeValue(NULL, szPrivilege, &tp.Privileges[0].Luid);
    tp.Privileges->Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    fOk = (GetLastError() == ERROR_SUCCESS);
    CloseHandle(hToken);
  }
  return fOk;
}


Memory::Memory()
{
}

Memory::~Memory()
{
  destory();
}

bool Memory::init()
{
#if 0
  return EnablePrivilege(SE_DEBUG_NAME, TRUE);

#else  
  if(isInited())
    return true;
  hackDll = LoadLibraryA("DLL.dll");
  return isInited();

#endif // 0
}

void Memory::destory()
{
#if 0
#else
  if (!isInited())
    return;

  FreeLibrary(hackDll);
#endif
}

bool Memory::attach(int pid)
{
  detach();
  pid_ = pid;
  bool ret = EnablePrivilege(SE_DEBUG_NAME, TRUE);
  targetProcess_ = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, pid_);

  //return targetProcess_.valid();
  return targetProcess_ != NULL;
}

void Memory::detach()
{
  if (!isAttach())
    return;

  //targetProcess_.close();
  CloseHandle(targetProcess_);
  pid_ = 0;
}
