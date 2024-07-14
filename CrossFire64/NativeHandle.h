#pragma once
#include <Windows.h>
class Handle
{
public:
  Handle() = default;
  Handle(HANDLE handle):handle_(handle){};
  ~Handle(){close(); };
  inline void close(){ if (!valid()) return; CloseHandle(handle_); };
  bool valid() const{return handle_ != NULL && handle_ != INVALID_HANDLE_VALUE;};
  inline operator bool() const {return valid(); };
  inline operator HANDLE()const {return handle_;}

private:
  HANDLE handle_ = NULL;
};

