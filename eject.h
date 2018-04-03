#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <wchar.h>
#include <tchar.h>
#include <stdio.h>
#include "define.h"

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) {
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid;

	if (!OpenProcessToken(
		GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken))
	{
		fprintf(stderr, "[-] OpenProcessToken error : %u\n", GetLastError());
		return FALSE;
	}

	if (!LookupPrivilegeValue(
		NULL,
		lpszPrivilege,
		&luid))
	{
		fprintf(stderr, "[-] LookupPrivilegeValue error : %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		fprintf(stderr, "[-] AdjustTokenPrivileges error : %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		fprintf(stderr, "[-] The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

BOOL EjectDll(DWORD dwPID, LPCTSTR szDllName) {
	BOOL bMore = FALSE, bFound = FALSE;
	HANDLE hSnapshot, hProcess, hThread;
	HMODULE hModule = NULL;
	MODULEENTRY32 me = { sizeof(me) };
	LPTHREAD_START_ROUTINE pThreadProc;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

	/* Check loaded modules in the process */
	bMore = Module32First(hSnapshot, &me);
	do {
		if (COMPARE_NAME(me.szModule, szDllName) || COMPARE_NAME(me.szExePath, szDllName))
		{
			bFound = TRUE;
			break;
		}
	} while (bMore = Module32Next(hSnapshot, &me));
	if (!bFound) {
		CloseHandle(hSnapshot);
		return FALSE;
	}

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))) {
		fprintf(stderr, "[-] OpenProcess(%d) error : %u\n", dwPID, GetLastError());
		return FALSE;
	}

	/* Get FreeLibrary() address of kernel32.dll */
	hModule = GetModuleHandle(L"kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
	hThread = CreateRemoteThread(
		hProcess,			// Attach the target process
		NULL,
		0,
		pThreadProc,		// FreeLibrary address of kernel32.dll
		me.modBaseAddr,		// kernel32.dll base address
		0,
		NULL);
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
	CloseHandle(hSnapshot);

	return TRUE;
}

/* DLL will be dettached. */
BOOL Eject(const WCHAR* wcPName, const WCHAR* wcDName) {
	DWORD dwPID;

	dwPID = Search_PID(wcPName);
	printf("%u\n", dwPID);

	if (!SetPrivilege(SE_DEBUG_NAME, TRUE))
		return FALSE;

	if (EjectDll(dwPID, wcDName)) {
		printf("EjectDLL(%d, \"%ls\") sucess!!!", dwPID, wcDName);
		return TRUE;
	}
	else {
		printf("EjectDLL(%d, \"%ls\") failed!!!", dwPID, wcDName);
		return FALSE;
	}
}
