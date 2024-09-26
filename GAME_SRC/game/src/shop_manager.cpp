#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "desc_client.h"
#include "shop_manager.h"
#include "group_text_parse_tree.h"
#include "shopEx.h"
#include <boost/algorithm/string/predicate.hpp>
#include "shop_manager.h"
#include "offlineshop_manager.h"
#include <cctype>

CShopManager::CShopManager()
{
}

CShopManager::~CShopManager()
{
	Destroy();
}

bool CShopManager::Initialize(TShopTable * table, int size)
{

	if (!m_map_pkShop.empty())
	{
		for (TShopMap::iterator it = m_map_pkShop.begin(); it != m_map_pkShop.end(); it++)
		{
			it->second->RemoveAllGuests();
		}
	}

	m_map_pkShop.clear();
	m_map_pkShopByNPCVnum.clear();

	int i;

	for (i = 0; i < size; ++i, ++table)
	{
		LPSHOP shop = M2_NEW CShop;

		if (!shop->Create(table->dwVnum, table->dwNPCVnum, table->items))
		{
			M2_DELETE(shop);
			continue;
		}

		m_map_pkShop.insert(TShopMap::value_type(table->dwVnum, shop));
		m_map_pkShopByNPCVnum.insert(TShopMap::value_type(table->dwNPCVnum, shop));
	}
	char szShopTableExFileName[256];

	snprintf(szShopTableExFileName, sizeof(szShopTableExFileName),
		"%s/shop_table_ex.txt", LocaleService_GetBasePath().c_str());

#if defined(ENABLE_RENEWAL_SHOPEX)
	return true;
#else
	return ReadShopTableEx(szShopTableExFileName);
#endif
}

void CShopManager::Destroy()
{
#if defined(ENABLE_RENEWAL_SHOPEX)
	for (TShopMap::iterator it = m_map_pkShopByNPCVnum.begin(); it != m_map_pkShopByNPCVnum.end(); ++it)
		delete it->second;
	m_map_pkShopByNPCVnum.clear();
#else
	TShopMap::iterator it = m_map_pkShop.begin();

	while (it != m_map_pkShop.end())
	{
		M2_DELETE(it->second);
		++it;
	}

	m_map_pkShop.clear();
#endif
}

LPSHOP CShopManager::Get(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShop.find(dwVnum);

	if (it == m_map_pkShop.end())
		return NULL;

	return (it->second);
}

LPSHOP CShopManager::GetByNPCVnum(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShopByNPCVnum.find(dwVnum);

	if (it == m_map_pkShopByNPCVnum.end())
		return NULL;

	return (it->second);
}




bool CShopManager::StartShopping(LPCHARACTER pkChr, LPCHARACTER pkChrShopKeeper, int iShopVnum)
{
	if (pkChr->GetShopOwner() == pkChrShopKeeper)
		return false;
	// this method is only for NPC
	if (pkChrShopKeeper->IsPC())
		return false;

	//PREVENT_TRADE_WINDOW
	if (pkChr->IsOpenSafebox() || pkChr->GetExchange() || pkChr->GetMyShop() || pkChr->IsCubeOpen())
	{
		pkChr->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다른 거래창이 열린상태에서는 상점거래를 할수 가 없습니다."));
		return false;
	}
	//END_PREVENT_TRADE_WINDOW

#if !defined(BL_REMOTE_SHOP)
	long distance = DISTANCE_APPROX(pkChr->GetX() - pkChrShopKeeper->GetX(), pkChr->GetY() - pkChrShopKeeper->GetY());

	if (distance >= SHOP_MAX_DISTANCE)
	{
		sys_log(1, "SHOP: TOO_FAR: %s distance %d", pkChr->GetName(), distance);
		return false;
	}
#endif

	LPSHOP pkShop;

	if (iShopVnum)
		pkShop = Get(iShopVnum);
	else
		pkShop = GetByNPCVnum(pkChrShopKeeper->GetRaceNum());

	if (!pkShop)
	{
		sys_log(1, "SHOP: NO SHOP");
		return false;
	}

	bool bOtherEmpire = false;

	if (pkChr->GetEmpire() != pkChrShopKeeper->GetEmpire())
		bOtherEmpire = true;

	pkShop->AddGuest(pkChr, pkChrShopKeeper->GetVID(), bOtherEmpire);
	pkChr->SetShopOwner(pkChrShopKeeper);
	sys_log(0, "SHOP: START: %s", pkChr->GetName());
	return true;
}

LPSHOP CShopManager::FindPCShop(DWORD dwVID)
{
	TShopMap::iterator it = m_map_pkShopByPC.find(dwVID);

	if (it == m_map_pkShopByPC.end())
		return NULL;

	return it->second;
}

LPSHOP CShopManager::CreatePCShop(LPCHARACTER ch, TShopItemTable * pTable, BYTE bItemCount)
{
	if (FindPCShop(ch->GetVID()))
		return NULL;

	LPSHOP pkShop = M2_NEW CShop;
	pkShop->SetPCShop(ch);
	pkShop->SetShopItems(pTable, bItemCount);

	m_map_pkShopByPC.insert(TShopMap::value_type(ch->GetVID(), pkShop));
	return pkShop;
}

void CShopManager::DestroyPCShop(LPCHARACTER ch)
{
	LPSHOP pkShop = FindPCShop(ch->GetVID());

	if (!pkShop)
		return;

	//PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	m_map_pkShopByPC.erase(ch->GetVID());
	M2_DELETE(pkShop);
}


void CShopManager::StopShopping(LPCHARACTER ch)
{
	LPSHOP shop;

	if (!(shop = ch->GetShop()))
		return;

	//PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	shop->RemoveGuest(ch);
	sys_log(0, "SHOP: END: %s", ch->GetName());
}


void CShopManager::Buy(LPCHARACTER ch, BYTE pos)
{
#ifdef ENABLE_NEWSTUFF
	if (0 != g_BuySellTimeLimitValue)
	{
		if (get_dword_time() < ch->GetLastBuySellTime()+g_BuySellTimeLimitValue)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 골드를 버릴 수 없습니다."));
			return;
		}
	}

	ch->SetLastBuySellTime(get_dword_time());
#endif
	if (!ch->GetShop())
		return;

	if (!ch->GetShopOwner())
		return;

#if !defined(BL_REMOTE_SHOP)
	if (DISTANCE_APPROX(ch->GetX() - ch->GetShopOwner()->GetX(), ch->GetY() - ch->GetShopOwner()->GetY()) > 2000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("≫уБ?°ъАЗ °Её®°? ?К№≪ ёЦ?о №°°ЗА≫ ≫м ?ц ?ш?А?П?Щ."));
		return;
	}
#endif

	CShop* pkShop = ch->GetShop();

	if (!pkShop->IsPCShop())
	{
		//if (pkShop->GetVnum() == 0)
		//	return;
		//const CMob* pkMob = CMobManager::instance().Get(pkShop->GetNPCVnum());
		//if (!pkMob)
		//	return;

		//if (pkMob->m_table.bType != CHAR_TYPE_NPC)
		//{
		//	return;
		//}
	}
	else
	{
	}

	//PREVENT_ITEM_COPY
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	int ret = pkShop->Buy(ch, pos);

	if (SHOP_SUBHEADER_GC_OK != ret)
	{
		TPacketGCShop pack;

		pack.header	= HEADER_GC_SHOP;
		pack.subheader	= ret;
		pack.size	= sizeof(TPacketGCShop);

		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}
}

void CShopManager::Sell(LPCHARACTER ch, BYTE bCell, BYTE bCount)
{
#ifdef ENABLE_NEWSTUFF
	if (0 != g_BuySellTimeLimitValue)
	{
		if (get_dword_time() < ch->GetLastBuySellTime()+g_BuySellTimeLimitValue)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 골드를 버릴 수 없습니다."));
			return;
		}
	}

	ch->SetLastBuySellTime(get_dword_time());
#endif
	if (!ch->GetShop())
		return;

	if (!ch->GetShopOwner())
		return;

	if (!ch->CanHandleItem())
		return;

	if (ch->GetShop()->IsPCShop())
		return;

#if !defined(BL_REMOTE_SHOP)
	if (DISTANCE_APPROX(ch->GetX() - ch->GetShopOwner()->GetX(), ch->GetY() - ch->GetShopOwner()->GetY()) > 2000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("≫уБ?°ъАЗ °Её®°? ?К№≪ ёЦ?о №°°ЗА≫ ЖИ ?ц ?ш?А?П?Щ."));
		return;
	}
#endif

	LPITEM item = ch->GetInventoryItem(bCell);

	if (!item)
		return;

	if (item->IsEquipped() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("착용 중인 아이템은 판매할 수 없습니다."));
		return;
	}

	if (true == item->isLocked())
	{
		return;
	}
#ifdef ENABLE_SOULBIND_SYSTEM
	if(item->IsSealed()){
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Can't sell sealed item."));
		return;
	}
#endif
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SELL))
		return;

	long long dwPrice;

	if (bCount == 0 || bCount > item->GetCount())
		bCount = item->GetCount();

	dwPrice = item->GetShopBuyPrice();

	if (IS_SET(item->GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (dwPrice == 0)
			dwPrice = bCount;
		else
			dwPrice = bCount / dwPrice;
	}
	else
		dwPrice *= bCount;

	dwPrice /= 5;


	DWORD dwTax = 0;
	int iVal = 3;

	{
		dwTax = dwPrice * iVal/100;
		dwPrice -= dwTax;
	}

	if (test_server)
		sys_log(0, "Sell Item price id %d %s itemid %d", ch->GetPlayerID(), ch->GetName(), item->GetID());

	const long long nTotalMoney = static_cast<long long>(ch->GetGold()) + static_cast<long long>(dwPrice);

	if (GOLD_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] id %u name %s gold %lld", ch->GetPlayerID(), ch->GetName(), ch->GetGold());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("20??? ???? ??? ?? ????."));
		return;
	}

	sys_log(0, "SHOP: SELL: %s item name: %s(x%d):%u price: %lld", ch->GetName(), item->GetName(), bCount, item->GetID(), dwPrice);

#ifdef ENABLE_EXTENDED_BATTLE_PASS
	ch->UpdateExtBattlePassMissionProgress(BP_ITEM_SELL, bCount, item->GetVnum());
#endif

	if (iVal > 0)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("판매금액의 %d %% 가 세금으로 나가게됩니다"), iVal);

	DBManager::instance().SendMoneyLog(MONEY_LOG_SHOP, item->GetVnum(), dwPrice);

#ifdef ENABLE_REWARD_SYSTEM
	CHARACTER_MANAGER::Instance().DoReward(ch, REWARD_MISSION_SELL_ITEM, item->GetVnum(), bCount);
#endif
	if (bCount == item->GetCount())
		ITEM_MANAGER::instance().RemoveItem(item, "SELL");
	else
		item->SetCount(item->GetCount() - bCount);


	CMonarch::instance().SendtoDBAddMoney(dwTax, ch->GetEmpire(), ch);

	ch->PointChange(POINT_GOLD, dwPrice, false);
}

bool CompareShopItemName(const SShopItemTable& lhs, const SShopItemTable& rhs)
{
	TItemTable* lItem = ITEM_MANAGER::instance().GetTable(lhs.vnum);
	TItemTable* rItem = ITEM_MANAGER::instance().GetTable(rhs.vnum);
	if (lItem && rItem)
		return strcmp(lItem->szLocaleName, rItem->szLocaleName) < 0;
	else
		return true;
}

bool ConvertToShopItemTable(IN CGroupNode* pNode, OUT TShopTableEx& shopTable)
{
	if (!pNode->GetValue("vnum", 0, shopTable.dwVnum))
	{
		sys_err("Group %s does not have vnum.", pNode->GetNodeName().c_str());
		return false;
	}

	if (!pNode->GetValue("name", 0, shopTable.name))
	{
		sys_err("Group %s does not have name.", pNode->GetNodeName().c_str());
		return false;
	}

	if (shopTable.name.length() >= SHOP_TAB_NAME_MAX)
	{
		sys_err("Shop name length must be less than %d. Error in Group %s, name %s", SHOP_TAB_NAME_MAX, pNode->GetNodeName().c_str(), shopTable.name.c_str());
		return false;
	}

	std::string stCoinType;
	if (!pNode->GetValue("cointype", 0, stCoinType))
	{
		stCoinType = "Gold";
	}

	if (boost::iequals(stCoinType, "Gold"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_GOLD;
	}
	else if (boost::iequals(stCoinType, "SecondaryCoin"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_SECONDARY_COIN;
	}
	else
	{
		sys_err("Group %s has undefine cointype(%s).", pNode->GetNodeName().c_str(), stCoinType.c_str());
		return false;
	}

	CGroupNode* pItemGroup = pNode->GetChildNode("items");
	if (!pItemGroup)
	{
		sys_err("Group %s does not have 'group items'.", pNode->GetNodeName().c_str());
		return false;
	}

	int itemGroupSize = pItemGroup->GetRowCount();
	std::vector <TShopItemTable> shopItems(itemGroupSize);
	if (itemGroupSize >= SHOP_HOST_ITEM_MAX_NUM)
	{
		sys_err("count(%d) of rows of group items of group %s must be smaller than %d", itemGroupSize, pNode->GetNodeName().c_str(), SHOP_HOST_ITEM_MAX_NUM);
		return false;
	}

	for (int i = 0; i < itemGroupSize; i++)
	{
		if (!pItemGroup->GetValue(i, "vnum", shopItems[i].vnum))
		{
			sys_err("row(%d) of group items of group %s does not have vnum column", i, pNode->GetNodeName().c_str());
			return false;
		}

		if (!pItemGroup->GetValue(i, "count", shopItems[i].count))
		{
			sys_err("row(%d) of group items of group %s does not have count column", i, pNode->GetNodeName().c_str());
			return false;
		}
		if (!pItemGroup->GetValue(i, "price", shopItems[i].price))
		{
			sys_err("row(%d) of group items of group %s does not have price column", i, pNode->GetNodeName().c_str());
			return false;
		}
	}
	std::string stSort;
	if (!pNode->GetValue("sort", 0, stSort))
	{
		stSort = "None";
	}

	if (boost::iequals(stSort, "Asc"))
	{
		std::sort(shopItems.begin(), shopItems.end(), CompareShopItemName);
	}
	else if(boost::iequals(stSort, "Desc"))
	{
		std::sort(shopItems.rbegin(), shopItems.rend(), CompareShopItemName);
	}

	CGrid grid = CGrid(5, 9);
	int iPos;

	msl::refill(shopTable.items);

	for (size_t i = 0; i < shopItems.size(); i++)
	{
		TItemTable * item_table = ITEM_MANAGER::instance().GetTable(shopItems[i].vnum);
		if (!item_table)
		{
			sys_err("vnum(%d) of group items of group %s does not exist", shopItems[i].vnum, pNode->GetNodeName().c_str());
			return false;
		}

		iPos = grid.FindBlank(1, item_table->bSize);

		grid.Put(iPos, 1, item_table->bSize);
		shopTable.items[iPos] = shopItems[i];
	}

	shopTable.byItemCount = shopItems.size();
	return true;
}

#if defined(ENABLE_RENEWAL_SHOPEX)
bool CShopManager::InitializeShopEX(TShopTable* table, int size)
{
	typedef std::multimap <DWORD, TShopTableEx> TMapNPCshop;
	TMapNPCshop map_npcShop;

	for (int i = 0; i < size; ++i, ++table)
	{
		TShopTableEx shopTable;
		shopTable.dwVnum = table->dwVnum;
		shopTable.name = table->szShopName;
		shopTable.coinType = SHOP_COIN_TYPE_GOLD;

		if (shopTable.name.length() >= SHOP_TAB_NAME_MAX) {
			sys_err("Shop name length must be less than %d. Error %s", SHOP_TAB_NAME_MAX, shopTable.name.c_str());
			return false;
		}

		CGrid grid(5, 9);

		memset(&shopTable.items[0], 0, sizeof(shopTable.items));

		for (size_t j = 0; j < table->byItemCount; j++)
		{
			TItemTable* item_table = ITEM_MANAGER::instance().GetTable(table->items[j].vnum);
			if (!item_table) {
				sys_err("vnum(%d) of group items of group %s does not exist.", table->items[j].vnum, shopTable.name.c_str());
				return false;
			}

			int iPos = grid.FindBlank(1, item_table->bSize);
			if (iPos == -1) {
				sys_err("vnum(%d) of group items of group %s there is no space!", table->items[j].vnum, shopTable.name.c_str());
				return false;
			}

			grid.Put(iPos, 1, item_table->bSize);
			shopTable.items[iPos] = table->items[j];
			shopTable.byItemCount++;
		}
		if (m_map_pkShopByNPCVnum.find(table->dwNPCVnum) != m_map_pkShopByNPCVnum.end()) {
			sys_err("NPCVNUM(%d) already used.", table->dwNPCVnum);
			return false;
		}

		map_npcShop.insert(TMapNPCshop::value_type(table->dwNPCVnum, shopTable));
	}

	for (TMapNPCshop::iterator it = map_npcShop.begin(); it != map_npcShop.end(); ++it)
	{
		const DWORD npcVnum = it->first;
		TShopTableEx& table = it->second;

		if (m_map_pkShop.find(table.dwVnum) != m_map_pkShop.end()) {
			sys_err("Shop vnum(%d) already exists.", table.dwVnum);
			return false;
		}

		TShopMap::const_iterator shop_it = m_map_pkShopByNPCVnum.find(npcVnum);
		LPSHOPEX pkShopEx = NULL;

		if (m_map_pkShopByNPCVnum.end() == shop_it) {
			pkShopEx = new CShopEx;
			pkShopEx->Create(0, npcVnum);
			m_map_pkShopByNPCVnum.insert(TShopMap::value_type(npcVnum, pkShopEx));
		}
		else {
			pkShopEx = dynamic_cast <CShopEx*> (shop_it->second);
			if (!pkShopEx) {
				sys_err("NPC(%d) Shop is not extended version.", shop_it->first);
				return false;
			}
		}

		if (pkShopEx->GetTabCount() >= SHOP_TAB_COUNT_MAX) {
			sys_err("ShopEx cannot have tab more than %d.", SHOP_TAB_COUNT_MAX);
			delete pkShopEx;
			return false;
		}

		pkShopEx->AddShopTable(table);
		m_map_pkShop.insert(TShopMap::value_type(table.dwVnum, pkShopEx));
	}

	return true;
}
#endif

bool CShopManager::ReadShopTableEx(const char* stFileName)
{


	FILE* fp = fopen(stFileName, "rb");
	if (NULL == fp)
		return true;
	fclose(fp);

	CGroupTextParseTreeLoader loader;
	if (!loader.Load(stFileName))
	{
		sys_err("%s Load fail.", stFileName);
		return false;
	}

	CGroupNode* pShopNPCGroup = loader.GetGroup("shopnpc");
	if (NULL == pShopNPCGroup)
	{
		sys_err("Group ShopNPC is not exist.");
		return false;
	}

	typedef std::multimap <DWORD, TShopTableEx> TMapNPCshop;
	TMapNPCshop map_npcShop;
	for (int i = 0; i < pShopNPCGroup->GetRowCount(); i++)
	{
		DWORD npcVnum;
		std::string shopName;
		if (!pShopNPCGroup->GetValue(i, "npc", npcVnum) || !pShopNPCGroup->GetValue(i, "group", shopName))
		{
			sys_err("Invalid row(%d). Group ShopNPC rows must have 'npc', 'group' columns", i);
			return false;
		}
		std::transform(shopName.begin(), shopName.end(), shopName.begin(), (int(*)(int))std::tolower);
		CGroupNode* pShopGroup = loader.GetGroup(shopName.c_str());
		if (!pShopGroup)
		{
			sys_err("Group %s is not exist.", shopName.c_str());
			return false;
		}
		TShopTableEx table;
		if (!ConvertToShopItemTable(pShopGroup, table))
		{
			sys_err("Cannot read Group %s.", shopName.c_str());
			return false;
		}
		if (m_map_pkShopByNPCVnum.find(npcVnum) != m_map_pkShopByNPCVnum.end())
		{
			sys_err("%d cannot have both original shop and extended shop", npcVnum);
			return false;
		}

		map_npcShop.insert(TMapNPCshop::value_type(npcVnum, table));
	}

	for (TMapNPCshop::iterator it = map_npcShop.begin(); it != map_npcShop.end(); ++it)
	{
		DWORD npcVnum = it->first;
		TShopTableEx& table = it->second;
		if (m_map_pkShop.find(table.dwVnum) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exists", table.dwVnum);
			return false;
		}
		TShopMap::iterator shop_it = m_map_pkShopByNPCVnum.find(npcVnum);

		LPSHOPEX pkShopEx = NULL;
		if (m_map_pkShopByNPCVnum.end() == shop_it)
		{
			pkShopEx = M2_NEW CShopEx;
			pkShopEx->Create(0, npcVnum);
			m_map_pkShopByNPCVnum.insert(TShopMap::value_type(npcVnum, pkShopEx));
		}
		else
		{
			pkShopEx = dynamic_cast <CShopEx*> (shop_it->second);
			if (NULL == pkShopEx)
			{
				sys_err("WTF!!! It can't be happend. NPC(%d) Shop is not extended version.", shop_it->first);
				return false;
			}
		}

		if (pkShopEx->GetTabCount() >= SHOP_TAB_COUNT_MAX)
		{
			sys_err("ShopEx cannot have tab more than %d", SHOP_TAB_COUNT_MAX);
			return false;
		}

		if (pkShopEx->GetVnum() != 0 && m_map_pkShop.find(pkShopEx->GetVnum()) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exist.", pkShopEx->GetVnum());
			return false;
		}
		m_map_pkShop.insert(TShopMap::value_type (pkShopEx->GetVnum(), pkShopEx));
		pkShopEx->AddShopTable(table);
	}

	return true;
}

#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
void CShopManager::ShopSearchProcess(LPCHARACTER ch, const TPacketCGPrivateShopSearch* p)
{
	if (ch == NULL || ch->GetDesc() == NULL || p == NULL)
		return;

	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_CLOSE_TABS"));
		return;
	}

	TEMP_BUFFER buf;

	if (strlen(p->szItemName) != 0) {
		if (strcasecmp(p->szItemName, "'") == 0) { return; }
		if (strcasecmp(p->szItemName, "%") == 0) { return; }
		if (strcasecmp(p->szItemName, "\\") == 0) { return; }
		if (strcasecmp(p->szItemName, "?") == 0) { return; }
		if (strcasecmp(p->szItemName, "!") == 0) { return; }
		if (strcasecmp(p->szItemName, "[") == 0) { return; }
		if (strcasecmp(p->szItemName, "`") == 0) { return; }
		if (strcasecmp(p->szItemName, "like") == 0) { return; }
		if (strcasecmp(p->szItemName, "drop") == 0) { return; }
		if (strcasecmp(p->szItemName, "delete") == 0) { return; }
		if (strcasecmp(p->szItemName, "truncate") == 0) { return; }
	}
	
	char szQuery[QUERY_MAX_LEN] = "";
	char szWhere[QUERY_MAX_LEN] = "";
	{ snprintf(szWhere, sizeof(szWhere), " where (offi.status=0)"); }
	if (strlen(p->szItemName) != 0) snprintf(szWhere, sizeof(szWhere), "%s and (CAST(ip.locale_name AS CHAR(32) CHARSET Latin5) like '%%%%%s%%%%')", szWhere, p->szItemName);

	if (p->iMinGold > 0) { snprintf(szWhere, sizeof(szWhere), "%s and (offi.price >= %d)", szWhere, p->iMinGold); }
	if (p->iMaxGold > 0) { snprintf(szWhere, sizeof(szWhere), "%s and (offi.price <= %d)", szWhere, p->iMaxGold); }
	if (p->bMaskType != ITEM_NONE) { snprintf(szWhere, sizeof(szWhere), "%s and (ip.type=%d)", szWhere, p->bMaskType); }
	if (p->iMaskSub != -1) { snprintf(szWhere, sizeof(szWhere), "%s and (ip.subtype=%d)", szWhere, p->iMaskSub); }

	switch (p->bJob)
	{
		case JOB_WARRIOR: snprintf(szWhere, sizeof(szWhere), "%s and (ip.antiflag & %d) = 0", szWhere, ITEM_ANTIFLAG_WARRIOR); break;
		case JOB_ASSASSIN: snprintf(szWhere, sizeof(szWhere), "%s and (ip.antiflag & %d) = 0", szWhere, ITEM_ANTIFLAG_ASSASSIN); break;
		case JOB_SHAMAN: snprintf(szWhere, sizeof(szWhere), "%s and (ip.antiflag & %d) = 0", szWhere, ITEM_ANTIFLAG_SHAMAN); break;
		case JOB_SURA: snprintf(szWhere, sizeof(szWhere), "%s and (ip.antiflag & %d) = 0", szWhere, ITEM_ANTIFLAG_SURA); break;
#if defined(ENABLE_WOLFMAN_CHARACTER)
		case JOB_WOLFMAN: snprintf(szWhere, sizeof(szWhere), "%s and (ip.antiflag & %d) = 0", szWhere, ITEM_ANTIFLAG_WOLFMAN); break;
#endif
		default: break;
	}
	{
		snprintf(szWhere, sizeof(szWhere), "%s and (offnpc.mapIndex=%ld)", szWhere, ch->GetMapIndex());
		snprintf(szWhere, sizeof(szWhere), "%s and (offnpc.channel=%d)", szWhere, g_bChannel);
	}

	const uint16_t maxItemResult = 700;
	{
		snprintf(szQuery, sizeof(szQuery), "select offi.owner_id, offi.id, REPLACE(IFNULL(offnpc.name, 'Pazar Yok'),' ','_') as npc, ip.locale_name, offi.vnum, offi.count , offi.price,"
			" offi.socket0, offi.socket1, offi.socket2, offi.socket3,"
			" offi.attrtype0, offi.attrvalue0, offi.attrtype1, offi.attrvalue1, offi.attrtype2, offi.attrvalue2, offi.attrtype3, offi.attrvalue3, offi.attrtype4, offi.attrvalue4, offi.attrtype5, offi.attrvalue5, offi.attrtype6, offi.attrvalue6,"
			" offi.pos,"
			" offnpc.mapIndex, offnpc.channel, offnpc.time, offnpc.x, offnpc.y"
			" from player.offline_shop_item as offi"
			" left JOIN player.offline_shop_npc as offnpc on offnpc.owner_id = offi.owner_id"
			" LEFT JOIN player.item_proto as ip on ip.vnum=offi.vnum"
			" %s"
			" limit 0,%d;"
			, szWhere, maxItemResult);
	}
	sys_err("shopSearch player: %s | query: %s", ch->GetName(), szQuery);
	auto pSearchQuery(DBManager::instance().DirectQuery(szQuery));
	if (pSearchQuery->uiSQLErrno != 0) { ch->ChatPacket(CHAT_TYPE_INFO, "Pazar arama hatasi: %ld", pSearchQuery->uiSQLErrno); return; }
	if (!pSearchQuery->Get()->uiNumRows) { ch->ChatPacket(CHAT_TYPE_INFO, "Sonuc yok."); return; }

	while (MYSQL_ROW row1 = mysql_fetch_row(pSearchQuery->Get()->pSQLResult)) {
		TPacketGCPrivateShopSearchItem pack2;
		pack2.dwShopPID = 0;
		DWORD dwPID = 0; str_to_number(dwPID, row1[0]);
		DWORD dwOfflineShopVID = COfflineShopManager::instance().FindMyOfflineShop(dwPID);
		if (dwOfflineShopVID) { pack2.dwShopPID = dwOfflineShopVID; }
		
		//ch->ChatPacket(CHAT_TYPE_INFO, "offVid: %d", pack2.dwShopPID);
		
		strlcpy(pack2.szSellerName, row1[2], sizeof(pack2.szSellerName));
		str_to_number(pack2.item.vnum, row1[4]);
		str_to_number(pack2.item.count, row1[5]);
		str_to_number(pack2.item.price, row1[6]);
		str_to_number(pack2.item.display_pos, row1[24]);
		uint8_t cur = 7;
		for (uint8_t x = 0; x < ITEM_SOCKET_MAX_NUM; x++) { str_to_number(pack2.item.alSockets[x], row1[cur++]); }
		for (uint8_t j = 0; j < ITEM_ATTRIBUTE_MAX_NUM; j++) {
			str_to_number(pack2.item.aAttr[j].bType, row1[cur++]);
			str_to_number(pack2.item.aAttr[j].sValue, row1[cur++]);
		}
		buf.write(&pack2, sizeof(pack2));
	}

	/*
	for (std::map<DWORD, CShop*>::const_iterator it = m_map_pkShopByPC.begin(); it != m_map_pkShopByPC.end(); ++it)
	{
		CShop* tShopTable = it->second;
		if (tShopTable == NULL)
			continue;

		LPCHARACTER GetOwner = tShopTable->GetShopOwner();
		if (GetOwner == NULL || ch == GetOwner)
			continue;

		const std::vector<CShop::SHOP_ITEM>& vItemVec = tShopTable->GetItemVector();
		for (std::vector<CShop::SHOP_ITEM>::const_iterator ShopIter = vItemVec.begin(); ShopIter != vItemVec.end(); ++ShopIter)
		{
			LPITEM item = ShopIter->pkItem;
			if (item == NULL)
				continue;

			if (strncasecmp(item->GetName(), p->szItemName, strlen(p->szItemName)))
				continue;

			if ((p->iMinRefine <= item->GetRefineLevel() && p->iMaxRefine >= item->GetRefineLevel()) == false)
				continue;

			if ((p->iMinLevel <= item->GetLevelLimit() && p->iMaxLevel >= item->GetLevelLimit()) == false)
				continue;

			if ((p->iMinGold <= ShopIter->price && p->iMaxGold >= ShopIter->price) == false)
				continue;

#if defined(ENABLE_CHEQUE_SYSTEM)
			if ((p->iMinCheque <= ShopIter->cheque_price && p->iMaxCheque >= ShopIter->cheque_price) == false)
				continue;
#endif

			if (p->bMaskType != ITEM_NONE && p->bMaskType != item->GetType()) // ITEM_NONE: All Categories
				continue;

			if (p->iMaskSub != -1 && p->iMaskSub != item->GetSubType()) // -1: No SubType Check
				continue;

			switch (p->bJob)
			{
			case JOB_WARRIOR:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
					continue;
				break;

			case JOB_ASSASSIN:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
					continue;
				break;

			case JOB_SHAMAN:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
					continue;
				break;

			case JOB_SURA:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
					continue;
				break;

#if defined(ENABLE_WOLFMAN_CHARACTER)
			case JOB_WOLFMAN:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
					continue;
				break;
#endif
			}

			TPacketGCPrivateShopSearchItem pack2;
			pack2.item.vnum = ShopIter->vnum;
			pack2.item.price = ShopIter->price;
			pack2.item.count = ShopIter->count;
#if defined(ENABLE_CHEQUE_SYSTEM)
			pack2.item.byChequePrice = ShopIter->cheque_price;
#endif
			pack2.item.display_pos = static_cast<BYTE>(std::distance(vItemVec.begin(), ShopIter));
			pack2.dwShopPID = GetOwner->GetPlayerID();
			std::memcpy(&pack2.szSellerName, GetOwner->GetName(), sizeof(pack2.szSellerName));
			std::memcpy(&pack2.item.alSockets, item->GetSockets(), sizeof(pack2.item.alSockets));
			std::memcpy(&pack2.item.aAttr, item->GetAttributes(), sizeof(pack2.item.aAttr));
#if defined(__BL_TRANSMUTATION__)
			pack2.item.dwTransmutationVnum = item->GetTransmutationVnum();
#endif
			buf.write(&pack2, sizeof(pack2));
		}
	}
	*/

	if (buf.size() <= 0)
		return;

	TPacketGCPrivateShopSearch pack;
	pack.header = HEADER_GC_PRIVATE_SHOP_SEARCH;
	pack.size = static_cast<WORD>(sizeof(pack) + buf.size());
	ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

#include "unique_item.h"
#include "target.h"
void CShopManager::ShopSearchBuy(LPCHARACTER ch, const TPacketCGPrivateShopSearchBuyItem* p)
{
	if (ch == NULL || ch->GetDesc() == NULL || p == NULL)
		return;

	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_CLOSE_TABS"));
		return;
	}

	LPCHARACTER ShopCH = CHARACTER_MANAGER::instance().FindByPID(p->dwShopPID);
	if (ShopCH == NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_NO_SHOP"));
		return;
	}

	if (ch == ShopCH) // what?
		return;

	CShop* pkShop = ShopCH->GetMyShop();
	if (pkShop == NULL || pkShop->IsPCShop() == false)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_NO_SHOP"));
		return;
	}

	const BYTE bState = ch->GetPrivateShopSearchState();
	switch (bState)
	{
	case SHOP_SEARCH_LOOKING:
	{
		if (ch->CountSpecifyItem(PRIVATE_SHOP_SEARCH_LOOKING_GLASS) == 0)
		{
			const TItemTable* GlassTable = ITEM_MANAGER::instance().GetTable(PRIVATE_SHOP_SEARCH_LOOKING_GLASS);
			if (GlassTable)
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_WHERE_IS_ITEM"), GlassTable->szLocaleName);
			return;
		}
		if (ch->GetMapIndex() != ShopCH->GetMapIndex())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_SAMEMAP_ERR"));
			return;
		}

		const DWORD dwSellerVID(ShopCH->GetVID());
		if (CTargetManager::instance().GetTargetInfo(ch->GetPlayerID(), TARGET_TYPE_VID_SHOP_SEARCH, dwSellerVID))
			CTargetManager::instance().DeleteTarget(ch->GetPlayerID(), SHOP_SEARCH_INDEX, "__SHOPSEARCH_TARGET__");

		CTargetManager::Instance().CreateTarget(ch->GetPlayerID(), SHOP_SEARCH_INDEX, "__SHOPSEARCH_TARGET__", TARGET_TYPE_VID_SHOP_SEARCH, dwSellerVID, 0, ch->GetMapIndex(), "Shop Search", 1);

		if (CTargetManager::instance().GetTargetInfo(ch->GetPlayerID(), TARGET_TYPE_VID_SHOP_SEARCH, dwSellerVID))
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_SUCCESS_TARGET"));
		break;
	}

	case SHOP_SEARCH_TRADING:
	{
		if (ch->CountSpecifyItem(PRIVATE_SHOP_SEARCH_TRADING_GLASS) == 0)
		{
			const TItemTable* GlassTable = ITEM_MANAGER::instance().GetTable(PRIVATE_SHOP_SEARCH_TRADING_GLASS);
			if (GlassTable)
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_WHERE_IS_ITEM"), GlassTable->szLocaleName);
			return;
		}

		ch->SetMyShopTime();
		int ret = pkShop->Buy(ch, p->bPos, true);

		if (SHOP_SUBHEADER_GC_OK != ret)
		{
			TPacketGCShop pack;
			pack.header = HEADER_GC_SHOP;
			pack.subheader = static_cast<BYTE>(ret);
			pack.size = sizeof(TPacketGCShop);
			ch->GetDesc()->Packet(&pack, sizeof(pack));
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_OK"));

		break;
	}
	default:
		sys_err("ShopSearchBuy ch(%s) wrong state(%d)", ch->GetName(), bState);
		break;
	}
}
#endif
//martysama0134's 2022
