/*
	* Filename : offline_shop.cpp
	* Version : 0.1
	* Description : --
*/

#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
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
#include "offline_shop.h"
#include "p2p.h"
#include "offlineshop_manager.h"
#include "desc_client.h"
#include "target.h"

COfflineShop::COfflineShop() : m_pkOfflineShopNPC(nullptr), m_pkOfflineShopBorderStyle(0), m_llMapIndex(0), m_iTime(0){}

COfflineShop::~COfflineShop()
{
	TPacketGCShop pack;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_END;
	pack.size = sizeof(TPacketGCShop);

	Broadcast(&pack, sizeof(pack));

	for (auto& idx : m_map_guest) {
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(idx);
		if (ch) ch->SetOfflineShop(nullptr);
	}
}

void COfflineShop::SetOfflineShopNPC(LPCHARACTER npc)
{
	m_pkOfflineShopNPC = npc;
}

void COfflineShop::SetOfflineShopItems(DWORD dwOwnerPID, TOfflineShopItemTable* pTable, BYTE bItemCount)
{
	if (bItemCount > OFFLINE_SHOP_HOST_ITEM_MAX_NUM) return;

	for (uint8_t i = 0; i < bItemCount; ++i) {
		const TItemTable* item_table;
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwOwnerPID);
		LPITEM pkItem = ch->GetItem(pTable->pos);
		if (!pkItem) continue;
		if (!pkItem->GetVnum()) continue;
		item_table = ITEM_MANAGER::instance().GetTable(pkItem->GetVnum());
		if (!item_table) { sys_err("OfflineShop: no item table by item vnum #%d", pkItem->GetVnum()); continue; }
		DBManager::instance().Query("INSERT INTO %soffline_shop_item ("
			"id, owner_id, pos, count"
			", price"
			", vnum"
			", socket0, socket1, socket2, socket3"
			", attrtype0, attrvalue0"
			", attrtype1, attrvalue1"
			", attrtype2, attrvalue2"
			", attrtype3, attrvalue3"
			", attrtype4, attrvalue4"
			", attrtype5, attrvalue5"
			", attrtype6, attrvalue6"
			") VALUES ("
			"%u, %u, %d, %u" // id, owner_id, pos, count
			", %lld" // price
			", %u" // vnum
			", %ld, %ld, %ld, %ld" // socket0, socket1, socket2, socket3
			", %d, %d" // attrtype0, attrvalue0
			", %d, %d" // attrtype1, attrvalue1
			", %d, %d" // attrtype2, attrvalue2
			", %d, %d" // attrtype3, attrvalue3
			", %d, %d" // attrtype4, attrvalue4
			", %d, %d" // attrtype5, attrvalue5
			", %d, %d" // attrtype6, attrvalue6
			")", get_table_postfix(),
			pkItem->GetID(), ch->GetPlayerID(), pTable->display_pos, pkItem->GetCount()
			, pTable->price
			, pkItem->GetVnum()
			, pkItem->GetSocket(0)
			, pkItem->GetSocket(1)
			, pkItem->GetSocket(2)
			, pkItem->GetSocket(3)
			, pkItem->GetAttributeType(0), pkItem->GetAttributeValue(0)
			, pkItem->GetAttributeType(1), pkItem->GetAttributeValue(1)
			, pkItem->GetAttributeType(2), pkItem->GetAttributeValue(2)
			, pkItem->GetAttributeType(3), pkItem->GetAttributeValue(3)
			, pkItem->GetAttributeType(4), pkItem->GetAttributeValue(4)
			, pkItem->GetAttributeType(5), pkItem->GetAttributeValue(5)
			, pkItem->GetAttributeType(6), pkItem->GetAttributeValue(6)
		);
		ITEM_MANAGER::instance().RemoveItem(pkItem);
		++pTable;
	}
}

void COfflineShop::AddItem(LPCHARACTER ch, LPITEM pkItem, BYTE bPos, long long iPrice)
{
	DBManager::instance().Query("INSERT INTO %soffline_shop_item ("
		"id, owner_id, pos, count"
		", price"
		", vnum"
		", socket0, socket1, socket2, socket3"
		", attrtype0, attrvalue0"
		", attrtype1, attrvalue1"
		", attrtype2, attrvalue2"
		", attrtype3, attrvalue3"
		", attrtype4, attrvalue4"
		", attrtype5, attrvalue5"
		", attrtype6, attrvalue6"
		") VALUES ("
		"%u, %u, %d, %u" // id, owner_id, pos, count
		", %lld" // price
		", %u" // vnum
		", %ld, %ld, %ld, %ld" // socket0, socket1, socket2
		", %d, %d" // attrtype0, attrvalue0
		", %d, %d" // attrtype1, attrvalue1
		", %d, %d" // attrtype2, attrvalue2
		", %d, %d" // attrtype3, attrvalue3
		", %d, %d" // attrtype4, attrvalue4
		", %d, %d" // attrtype5, attrvalue5
		", %d, %d" // attrtype6, attrvalue6
		")", get_table_postfix(),
		pkItem->GetID(), ch->GetPlayerID(), bPos, pkItem->GetCount()
		, iPrice
		, pkItem->GetVnum()
		, pkItem->GetSocket(0)
		, pkItem->GetSocket(1)
		, pkItem->GetSocket(2)
		, pkItem->GetSocket(3)
		, pkItem->GetAttributeType(0), pkItem->GetAttributeValue(0)
		, pkItem->GetAttributeType(1), pkItem->GetAttributeValue(1)
		, pkItem->GetAttributeType(2), pkItem->GetAttributeValue(2)
		, pkItem->GetAttributeType(3), pkItem->GetAttributeValue(3)
		, pkItem->GetAttributeType(4), pkItem->GetAttributeValue(4)
		, pkItem->GetAttributeType(5), pkItem->GetAttributeValue(5)
		, pkItem->GetAttributeType(6), pkItem->GetAttributeValue(6)
	);

	pkItem->RemoveFromCharacter();
	M2_DESTROY_ITEM(pkItem);
	ch->Save();
	Refresh(ch);
}

void COfflineShop::RemoveItem(LPCHARACTER ch, BYTE bPos)
{
	if (!ch) { return; }
	if (bPos >= OFFLINE_SHOP_HOST_ITEM_MAX_NUM) { sys_log(0, "COfflineShop::RemoveItem - Overflow slot! [%s]", ch->GetName()); return; }

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),
		"SELECT id, pos, count, vnum"
		", socket0, socket1, socket2, socket3"
		", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6"
		" FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos);
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows == 0) { Refresh(ch); return; }

	TPlayerItem item;
	for (my_ulonglong i = 0; i < mysql_num_rows(pMsg->Get()->pSQLResult); ++i) {
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		int32_t cur = 0;
		str_to_number(item.id, row[cur++]);
		str_to_number(item.pos, row[cur++]);
		str_to_number(item.count, row[cur++]);
		str_to_number(item.vnum, row[cur++]);
		for (uint8_t x = 0; x < ITEM_SOCKET_MAX_NUM; x++) str_to_number(item.alSockets[x], row[cur++]);
		for (uint8_t j = 0; j < ITEM_ATTRIBUTE_MAX_NUM; j++) {
			str_to_number(item.aAttr[j].bType, row[cur++]);
			str_to_number(item.aAttr[j].sValue, row[cur++]);
		}
	}

	LPITEM pItem = ITEM_MANAGER::instance().CreateItem(item.vnum, item.count, item.id);
	if (!pItem) { ch->ChatPacket(CHAT_TYPE_INFO, "[LS:3589]"); return; }

	pItem->SetAttributes(item.aAttr);
	pItem->SetSockets(item.alSockets);

	const int32_t iEmptyPos = pItem->IsDragonSoul() ? ch->GetEmptyDragonSoulInventory(pItem) : ch->GetEmptyInventory(pItem->GetSize());
	if (iEmptyPos < 0) { ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지품에 빈 공간이 없습니다.")); M2_DESTROY_ITEM(pItem); return; }
	DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), bPos);
	if (pItem->IsDragonSoul()) pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
	else {
		if (!ch->exInven_PutOn(pItem, true)) pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
	}
	ITEM_MANAGER::instance().FlushDelayedSave(pItem);
	ch->Save();
	BroadcastUpdateItem(bPos, ch->GetPlayerID(), true);
}

bool COfflineShop::AddGuest(LPCHARACTER ch, LPCHARACTER npc)
{
	if (!ch || (ch && !ch->GetDesc())) return false;
	if (ch && (ch->HaveAnotherPagesOpen())) {
		ch->ChatPacket(CHAT_TYPE_INFO, "Acik olan pencereleri kapat ve tekrar dene.");
		return false;
	}

	ch->SetOfflineShop(this);
	DWORD dwPID = ch->GetPlayerID();
	m_map_guest.emplace_back(dwPID);

	TPacketGCShop pack;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_START;

	TPacketGCOfflineShopStart pack2;
	memset(&pack2, 0, sizeof(pack2));
	pack2.owner_vid = npc->GetVID();

	if (GetLeftItemCount(npc->GetOfflineShopRealOwner()) == 0) {
		DBManager::instance().DirectQuery("DELETE FROM player.offline_shop_npc WHERE owner_id = %u", npc->GetOfflineShopRealOwner());
		DBManager::instance().DirectQuery("UPDATE %soffline_shop_item SET status = 1 WHERE owner_id = %u and status = 0", get_table_postfix(), npc->GetOfflineShopRealOwner());
		ch->SetOfflineShop(nullptr);
		ch->SetOfflineShopOwner(nullptr);
		M2_DESTROY_CHARACTER(npc);
		return false;
	}
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),
		"SELECT pos, count, vnum"
		", price"
		", socket0,socket1,socket2,socket3"
		", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6"
		" FROM %soffline_shop_item WHERE owner_id = %u and status = 0", get_table_postfix(), npc->GetOfflineShopRealOwner()
	);

	auto pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->uiSQLErrno != 0) { ch->SetOfflineShop(nullptr); return false; }

	for (my_ulonglong i = 0; i < mysql_num_rows(pMsg->Get()->pSQLResult); ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		BYTE cur = 0;
		BYTE bPos = 0;
		str_to_number(bPos, row[cur++]);	/*0*/
		if (bPos >= OFFLINE_SHOP_HOST_ITEM_MAX_NUM) { continue; }

		str_to_number(pack2.items[bPos].count, row[cur++]);	/*1*/
		str_to_number(pack2.items[bPos].vnum, row[cur++]);	/*2*/
		str_to_number(pack2.items[bPos].price, row[cur++]);	/*4*/
		for (BYTE b = 0; b < ITEM_SOCKET_MAX_NUM; b++) str_to_number(pack2.items[bPos].alSockets[b], row[cur++]);
		for (BYTE c = 0; c < ITEM_ATTRIBUTE_MAX_NUM; c++) {
			str_to_number(pack2.items[bPos].aAttr[c].bType, row[cur++]);
			str_to_number(pack2.items[bPos].aAttr[c].sValue, row[cur++]);
		}
	}

	pack.size = sizeof(pack) + sizeof(pack2);
	if (ch && ch->GetDesc()) {
		if (!ch->GetDesc()->IsPhase(PHASE_GAME)) { ch->SetOfflineShop(nullptr); return false; }
		ch->GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
		ch->GetDesc()->Packet(&pack2, sizeof(TPacketGCOfflineShopStart));
	}

	return true;
}

void COfflineShop::SetGuestMap(LPCHARACTER ch)
{
	if (!ch) return;
	DWORD dwPID = ch->GetPlayerID();
	auto it = std::find(m_map_guest.begin(), m_map_guest.end(), dwPID);
	if (it != m_map_guest.end()) return;
	m_map_guest.emplace_back(dwPID);
}

void COfflineShop::RemoveGuestMap(LPCHARACTER ch)
{
	if (!ch) return;
	if (ch->GetOfflineShop() != this) return;
	DWORD dwPID = ch->GetPlayerID();
	auto it = std::find(m_map_guest.begin(), m_map_guest.end(), dwPID);
	if (it != m_map_guest.end()) m_map_guest.erase(it);
}

void COfflineShop::RemoveGuest(LPCHARACTER ch)
{
	if (!ch || (ch && !ch->GetDesc())) return;
	if (ch->GetOfflineShop() != this) return;
	DWORD dwPID = ch->GetPlayerID();
	auto it = std::find(m_map_guest.begin(), m_map_guest.end(), dwPID);
	if (it != m_map_guest.end()) m_map_guest.erase(it);
	ch->SetOfflineShop(NULL);

	TPacketGCShop pack;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_END;
	pack.size = sizeof(TPacketGCShop);

	if (ch->GetDesc()) ch->GetDesc()->Packet(&pack, sizeof(pack));
}

void COfflineShop::RemoveAllGuest()
{
	TPacketGCShop pack;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_END;
	pack.size = sizeof(TPacketGCShop);

	Broadcast(&pack, sizeof(pack));

	for (auto& idx : m_map_guest) {
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(idx);
		if (ch) {
			ch->SetOfflineShop(nullptr);
			ch->SetOfflineShopOwner(nullptr);
		}
	}
}

void COfflineShop::Destroy(LPCHARACTER npc)
{
	RemoveAllGuest();
	if (npc) M2_DESTROY_CHARACTER(npc);
	DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_npc WHERE owner_id = %u", get_table_postfix(), npc->GetOfflineShopRealOwner());
	DBManager::instance().DirectQuery("UPDATE %soffline_shop_item SET status = 1 WHERE owner_id = %u and status = 0", get_table_postfix(), npc->GetOfflineShopRealOwner());
}

int32_t COfflineShop::Buy(LPCHARACTER ch, BYTE bPos, DWORD item_id, long long llPriceCheck)
{
	if (!ch || (ch && !ch->GetDesc())) return SHOP_SUBHEADER_GC_INVALID_POS;

	const LPCHARACTER pkShopNPC = ch->GetOfflineShopOwner();
	if (pkShopNPC == nullptr) { return SHOP_SUBHEADER_CG_END; }
	const DWORD pazarciPID = pkShopNPC->GetOfflineShopRealOwner();
	if (!pazarciPID) { return SHOP_SUBHEADER_CG_END; }
	if (pkShopNPC != nullptr) {
		if (pazarciPID == ch->GetPlayerID()) { ch->ChatPacket(CHAT_TYPE_INFO, "[LS:3597]"); return SHOP_SUBHEADER_GC_BUY_FROM_OWNSHOP; }
	}

	if (bPos >= OFFLINE_SHOP_HOST_ITEM_MAX_NUM) {
		sys_log(0, "OfflineShop::Buy : invalid position %d : %s", bPos, ch->GetName());
		return SHOP_SUBHEADER_GC_INVALID_POS;
	}

	auto it = std::find(m_map_guest.begin(), m_map_guest.end(), ch->GetPlayerID());
	if (it == m_map_guest.end()) return SHOP_SUBHEADER_GC_END;

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),
		"SELECT id, count, vnum"
		", price"
		", socket0,socket1,socket2,socket3"
		", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6"
		" FROM %soffline_shop_item WHERE owner_id = %u and pos = %d and status = 0", get_table_postfix(), pazarciPID, bPos
	);

	auto pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->uiSQLErrno != 0) { return SHOP_SUBHEADER_GC_END; }

	long long dwPrice = 0;
	TPlayerItem item;
	for (my_ulonglong i = 0; i < mysql_num_rows(pMsg->Get()->pSQLResult); ++i) {
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		int32_t cur = 0;
		str_to_number(item.id, row[cur++]);
		str_to_number(item.count, row[cur++]);
		str_to_number(item.vnum, row[cur++]);
		str_to_number(dwPrice, row[cur++]);
		for (uint8_t x = 0; x < ITEM_SOCKET_MAX_NUM; x++) str_to_number(item.alSockets[x], row[cur++]);
		for (uint8_t j = 0; j < ITEM_ATTRIBUTE_MAX_NUM; j++) {
			str_to_number(item.aAttr[j].bType, row[cur++]);
			str_to_number(item.aAttr[j].sValue, row[cur++]);
		}
	}

	sys_log(0, "OfflineShop::Buy : name %s pos %d", ch->GetName(), bPos);

	long long llPrice = dwPrice;
	if (llPrice > 0) {
		if (ch->GetGold() < llPrice) {
			sys_log(1, "Shop::Buy : Not enough money : %s has %lld, price %lld", ch->GetName(), ch->GetGold(), llPrice);
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}
	}

	if (item_id != 0 && (item_id != item.id))
		return SHOP_SUBHEADER_GC_REMOVED_FROM_SHOP;

	if (llPriceCheck != 0 && (llPriceCheck != llPrice))
		return SHOP_SUBHEADER_GC_PRICE_CHANGED;

	LPITEM pItem = ITEM_MANAGER::Instance().CreateItem(item.vnum, item.count, item.id);
	if (!pItem) return SHOP_SUBHEADER_GC_SOLD_OUT;
	if (pItem->IsDragonSoul() && (ch && !ch->DragonSoul_IsQualified())) { M2_DESTROY_ITEM(pItem); return SHOP_SUBHEADER_GC_YOU_DONT_HAVE_DS; }
	const int32_t iEmptyPos = pItem->IsDragonSoul() ? ch->GetEmptyDragonSoulInventory(pItem) : ch->GetEmptyInventory(pItem->GetSize());
	if (iEmptyPos < 0) { M2_DESTROY_ITEM(pItem); return SHOP_SUBHEADER_GC_INVENTORY_FULL; }
	pItem->SetAttributes(item.aAttr);
	pItem->SetSockets(item.alSockets);

	if (pItem->IsDragonSoul()) pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
	else {
		if (!ch->exInven_PutOn(pItem, true)) pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
	}
	ITEM_MANAGER::instance().FlushDelayedSave(pItem);

	if (llPrice > 0) DBManager::instance().DirectQuery("UPDATE %splayer SET shop_gold = shop_gold + %lld WHERE id = %u", get_table_postfix(), llPrice, ch->GetOfflineShopOwner()->GetOfflineShopRealOwner());
	
	RemoveItem(ch->GetOfflineShopOwner()->GetOfflineShopRealOwner(), bPos);
	BroadcastUpdateItem(bPos, ch->GetOfflineShopOwner()->GetOfflineShopRealOwner(), true);

	if (llPrice > 0) ch->PointChange(POINT_GOLD, -llPrice, false);

	ch->Save();

	{ ///* log sales *///
		char szInsertQuery[QUERY_MAX_LEN];
		snprintf(szInsertQuery, sizeof(szInsertQuery), "INSERT INTO %soffline_shop_sales ("
			"buyer_id, "
			"buyer_name, "
			"item_owner, "
			"item_vnum, "
			"item_count, "
			"item_price, "
			"datetime) VALUES("
			"%u, "
			"'%s', "
			"%u, "
			"%u, "
			"%d, "
			"%lld, "
			"%d)",
			get_table_postfix(),
			ch->GetPlayerID(),
			ch->GetName(),
			ch->GetOfflineShopOwner()->GetOfflineShopRealOwner(),
			item.vnum,
			item.count,
			llPrice,
			get_global_time()
		);
		auto pMsg(DBManager::instance().DirectQuery(szInsertQuery));
	}

	DWORD dwOwnerPID_s = ch->GetOfflineShopOwner()->GetOfflineShopRealOwner();
	LPCHARACTER ownerChar = CHARACTER_MANAGER::instance().FindByPID(dwOwnerPID_s);
	if (!ownerChar) {
		CCI* pcci = P2P_MANAGER::instance().FindByPID(dwOwnerPID_s);
		if (pcci) {
			TPacketGGOfflineShopAdviseOwnerSell packetReload;
			packetReload.bHeader = HEADER_GG_OFFLINE_SHOP_ADVISE_PLAYER;
			packetReload.dwOwnerPID = dwOwnerPID_s;
			packetReload.itemSelled = item.vnum;
			packetReload.itemPrice = llPrice;
			strlcpy(packetReload.buyerName, ch->GetName(), sizeof(packetReload.buyerName));
			P2P_MANAGER::instance().Send(&packetReload, sizeof(TPacketGGOfflineShopAdviseOwnerSell));
		}
	}
	else
		COfflineShopManager::instance().AdviseItemSell(ownerChar, item.vnum, llPrice, ch->GetName());

	if (m_pkOfflineShopNPC && m_pkOfflineShopNPC->IsOfflineShopNPC()) {
		if (GetLeftItemCount(ch->GetOfflineShopOwner()->GetOfflineShopRealOwner()) <= 0) m_pkOfflineShopNPC->DestroyOfflineShop();
	}
	return (SHOP_SUBHEADER_GC_OK);
}

void COfflineShop::BroadcastUpdateItem(BYTE bPos, DWORD dwPID, bool bDestroy)
{
	TPacketGCShop pack;
	TPacketGCOfflineShopUpdateItem pack2;
	TEMP_BUFFER buf;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_UPDATE_ITEM;
	pack.size = sizeof(pack) + sizeof(pack2);
	pack2.pos = bPos;
	if (bDestroy) {
		pack2.item.vnum = 0;
		pack2.item.count = 0;
		pack2.item.price = 0;
		memset(pack2.item.alSockets, 0, sizeof(pack2.item.alSockets));
		memset(pack2.item.aAttr, 0, sizeof(pack2.item.aAttr));
	}
	else {
		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery),
			"SELECT vnum, count"
			", price"
			", socket0,socket1,socket2,socket3"
			", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6"
			" FROM %soffline_shop_item WHERE owner_id = %u and pos = %d limit 0,1;", get_table_postfix(), dwPID, bPos
		);

		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
		if (pMsg->Get()->uiNumRows == 0) {
			pack2.item.vnum = 0;
			pack2.item.count = 0;
			pack2.item.price = 0;
			memset(pack2.item.alSockets, 0, sizeof(pack2.item.alSockets));
			memset(pack2.item.aAttr, 0, sizeof(pack2.item.aAttr));
		}
		else {
			for (my_ulonglong i = 0; i < mysql_num_rows(pMsg->Get()->pSQLResult); ++i) {
				MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
				int32_t cur = 0;

				str_to_number(pack2.item.vnum, row[cur++]);
				str_to_number(pack2.item.count, row[cur++]);
				str_to_number(pack2.item.price, row[cur++]);
				for (BYTE j = 0; j < ITEM_SOCKET_MAX_NUM; j++) str_to_number(pack2.item.alSockets[j], row[cur++]);
				for (BYTE n = 0; n < ITEM_ATTRIBUTE_MAX_NUM; n++) {
					str_to_number(pack2.item.aAttr[n].bType, row[cur++]);
					str_to_number(pack2.item.aAttr[n].sValue, row[cur++]);
				}
			}
		}
	}

	buf.write(&pack, sizeof(pack));
	buf.write(&pack2, sizeof(pack2));
	Broadcast(buf.read_peek(), buf.size());
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPID);
	if (!ch) {
		CCI* pcci = P2P_MANAGER::instance().FindByPID(dwPID);
		if (pcci) RefreshP2P(dwPID);
	}
	else Refresh(ch);
}

void COfflineShop::BroadcastUpdatePrice(DWORD dwPID, BYTE bPos, long long dwPrice)
{
	TPacketGCShop pack;
	TPacketGCShopUpdatePrice pack2;
	TEMP_BUFFER buf;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_UPDATE_PRICE;
	pack.size = sizeof(pack) + sizeof(pack2);
	pack2.bPos = bPos;
	pack2.dwShopVid = (DWORD)(GetOfflineShopNPC() ? GetOfflineShopNPC()->GetVID() : 0);
	pack2.iPrice = dwPrice;
	buf.write(&pack, sizeof(pack));
	buf.write(&pack2, sizeof(pack2));
	Broadcast(buf.read_peek(), buf.size());
	BroadcastUpdateItem(bPos, dwPID, false);
}

void COfflineShop::Refresh(LPCHARACTER ch)
{
	if (!ch || (ch && !ch->GetDesc())) return;
	TPacketGCShop pack;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_UPDATE_ITEM2;
	TPacketGCOfflineShopStart pack2;
	memset(&pack2, 0, sizeof(pack2));
	pack2.owner_vid = 0;

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),
		"SELECT pos, count, vnum"
		", price"
		", socket0,socket1,socket2,socket3"
		", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6"
		" FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID()
	);

	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	for (my_ulonglong i = 0; i < mysql_num_rows(pMsg->Get()->pSQLResult); ++i) {
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		int32_t cur = 0;

		BYTE bPos = 0;
		str_to_number(bPos, row[cur++]);

		str_to_number(pack2.items[bPos].count, row[cur++]);
		str_to_number(pack2.items[bPos].vnum, row[cur++]);
		str_to_number(pack2.items[bPos].price, row[cur++]);
		for (BYTE j = 0; j < ITEM_SOCKET_MAX_NUM; j++) str_to_number(pack2.items[bPos].alSockets[j], row[cur++]);
		for (BYTE n = 0; n < ITEM_ATTRIBUTE_MAX_NUM; n++) {
			str_to_number(pack2.items[bPos].aAttr[n].bType, row[cur++]);
			str_to_number(pack2.items[bPos].aAttr[n].sValue, row[cur++]);
		}
	}

	pack.size = sizeof(pack) + sizeof(pack2);
	if (!ch->GetDesc()->IsPhase(PHASE_GAME)) { return; }
	if (ch->GetDesc()) {
		ch->GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
		ch->GetDesc()->Packet(&pack2, sizeof(TPacketGCOfflineShopStart));
	}
}

bool COfflineShop::RemoveItem(DWORD dwVID, BYTE bPos)
{
	DBManager::instance().Query("DELETE FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), dwVID, bPos);
	return true;
}

BYTE COfflineShop::GetLeftItemCount(DWORD dwPID)
{
	auto pMsg(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM %soffline_shop_item WHERE owner_id = %u and status = 0;", get_table_postfix(), dwPID));
	if (pMsg->uiSQLErrno != 0) { return 1; }
	if (pMsg->Get()->uiNumRows == 0) { return 0; }
	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	BYTE bCount = 0;
	str_to_number(bCount, row[0]);
	return bCount;
}

void COfflineShop::Broadcast(const void* data, int32_t bytes)
{
	if (!data || !bytes) return;
	for (auto& idx : m_map_guest) {
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(idx);
		LPDESC d = nullptr;
		if (!ch || (ch && !(d = ch->GetDesc()))) continue;
		d->Packet(data, bytes);
	}
}

void COfflineShop::RefreshP2P(DWORD dwPID)
{
	TPacketGCOfflineShopStartP2P pack2;
	pack2.header = HEADER_GC_OFFLINE_SHOP_START_P2P;
	pack2.owner_vid = dwPID;

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),
		"SELECT pos, count, vnum"
		", price"
		", socket0,socket1,socket2,socket3"
		", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6"
		" FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), dwPID
	);

	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	for (my_ulonglong i = 0; i < mysql_num_rows(pMsg->Get()->pSQLResult); ++i) {
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		int32_t cur = 0;

		BYTE bPos = 0;
		str_to_number(bPos, row[cur++]);

		str_to_number(pack2.items[bPos].count, row[cur++]);
		str_to_number(pack2.items[bPos].vnum, row[cur++]);
		str_to_number(pack2.items[bPos].price, row[cur++]);
		for (BYTE j = 0; j < ITEM_SOCKET_MAX_NUM; j++) str_to_number(pack2.items[bPos].alSockets[j], row[cur++]);
		for (BYTE n = 0; n < ITEM_ATTRIBUTE_MAX_NUM; n++) {
			str_to_number(pack2.items[bPos].aAttr[n].bType, row[cur++]);
			str_to_number(pack2.items[bPos].aAttr[n].sValue, row[cur++]);
		}
	}

	CCI* pcci = P2P_MANAGER::instance().FindByPID(dwPID);
	if (!pcci || (pcci && !pcci->pkDesc)) return;
	pcci->pkDesc->SetRelay(pcci->szName);
	pcci->pkDesc->Packet(&pack2, sizeof(pack2));
	pcci->pkDesc->SetRelay("");
}

void COfflineShop::AddItemP2P(DWORD dwPID, TOfflineShopItem pkItem, BYTE bPos, long long iPrice)
{
	DBManager::instance().Query("INSERT INTO %soffline_shop_item ("
		"id, owner_id, pos, count"
		", price"
		", vnum"
		", socket0, socket1, socket2, socket3"
		", attrtype0, attrvalue0"
		", attrtype1, attrvalue1"
		", attrtype2, attrvalue2"
		", attrtype3, attrvalue3"
		", attrtype4, attrvalue4"
		", attrtype5, attrvalue5"
		", attrtype6, attrvalue6"
		") VALUES ("
		"%u, %u, %d, %u" // id, owner_id, pos, count
		", %lld" // price
		", %u" // vnum
		", %ld, %ld, %ld, %ld" // socket0, socket1, socket2
		", %d, %d" // attrtype0, attrvalue0
		", %d, %d" // attrtype1, attrvalue1
		", %d, %d" // attrtype2, attrvalue2
		", %d, %d" // attrtype3, attrvalue3
		", %d, %d" // attrtype4, attrvalue4
		", %d, %d" // attrtype5, attrvalue5
		", %d, %d" // attrtype6, attrvalue6
		")", get_table_postfix(),
		pkItem.id, dwPID, bPos, pkItem.count
		, iPrice
		, pkItem.vnum
		, pkItem.alSockets[0]
		, pkItem.alSockets[1]
		, pkItem.alSockets[2]
		, pkItem.alSockets[3]
		, pkItem.aAttr[0].bType, pkItem.aAttr[0].sValue
		, pkItem.aAttr[1].bType, pkItem.aAttr[1].sValue
		, pkItem.aAttr[2].bType, pkItem.aAttr[2].sValue
		, pkItem.aAttr[3].bType, pkItem.aAttr[3].sValue
		, pkItem.aAttr[4].bType, pkItem.aAttr[4].sValue
		, pkItem.aAttr[5].bType, pkItem.aAttr[5].sValue
		, pkItem.aAttr[6].bType, pkItem.aAttr[6].sValue
	);
	RefreshP2P(dwPID);
}

void COfflineShop::RemoveItemP2P(DWORD dwPID, BYTE bPos)
{
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),
		"SELECT id, pos, count, vnum"
		", socket0, socket1, socket2, socket3"
		", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6"
		" FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), dwPID, bPos);
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows == 0) { return; }
	TPlayerItem item;
	for (my_ulonglong i = 0; i < mysql_num_rows(pMsg->Get()->pSQLResult); ++i) {
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		int32_t cur = 0;
		str_to_number(item.id, row[cur++]);
		str_to_number(item.pos, row[cur++]);
		str_to_number(item.count, row[cur++]);
		str_to_number(item.vnum, row[cur++]);
		for (uint8_t x = 0; x < ITEM_SOCKET_MAX_NUM; x++) str_to_number(item.alSockets[x], row[cur++]);
		for (uint8_t j = 0; j < ITEM_ATTRIBUTE_MAX_NUM; j++) {
			str_to_number(item.aAttr[j].bType, row[cur++]);
			str_to_number(item.aAttr[j].sValue, row[cur++]);
		}
	}

	TPacketGGRemoveOfflineShopItem p;
	p.bHeader = HEADER_GG_OFFLINE_SHOP_REMOVE_ITEM;
	p.bSubHeader = OFFLINE_SHOP_FORCE_REMOVE_ITEM;
	p.dwOwnerPID = dwPID;
	p.bPos = bPos;
	p.item = item;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGRemoveOfflineShopItem));
	DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), dwPID, bPos);
	BroadcastUpdateItem(bPos, dwPID, true);
}


void COfflineShop::SetOfflineShopBorderStyle(BYTE bBorderStyle)
{
	m_pkOfflineShopBorderStyle = bBorderStyle;
}

bool COfflineShop::ChangeItemPrice(DWORD dwPID, BYTE bPos, long long llPrice)
{
	DBManager::instance().DirectQuery("UPDATE %soffline_shop_item SET price = %lld WHERE owner_id = %u and pos = %d", get_table_postfix(), llPrice, dwPID, bPos);
	BroadcastUpdatePrice(dwPID, bPos, llPrice);
	return true;
}
