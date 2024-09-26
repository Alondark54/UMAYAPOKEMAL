#ifdef ENABLE_URIEL_AC
#pragma once

#include <Windows.h>
#pragma comment(lib, "prime.lib")
#pragma comment(linker, "/ALIGN:0x10000")

#pragma pack(push, 1)
typedef struct _InitData
{
	DWORD dwVersion;
	void* Login;
	void* AddInputEvent;
	void* CheckInputEvent;
	void* ClearInputEvent;
	void* SetAttackKeyState;
	void* CheckAttackKeyState;
	void* GetEncryptedTargetVID;
	void* SetAttackSpeed;
	void* CheckAttackSpeed;
	void* GetCharacterStatePos;
	void* SetAttackVictim;
	void* ClearInstance;
	void* SetMoveEvent;
	void* CheckMoveEvent;
	void* AddPointer;
	void* CheckReturn;
	void* SetReservedMode;
	void* CheckReservedMode;
	void* GetHWID;
	void* EncodePointer;
	void* DecodePointer;
} InitData, * PInitData;
#pragma pack(pop)

extern "C" __declspec(dllimport) bool Init(PInitData init_data);
typedef bool 	(WINAPI* _Login)(const char* username, DWORD, DWORD, DWORD*);
typedef bool 	(WINAPI* _AddInputEvent)(DWORD);
typedef bool 	(WINAPI* _CheckInputEvent)(DWORD);
typedef void 	(WINAPI* _ClearInputEvent)(DWORD);
typedef void 	(WINAPI* _SetAttackKeyState)(bool);
typedef void 	(WINAPI* _CheckAttackKeyState)(bool);
typedef bool 	(WINAPI* _GetEncryptedTargetVID)(DWORD*, void*, DWORD, DWORD);
typedef bool 	(WINAPI* _SetAttackSpeed)(DWORD, DWORD);
typedef bool 	(WINAPI* _CheckAttackSpeed)(float, DWORD);
typedef bool 	(WINAPI* _GetCharacterStatePos)(LONG*, LONG*, DWORD, DWORD);
typedef bool 	(WINAPI* _SetAttackVictim)(void*);
typedef void 	(WINAPI* _ClearInstance)(DWORD);
typedef bool	(WINAPI* _SetMoveEvent)(DWORD*, DWORD);
typedef bool	(WINAPI* _CheckMoveEvent)(DWORD*);
typedef bool 	(WINAPI* _AddPointer)(void*, void*);
typedef void	(WINAPI* _CheckReturn)(DWORD);
typedef void	(WINAPI* _SetReservedMode)(DWORD, DWORD);
typedef void	(WINAPI* _CheckReservedMode)(DWORD);
typedef void	(WINAPI* _GetHWID)(std::string&);
typedef void*	(WINAPI* _EncodePointer)(void*);
typedef void*	(WINAPI* _DecodePointer)(void*);

namespace urielac
{
	DWORD Initialize();
	extern _Login					Login;
	extern _AddInputEvent			AddInputEvent;
	extern _CheckInputEvent			CheckInputEvent;
	extern _ClearInputEvent			ClearInputEvent;
	extern _SetAttackKeyState		SetAttackKeyState;
	extern _CheckAttackKeyState		CheckAttackKeyState;
	extern _GetEncryptedTargetVID	GetEncryptedTargetVID;
	extern _SetAttackSpeed			SetAttackSpeed;
	extern _CheckAttackSpeed		CheckAttackSpeed;
	extern _GetCharacterStatePos	GetCharacterStatePos;
	extern _SetAttackVictim			SetAttackVictim;
	extern _ClearInstance			ClearInstance;
	extern _SetMoveEvent			SetMoveEvent;
	extern _CheckMoveEvent			CheckMoveEvent;
	extern _AddPointer				AddPointer;
	extern _CheckReturn				CheckReturn;
	extern _SetReservedMode			SetReservedMode;
	extern _CheckReservedMode		CheckReservedMode;
	extern _GetHWID					GetHWID;
	extern _EncodePointer			EncodePointer;
	extern _DecodePointer			DecodePointer;
};
#endif