#ifdef ENABLE_URIEL_AC

#include "StdAfx.h"
#include "urielac.h"

_Login					urielac::Login = 0;
_AddInputEvent			urielac::AddInputEvent = 0;
_CheckInputEvent		urielac::CheckInputEvent = 0;
_ClearInputEvent		urielac::ClearInputEvent = 0;
_SetAttackKeyState		urielac::SetAttackKeyState = 0;
_CheckAttackKeyState	urielac::CheckAttackKeyState = 0;
_GetEncryptedTargetVID	urielac::GetEncryptedTargetVID = 0;
_SetAttackSpeed			urielac::SetAttackSpeed = 0;
_CheckAttackSpeed		urielac::CheckAttackSpeed = 0;
_GetCharacterStatePos	urielac::GetCharacterStatePos = 0;
_SetAttackVictim		urielac::SetAttackVictim = 0;
_ClearInstance			urielac::ClearInstance = 0;
_SetMoveEvent			urielac::SetMoveEvent = 0;
_CheckMoveEvent			urielac::CheckMoveEvent = 0;
_AddPointer				urielac::AddPointer = 0;
_CheckReturn			urielac::CheckReturn = 0;
_SetReservedMode		urielac::SetReservedMode = 0;
_CheckReservedMode		urielac::CheckReservedMode = 0;
_GetHWID				urielac::GetHWID = 0;
_EncodePointer			urielac::EncodePointer = 0;
_DecodePointer			urielac::DecodePointer = 0;


DWORD urielac::Initialize()
{
	InitData data = { 0 };
	data.dwVersion = 3;
	
	if (!Init(&data))
		return GetLastError();

	if (data.AddPointer == nullptr) return 100;
	urielac::AddPointer = (_AddPointer)data.AddPointer;
	urielac::AddPointer(&urielac::AddPointer, data.AddPointer);

	if (data.Login == nullptr) return 101;
	urielac::Login = (_Login)data.Login;
	urielac::AddPointer(&urielac::Login, data.Login);

	if (data.AddInputEvent == nullptr) return 102;
	urielac::AddInputEvent = (_AddInputEvent)data.AddInputEvent;
	urielac::AddPointer(&urielac::AddInputEvent, data.AddInputEvent);

	if (data.CheckInputEvent == nullptr) return 103;
	urielac::CheckInputEvent = (_CheckInputEvent)data.CheckInputEvent;
	urielac::AddPointer(&urielac::CheckInputEvent, data.CheckInputEvent);

	if (data.ClearInputEvent == nullptr) return 104;
	urielac::ClearInputEvent = (_ClearInputEvent)data.ClearInputEvent;
	urielac::AddPointer(&urielac::ClearInputEvent, data.ClearInputEvent);

	if (data.SetAttackKeyState == nullptr) return 105;
	urielac::SetAttackKeyState = (_SetAttackKeyState)data.SetAttackKeyState;
	urielac::AddPointer(&urielac::SetAttackKeyState, data.SetAttackKeyState);

	if (data.CheckAttackKeyState == nullptr) return 106;
	urielac::CheckAttackKeyState = (_CheckAttackKeyState)data.CheckAttackKeyState;
	urielac::AddPointer(&urielac::CheckAttackKeyState, data.CheckAttackKeyState);

	if (data.GetEncryptedTargetVID == nullptr) return 107;
	urielac::GetEncryptedTargetVID = (_GetEncryptedTargetVID)data.GetEncryptedTargetVID;
	urielac::AddPointer(&urielac::GetEncryptedTargetVID, data.GetEncryptedTargetVID);

	if (data.SetAttackSpeed == nullptr) return 108;
	urielac::SetAttackSpeed = (_SetAttackSpeed)data.SetAttackSpeed;
	urielac::AddPointer(&urielac::SetAttackSpeed, data.SetAttackSpeed);

	if (data.CheckAttackSpeed == nullptr) return 109;
	urielac::CheckAttackSpeed = (_CheckAttackSpeed)data.CheckAttackSpeed;
	urielac::AddPointer(&urielac::CheckAttackSpeed, data.CheckAttackSpeed);

	if (data.GetCharacterStatePos == nullptr) return 110;
	urielac::GetCharacterStatePos = (_GetCharacterStatePos)data.GetCharacterStatePos;
	urielac::AddPointer(&urielac::GetCharacterStatePos, data.GetCharacterStatePos);

	if (data.SetAttackVictim == nullptr) return 111;
	urielac::SetAttackVictim = (_SetAttackVictim)data.SetAttackVictim;
	urielac::AddPointer(&urielac::SetAttackVictim, data.SetAttackVictim);

	if (data.ClearInstance == nullptr) return 112;
	urielac::ClearInstance = (_ClearInstance)data.ClearInstance;
	urielac::AddPointer(&urielac::ClearInstance, data.ClearInstance);

	if (data.SetMoveEvent == nullptr) return 113;
	urielac::SetMoveEvent = (_SetMoveEvent)data.SetMoveEvent;
	urielac::AddPointer(&urielac::SetMoveEvent, data.SetMoveEvent);

	if (data.CheckMoveEvent == nullptr) return 114;
	urielac::CheckMoveEvent = (_CheckMoveEvent)data.CheckMoveEvent;
	urielac::AddPointer(&urielac::CheckMoveEvent, data.CheckMoveEvent);

	if (data.CheckReturn == nullptr) return 115;
	urielac::CheckReturn = (_CheckReturn)data.CheckReturn;
	urielac::AddPointer(&urielac::CheckReturn, data.CheckReturn);
	
	if (data.SetReservedMode == nullptr) return 116;
	urielac::SetReservedMode = (_SetReservedMode)data.SetReservedMode;
	urielac::AddPointer(&urielac::SetReservedMode, data.SetReservedMode);

	if (data.CheckReservedMode == nullptr) return 117;
	urielac::CheckReservedMode = (_CheckReservedMode)data.CheckReservedMode;
	urielac::AddPointer(&urielac::CheckReservedMode, data.CheckReservedMode);

	if (data.GetHWID == nullptr) return 118;
	urielac::GetHWID = (_GetHWID)data.GetHWID;
	urielac::AddPointer(&urielac::GetHWID, data.GetHWID);
	
	if (data.EncodePointer == nullptr) return 119;
	urielac::EncodePointer = (_EncodePointer)data.EncodePointer;
	urielac::AddPointer(&urielac::EncodePointer, data.EncodePointer);

	if (data.DecodePointer == nullptr) return 120;
	urielac::DecodePointer = (_DecodePointer)data.DecodePointer;
	urielac::AddPointer(&urielac::DecodePointer, data.DecodePointer);

	return 0;
}

#endif