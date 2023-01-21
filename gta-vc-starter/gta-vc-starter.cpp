#include <iostream>
#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>

void EnableDebugPriv()
{
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tkp;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);

	CloseHandle(hToken);
}

LPPROCESS_INFORMATION StartGtavc(char* pDestCmdLine)
{
	LPSTARTUPINFOA pStartupInfo = new STARTUPINFOA();
	LPPROCESS_INFORMATION pProcessInfo = new PROCESS_INFORMATION();
	CreateProcessA(NULL, pDestCmdLine, 0, 0, 0, 0, 0, 0, pStartupInfo, pProcessInfo);
	return pProcessInfo;
}

BOOL SetVcRunOnCPU0(HANDLE hProcess)
{
	DWORD_PTR mask = 1 << 0;
	return SetProcessAffinityMask(hProcess, mask);
}

using namespace std;
int main()
{
	char gtavcpath[] = "gta-vc.exe";
	EnableDebugPriv();
	LPPROCESS_INFORMATION pProcessInfo = StartGtavc(gtavcpath);
	if (!pProcessInfo || !pProcessInfo->hProcess)
	{
		printf("[-] Failed to start gta-vc.exe! Did you really put this program in the same folder with gta-vc.exe?");
		return -1;
	}
	printf("[*] Process id:%d\n", pProcessInfo->dwProcessId);
	if (SetVcRunOnCPU0(pProcessInfo->hProcess))
	{
		printf("[+] Success setting CPU affinity for gta-vc.exe");
		return 0;
	}
	printf("[-] Failed to set gta-vc.exe's CPU affinity\n");
	return -1;
}
