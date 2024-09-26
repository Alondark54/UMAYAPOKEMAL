#include "stdafx.h"
#include "config.h"
#include "char.h"
#include "item_manager.h"
#include "log.h"
#include "item.h"
#include "char_manager.h"
#include "utils.h"

std::vector<DWORD> basicWeaponChests = { 50187, 50188, 50189, 50212, 50213, 50214, 54043, 50190, 50191, 50192, 50193, 50194, 50195, 50196 };
bool isItem_BasicWeaponChests(DWORD itemVnum) { return std::find(basicWeaponChests.begin(), basicWeaponChests.end(), itemVnum) != basicWeaponChests.end(); }
bool isItem_CorDraconis(DWORD itemVnum) { return (itemVnum > 51500 && itemVnum < 52000); }

int32_t CHARACTER::exInven_IsExInvenItem(LPITEM item) const
{
	const BYTE itemType = item->GetType();
	const BYTE itemSubType = item->GetSubType();
	const DWORD itemVnum = item->GetVnum();

	if (isItem_CorDraconis(itemVnum)) { return 4; }
	if (isItem_BasicWeaponChests(itemVnum)) { return -1; }

	if (itemType == ITEM_METIN) { return 1; }
	if (itemType == ITEM_GIFTBOX) {
		if (isItem_BasicWeaponChests(itemVnum)) { return -1; }
		return 3;
	}

	if (itemType == ITEM_SKILLBOOK) { return 0; }
	if (itemVnum >= 55003 && itemVnum <= 55007) { return 0; }
	if (itemVnum >= 55010 && itemVnum <= 55026) { return 0; }
	if (itemVnum >= 50301 && itemVnum <= 50306) { return 0; }
	if (itemVnum >= 50311 && itemVnum <= 50320) { return 0; }
	if (itemVnum >= 55034 && itemVnum <= 55040) { return 0; }

	if (itemVnum >= 50721 && itemVnum <= 50728) { return 4; }
	if (itemVnum >= 50801 && itemVnum <= 50804) { return 4; }
	if (itemVnum >= 50814 && itemVnum <= 50820) { return 4; }

	if (itemType == ITEM_MATERIAL) { return 2; }
	if (itemType == ITEM_RESOURCE) { return 2; }
	return -1;
}

int32_t CHARACTER::exInven_FindPos(LPITEM item, bool forceEmpty) const
{
	int32_t yKonum = -1, bosAlan = -1;
	if (!item->IsStackable()) { forceEmpty = true; }
	const int32_t itemKonum = exInven_IsExInvenItem(item);
	if (itemKonum == -1) { return yKonum; }
	if (itemKonum == 0) {
		const int32_t bkBas = EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 0);
		const int32_t bkSon = EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 4);
		for (int32_t i = bkBas; i < bkSon; ++i) {
			LPITEM tItem = GetInventoryItem(i);
			if (!tItem) {
				if (forceEmpty) { return i; }
				if (bosAlan == -1) { bosAlan = i; }
				continue;
			}
			if (forceEmpty) { continue; }
			if (tItem->IsExchanging()) { continue; }
			if (tItem->GetVnum()    != item->GetVnum()) { continue; }
			if (tItem->GetSocket(0) != item->GetSocket(0)) { continue; }
			if (tItem->GetCount()   == g_bItemCountLimit) { continue; }
			if (tItem->GetCount()   <  g_bItemCountLimit) { return i; }
		}
		if (yKonum == -1 && bosAlan != -1) { yKonum = bosAlan; }
		
		return yKonum;
	}
	if (itemKonum == 1)	{
		const int32_t tasBas = EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 4);
		const int32_t tasSon = EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 8);
		for (int32_t i = tasBas; i < tasSon; ++i) {
			LPITEM tItem = GetInventoryItem(i);
			if (!tItem) {
				if (forceEmpty) { return i; }
				if (bosAlan == -1) { bosAlan = i; }
				continue;
			}
			if (forceEmpty) { continue; }
			if (tItem->IsExchanging()) { continue; }
			if (tItem->GetVnum()  != item->GetVnum()) { continue; }
			if (tItem->GetCount() == g_bItemCountLimit) { continue; }
			if (tItem->GetCount() <  g_bItemCountLimit) { return i; }
		}
		if (yKonum == -1 && bosAlan != -1) { yKonum = bosAlan; }
		return yKonum;
	}
	if (itemKonum == 2) {
		const int32_t basBas = EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 8);
		const int32_t basSon = EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 12);
		for (int32_t i = basBas; i < basSon; ++i) {
			LPITEM tItem = GetInventoryItem(i);
			if (!tItem) {
				if (forceEmpty) { return i; }
				if (bosAlan == -1) { bosAlan = i; }
				continue;
			}
			if (forceEmpty) { continue; }
			if (tItem->IsExchanging()) { continue; }
			if (tItem->GetVnum() != item->GetVnum()) { continue; }
			if (tItem->GetCount() == g_bItemCountLimit) { continue; }
			if (tItem->GetCount() <  g_bItemCountLimit) { return i; }
		}
		if (yKonum == -1 && bosAlan != -1) { yKonum = bosAlan; }
		return yKonum;
	}
	if (itemKonum == 3) {
		const int32_t sandikBas = EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 12);
		const int32_t sandikSon = EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 16);
		for (int32_t i = sandikBas; i < sandikSon; ++i) {
			LPITEM tItem = GetInventoryItem(i);
			if (!tItem) {
				if (forceEmpty) { return i; }
				if (bosAlan == -1) { bosAlan = i; }
				continue;
			}
			if (forceEmpty) { continue; }
			if (tItem->IsExchanging()) { continue; }
			if (tItem->GetVnum()    != item->GetVnum()) { continue; }
			if (tItem->GetSocket(0) != item->GetSocket(0)) { continue; }
			if (tItem->GetCount()   == g_bItemCountLimit) { continue; }
			if (tItem->GetCount()   <  g_bItemCountLimit) { return i; }
		}
		if (yKonum == -1 && bosAlan != -1) { yKonum = bosAlan; }
		return yKonum;
	}
	if (itemKonum == 4) {
		const int32_t sandikBas = EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 16);
		const int32_t sandikSon = EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 20);
		for (int32_t i = sandikBas; i < sandikSon; ++i) {
			LPITEM tItem = GetInventoryItem(i);
			if (!tItem) {
				if (forceEmpty) { return i; }
				if (bosAlan == -1) { bosAlan = i; }
				continue;
			}
			if (forceEmpty) { continue; }
			if (tItem->IsExchanging()) { continue; }
			if (tItem->GetVnum() != item->GetVnum()) { continue; }
			if (tItem->GetSocket(0) != item->GetSocket(0)) { continue; }
			if (tItem->GetCount() == g_bItemCountLimit) { continue; }
			if (tItem->GetCount() < g_bItemCountLimit) { return i; }
		}
		if (yKonum == -1 && bosAlan != -1) { yKonum = bosAlan; }
		return yKonum;
	}

	return -1;
}

bool CHARACTER::exInven_PutOn(LPITEM &item, bool isGiveInfo)
{
	const DWORD itemVnum = item->GetVnum();
	const BYTE itemCount = item->GetCount();
	const int32_t yKonum = exInven_FindPos(item, itemCount == g_bItemCountLimit);
	if (yKonum == -1) { return false; }
	const int32_t itemKonum = exInven_IsExInvenItem(item);

	LPITEM tItem = GetInventoryItem(yKonum);
	if (!tItem) {
		item->AddToCharacter(this, TItemPos(INVENTORY, yKonum));
		if (isGiveInfo) {
			switch (itemKonum) {
				case 0: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
				case 1: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
				case 2: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
				case 3: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
				case 4: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
				default: break;
			}
		}
		return true;
	}

	const int32_t toplamAdet = tItem->GetCount() + itemCount;
	if (toplamAdet <= g_bItemCountLimit){
		tItem->SetCount(toplamAdet);
		if (isGiveInfo) {
			switch (itemKonum) {
				case 0: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
				case 1: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
				case 2: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
				case 3: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
				case 4: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
				default: break;
			}
		}
		ITEM_MANAGER::instance().DestroyItem(item);
		item = ITEM_MANAGER::instance().CreateItem(itemVnum, itemCount);
		//item = tItem;
		return true;
	}

	tItem->SetCount(g_bItemCountLimit);
	const int32_t artanAdet = toplamAdet % g_bItemCountLimit;
	const int32_t artanKonum = exInven_FindPos(item, true);
	item->SetCount(artanAdet);
	if (artanKonum != -1) { item->AddToCharacter(this, TItemPos(INVENTORY, artanKonum)); }
	if (artanKonum == -1){
		item->SetOwnership(this);
		item->AddToGround(GetMapIndex(), GetXYZ(), false);
	}

	if (isGiveInfo) {
		switch (itemKonum) {
			case 0: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
			case 1: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
			case 2: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
			case 3: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
			case 4: ChatPacket(CHAT_TYPE_INFO, "%s itemi %d adet K Envanterine tasindi.", item->GetName(), itemCount); break;
			default: break;
		}
	}
	return true;
}

void CHARACTER::exInven_Build()
{
	SetQuestFlag("exInventory.lastStack", get_global_time());

	std::vector<LPITEM> vec_Items;
	LPITEM item = nullptr;
	WORD minSize = static_cast<WORD>(EXINVENTORY_POS_START);
	WORD maxSize = static_cast<WORD>(EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 4));
	for (WORD i = minSize; i < maxSize; ++i) {
		if ((item = GetInventoryItem(i))) {
			LPITEM tempItem = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), item->GetCount());
			if (!tempItem) { sys_err("BKDepo tempItem olusmadi.. %d, %d", item->GetVnum(), item->GetCount()); continue; }
			item->CopyAttributeTo(tempItem);
			item->CopySocketTo(tempItem);
			vec_Items.push_back(tempItem);
			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
	if (vec_Items.empty()) { goto TasDuzenle; }
	std::sort(vec_Items.begin(), vec_Items.end(), [](const LPITEM& i1, const LPITEM& i2) { return i1->GetSocket(0) < i2->GetSocket(0); });
	for (std::vector<LPITEM>::iterator satirItem = vec_Items.begin(); satirItem != vec_Items.end(); ++satirItem) {
		if (!exInven_PutOn(*satirItem)) {
			LPITEM curItem = *satirItem;
			curItem->SetOwnership(this);
			curItem->AddToGround(GetMapIndex(), GetXYZ(), false);
		}
	}

TasDuzenle:
	vec_Items.clear();
	minSize = static_cast<WORD>(EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 4));
	maxSize = static_cast<WORD>(EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 8));
	int32_t sonSira = 0, sonTur = 0, sonTurAdet = 0, sonTasVnum = 0, sonTasAdet = 0;
	for (WORD i = minSize; i < maxSize; ++i) {
		if ((item = GetInventoryItem(i))) {
			LPITEM tempItem = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), item->GetCount());
			if (!tempItem) { sys_err("BKDepo tempItem olusmadi.. %d, %d", item->GetVnum(), item->GetCount()); continue; }
			item->CopyAttributeTo(tempItem);
			item->CopySocketTo(tempItem);
			vec_Items.push_back(tempItem);
			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
	if (vec_Items.empty()) { goto MeteryalDuzenle; }
	std::sort(vec_Items.begin(), vec_Items.end(), [](const LPITEM& i1, const LPITEM& i2) {
		return i1->GetProto()->aApplies[0].bType == i2->GetProto()->aApplies[0].bType ? i1->GetProto()->aApplies[0].lValue < i2->GetProto()->aApplies[0].lValue : i1->GetProto()->aApplies[0].bType < i2->GetProto()->aApplies[0].bType;
	});
	for (std::vector<LPITEM>::iterator satirItem = vec_Items.begin(); satirItem != vec_Items.end(); ++satirItem) {
		LPITEM vecItem = *satirItem;
		const TItemTable* tProto = vecItem->GetProto();
		const int32_t iVnum = vecItem->GetVnum();
		int32_t iAdet = vecItem->GetCount();
		//int32_t iArti = static_cast<int32_t>(vecItem->GetVnum() % 1000) / 100;
		const int32_t iTuru = tProto->aApplies[0].bType;
		if (sonTur != 0 && sonTur != iTuru) {
			sonSira += 5;
			if (sonTurAdet>5) { sonSira += 5; }
			if (sonTurAdet>10) { sonSira += 5; }
			if (sonTurAdet>15) { sonSira += 5; }
			if (sonTurAdet>20) { sonSira += 5; }
			if (sonTurAdet>25) { sonSira += 5; }
			if (sonTurAdet>30) { sonSira += 5; }
			sonTurAdet = 0;
		}
		const int32_t iKonum = minSize + sonSira + sonTurAdet;
		sonTur = iTuru;
		const int32_t yAdet = (sonTasAdet + iAdet);

		if (!(tProto->dwFlags & ITEM_FLAG_STACKABLE)) { goto YeniTasiEkle; }

		if (sonTasVnum != iVnum) { goto YeniTasiEkle; }
		if (sonTasAdet >= g_bItemCountLimit) { goto YeniTasiEkle; }
		if (yAdet <= g_bItemCountLimit) {
			LPITEM eItem = GetInventoryItem(iKonum - 1);
			if (!eItem) { goto YeniTasiEkle; }
			eItem->SetCount(yAdet);
			sonTasAdet = yAdet;
			continue;
		}
		if (yAdet> g_bItemCountLimit) {
			LPITEM eItem = GetInventoryItem(iKonum - 1);
			if (!eItem) { goto YeniTasiEkle; }

			eItem->SetCount(g_bItemCountLimit);
			iAdet = yAdet - g_bItemCountLimit;
			vecItem->SetCount(iAdet);
		}
	YeniTasiEkle:
		sonTasVnum = iVnum;
		sonTasAdet = iAdet;
		sonTurAdet++;
		vecItem->AddToCharacter(this, TItemPos(INVENTORY, iKonum));
	}
	
MeteryalDuzenle:
	vec_Items.clear();
	minSize = static_cast<WORD>(EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 8));
	maxSize = static_cast<WORD>(EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 12));
	for (WORD i = minSize; i < maxSize; ++i) {
		if ((item = GetInventoryItem(i))) {
			LPITEM tempItem = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), item->GetCount());
			if (!tempItem) { sys_err("BKDepo tempItem olusmadi.. %d, %d", item->GetVnum(), item->GetCount()); continue; }
			item->CopyAttributeTo(tempItem);
			item->CopySocketTo(tempItem);
			vec_Items.push_back(tempItem);
			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
	if (vec_Items.empty()) { goto SandikDuzenle; }
	std::sort(vec_Items.begin(), vec_Items.end(), [](const LPITEM& i1, const LPITEM& i2) { return i1->GetVnum() < i2->GetVnum(); });
	for (std::vector<LPITEM>::iterator satirItem = vec_Items.begin(); satirItem != vec_Items.end(); ++satirItem) {
		if (!exInven_PutOn(*satirItem)) {
			LPITEM curItem = *satirItem;
			curItem->SetOwnership(this);
			curItem->AddToGround(GetMapIndex(), GetXYZ(), false);
		}
	}

SandikDuzenle:
	vec_Items.clear();
	minSize = static_cast<WORD>(EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 12));
	maxSize = static_cast<WORD>(EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 16));
	for (WORD i = minSize; i < maxSize; ++i) {
		if ((item = GetInventoryItem(i))) {
			LPITEM tempItem = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), item->GetCount());
			if (!tempItem) { sys_err("BKDepo tempItem olusmadi.. %d, %d", item->GetVnum(), item->GetCount()); continue; }
			item->CopyAttributeTo(tempItem);
			item->CopySocketTo(tempItem);
			vec_Items.push_back(tempItem);
			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
	if (vec_Items.empty()) { goto cicekDuzenle; }
	std::sort(vec_Items.begin(), vec_Items.end(), [](const LPITEM& i1, const LPITEM& i2) { return i1->GetVnum() < i2->GetVnum(); });
	for (std::vector<LPITEM>::iterator satirItem = vec_Items.begin(); satirItem != vec_Items.end(); ++satirItem) {
		if (!exInven_PutOn(*satirItem)) {
			LPITEM curItem = *satirItem;
			curItem->SetOwnership(this);
			curItem->AddToGround(GetMapIndex(), GetXYZ(), false);
		}
	}

cicekDuzenle:
	vec_Items.clear();
	minSize = static_cast<WORD>(EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 16));
	maxSize = static_cast<WORD>(EXINVENTORY_POS_START + (INVENTORY_PAGE_SIZE * 20));
	for (WORD i = minSize; i < maxSize; ++i) {
		if ((item = GetInventoryItem(i))) {
			LPITEM tempItem = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), item->GetCount());
			if (!tempItem) { sys_err("BKDepo tempItem olusmadi.. %d, %d", item->GetVnum(), item->GetCount()); continue; }
			item->CopyAttributeTo(tempItem);
			item->CopySocketTo(tempItem);
			vec_Items.push_back(tempItem);
			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
	if (vec_Items.empty()) { goto Bitti; }
	std::sort(vec_Items.begin(), vec_Items.end(), [](const LPITEM& i1, const LPITEM& i2) { return i1->GetVnum() < i2->GetVnum(); });
	for (std::vector<LPITEM>::iterator satirItem = vec_Items.begin(); satirItem != vec_Items.end(); ++satirItem) {
		if (!exInven_PutOn(*satirItem)) {
			LPITEM curItem = *satirItem;
			curItem->SetOwnership(this);
			curItem->AddToGround(GetMapIndex(), GetXYZ(), false);
		}
	}

Bitti:
	vec_Items.clear();
	Save();
	ChatPacket(CHAT_TYPE_INFO, "[LS:3607]");
}
