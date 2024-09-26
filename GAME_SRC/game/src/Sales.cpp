#include "stdafx.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <ctime>
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "locale_service.h"
#include "questmanager.h"

#ifdef NEW_SALES_SYSTEM
#include "Sales.h"

CSales::CSales()
{
}

CSales::~CSales()
{
}


void CSales::SalesCache()
{
	if (g_bAuthServer) return;

	TSalesInfo *info = NULL;
	TSalesInfoMap::iterator iter;
	for (iter=m_salesvector.begin(); iter!=m_salesvector.end(); ++iter)
	{
		info = *iter;
		M2_DELETE(info);
	}
	m_salesvector.clear();

	FILE	*fp;
	char	one_line[256];
	const char	*delim			= "#";
	char	*v;
	char szSalesList[256];
	DWORD vnum = 0;
	DWORD count = 0;
	snprintf(szSalesList, sizeof(szSalesList), "%s/sales.txt", LocaleService_GetBasePath().c_str());

	fp = fopen(szSalesList, "r");

	if (fp == NULL)
	{
		sys_err("sales.txt bulunamadý!");
		return;
	}
			
	while (fgets(one_line, 256, fp))
	{
		if (one_line[0] == '#')
			continue;
			
		info = M2_NEW TSalesInfo;
		memset(info, 0x00, sizeof(TSalesInfo));

		v = strtok(one_line, delim);
		str_to_number(info->sales_id, v);

		v = strtok(NULL, delim);
		str_to_number(info->sales_price, v);

		v = strtok(NULL, delim);
		str_to_number(info->normal_price, v);
		
		v = strtok(NULL, delim);
		str_to_number(info->type, v);
		
		v = strtok(NULL, delim);
		str_to_number(info->day, v);
		
		v = strtok(NULL, delim);
		str_to_number(info->month, v);
		
		v = strtok(NULL, delim);
		str_to_number(info->hour, v);
		
		v = strtok(NULL, delim);
		str_to_number(info->minute, v);


		v = strtok(NULL, delim);
		
		info->items.clear();
		
		bool canadd = false;
		while(canadd == false)
		{
			str_to_number(vnum, v);
			v = strtok(NULL, delim);
			str_to_number(count, v);
			info->items.insert(std::make_pair(vnum, count));
			v = strtok(NULL, delim);
			if (v == NULL)
			{
				canadd = true;
			}
		}
		
		if (info->items.size() > 0)
		{
			m_salesvector.push_back(info);
		}

		if (NULL == v)
		{
			continue;
		}
	}

	fclose(fp);
}

void CSales::BuySales(LPCHARACTER ch, DWORD id)
{
	if (!ch || id <= 0)
		return;
	
	
	if (quest::CQuestManager::instance().GetEventFlag("sales") == 0) return;
	
	TSalesInfo *info = NULL;
	TSalesInfoMap::iterator	iter;
	
	time_t cur_Time = get_global_time();
    struct tm vKey3 = *localtime(&cur_Time);
	
	struct tm vKey = {0};
	
	vKey.tm_mday = vKey3.tm_mday;
	vKey.tm_mon = vKey3.tm_mon;
	vKey.tm_year = vKey3.tm_year;
	
	vKey.tm_hour = vKey3.tm_hour;
	vKey.tm_min = vKey3.tm_min;
	vKey.tm_sec = vKey3.tm_sec;
	
	time_t t = mktime(&vKey);

	for (iter=m_salesvector.begin(); iter!=m_salesvector.end(); ++iter)
	{
		info = *iter;

		if (info->sales_id != id) continue;
		
		struct tm vKey2 = {0};
		vKey2.tm_mday = info->day;
		vKey2.tm_mon = info->month-1;
		vKey2.tm_year = 123;
		
		vKey2.tm_hour = info->hour;
		vKey2.tm_min = info->minute;
		vKey2.tm_sec = 0;
		
		time_t t2 = mktime(&vKey2);
		
		DWORD price = info->sales_price;
		
		
		if (t2-t > 0)
		{
			if (info->type > 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO,"Teklif henüz baþlamadý!");
				return;
			}
			else
			{
				if (price > ch->GetCoins())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Teklifi almak için yeterli EP'in yok.");
					return;
				}
				else
				{
					
					std::map<DWORD, DWORD>::iterator it = info->items.begin();
					while (it != info->items.end())
					{
						ch->AutoGiveItem(it->first, it->second);
						it++;
					}	
				}
				ch->SetCoins(price,0);
			}
		}
		else
		{
			if (info->type > 0)
			{
				
				
				long remain = t2-t+(info->type*60*60);
				if (remain > 0)
				{
					if (price > ch->GetCoins())
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "Teklifi almak için yeterli EP'in yok.");
						return;
					}
					else
					{
						
						std::map<DWORD, DWORD>::iterator it = info->items.begin();
						while (it != info->items.end())
						{
							ch->AutoGiveItem(it->first, it->second);
							it++;
						}
						
					}
					ch->SetCoins(price,0);
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO,"Teklifin süresi doldu!");
					return;
				}
				
				
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO,"Teklifin süresi doldu!");
				return;
			}
		}
		break;

	}
}

void CSales::List(LPCHARACTER ch)
{
	
	if (!ch)
		return;
		
	if (quest::CQuestManager::instance().GetEventFlag("sales") == 0) return;
	
	TSalesInfo *info = NULL;
	TSalesInfoMap::iterator	iter;
	
	time_t cur_Time = get_global_time();
    struct tm vKey3 = *localtime(&cur_Time);
	
	struct tm vKey = {0};
	
	vKey.tm_mday = vKey3.tm_mday;
	vKey.tm_mon = vKey3.tm_mon;
	vKey.tm_year = vKey3.tm_year;
	
	vKey.tm_hour = vKey3.tm_hour;
	vKey.tm_min = vKey3.tm_min;
	vKey.tm_sec = vKey3.tm_sec;
	

	time_t t = mktime(&vKey);
	
	ch->ChatPacket(CHAT_TYPE_COMMAND, "SalesClearList");
	for (iter=m_salesvector.begin(); iter!=m_salesvector.end(); ++iter)
	{
		info = *iter;


		struct tm vKey2 = {0};
		vKey2.tm_mday = info->day;
		vKey2.tm_mon = info->month-1;
		vKey2.tm_year = 123; // yýl
		
		vKey2.tm_hour = info->hour;
		vKey2.tm_min = info->minute;
		vKey2.tm_sec = 0;
		
		time_t t2 = mktime(&vKey2);
			
		
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SalesNewList %d %d %d %d %d",info->sales_id, info->type, info->normal_price, info->sales_price, t2);
		// ch->ChatPacket(CHAT_TYPE_INFO, "SalesNewList %d %d %d %d %ld %ld",info->sales_id, info->type, info->normal_price, info->sales_price, t2, t);
		
		std::map<DWORD, DWORD>::iterator it = info->items.begin();
		while (it != info->items.end())
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "SalesInsertItem %d %d %d",info->sales_id, it->first, it->second);
			it++;
		}
		
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SalesUpdateList %d",info->sales_id);
		
	}

}

bool CSales::CheckSales()
{
	
	if (m_salesvector.size() == 0)
		return false;

	return true;
}


#endif