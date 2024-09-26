#pragma once
#include <set>
#include <map>
#include "Peer.h"
#include "../../common/tables.h"

class LuckyDrawManager : public singleton<LuckyDrawManager>
{
public:
	LuckyDrawManager();
	virtual ~LuckyDrawManager();

	void GetCurrentLuckyDraw(CPeer* peer);
	void UpdateCurrentLuckyDraw();

	void AddParticipant(CPeer* peer, DWORD dwHandle, TPacketGDLuckyDrawAddParticipant* data);
	void ActionFinishLuckyDraw(DWORD dLuckyDrawId);
private:
	std::unique_ptr<TLuckyDrawTable> t_CurrentLuckyDrawTable = nullptr;
	std::vector<DWORD> a_Participants;
	std::vector<DWORD> a_ParticipantsAccounts;
	bool b_IsChanging = false;

	void GetParticipants();
	DWORD GetAccountMallLastPosition(DWORD accountId);
	void AddToMall(DWORD accountId, DWORD pos, DWORD itemVnum, DWORD itemCount);
	void GetPlayerName(DWORD playerId, char* playerName);
	
};