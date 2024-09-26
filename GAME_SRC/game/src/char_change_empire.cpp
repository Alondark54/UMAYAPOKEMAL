#include "stdafx.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "db.h"
#include "guild_manager.h"
#include "marriage.h"
#include "../../common/CommonDefines.h"


int CHARACTER::ChangeEmpire(BYTE empire)
{
	if (GetEmpire() == empire)
		return 1;

#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
	if (!CanChangeEmpireOfflineShopCheck())
		return 4;
#endif

	char szQuery[1024+1];
	DWORD dwAID;
	DWORD dwPID[4];
	memset(dwPID, 0, sizeof(dwPID));

	{
		snprintf(szQuery, sizeof(szQuery),
				"SELECT id, pid1, pid2, pid3, pid4"
				#ifdef ENABLE_PLAYER_PER_ACCOUNT5
				", pid5"
				#endif
				" FROM player_index%s WHERE pid1=%u OR pid2=%u OR pid3=%u OR pid4=%u OR pid5=%u AND empire=%u",
				get_table_postfix(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetEmpire());
		auto msg(DBManager::instance().DirectQuery(szQuery));
		if (msg->Get()->uiNumRows == 0)
		{
			return 0;
		}

		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		str_to_number(dwAID, row[0]);
		str_to_number(dwPID[0], row[1]);
		str_to_number(dwPID[1], row[2]);
		str_to_number(dwPID[2], row[3]);
		str_to_number(dwPID[3], row[4]);
	}

	const int loop = 4;

	{


		DWORD dwGuildID[4];
		CGuild * pGuild[4];
		std::unique_ptr<SQLMsg> pMsg;

		for (int i = 0; i < loop; ++i)
		{
			snprintf(szQuery, sizeof(szQuery), "SELECT guild_id FROM guild_member%s WHERE pid=%u", get_table_postfix(), dwPID[i]);

			pMsg = DBManager::instance().DirectQuery(szQuery);

			if (pMsg != NULL)
			{
				if (pMsg->Get()->uiNumRows > 0)
				{
					MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

					str_to_number(dwGuildID[i], row[0]);

					pGuild[i] = CGuildManager::instance().FindGuild(dwGuildID[i]);

					if (pGuild[i] != NULL)
						return 2;
				}
				else
				{
					dwGuildID[i] = 0;
					pGuild[i] = NULL;
				}
			}
		}
	}

	{


		for (int i = 0; i < loop; ++i)
		{
			if (marriage::CManager::instance().IsEngagedOrMarried(dwPID[i]) == true)
				return 3;
		}
	}

	{

		snprintf(szQuery, sizeof(szQuery), "UPDATE player_index%s SET empire=%u WHERE pid1=%u OR pid2=%u OR pid3=%u OR pid4=%u"
				#ifdef ENABLE_PLAYER_PER_ACCOUNT5
				" OR pid5=%u"
				#endif
				" AND empire=%u",
				get_table_postfix(), empire, GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetEmpire());

		auto msg(DBManager::instance().DirectQuery(szQuery));
		if (msg->Get()->uiAffectedRows > 0)
		{

			SetChangeEmpireCount();

			return 999;
		}
	}

	return 0;
}

int CHARACTER::GetChangeEmpireCount() const
{
	char szQuery[1024+1];
	DWORD dwAID = GetAID();

	if (dwAID == 0)
		return 0;

	snprintf(szQuery, sizeof(szQuery), "SELECT change_count FROM change_empire WHERE account_id = %u", dwAID);

	auto pMsg = DBManager::instance().DirectQuery(szQuery);
	if (pMsg != NULL)
	{
		if (pMsg->Get()->uiNumRows == 0)
			return 0;

		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

		DWORD count = 0;
		str_to_number(count, row[0]);
		return count;
	}

	return 0;
}

void CHARACTER::SetChangeEmpireCount()
{
	char szQuery[1024+1];

	DWORD dwAID = GetAID();

	if (dwAID == 0) return;

	int count = GetChangeEmpireCount();

	if (count == 0)
	{
		count++;
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO change_empire VALUES(%u, %d, NOW())", dwAID, count);
	}
	else
	{
		count++;
		snprintf(szQuery, sizeof(szQuery), "UPDATE change_empire SET change_count=%d WHERE account_id=%u", count, dwAID);
	}

	auto pmsg(DBManager::instance().DirectQuery(szQuery));
}

DWORD CHARACTER::GetAID() const
{
	char szQuery[1024+1];
	DWORD dwAID = 0;
	snprintf(szQuery, sizeof(szQuery), "SELECT id FROM player_index%s WHERE pid1=%u OR pid2=%u OR pid3=%u OR pid4=%u"
			#ifdef ENABLE_PLAYER_PER_ACCOUNT5
			" OR pid5=%u"
			#endif
			" AND empire=%u",
			get_table_postfix(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetPlayerID(), GetEmpire());

	auto pMsg = DBManager::instance().DirectQuery(szQuery);
	if (pMsg != NULL)
	{
		if (pMsg->Get()->uiNumRows == 0)
			return 0;

		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(dwAID, row[0]);
		return dwAID;
	}
	else
	{
		return 0;
	}
}
//martysama0134's 2022

#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
bool CHARACTER::CanChangeEmpireOfflineShopCheck()
{
	DWORD dwPID = GetPlayerID();

	char szQuery[QUERY_MAX_LEN];
	snprintf(szQuery, sizeof(szQuery),
		"SELECT "
		"a.pid1, "
		"a.pid2, "
		"a.pid3, "
		"a.pid4, "
		"a.pid5, "
		"b.owner_id "
		"FROM player_index%s a "
		"INNER JOIN offline_shop_npc%s b "
		"ON "
		"a.pid1 = b.owner_id OR "
		"a.pid2 = b.owner_id OR "
		"a.pid3 = b.owner_id OR "
		"a.pid4 = b.owner_id OR "
		"a.pid5 = b.owner_id "
		"WHERE "
		"a.pid1 = %u OR "
		"a.pid2 = %u OR "
		"a.pid3 = %u OR "
		"a.pid4 = %u OR "
		"a.pid5 = %u",
		get_table_postfix(),	// a
		get_table_postfix(),	// b
		dwPID,	// 1
		dwPID,	// 2
		dwPID,	// 3
		dwPID,	// 4
		dwPID	// 5
	);

	auto pMsg(DBManager::instance().DirectQuery(szQuery));
	if (pMsg != nullptr) {
		if (pMsg->Get()->uiNumRows == 0) return true;
		return false;
	}
	return false;
}
#endif