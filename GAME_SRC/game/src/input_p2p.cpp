#include "stdafx.h"
#include "../../common/billing.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "party.h"
#include "messenger_manager.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "xmas_event.h"
#include "affect.h"
#include "castle.h"
#include "dev_log.h"
#include "locale_service.h"
#include "questmanager.h"
#include "pcbang.h"
#include "skill.h"
#include "threeway_war.h"
#ifdef YONETICI_PM
#include "buffer_manager.h"
#endif
#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
#include "offlineshop_manager.h"
#include "sectree_manager.h"
#endif
#ifdef NEW_SALES_SYSTEM
	#include "Sales.h"
#endif
#ifdef FATE_ROULETTE
	#include "fateroulette.h"
#endif

#ifdef ENABLE_CAOS_EVENT
#include "NewCaosEvent.h"
#endif

#ifdef ENABLE_NEW_MISSIONS
#include "NewMissions.hxx"
#endif

////////////////////////////////////////////////////////////////////////////////
// Input Processor
CInputP2P::CInputP2P()
{
	BindPacketInfo(&m_packetInfoGG);
}

void CInputP2P::Login(LPDESC d, const char * c_pData)
{
	P2P_MANAGER::instance().Login(d, (TPacketGGLogin *) c_pData);
}

void CInputP2P::Logout(LPDESC d, const char * c_pData)
{
	TPacketGGLogout * p = (TPacketGGLogout *) c_pData;
	P2P_MANAGER::instance().Logout(p->szName);
}

int CInputP2P::Relay(LPDESC d, const char * c_pData, size_t uiBytes)
{
	TPacketGGRelay * p = (TPacketGGRelay *) c_pData;

	if (uiBytes < sizeof(TPacketGGRelay) + p->lSize)
		return -1;

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	sys_log(0, "InputP2P::Relay : %s size %d", p->szName, p->lSize);

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(p->szName);

	const BYTE* c_pbData = (const BYTE *) (c_pData + sizeof(TPacketGGRelay));

	if (!pkChr)
		return p->lSize;

	if (*c_pbData == HEADER_GC_WHISPER)
	{
		if (pkChr->IsBlockMode(BLOCK_WHISPER))
		{

			return p->lSize;
		}

		char buf[1024];
		memcpy(buf, c_pbData, MIN(p->lSize, sizeof(buf)));

		TPacketGCWhisper* p2 = (TPacketGCWhisper*) buf;

		BYTE bToEmpire = (p2->bType >> 4);
		p2->bType = p2->bType & 0x0F;
		if(p2->bType == 0x0F) {

			p2->bType = WHISPER_TYPE_SYSTEM;
		} else {
			if (!pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
				if (bToEmpire >= 1 && bToEmpire <= 3 && pkChr->GetEmpire() != bToEmpire)
				{
					ConvertEmpireText(bToEmpire,
							buf + sizeof(TPacketGCWhisper),
							p2->wSize - sizeof(TPacketGCWhisper),
							10+2*pkChr->GetSkillPower(SKILL_LANGUAGE1 + bToEmpire - 1));
				}
		}

		pkChr->GetDesc()->Packet(buf, p->lSize);
	}
	else
		pkChr->GetDesc()->Packet(c_pbData, p->lSize);

	return (p->lSize);
}

#ifdef ENABLE_FULL_NOTICE
int CInputP2P::Notice(LPDESC d, const char * c_pData, size_t uiBytes, bool bBigFont)
#else
int CInputP2P::Notice(LPDESC d, const char * c_pData, size_t uiBytes)
#endif
{
	TPacketGGNotice * p = (TPacketGGNotice *) c_pData;

	if (uiBytes < sizeof(TPacketGGNotice) + p->lSize)
		return -1;

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char szBuf[256+1];
	strlcpy(szBuf, c_pData + sizeof(TPacketGGNotice), MIN(p->lSize + 1, sizeof(szBuf)));
#ifdef ENABLE_FULL_NOTICE
	SendNotice(szBuf, bBigFont);
#else
	SendNotice(szBuf);
#endif
	return (p->lSize);
}

#ifdef YONETICI_PM
int CInputP2P::BulkWhisperSend(LPDESC d, const char * c_pData, size_t uiBytes)
{
	TPacketGGBulkWhisper * p = (TPacketGGBulkWhisper *)c_pData;

	if (uiBytes < sizeof(TPacketGGBulkWhisper) + p->lSize)
		return -1;

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char szBuf[CHAT_MAX_LEN + 1];
	strlcpy(szBuf, c_pData + sizeof(TPacketGGBulkWhisper), MIN(p->lSize + 1, sizeof(szBuf)));
	SendBulkWhisper(szBuf);

	return (p->lSize);
}
#endif

int CInputP2P::MonarchNotice(LPDESC d, const char * c_pData, size_t uiBytes)
{
	TPacketGGMonarchNotice * p = (TPacketGGMonarchNotice *) c_pData;

	if (uiBytes < p->lSize + sizeof(TPacketGGMonarchNotice))
		return -1;

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char szBuf[256+1];
	strlcpy(szBuf, c_pData + sizeof(TPacketGGMonarchNotice), MIN(p->lSize + 1, sizeof(szBuf)));
	SendMonarchNotice(p->bEmpire, szBuf);
	return (p->lSize);
}

int CInputP2P::MonarchTransfer(LPDESC d, const char* c_pData)
{
	TPacketMonarchGGTransfer* p = (TPacketMonarchGGTransfer*) c_pData;
	LPCHARACTER pTargetChar = CHARACTER_MANAGER::instance().FindByPID(p->dwTargetPID);

	if (pTargetChar != NULL)
	{
		unsigned int qIndex = quest::CQuestManager::instance().GetQuestIndexByName("monarch_transfer");

		if (qIndex != 0)
		{
			pTargetChar->SetQuestFlag("monarch_transfer.x", p->x);
			pTargetChar->SetQuestFlag("monarch_transfer.y", p->y);
			quest::CQuestManager::instance().Letter(pTargetChar->GetPlayerID(), qIndex, 0);
		}
	}

	return 0;
}

int CInputP2P::Guild(LPDESC d, const char* c_pData, size_t uiBytes)
{
	TPacketGGGuild * p = (TPacketGGGuild *) c_pData;
	uiBytes -= sizeof(TPacketGGGuild);
	c_pData += sizeof(TPacketGGGuild);

	CGuild * g = CGuildManager::instance().FindGuild(p->dwGuild);

	switch (p->bSubHeader)
	{
		case GUILD_SUBHEADER_GG_CHAT:
			{
				if (uiBytes < sizeof(TPacketGGGuildChat))
					return -1;

				TPacketGGGuildChat * p = (TPacketGGGuildChat *) c_pData;

				if (g)
					g->P2PChat(p->szText);

				return sizeof(TPacketGGGuildChat);
			}

		case GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS:
			{
				if (uiBytes < sizeof(int))
					return -1;

				int iBonus = *((int *) c_pData);
				CGuild* pGuild = CGuildManager::instance().FindGuild(p->dwGuild);
				if (pGuild)
				{
					pGuild->SetMemberCountBonus(iBonus);
				}
				return sizeof(int);
			}
		default:
			sys_err ("UNKNOWN GUILD SUB PACKET");
			break;
	}
	return 0;
}


struct FuncShout
{
	const char * m_str;
	BYTE m_bEmpire;

	FuncShout(const char * str, BYTE bEmpire) : m_str(str), m_bEmpire(bEmpire)
	{
	}

	void operator () (LPDESC d)
	{
#ifdef ENABLE_NEWSTUFF
		if (!d->GetCharacter() || (!g_bGlobalShoutEnable && d->GetCharacter()->GetGMLevel() == GM_PLAYER && d->GetEmpire() != m_bEmpire))
			return;
#else
		if (!d->GetCharacter() || (d->GetCharacter()->GetGMLevel() == GM_PLAYER && d->GetEmpire() != m_bEmpire))
			return;
#endif
#ifdef	SOHBET_DURDUR
		if(d->GetCharacter()->GetChatDurdur() == 1) 
			return;
#endif
		d->GetCharacter()->ChatPacket(CHAT_TYPE_SHOUT, "%s", m_str);
	}
};

void SendShout(const char * szText, BYTE bEmpire)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), FuncShout(szText, bEmpire));
}

void CInputP2P::Shout(const char * c_pData)
{
	TPacketGGShout * p = (TPacketGGShout *) c_pData;
	SendShout(p->szText, p->bEmpire);
}

void CInputP2P::Disconnect(const char * c_pData)
{
	TPacketGGDisconnect * p = (TPacketGGDisconnect *) c_pData;

	LPDESC d = DESC_MANAGER::instance().FindByLoginName(p->szLogin);

	if (!d)
		return;

	if (!d->GetCharacter())
	{
		d->SetPhase(PHASE_CLOSE);
	}
	else
		d->DisconnectOfSameLogin();
}

void CInputP2P::Setup(LPDESC d, const char * c_pData)
{
	TPacketGGSetup * p = (TPacketGGSetup *) c_pData;
	sys_log(0, "P2P: Setup %s:%d", d->GetHostName(), p->wPort);
	d->SetP2P(d->GetHostName(), p->wPort, p->bChannel);
}

void CInputP2P::MessengerAdd(const char * c_pData)
{
	TPacketGGMessenger * p = (TPacketGGMessenger *) c_pData;
	sys_log(0, "P2P: Messenger Add %s %s", p->szAccount, p->szCompanion);
	MessengerManager::instance().__AddToList(p->szAccount, p->szCompanion);
}

void CInputP2P::MessengerRemove(const char * c_pData)
{
	TPacketGGMessenger * p = (TPacketGGMessenger *) c_pData;
	sys_log(0, "P2P: Messenger Remove %s %s", p->szAccount, p->szCompanion);
	MessengerManager::instance().__RemoveFromList(p->szAccount, p->szCompanion);
}

void CInputP2P::FindPosition(LPDESC d, const char* c_pData)
{
	TPacketGGFindPosition* p = (TPacketGGFindPosition*) c_pData;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(p->dwTargetPID);
#ifdef ENABLE_CMD_WARP_IN_DUNGEON
	if (ch)
#else
	if (ch && ch->GetMapIndex() < 10000)
#endif
	{
		TPacketGGWarpCharacter pw;
		pw.header = HEADER_GG_WARP_CHARACTER;
		pw.pid = p->dwFromPID;
		pw.x = ch->GetX();
		pw.y = ch->GetY();
#ifdef ENABLE_CMD_WARP_IN_DUNGEON
		pw.mapIndex = (ch->GetMapIndex() < 10000) ? 0 : ch->GetMapIndex();
#endif
		d->Packet(&pw, sizeof(pw));
	}
}

void CInputP2P::WarpCharacter(const char* c_pData)
{
	TPacketGGWarpCharacter* p = (TPacketGGWarpCharacter*) c_pData;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(p->pid);
#ifdef ENABLE_CMD_WARP_IN_DUNGEON
	if (ch)
		ch->WarpSet(p->x, p->y, p->mapIndex);
#else
	if (ch)
		ch->WarpSet(p->x, p->y);
#endif
}

void CInputP2P::GuildWarZoneMapIndex(const char* c_pData)
{
	TPacketGGGuildWarMapIndex * p = (TPacketGGGuildWarMapIndex*) c_pData;
	CGuildManager & gm = CGuildManager::instance();

	sys_log(0, "P2P: GuildWarZoneMapIndex g1(%u) vs g2(%u), mapIndex(%d)", p->dwGuildID1, p->dwGuildID2, p->lMapIndex);

	CGuild * g1 = gm.FindGuild(p->dwGuildID1);
	CGuild * g2 = gm.FindGuild(p->dwGuildID2);

	if (g1 && g2)
	{
		g1->SetGuildWarMapIndex(p->dwGuildID2, p->lMapIndex);
		g2->SetGuildWarMapIndex(p->dwGuildID1, p->lMapIndex);
	}
}

void CInputP2P::Transfer(const char * c_pData)
{
	TPacketGGTransfer * p = (TPacketGGTransfer *) c_pData;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(p->szName);

	if (ch)
		ch->WarpSet(p->lX, p->lY);
}

#ifdef NEW_SALES_SYSTEM
void CInputP2P::Sales(const char * c_pData)
{
	TPacketGGSales * p = (TPacketGGSales *) c_pData;
	CSales::instance().SalesCache();
}
#endif

#ifdef FATE_ROULETTE
void CInputP2P::FateRoulette(const char * c_pData)
{
	TPacketGGFateRoulette * p = (TPacketGGFateRoulette *) c_pData;
	CFateRoulette::instance().Initialize();
}
#endif

void CInputP2P::XmasWarpSanta(const char * c_pData)
{
	TPacketGGXmasWarpSanta * p =(TPacketGGXmasWarpSanta *) c_pData;

	if (p->bChannel == g_bChannel && map_allow_find(p->lMapIndex))
	{
		int	iNextSpawnDelay = 50 * 60;

		xmas::SpawnSanta(p->lMapIndex, iNextSpawnDelay);

		TPacketGGXmasWarpSantaReply pack_reply;
		pack_reply.bHeader = HEADER_GG_XMAS_WARP_SANTA_REPLY;
		pack_reply.bChannel = g_bChannel;
		P2P_MANAGER::instance().Send(&pack_reply, sizeof(pack_reply));
	}
}

void CInputP2P::XmasWarpSantaReply(const char* c_pData)
{
	TPacketGGXmasWarpSantaReply* p = (TPacketGGXmasWarpSantaReply*) c_pData;

	if (p->bChannel == g_bChannel)
	{
		CharacterVectorInteractor i;

		if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(xmas::MOB_SANTA_VNUM, i))
		{
			CharacterVectorInteractor::iterator it = i.begin();

			while (it != i.end()) {
				M2_DESTROY_CHARACTER(*it++);
			}
		}
	}
}

void CInputP2P::LoginPing(LPDESC d, const char * c_pData)
{
	TPacketGGLoginPing * p = (TPacketGGLoginPing *) c_pData;

	SendBillingExpire(p->szLogin, BILLING_DAY, 0, NULL);

	if (!g_pkAuthMasterDesc) // If I am master, I have to broadcast
		P2P_MANAGER::instance().Send(p, sizeof(TPacketGGLoginPing), d);
}

// BLOCK_CHAT
void CInputP2P::BlockChat(const char * c_pData)
{
	TPacketGGBlockChat * p = (TPacketGGBlockChat *) c_pData;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(p->szName);

	if (ch)
	{
		sys_log(0, "BLOCK CHAT apply name %s dur %d", p->szName, p->lBlockDuration);
		ch->AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, p->lBlockDuration, 0, true);
	}
	else
	{
		sys_log(0, "BLOCK CHAT fail name %s dur %d", p->szName, p->lBlockDuration);
	}
}
// END_OF_BLOCK_CHAT
//

void CInputP2P::PCBangUpdate(const char* c_pData)
{
	TPacketPCBangUpdate* p = (TPacketPCBangUpdate*)c_pData;

	CPCBangManager::instance().RequestUpdateIPList(p->ulPCBangID);
}

#ifdef ENABLE_MULTI_FARM_BLOCK
void CInputP2P::MultiFarm(const char* c_pData)
{
	TPacketGGMultiFarm* p = (TPacketGGMultiFarm*)c_pData;
	if(p->subHeader == MULTI_FARM_SET)
		CHARACTER_MANAGER::Instance().CheckMultiFarmAccount(p->playerIP, p->playerID, p->playerName, p->farmStatus, p->affectType, p->affectTime, true);
	else if (p->subHeader == MULTI_FARM_REMOVE)
		CHARACTER_MANAGER::Instance().RemoveMultiFarm(p->playerIP, p->playerID, true);
}
#endif

void CInputP2P::IamAwake(LPDESC d, const char * c_pData)
{
	std::string hostNames;
	P2P_MANAGER::instance().GetP2PHostNames(hostNames);
	sys_log(0, "P2P Awakeness check from %s. My P2P connection number is %d. and details...\n%s", d->GetHostName(), P2P_MANAGER::instance().GetDescCount(), hostNames.c_str());
}

#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
struct FFindOfflineShop
{
	const char* szName;
	FFindOfflineShop(const char* c_szName) : szName(c_szName) {};
	void operator()(LPENTITY ent) {
		if (!ent) return;
		if (ent->IsType(ENTITY_CHARACTER)) {
			LPCHARACTER ch = (LPCHARACTER)ent;
			if (ch->IsOfflineShopNPC() && !strcmp(szName, ch->GetName())) ch->DestroyOfflineShop();
		}
	}
};

struct FChangeOfflineShopTime
{
	int iTime;
	DWORD dwOwnerPID;

	FChangeOfflineShopTime(int time, DWORD dwOwner) : iTime(time), dwOwnerPID(dwOwner) {};

	void operator()(LPENTITY ent) {
		if (!ent) return;
		if (ent->IsType(ENTITY_CHARACTER)) {
			LPCHARACTER ch = (LPCHARACTER)ent;
			if (ch->IsOfflineShopNPC() && ch->GetOfflineShopRealOwner() == dwOwnerPID) {
				DBManager::instance().DirectQuery("UPDATE %soffline_shop_npc SET time = time + %d WHERE owner_id = %u", get_table_postfix(), iTime, dwOwnerPID);
				ch->StopOfflineShopUpdateEvent();
				ch->SetOfflineShopTimer(iTime);
				ch->StartOfflineShopUpdateEvent();
			}
		}
	}
};

void CInputP2P::RemoveOfflineShop(LPDESC d, const char* c_pData)
{
	TPacketGGRemoveOfflineShop* p = (TPacketGGRemoveOfflineShop*)c_pData;
	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(p->lMapIndex);
	if (pMap) {
		FFindOfflineShop offlineshop(p->szNpcName);
		pMap->for_each(offlineshop);
	}
}

void CInputP2P::ChangeOfflineShopTime(LPDESC d, const char* c_pData)
{
	TPacketGGChangeOfflineShopTime* p = (TPacketGGChangeOfflineShopTime*)c_pData;
	int32_t iTime = p->bTime * 60;
	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(p->lMapIndex);
	FChangeOfflineShopTime offlineShopTime(iTime, p->dwOwnerPID);
	if (pMap) pMap->for_each(offlineShopTime);
}

void CInputP2P::OfflineShopBuy(LPDESC d, const char* c_pData)
{
	return;
	//TPacketGGOfflineShopBuy* p = (TPacketGGOfflineShopBuy*)c_pData;
}

void CInputP2P::LoadOfflineShopPanelData(LPDESC d, const char* c_pData)
{
	TPacketGGOpenOffShopPanel* p = (TPacketGGOpenOffShopPanel*)c_pData;
	DWORD dwOwnerPID = p->dwOwnerPID;
	BYTE shopChannel = p->shopChannel;

	switch (p->bSubHeader) {
	case OFFLINE_SHOP_PANEL_SEARCH_SHOP: {
		DWORD offShopVID = COfflineShopManager::instance().FindMyOfflineShop(dwOwnerPID);
		if (!offShopVID) return;
		COfflineShopManager::instance().RefreshP2P(dwOwnerPID);
		int32_t shop_map_index = COfflineShopManager::instance().GetMapIndex(dwOwnerPID);
		int32_t shop_left_time = COfflineShopManager::instance().GetLeftTime(dwOwnerPID);
		const char* shopName = COfflineShopManager::instance().GetOfflineShopSign(dwOwnerPID);
		DWORD displayedCount = 0;

		TPacketGGOpenOffShopPanel sendShopData;
		sendShopData.bHeader = HEADER_GG_OFFLINE_SHOP_OPEN_PANEL;
		sendShopData.bSubHeader = OFFLINE_SHOP_PANEL_SEND_DATA;
		sendShopData.lMapIndex = shop_map_index;
		sendShopData.iTime = shop_left_time;
		sendShopData.shopChannel = shopChannel;
		sendShopData.dwOwnerPID = dwOwnerPID;
		sendShopData.displayedCount = displayedCount;
		strlcpy(sendShopData.shopName, shopName, sizeof(sendShopData.shopName));
		P2P_MANAGER::instance().Send(&sendShopData, sizeof(TPacketGGOpenOffShopPanel));
	}
									   break;
	case OFFLINE_SHOP_PANEL_SEND_DATA: {
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwOwnerPID);
		if (!ch) return;
		bool hasShop = ch->HasOfflineShop();
		if (!hasShop) {
			sys_log(0, "OFFLINE_SHOP_PANEL_SEND_DATA - Player %s searching data without offline shop!", ch->GetName());
			return;
		}

		int32_t shop_map_index = p->lMapIndex;
		int32_t shop_left_time = p->iTime;
		const char* shop_name = p->shopName;
		DWORD displayedCount = p->displayedCount;
		ch->OfflineShopPanelPacket(static_cast<BYTE>(hasShop), shop_name, shop_map_index, shopChannel, shop_left_time, displayedCount);
	}
									 break;
	default: return;
	}
}

void CInputP2P::AddOfflineShopItemData(LPDESC d, const char* c_pData)
{
	TPacketGGAddOfflineShopItem* p = (TPacketGGAddOfflineShopItem*)c_pData;
	DWORD dwOwnerPID = p->dwOwnerPID;
	DWORD offShopVID = COfflineShopManager::instance().FindMyOfflineShop(dwOwnerPID);
	if (!offShopVID) return;
	COfflineShopManager::instance().AddItemP2P(dwOwnerPID, p->itemAdd, p->pos, p->price);
}

void CInputP2P::RemoveOfflineShopItemData(LPDESC d, const char* c_pData)
{
	TPacketGGRemoveOfflineShopItem* p = (TPacketGGRemoveOfflineShopItem*)c_pData;
	DWORD dwOwnerPID = p->dwOwnerPID;

	switch (p->bSubHeader) {
	case OFFLINE_SHOP_SEND_REMOVE_ITEM: {
		DWORD offShopVID = COfflineShopManager::instance().FindMyOfflineShop(dwOwnerPID);
		if (!offShopVID) return;
		COfflineShopManager::instance().RemoveItemP2P(dwOwnerPID, p->bPos);
	}
									  break;
	case OFFLINE_SHOP_FORCE_REMOVE_ITEM: {
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwOwnerPID);
		if (!ch) return;
		COfflineShopManager::instance().GiveItemP2P(dwOwnerPID, p->item);
	}
									   break;
	default: return;
	}
}

void CInputP2P::CloseOfflineShopData(LPDESC d, const char* c_pData)
{
	TPacketGGOfflineShopClose* p = (TPacketGGOfflineShopClose*)c_pData;
	DWORD dwOwnerPID = p->dwOwnerPID;
	DWORD offShopVID = COfflineShopManager::instance().FindMyOfflineShop(dwOwnerPID);
	if (!offShopVID) return;
	LPCHARACTER npc = CHARACTER_MANAGER::instance().Find(offShopVID);
	if (!npc) return;
	if (!COfflineShopManager::instance().CanCloseOfflineShopP2P(dwOwnerPID)) return;
	COfflineShopManager::instance().DestroyOfflineShop(nullptr, npc->GetVID(), false);
}

void CInputP2P::ChangeOfflineShopName(LPDESC d, const char* c_pData)
{
	TPacketGGOfflineShopChangeName* p = (TPacketGGOfflineShopChangeName*)c_pData;
	DWORD dwOwnerPID = p->dwOwnerPID;
	const char* szShopName = p->szSign;
	DWORD offShopVID = COfflineShopManager::instance().FindMyOfflineShop(dwOwnerPID);
	if (!offShopVID) return;
	COfflineShopManager::instance().ChangeOfflineShopName(dwOwnerPID, szShopName);
}

void CInputP2P::AdviseOfflineShopSellOwner(LPDESC d, const char* c_pData)
{
	TPacketGGOfflineShopAdviseOwnerSell* p = (TPacketGGOfflineShopAdviseOwnerSell*)c_pData;
	DWORD dwOwnerPID = p->dwOwnerPID;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwOwnerPID);
	if (!ch) return;
	COfflineShopManager::instance().AdviseItemSell(ch, p->itemSelled, p->itemPrice, p->buyerName);
}

void CInputP2P::ChangeOfflineShopItemPrice(LPDESC d, const char* c_pData)
{
	TPacketGGChangeOfflineShopItemPrice* p = (TPacketGGChangeOfflineShopItemPrice*)c_pData;
	DWORD dwOwnerPID = p->dwOwnerPID;
	DWORD offShopVID = COfflineShopManager::instance().FindMyOfflineShop(dwOwnerPID);
	if (!offShopVID) return;
	COfflineShopManager::instance().ChangeOfflineShopItemPriceP2P(dwOwnerPID, p->bPos, p->llPrice);
}
#endif

int CInputP2P::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	if (test_server)
		sys_log(0, "CInputP2P::Anlayze[Header %d]", bHeader);

	int iExtraLen = 0;

	switch (bHeader)
	{
		case HEADER_GG_SETUP:
			Setup(d, c_pData);
			break;

		case HEADER_GG_LOGIN:
			Login(d, c_pData);
			break;

		case HEADER_GG_LOGOUT:
			Logout(d, c_pData);
			break;

		case HEADER_GG_RELAY:
			if ((iExtraLen = Relay(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#ifdef ENABLE_FULL_NOTICE
		case HEADER_GG_BIG_NOTICE:
			if ((iExtraLen = Notice(d, c_pData, m_iBufferLeft, true)) < 0)
				return -1;
			break;
#endif
		case HEADER_GG_NOTICE:
			if ((iExtraLen = Notice(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

#ifdef YONETICI_PM
		case HEADER_GG_BULK_WHISPER:
			if ((iExtraLen = BulkWhisperSend(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;		
#endif

		case HEADER_GG_SHUTDOWN:
			sys_err("Accept shutdown p2p command from %s.", d->GetHostName());
			Shutdown(10);
			break;

		case HEADER_GG_GUILD:
			if ((iExtraLen = Guild(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_GG_SHOUT:
			Shout(c_pData);
			break;

		case HEADER_GG_DISCONNECT:
			Disconnect(c_pData);
			break;

		case HEADER_GG_MESSENGER_ADD:
			MessengerAdd(c_pData);
			break;

		case HEADER_GG_MESSENGER_REMOVE:
			MessengerRemove(c_pData);
			break;

		case HEADER_GG_FIND_POSITION:
			FindPosition(d, c_pData);
			break;

		case HEADER_GG_WARP_CHARACTER:
			WarpCharacter(c_pData);
			break;

		case HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX:
			GuildWarZoneMapIndex(c_pData);
			break;

		case HEADER_GG_TRANSFER:
			Transfer(c_pData);
			break;

		case HEADER_GG_XMAS_WARP_SANTA:
			XmasWarpSanta(c_pData);
			break;

		case HEADER_GG_XMAS_WARP_SANTA_REPLY:
			XmasWarpSantaReply(c_pData);
			break;

		case HEADER_GG_RELOAD_CRC_LIST:
			LoadValidCRCList();
			break;

		case HEADER_GG_CHECK_CLIENT_VERSION:
			CheckClientVersion();
			break;

		case HEADER_GG_LOGIN_PING:
			LoginPing(d, c_pData);
			break;

		case HEADER_GG_BLOCK_CHAT:
			BlockChat(c_pData);
			break;

#ifdef ENABLE_REWARD_SYSTEM
		case HEADER_GG_REWARD_INFO:
		{
			TPacketGGRewardInfo* data = (TPacketGGRewardInfo*)c_pData;
			CHARACTER_MANAGER::Instance().SendRewardInfo(data->bType, true);
		}
		break;
#endif

		case HEADER_GG_SIEGE:
			{
				TPacketGGSiege* pSiege = (TPacketGGSiege*)c_pData;
				castle_siege(pSiege->bEmpire, pSiege->bTowerCount);
			}
			break;

		case HEADER_GG_MONARCH_NOTICE:
			if ((iExtraLen = MonarchNotice(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_GG_MONARCH_TRANSFER :
			MonarchTransfer(d, c_pData);
			break;

		case HEADER_GG_PCBANG_UPDATE :
			PCBangUpdate(c_pData);
			break;

#ifdef NEW_SALES_SYSTEM	
		case HEADER_GG_SALES:
			Sales(c_pData);
			break;
#endif

#ifdef FATE_ROULETTE	
		case HEADER_GG_FATE_ROULLETTE:
			FateRoulette(c_pData);
			break;
#endif

		case HEADER_GG_CHECK_AWAKENESS:
			IamAwake(d, c_pData);
			break;

#ifdef ENABLE_MULTI_FARM_BLOCK
	case HEADER_GG_MULTI_FARM:
		MultiFarm(c_pData);
		break;
#endif


#ifdef ENABLE_NEW_MISSIONS
		case HEADER_GG_NEWMISSIONS:
			NewMissions(c_pData);
			break;
		case HEADER_GG_NEWMISSIONS_NEW_PLAYER:
			NewMissionsCharacter(c_pData);
			break;
		case HEADER_GG_GLOBAL_MISSIONS:
			NewGlobalMissionComplate(c_pData);
			break;
#endif

#ifdef ENABLE_CAOS_EVENT
		case HEADER_GG_CAOS_EVENT:
			CNewCaosEventManager::instance().SendP2PRequest();
			break;
#endif

#ifdef ENABLE_SWITCHBOT
		case HEADER_GG_SWITCHBOT:
			Switchbot(d, c_pData);
			break;
#endif
#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
		case HEADER_GG_REMOVE_OFFLINE_SHOP: RemoveOfflineShop(d, c_pData); break;
		case HEADER_GG_CHANGE_OFFLINE_SHOP_TIME: ChangeOfflineShopTime(d, c_pData); break;
		case HEADER_GG_OFFLINE_SHOP_OPEN_PANEL: LoadOfflineShopPanelData(d, c_pData); break;
		case HEADER_GG_OFFLINE_SHOP_ADD_ITEM: AddOfflineShopItemData(d, c_pData); break;
		case HEADER_GG_OFFLINE_SHOP_REMOVE_ITEM: RemoveOfflineShopItemData(d, c_pData); break;
		case HEADER_GG_OFFLINE_SHOP_CLOSE_SHOP: CloseOfflineShopData(d, c_pData); break;
		case HEADER_GG_OFFLINE_SHOP_CHANGE_NAME: ChangeOfflineShopName(d, c_pData); break;
		case HEADER_GG_OFFLINE_SHOP_ADVISE_PLAYER: AdviseOfflineShopSellOwner(d, c_pData); break;
		case HEADER_GG_OFFLINE_SHOP_CHANGE_ITEM_PRICE: ChangeOfflineShopItemPrice(d, c_pData); break;
#endif
	}



	return (iExtraLen);
}
//martysama0134's 2022
#ifdef ENABLE_SWITCHBOT
#include "new_switchbot.h"
void CInputP2P::Switchbot(LPDESC d, const char* c_pData)
{
	const TPacketGGSwitchbot* p = reinterpret_cast<const TPacketGGSwitchbot*>(c_pData);
	if (p->wPort != mother_port)
	{
		return;
	}

	CSwitchbotManager::Instance().P2PReceiveSwitchbot(p->table);
}
#endif

#ifdef ENABLE_NEW_MISSIONS
void CInputP2P::NewMissions(const char * c_pData)
{
	const auto * p = reinterpret_cast<const TPacketGGNewMissions*>(c_pData);
	CNewMissions::instance().UpdateMissionData(p->dwID, p->index, p->bMissionType, p->target_vnum, p->value);
}
void CInputP2P::NewMissionsCharacter(const char * c_pData)
{
	const auto * p = reinterpret_cast<const TPacketGGNewMissionsCharacter*>(c_pData);
	CNewMissions::instance().AddPlayer(p->dwID, p->index, p->missionType);
}
void CInputP2P::NewGlobalMissionComplate(const char * c_pData)
{
	const auto * p = reinterpret_cast<const TPacketGGNewGlobalMissions*>(c_pData);
	CNewMissions::instance().UpdateGlobalMissions(p->type, p->name);
}
#endif