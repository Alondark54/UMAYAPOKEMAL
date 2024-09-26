#include "stdafx.h"
#include "QID.h"
#include "DBManager.h"
#include "Peer.h"
#include "ClientManager.h"

#include "LuckyDrawManager.h"

LuckyDrawManager::LuckyDrawManager()
{
}

LuckyDrawManager::~LuckyDrawManager()
{
}

void LuckyDrawManager::GetCurrentLuckyDraw(CPeer* peer)
{
	if (!t_CurrentLuckyDrawTable) return;

	sys_log(0, "LuckyDraw-DID:%u", t_CurrentLuckyDrawTable->dId);
	peer->EncodeHeader(HEADER_DG_LUCKY_DRAW_CURRENT_LD, 0, sizeof(TLuckyDrawTable));
	peer->Encode(t_CurrentLuckyDrawTable.get(), sizeof(TLuckyDrawTable));
}

void LuckyDrawManager::UpdateCurrentLuckyDraw()
{
	if (t_CurrentLuckyDrawTable && !t_CurrentLuckyDrawTable->bIsFinished)
	{
		const time_t now = time(0);
		const struct tm vKey = *localtime(&now);
		int leftTime = t_CurrentLuckyDrawTable->dEndTime - now;
		sys_log(0, "LuckyDraw-leftTime %d endTime %d", leftTime, t_CurrentLuckyDrawTable->dEndTime);
		if (leftTime <= 0) 
		{
			ActionFinishLuckyDraw(t_CurrentLuckyDrawTable->dId);
			return;
		}
	}
	if (t_CurrentLuckyDrawTable && !t_CurrentLuckyDrawTable->bIsFinished) return;

	char szQuery[QUERY_MAX_LEN];

	snprintf(szQuery, sizeof(szQuery),
		"SELECT id, enter_price, participant_per_player, max_participant, total_participant, UNIX_TIMESTAMP(start_time), UNIX_TIMESTAMP(end_time), is_finished, winner1_id, winner2_id, winner3_id, award_data FROM player.lucky_draw WHERE lucky_draw.start_time < NOW() AND lucky_draw.end_time > NOW() LIMIT 1");

	std::unique_ptr<SQLMsg> pMsg = (CDBManager::instance().DirectQuery(szQuery));

	if (pMsg->Get()->uiNumRows == 0)
	{
		snprintf(szQuery, sizeof(szQuery),
			"SELECT id, enter_price, participant_per_player, max_participant, total_participant, UNIX_TIMESTAMP(start_time), UNIX_TIMESTAMP(end_time), is_finished, winner1_id, winner2_id, winner3_id, award_data FROM player.lucky_draw WHERE lucky_draw.start_time < NOW() ORDER BY start_time DESC LIMIT 1");

		pMsg = (CDBManager::instance().DirectQuery(szQuery));
		if (pMsg->Get()->uiNumRows == 0) return;
	}

	
	MYSQL_RES* pRes = pMsg->Get()->pSQLResult;
	
	for (uint i = 0; i < pMsg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pRes);
		int col = 0;

		DWORD dwID = 0;
		str_to_number(dwID, row[col++]);

		
		if (t_CurrentLuckyDrawTable && t_CurrentLuckyDrawTable->dId == dwID)
		{
			b_IsChanging = false;
			return;
		}
		else
		{
			b_IsChanging = true;
		}

		TLuckyDrawTable* ldData = new TLuckyDrawTable;
		memset(ldData, 0, sizeof(TLuckyDrawTable));

		ldData->dId = dwID;
		str_to_number(ldData->lEnterPrice, row[col++]);
		str_to_number(ldData->dParticipantPerPlayer, row[col++]);
		str_to_number(ldData->dMaxParticipant, row[col++]);
		str_to_number(ldData->dTotalParticipant, row[col++]);
		str_to_number(ldData->dStartTime, row[col++]);
		str_to_number(ldData->dEndTime, row[col++]);
		str_to_number(ldData->bIsFinished, row[col++]);
		str_to_number(ldData->dWinner1Id, row[col++]);
		str_to_number(ldData->dWinner2Id, row[col++]);
		str_to_number(ldData->dWinner3Id, row[col++]);
		
		int award_data[50];
		thecore_memcpy(award_data, row[col++], sizeof(award_data));
		
		memset(ldData->aAwardList, 0, sizeof(ldData->aAwardList));
		int j = 0;
		for (int i = 0; i < 50; i += 2)
		{
			if (i == 0)
			{
				ldData->dJoinItemVnum = award_data[i];
				ldData->dJoinItemCount = award_data[i + 1];
				continue;
			}
			ldData->aAwardList[j].dVnum = award_data[i];
			ldData->aAwardList[j].dCount = award_data[i + 1];
			j++;
		}

		if (ldData->dWinner1Id != 0 && ldData->dWinner2Id != 0 && ldData->dWinner3Id != 0)
		{
			GetPlayerName(ldData->dWinner1Id, ldData->winner1Name);
			GetPlayerName(ldData->dWinner2Id, ldData->winner2Name);
			GetPlayerName(ldData->dWinner3Id, ldData->winner3Name);
			sys_log(0, "LData - Name - %s", ldData->winner1Name);
		}
		t_CurrentLuckyDrawTable.reset(ldData);
		sys_log(0, "LData %u - %u - %u", ldData->dId, ldData->dJoinItemVnum, ldData->dJoinItemCount);
	}

	if (!t_CurrentLuckyDrawTable)
	{
		sys_log(0, "Lucky Draw Can't Load");
		return;
	}

	GetParticipants();

	sys_log(0, "Lucky Draw Loaded %d participants", t_CurrentLuckyDrawTable->dTotalParticipant);
}

void LuckyDrawManager::GetParticipants()
{
	if (!t_CurrentLuckyDrawTable) return;
	char szQuery[QUERY_MAX_LEN];

	snprintf(szQuery, sizeof(szQuery),
		"SELECT * FROM player.lucky_draw_participant WHERE lucky_draw_id=%u", t_CurrentLuckyDrawTable->dId);

	auto pMsg(CDBManager::instance().DirectQuery(szQuery));

	MYSQL_RES* pRes = pMsg->Get()->pSQLResult;
	int rowCount = pMsg->Get()->uiNumRows;
	a_Participants.clear();
	a_Participants.resize(pMsg->Get()->uiNumRows);
	a_ParticipantsAccounts.clear();
	a_ParticipantsAccounts.resize(pMsg->Get()->uiNumRows);
	for (uint i = 0; i < pMsg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pRes);
		DWORD playerId, accountId;
		str_to_number(playerId, row[2]);
		str_to_number(accountId, row[3]);
		a_Participants.at(i) = playerId;
		a_ParticipantsAccounts.at(i) = accountId;
	}
	b_IsChanging = false;
}

void LuckyDrawManager::AddParticipant(CPeer* peer, DWORD dwHandle, TPacketGDLuckyDrawAddParticipant* data)
{
	TPacketDGLuckyDrawAddParticipant tData;
	memset(&tData, 0, sizeof(TPacketDGLuckyDrawAddParticipant));

	if (!t_CurrentLuckyDrawTable || t_CurrentLuckyDrawTable->dId != data->dLuckyDrawId || t_CurrentLuckyDrawTable->bIsFinished || b_IsChanging)
	{
		sys_log(0, "LuckyDraw -> AddParticipant RES_FAILED %u-%u %d %d", t_CurrentLuckyDrawTable->dId, data->dLuckyDrawId, t_CurrentLuckyDrawTable->bIsFinished, b_IsChanging);
		tData.resType = LD_RES_FAILED;
		peer->EncodeHeader(HEADER_DG_LUCKY_DRAW_ADD_PARTICIPANT, dwHandle, sizeof(TPacketDGLuckyDrawAddParticipant));
		peer->Encode(&tData, sizeof(TPacketDGLuckyDrawAddParticipant));
		return;
	}

	DWORD playerParticipants = 0;

	for (auto it = a_Participants.begin(); it != a_Participants.end(); ++it)
	{
		if (*it == data->dPlayerId) playerParticipants++;
	}

	sys_log(0, "LuckyDraw -> AddParticipant Player Participant:%d", playerParticipants);

	if (playerParticipants >= t_CurrentLuckyDrawTable->dParticipantPerPlayer)
	{
		sys_log(0, "LuckyDraw -> AddParticipant MAX_REACH");
		tData.resType = LD_RES_MAX_REACH;
		peer->EncodeHeader(HEADER_DG_LUCKY_DRAW_ADD_PARTICIPANT, dwHandle, sizeof(TPacketDGLuckyDrawAddParticipant));
		peer->Encode(&tData, sizeof(TPacketDGLuckyDrawAddParticipant));
		return;
	}

	char queryStr[QUERY_MAX_LEN];
	snprintf(queryStr, sizeof(queryStr), "INSERT INTO player.lucky_draw_participant (lucky_draw_id, player_id, account_id) VALUES (%u, %u, %u)", data->dLuckyDrawId, data->dPlayerId, data->dAccountId);
	CDBManager::instance().AsyncQuery(queryStr);

	a_Participants.push_back(data->dPlayerId);
	a_ParticipantsAccounts.push_back(data->dAccountId);

	t_CurrentLuckyDrawTable->dTotalParticipant++;
	snprintf(queryStr, sizeof(queryStr), "UPDATE player.lucky_draw SET lucky_draw.total_participant=%u WHERE lucky_draw.id=%u", 
		t_CurrentLuckyDrawTable->dTotalParticipant, 
		data->dLuckyDrawId);
	CDBManager::instance().AsyncQuery(queryStr);


	tData.resType = LD_RES_SUCCESS;
	peer->EncodeHeader(HEADER_DG_LUCKY_DRAW_ADD_PARTICIPANT, dwHandle, sizeof(TPacketDGLuckyDrawAddParticipant));
	peer->Encode(&tData, sizeof(TPacketDGLuckyDrawAddParticipant));
}

void LuckyDrawManager::ActionFinishLuckyDraw(DWORD dLuckyDrawId)
{
	if (!t_CurrentLuckyDrawTable || a_Participants.size() < 3) return;

	
	t_CurrentLuckyDrawTable->bIsFinished = true;

	DWORD winner1Index = rand() % a_Participants.size();
	
	int breakCount = 0;
	DWORD winner2Index = rand() % a_Participants.size();
	while (a_Participants[winner1Index] == a_Participants[winner2Index]) 
	{
		winner2Index = rand() % a_Participants.size();
		breakCount++;
		if (breakCount == 1000) break;
	}
		
	breakCount = 0;
	DWORD winner3Index = rand() % a_Participants.size();
	while (a_Participants[winner1Index] == a_Participants[winner3Index] || a_Participants[winner2Index] == a_Participants[winner3Index])
	{
		winner3Index = rand() % a_Participants.size();
		breakCount++;
		if (breakCount == 1000) break;
	}

	t_CurrentLuckyDrawTable->dWinner1Id = a_Participants[winner1Index];
	t_CurrentLuckyDrawTable->dWinner2Id = a_Participants[winner2Index];
	t_CurrentLuckyDrawTable->dWinner3Id = a_Participants[winner3Index];

	GetPlayerName(t_CurrentLuckyDrawTable->dWinner1Id, t_CurrentLuckyDrawTable->winner1Name);
	GetPlayerName(t_CurrentLuckyDrawTable->dWinner2Id, t_CurrentLuckyDrawTable->winner2Name);
	GetPlayerName(t_CurrentLuckyDrawTable->dWinner3Id, t_CurrentLuckyDrawTable->winner3Name);

	char queryStr[QUERY_MAX_LEN];
	snprintf(queryStr, sizeof(queryStr), "UPDATE player.lucky_draw SET lucky_draw.is_finished=%d, lucky_draw.winner1_id=%d, lucky_draw.winner2_id=%d, lucky_draw.winner3_id=%d WHERE lucky_draw.id=%u",
		1,
		t_CurrentLuckyDrawTable->dWinner1Id,
		t_CurrentLuckyDrawTable->dWinner2Id,
		t_CurrentLuckyDrawTable->dWinner3Id,
		dLuckyDrawId);
	CDBManager::instance().AsyncQuery(queryStr);

	char queryStr2[QUERY_MAX_LEN];
	snprintf(queryStr2, sizeof(queryStr2), "DELETE FROM player.lucky_draw.participants WHERE lucky_draw.id=%u",dLuckyDrawId);
	CDBManager::instance().AsyncQuery(queryStr2);
}

void LuckyDrawManager::AddToMall(DWORD accountId, DWORD pos, DWORD itemVnum, DWORD itemCount)
{
	char queryStr[QUERY_MAX_LEN];
	snprintf(queryStr, sizeof(queryStr), "INSERT INTO player.item (owner_id, `window`, pos, count, vnum, socket0, socket1, socket2, socket3, socket4, socket5, attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6, sealbind) VALUES (%u, 'MALL', %u, %u, %u, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)",
		accountId,
		pos,
		itemCount,
		itemVnum);
	CDBManager::instance().AsyncQuery(queryStr);
}

DWORD LuckyDrawManager::GetAccountMallLastPosition(DWORD accountId)
{
	DWORD lastPosition = 0;
	char szQuery[QUERY_MAX_LEN];

	snprintf(szQuery, sizeof(szQuery),
		"SELECT pos FROM player.item WHERE owner_id = %u and window = 'MALL' ORDER BY id DESC LIMIT 0, 1", accountId);

	auto pMsg(CDBManager::instance().DirectQuery(szQuery));

	if (pMsg->Get()->uiNumRows == 0) return 0;

	MYSQL_RES* pRes = pMsg->Get()->pSQLResult;
	for (uint i = 0; i < pMsg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pRes);
		str_to_number(lastPosition, row[0]);
	}

	return lastPosition;
}

void LuckyDrawManager::GetPlayerName(DWORD playerId, char* playerName)
{
	DWORD lastPosition = 0;
	char szQuery[QUERY_MAX_LEN];
	char _playerName[25];

	snprintf(szQuery, sizeof(szQuery), "SELECT name FROM player.player WHERE id = %d;", playerId);

	auto pMsg(CDBManager::instance().DirectQuery(szQuery));

	if (pMsg->Get()->uiNumRows == 0) return;

	MYSQL_RES* pRes = pMsg->Get()->pSQLResult;
	for (uint i = 0; i < pMsg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pRes);
		strlcpy(playerName, row[0], sizeof(_playerName));
	}
}