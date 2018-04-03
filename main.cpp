#include "inject.h"
#include "eject.h"

void Usage(char* exe) {
	fprintf(stderr, "================ DLL Injector =================\n");
	fprintf(stderr, "[-] Usage : %s I|E [Target Process] [DLL Path]\n", exe);
	exit(-1);
}

void ConvertCtoWC(WCHAR* dest, const char* src) {
	DWORD dwPathLen = 0;

	dwPathLen = MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, src, strlen(src) + 1, dest, dwPathLen);
}

int main(int argc, char *argv[]) {
	WCHAR wcPName[MAX_PATH] = { 0, };
	WCHAR wcDName[MAX_PATH] = { 0, };

	if (argc != 4) Usage(argv[0]);

	ConvertCtoWC(wcPName, argv[2]);
	ConvertCtoWC(wcDName, argv[3]);
	printf("[+] Target Process : %ls\n", wcPName);
	printf("[+] DLL Name : %ls\n", wcDName);

	switch (*argv[1]) {
	case 'I':
		puts("[+] Inject Mode");
		printf("[+] Target Process : %ls\n", wcPName);
		printf("[+] DLL Name : %ls\n", wcDName);
		if (!Inject(wcPName, wcDName))
			return -1;

		break;

	case 'E':
		puts("[+] Eject Mode");
		printf("[+] Target Process : %ls\n", wcPName);
		printf("[+] DLL Name : %ls\n", wcDName);
		if (!Eject(wcPName, wcDName))
			return -1;

		break;

	default:
		Usage(argv[0]);

		break;
	}

	return 0;
}