#include "StdAfx.h"
#include <comutil.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <ctime>
#include <stdlib.h>
#include <io.h>
#include "_md5.h"
#include "_xor.h"
#include <psapi.h>

#pragma comment(lib, "psapi.lib") // psapi.lib'i derleyiciye bildirme
using namespace std;

void rclose()
{
	exit(-1);
	__asm mov ESP, 0;
	exit(0);
	TerminateProcess(GetCurrentProcess(), 0);
}
void metin2release()
{
	HMODULE hMod = GetModuleHandleA(XorStr<0x40,19,0xE37A3CD1>("\x12\x34\x2C\x2A\x2F\x04\x28\x33\x21\x0A\x22\x2E\x2D\x39\x60\x2B\x3C\x3D"+0xE37A3CD1).s); 

	if (hMod == NULL)
	{
		rclose();
	}

	MODULEINFO modInfo;
	BOOL result = GetModuleInformation(GetCurrentProcess(), hMod, &modInfo, sizeof(modInfo));

	if (result == FALSE)
	{
		rclose();
	}

	const DWORD expectedSizeOfImage = 9932800;
	if (modInfo.SizeOfImage != expectedSizeOfImage)
	{
		rclose();
	}
}

#pragma warning(disable:4172)
inline bool is_file_exist(const std::string& name) {
	std::ifstream infile(name.c_str());
	return infile.good();
}

__forceinline void __exit() {
	PostQuitMessage(0);
	TerminateProcess((HANDLE)-1, -1);

	CHAR __ntdlldll[] = { 'n', 't', 'd', 'l', 'l', '.', 'd', 'l', 'l', 0x0 }; // ntdll.dll
	CHAR __NtRaiseException[] = { 'N', 't', 'R', 'a', 'i', 's', 'e', 'E', 'x', 'c', 'e', 'p', 't', 'i', 'o', 'n', 0x0 }; //NtRaiseException
	DWORD adr = (DWORD)GetProcAddress(GetModuleHandleA(__ntdlldll), __NtRaiseException);
	__asm {
		mov ESP, 0
		jmp dword ptr adr
	};

	__asm {
		xor eax, eax
		leave
		ret
	}

	while (1) {}
}
__forceinline void close(const char* cArgFormat, ...) {
	char cTmpString[2000];
	CHAR _Filename[] = { 'R', 'u', 'n', 'i', 'k', 'A', 'c','.','t','x','t', 0x0 }; //RunikAc.txt Log File

	va_list vaArgList;
	va_start(vaArgList, cArgFormat);
	wvsprintfA(cTmpString, cArgFormat, vaArgList);
	va_end(vaArgList);

	time_t ct = time(0);
	struct tm ctm = *localtime(&ct);

	CHAR __timeformat[] = { '%', '0', '2', 'd', ':', '%', '0', '2', 'd', ':', '%', '0', '2', 'd', ' ', '-', ' ', '%', '0', '2', 'd', ':', '%', '0', '2', 'd', ':', '%', 'd', ' ', ':', ':', ' ', 0x0 }; // %02d:%02d:%02d - %02d:%02d:%d ::
	char cTimeBuf[1250];
	sprintf(cTimeBuf, __timeformat,
		ctm.tm_hour,
		ctm.tm_min,
		ctm.tm_sec,
		ctm.tm_mday,
		ctm.tm_mon + 1,
		1900 + ctm.tm_year);

	std::ofstream f(_Filename, std::ofstream::out | std::ofstream::app);
	f << cTimeBuf << cTmpString << '\n' << std::endl;
	f.close();

	__exit();
}

HMODULE hAntiModule;
void Initialize() // Anti Cheat Load
 {
	CHAR __Filename[] = { 'R', 'u', 'n', 'i', 'k', 'A', 'n', 't', 'i', 'C','h', 'e', 'a', 't','.','d','l','l', 0x0 }; // RunikAntiCheat//RunikAntiCheat.dll Static Library
	CHAR __Filenotfound[] = { 'A', 'n', 't', 'i', 'c', 'h', 'e', 'a', 't', ' ', 'f', 'i', 'l', 'e', ' ', 'd', 'o', 'e', 's', 'n', ' ', 't', ' ', 'f', 'o', 'u', 'n', 'd', 0x0 }; // Anticheat file doesn't found
	CHAR __LoadFail[] = { 'A', 'n', 't', 'i', 'c', 'h', 'e', 'a', 't', ' ', 'm', 'o', 'd', 'u', 'l', 'e', ' ', 'd', 'o', 'e', 's', 'n', ' ', 't', ' ', 'l', 'o', 'a', 'd', 'e', 'd', 0x0 }; // Anticheat module doesn't loaded
	CHAR __Apiname[] = { 'R', 'u', 'n', 'i', 'k', 'A', 'c', 0x0 }; // RunikAc Api Name Modules
	CHAR __exportfail[] = { 'A', 'n', 't', 'i', 'c', 'h', 'e', 'a', 't', ' ', 'c', 'o', 'u', 'l', 'd', ' ', 'n', 'o', 't', ' ', 'l', 'o', 'c', 'a', 't', 'e', ' ', 't', 'h', 'e', ' ', 'I', 'n', 'i', 't', 'i', 'a', 'l', 'i', 'z', 'e', ' ', 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', 0x0 }; // Anticheat could not locate the Initialize function

	//Kontrol 1
	if (!is_file_exist(__Filename))
		close(__Filenotfound);

	//Kontrol 2
	HMODULE hGetProcIDDLL = LoadLibraryA(__Filename);
	if (!hGetProcIDDLL)
		close(__LoadFail);
	hAntiModule = hGetProcIDDLL;

	//Kontrol 3
	DWORD DLL_Wrapper_Func = (DWORD)GetProcAddress(hGetProcIDDLL, __Apiname);
	if (!DLL_Wrapper_Func)
		close(__exportfail);
}

void FileControl()  // Dosya Kontrolü
{


	MD5 md5;
	CHAR __RootMsg[] = { 'R', 'o', 'o', 't', ' ', 'F', 'i', 'l', 'e', ' ', 'C','R','C', ' ', 'E','R','R','O','R', 0x0 }; // Root File CRC ERROR
	CHAR __IndexMsg[] = { 'I', 'n', 'd', 'e','x', ' ', 'F', 'i', 'l', 'e', ' ', 'C','R','C', ' ', 'E','R','R','O','R', 0x0 }; // İndex File CRC ERROR
	CHAR __AntiMsg[] = { 'A', 'n', 't', 'i',' ', 'C','h','e','a','t',' ', 'F', 'i', 'l', 'e', ' ', 'C','R','C', ' ', 'E','R','R','O','R', 0x0 }; // Anti Cheat File CRC ERROR
	CHAR __GlobalMsg[] = { 'U', 'n', 'k', 'n','o', 'w',' ', 'F', 'i', 'l', 'e', ' ', 'C','R','C', ' ', 'E','R','R','O','R', 0x0 }; // Unknow File CRC ERROR
	

	if (strcmp(md5.digestFile("RunikAntiCheat.dll"),/*7a6326c74f1c77b0450e81fc04493c85*/XorStr<0xE7,33,0xE5E6EF8F>("\xD0\x89\xDF\xD9\xD9\xDA\x8E\xD9\xDB\x96\xC0\x91\xC4\xC3\x97\xC6\xC3\xCD\xC9\x9F\xC3\xCD\x9B\x9D\xCF\x34\x35\x3B\x30\x67\x3D\x33"+0xE5E6EF8F).s) != 0) close(__AntiMsg); // Müşteri Dll Hash Key

}


void RunikUltimateControl()
{

	Initialize();
	FileControl();
	metin2release();
}
