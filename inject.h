#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <wchar.h>
#include <tchar.h>
#include <stdio.h>
#include "define.h"

DWORD Search_PID(const WCHAR *szExeFile) {
	HANDLE hSnapShot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	DWORD dwPid = 0;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	Process32First(hSnapShot, &pe32);
	do {
#ifdef _DEBUG
		printf("[+]File : %40ws, pid : %8d\n", pe32.szExeFile, pe32.th32ProcessID);
#endif
		if (COMPARE_NAME(pe32.szExeFile, szExeFile)) {
			dwPid = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapShot, &pe32));
	CloseHandle(hSnapShot);

	return dwPid;
}

BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath) {
	HANDLE hProcess = NULL, hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;
	DWORD dwBufSize = (DWORD)(wcslen(szDllPath) + 1) * sizeof(WCHAR);
	LPTHREAD_START_ROUTINE pThreadProc;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))) {
		fprintf(stderr, "[-] OpenProcess(%d) error : %u\n", dwPID, GetLastError());
		return FALSE;
	}

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);

	hMod = GetModuleHandle(L"kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");

	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}

/* DLL will be attached. */
BOOL Inject(const WCHAR* wcPName, const WCHAR* wcDName) {
	DWORD dwPID;

	dwPID = Search_PID(wcPName);
	printf("%u\n", dwPID);

	if (InjectDll(dwPID, wcDName)) {
		printf("InjectDLL(%d, \"%ls\") sucess!!!", dwPID, wcDName);
		return TRUE;
	}
	else {
		printf("InjectDLL(%d, \"%ls\") failed!!!", dwPID, wcDName);
		return FALSE;
	}
}
