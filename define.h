#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <wchar.h>
#include <tchar.h>

/* for test (ReverseCore) */
#define DEF_PROC_NAME (L"notepad.exe")
#define DEF_DLL_NAME  (L"myhack.dll")

/* Search process's pid with process's name */
#define COMPARE_NAME(x, y) (!wcsncmp(x, y, MAX_PATH))

/* function list */
DWORD Search_PID(const WCHAR *szExeFile);
BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);

BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath);
BOOL Inject(const WCHAR* wcPName, const WCHAR* wcDName);

BOOL EjectDll(DWORD dwPID, LPCTSTR szDllName);
BOOL Eject(const WCHAR* wcPName, const WCHAR* wcDName);
