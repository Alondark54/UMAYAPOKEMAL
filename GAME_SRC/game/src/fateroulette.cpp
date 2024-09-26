#include "stdafx.h"
#include "fateroulette.h"
#include "char.h"
#include "db.h"
#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"

extern int passes_per_sec;

CFateRoulette::CFateRoulette()
{
	Initialize();
}

CFateRoulette::~CFateRoulette()
{
	m_map_pkFateRoulette.clear();
}

void CFateRoulette::Initialize()
{
	m_map_pkFateRoulette.clear();

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"select vnum, count from player.fateroulette"));

	if (pMsg->Get()->uiNumRows == 0)
	{
		// sys_err("sql query null");
		return;
	}

	MYSQL_ROW row;
	DWORD vnum = 0;
	short count = 0;

	while (NULL != (row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		str_to_number(vnum, row[0]);
		str_to_number(count, row[1]);

		m_map_pkFateRoulette.insert(std::make_pair(vnum, count));

	}
	sys_err("Roulette size %d",m_map_pkFateRoulette.size());
}

void CFateRoulette::SetReward(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
	{
		return;
	}
	
	
	bool canuse = false;
	int usetype = 0;
	if (ch->GetGold() >= 25000000)
	{
		canuse = true;
		usetype = 1;
	}
		
	if (ch->CountSpecifyItem(30135) >= 1)
	{
		canuse = true;
		usetype = 2;
	}
		
	if (canuse == false)
	{
		ch->ChatPacket(CHAT_TYPE_INFO,"Cark biletin ya da yangin olmadan bu islemi yapamazsin.");
		ch->ChatPacket(CHAT_TYPE_COMMAND,"RouletteReset");
		return;
	}
		
	
	
	char buf[1024];
	int prepare = number(0, 15);
	DWORD size = ItemSize();
	for (int i = 0; i < 16; i++) 
	{
		
		int luck = number(0, (size-1));
		int x = 0;
		// sys_err("xd i %d luck %d prep %d",i,luck, prepare);
		for (TFateRoulette::iterator it = m_map_pkFateRoulette.begin(); it!=m_map_pkFateRoulette.end(); ++it)
		{
			int reward = it->first;
			short count = it->second;
			if (x == luck)
			{
				if (i == 0)
				{
					snprintf(buf, sizeof(buf), "%d|%d", reward, count);
				}
				else if (i == 15)
				{
					snprintf(buf, sizeof(buf), "%s|%d|%d|", buf, reward, count);
				}
				else
				{
					snprintf(buf, sizeof(buf), "%s|%d|%d", buf, reward, count);
				}
				if (prepare == i)
				{
					ch->SetFateReward(it->first);
					ch->SetFateRewardCount(it->second);
				}
					// sys_err("xd x %d reward %d count %d buf %s",x,reward, count, buf);
			}
			x++;
		}
	}
	
	if (usetype == 2)
	{
		ch->RemoveSpecifyItem(30135, 1);
	}
	
	if (usetype == 1)
	{
		ch->PointChange(POINT_GOLD, -25000000, true);
	}
	
	
	ch->ChatPacket(CHAT_TYPE_COMMAND,"RoulettePrepare %s", buf);
	ch->ChatPacket(CHAT_TYPE_COMMAND,"RouletteRun %d|%d", ((number(1, 3)*16)+prepare),prepare);
}

DWORD CFateRoulette::ItemSize()
{
	return m_map_pkFateRoulette.size();
}
