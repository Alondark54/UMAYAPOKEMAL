#include "stdafx.h"
#include "PythonShop.h"

#include "PythonNetworkStream.h"

//BOOL CPythonShop::GetSlotItemID(DWORD dwSlotPos, DWORD* pdwItemID)
//{
//	if (!CheckSlotIndex(dwSlotPos))
//		return FALSE;
//	const TShopItemData * itemData;
//	if (!GetItemData(dwSlotPos, &itemData))
//		return FALSE;
//	*pdwItemID=itemData->vnum;
//	return TRUE;
//}
void CPythonShop::SetTabCoinType(BYTE tabIdx, BYTE coinType)
{
	if (tabIdx >= m_bTabCount)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d.", tabIdx, SHOP_TAB_COUNT_MAX);
		return;
	}
	m_aShoptabs[tabIdx].coinType = coinType;
}

BYTE CPythonShop::GetTabCoinType(BYTE tabIdx)
{
	if (tabIdx >= m_bTabCount)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d.", tabIdx, SHOP_TAB_COUNT_MAX);
		return 0xff;
	}
	return m_aShoptabs[tabIdx].coinType;
}

void CPythonShop::SetTabName(BYTE tabIdx, const char* name)
{
	if (tabIdx >= m_bTabCount)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d.", tabIdx, SHOP_TAB_COUNT_MAX);
		return;
	}
	m_aShoptabs[tabIdx].name = name;
}

const char* CPythonShop::GetTabName(BYTE tabIdx)
{
	if (tabIdx >= m_bTabCount)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d.", tabIdx, SHOP_TAB_COUNT_MAX);
		return NULL;
	}

	return m_aShoptabs[tabIdx].name.c_str();
}

void CPythonShop::SetItemData(DWORD dwIndex, const TShopItemData & c_rShopItemData)
{
	BYTE tabIdx = dwIndex / SHOP_HOST_ITEM_MAX_NUM;
	DWORD dwSlotPos = dwIndex % SHOP_HOST_ITEM_MAX_NUM;

	SetItemData(tabIdx, dwSlotPos, c_rShopItemData);
}

BOOL CPythonShop::GetItemData(DWORD dwIndex, const TShopItemData ** c_ppItemData)
{
	BYTE tabIdx = dwIndex / SHOP_HOST_ITEM_MAX_NUM;
	DWORD dwSlotPos = dwIndex % SHOP_HOST_ITEM_MAX_NUM;

	return GetItemData(tabIdx, dwSlotPos, c_ppItemData);
}

void CPythonShop::SetItemData(BYTE tabIdx, DWORD dwSlotPos, const TShopItemData & c_rShopItemData)
{
	if (tabIdx >= SHOP_TAB_COUNT_MAX || dwSlotPos >= SHOP_HOST_ITEM_MAX_NUM)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d. dwSlotPos(%d) must be less than %d", tabIdx, SHOP_TAB_COUNT_MAX, dwSlotPos, SHOP_HOST_ITEM_MAX_NUM);
		return;
	}

	m_aShoptabs[tabIdx].items[dwSlotPos] = c_rShopItemData;
}

BOOL CPythonShop::GetItemData(BYTE tabIdx, DWORD dwSlotPos, const TShopItemData ** c_ppItemData)
{
	if (tabIdx >= SHOP_TAB_COUNT_MAX || dwSlotPos >= SHOP_HOST_ITEM_MAX_NUM)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d. dwSlotPos(%d) must be less than %d", tabIdx, SHOP_TAB_COUNT_MAX, dwSlotPos, SHOP_HOST_ITEM_MAX_NUM);
		return FALSE;
	}

	*c_ppItemData = &m_aShoptabs[tabIdx].items[dwSlotPos];

	return TRUE;
}
//
//BOOL CPythonShop::CheckSlotIndex(DWORD dwSlotPos)
//{
//	if (dwSlotPos >= SHOP_HOST_ITEM_MAX_NUM * SHOP_TAB_COUNT_MAX)
//		return FALSE;
//
//	return TRUE;
//}

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
void CPythonShop::SetOfflineShopItemData(DWORD dwIndex, const TOfflineShopItemData& c_rShopItemData)
{
	BYTE tabIdx = dwIndex / OFFLINE_SHOP_HOST_ITEM_MAX_NUM;
	DWORD dwSlotPos = dwIndex % OFFLINE_SHOP_HOST_ITEM_MAX_NUM;

	SetOfflineShopItemData(tabIdx, dwSlotPos, c_rShopItemData);
}

void CPythonShop::SetOfflineShopItemData(BYTE tabIdx, DWORD dwSlotPos, const TOfflineShopItemData& c_rShopItemData)
{
	if (tabIdx >= SHOP_TAB_COUNT_MAX || dwSlotPos >= OFFLINE_SHOP_HOST_ITEM_MAX_NUM)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d. dwSlotPos(%d) must be less than %d", tabIdx, SHOP_TAB_COUNT_MAX, dwSlotPos, OFFLINE_SHOP_HOST_ITEM_MAX_NUM);
		return;
	}

	m_aOfflineShoptabs[tabIdx].items[dwSlotPos] = c_rShopItemData;
}

BOOL CPythonShop::GetOfflineShopItemData(DWORD dwIndex, const TOfflineShopItemData** c_ppItemData)
{
	BYTE tabIdx = dwIndex / OFFLINE_SHOP_HOST_ITEM_MAX_NUM;
	DWORD dwSlotPos = dwIndex % OFFLINE_SHOP_HOST_ITEM_MAX_NUM;

	return GetOfflineShopItemData(tabIdx, dwSlotPos, c_ppItemData);
}

BOOL CPythonShop::GetOfflineShopItemData(BYTE tabIdx, DWORD dwSlotPos, const TOfflineShopItemData** c_ppItemData)
{
	if (tabIdx >= SHOP_TAB_COUNT_MAX || dwSlotPos >= OFFLINE_SHOP_HOST_ITEM_MAX_NUM)
	{
		TraceError("Out of Index. tabIdx(%d) must be less than %d. dwSlotPos(%d) must be less than %d", tabIdx, SHOP_TAB_COUNT_MAX, dwSlotPos, OFFLINE_SHOP_HOST_ITEM_MAX_NUM);
		return FALSE;
	}

	if (m_aOfflineShoptabs[tabIdx].items[dwSlotPos].vnum)
	{
		*c_ppItemData = &m_aOfflineShoptabs[tabIdx].items[dwSlotPos];
		return TRUE;
	}

	*c_ppItemData = NULL;
	return FALSE;
}
#endif

void CPythonShop::ClearPrivateShopStock()
{
	m_PrivateShopItemStock.clear();
}
void CPythonShop::AddPrivateShopItemStock(TItemPos ItemPos, BYTE dwDisplayPos, long long dwPrice)
{
	DelPrivateShopItemStock(ItemPos);

	TShopItemTable SellingItem;
	SellingItem.vnum = 0;
	SellingItem.count = 0;
	SellingItem.pos = ItemPos;
	SellingItem.price = dwPrice;
	SellingItem.display_pos = dwDisplayPos;
	m_PrivateShopItemStock.insert(std::make_pair(ItemPos, SellingItem));
}
void CPythonShop::DelPrivateShopItemStock(TItemPos ItemPos)
{
	if (m_PrivateShopItemStock.end() == m_PrivateShopItemStock.find(ItemPos))
		return;

	m_PrivateShopItemStock.erase(ItemPos);
}
long long CPythonShop::GetPrivateShopItemPrice(TItemPos ItemPos)
{
	TPrivateShopItemStock::iterator itor = m_PrivateShopItemStock.find(ItemPos);

	if (m_PrivateShopItemStock.end() == itor)
		return 0;

	TShopItemTable & rShopItemTable = itor->second;
	return rShopItemTable.price;
}
struct ItemStockSortFunc
{
	bool operator() (TShopItemTable & rkLeft, TShopItemTable & rkRight)
	{
		return rkLeft.display_pos < rkRight.display_pos;
	}
};
void CPythonShop::BuildPrivateShop(const char * c_szName)
{
	std::vector<TShopItemTable> ItemStock;
	ItemStock.reserve(m_PrivateShopItemStock.size());

	TPrivateShopItemStock::iterator itor = m_PrivateShopItemStock.begin();
	for (; itor != m_PrivateShopItemStock.end(); ++itor)
	{
		ItemStock.push_back(itor->second);
	}

	std::sort(ItemStock.begin(), ItemStock.end(), ItemStockSortFunc());

	CPythonNetworkStream::Instance().SendBuildPrivateShopPacket(c_szName, ItemStock);
}

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
struct OfflineShopItemStockSortFunc
{
	bool operator() (TOfflineShopItemTable& rkLeft, TOfflineShopItemTable& rkRight) {
		return rkLeft.display_pos < rkRight.display_pos;
	}
};

void CPythonShop::ClearOfflineShopStock()
{
	m_OfflineShopItemStock.clear();
}

void CPythonShop::AddOfflineShopItemStock(TItemPos ItemPos, BYTE dwDisplayPos, long long dwPrice)
{
	DelOfflineShopItemStock(ItemPos);

	TOfflineShopItemTable SellingItem;
	SellingItem.vnum = 0;
	SellingItem.count = 0;
	SellingItem.pos = ItemPos;
	SellingItem.price = dwPrice;
	SellingItem.display_pos = dwDisplayPos;
	m_OfflineShopItemStock.insert(std::make_pair(ItemPos, SellingItem));
}

void CPythonShop::DelOfflineShopItemStock(TItemPos ItemPos)
{
	if (m_OfflineShopItemStock.end() == m_OfflineShopItemStock.find(ItemPos))
		return;

	m_OfflineShopItemStock.erase(ItemPos);
}

long long CPythonShop::GetOfflineShopItemPrice(TItemPos ItemPos)
{
	TOfflineShopItemStock::iterator itor = m_OfflineShopItemStock.find(ItemPos);

	if (m_OfflineShopItemStock.end() == itor)
		return 0;

	TOfflineShopItemTable& rShopItemTable = itor->second;
	return rShopItemTable.price;
}

void CPythonShop::BuildOfflineShop(const char* c_szName, BYTE bNpcType, BYTE bBoardStyle)
{
	std::vector<TOfflineShopItemTable> ItemStock;
	ItemStock.reserve(m_OfflineShopItemStock.size());

	TOfflineShopItemStock::iterator itor = m_OfflineShopItemStock.begin();
	for (; itor != m_OfflineShopItemStock.end(); ++itor) { ItemStock.push_back(itor->second); }

	std::sort(ItemStock.begin(), ItemStock.end(), OfflineShopItemStockSortFunc());

	CPythonNetworkStream::Instance().SendBuildOfflineShopPacket(c_szName, ItemStock, bNpcType, bBoardStyle);
}
#endif

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
void CPythonShop::Open(BOOL isPrivateShop, BOOL isMainPrivateShop, BOOL isOfflineShop)
#else
void CPythonShop::Open(BOOL isPrivateShop, BOOL isMainPrivateShop)
#endif
{
	m_isShoping = TRUE;
	m_isPrivateShop = isPrivateShop;
	m_isMainPlayerPrivateShop = isMainPrivateShop;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	m_isOfflineShop = isOfflineShop;
#endif
}

void CPythonShop::Close()
{
	m_isShoping = FALSE;
	m_isPrivateShop = FALSE;
	m_isMainPlayerPrivateShop = FALSE;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	m_isOfflineShop = FALSE;
#endif
}

BOOL CPythonShop::IsOpen()
{
	return m_isShoping;
}

BOOL CPythonShop::IsPrivateShop()
{
	return m_isPrivateShop;
}

BOOL CPythonShop::IsMainPlayerPrivateShop()
{
	return m_isMainPlayerPrivateShop;
}

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
BOOL CPythonShop::IsOfflineShop()
{
	return m_isOfflineShop;
}
#endif

void CPythonShop::Clear()
{
	m_isShoping = FALSE;
	m_isPrivateShop = FALSE;
	m_isMainPlayerPrivateShop = FALSE;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	m_isOfflineShop = FALSE;
#endif
	ClearPrivateShopStock();
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	ClearOfflineShopStock();
#endif
	m_bTabCount = 1;

	for (int i = 0; i < SHOP_TAB_COUNT_MAX; i++)
	{
		// @fixme016 BEGIN
		m_aShoptabs[i].coinType = SHOP_COIN_TYPE_GOLD;
		m_aShoptabs[i].name = "";
		// @fixme016 END
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		m_aOfflineShoptabs[i].coinType = SHOP_COIN_TYPE_GOLD;
		m_aOfflineShoptabs[i].name = "";
#endif
		memset (m_aShoptabs[i].items, 0, sizeof(TShopItemData) * SHOP_HOST_ITEM_MAX_NUM);
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		memset(m_aOfflineShoptabs[i].items, 0, sizeof(TOfflineShopItemData) * OFFLINE_SHOP_HOST_ITEM_MAX_NUM);
#endif
	}
}

CPythonShop::CPythonShop(void)
{
	Clear();
}

CPythonShop::~CPythonShop(void)
{
#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
	ClearShopSearchData();
#endif
}

#if defined(ENABLE_RENEWAL_SHOPEX)
PyObject* shopGetItemPriceType(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->price_type);

	return Py_BuildValue("i", 1);
}
PyObject* shopGetItemPriceVnum(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->price_vnum);

	return Py_BuildValue("i", 0);
}
#endif

PyObject * shopOpen(PyObject * poSelf, PyObject * poArgs)
{
	int isPrivateShop = false;
	PyTuple_GetInteger(poArgs, 0, &isPrivateShop);
	int isMainPrivateShop = false;
	PyTuple_GetInteger(poArgs, 1, &isMainPrivateShop);
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	int isOfflineShop = false;
	PyTuple_GetInteger(poArgs, 2, &isOfflineShop);
#endif

	CPythonShop& rkShop=CPythonShop::Instance();
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	rkShop.Open(isPrivateShop, isMainPrivateShop, isOfflineShop);
#else
	rkShop.Open(isPrivateShop, isMainPrivateShop);
#endif
	return Py_BuildNone();
}

PyObject * shopClose(PyObject * poSelf, PyObject * poArgs)
{
	CPythonShop& rkShop=CPythonShop::Instance();
	rkShop.Close();
	return Py_BuildNone();
}

PyObject * shopIsOpen(PyObject * poSelf, PyObject * poArgs)
{
	CPythonShop& rkShop=CPythonShop::Instance();
	return Py_BuildValue("i", rkShop.IsOpen());
}

PyObject * shopIsPrviateShop(PyObject * poSelf, PyObject * poArgs)
{
	CPythonShop& rkShop=CPythonShop::Instance();
	return Py_BuildValue("i", rkShop.IsPrivateShop());
}

PyObject * shopIsMainPlayerPrivateShop(PyObject * poSelf, PyObject * poArgs)
{
	CPythonShop& rkShop=CPythonShop::Instance();
	return Py_BuildValue("i", rkShop.IsMainPlayerPrivateShop());
}

PyObject * shopGetItemID(PyObject * poSelf, PyObject * poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TShopItemData * c_pItemData;
	if (CPythonShop::Instance().GetItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->vnum);

	return Py_BuildValue("i", 0);
}

PyObject * shopGetItemCount(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const TShopItemData * c_pItemData;
	if (CPythonShop::Instance().GetItemData(iIndex, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->count);

	return Py_BuildValue("i", 0);
}

PyObject * shopGetItemPrice(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const TShopItemData * c_pItemData;
	if (CPythonShop::Instance().GetItemData(iIndex, &c_pItemData))
		return Py_BuildValue("L", c_pItemData->price);

	return Py_BuildValue("i", 0);
}

PyObject * shopGetItemMetinSocket(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BuildException();

	const TShopItemData * c_pItemData;
	if (CPythonShop::Instance().GetItemData(iIndex, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->alSockets[iMetinSocketIndex]);

	return Py_BuildValue("i", 0);
}

#ifdef ENABLE_MULTISHOP
PyObject* shopGetBuyWithItem(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->wPriceVnum);

	return Py_BuildValue("i", 0);
}

PyObject* shopGetBuyWithItemCount(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->wPrice);

	return Py_BuildValue("i", 0);
}
#endif

PyObject * shopGetItemAttribute(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	int iAttrSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotIndex))
		return Py_BuildException();

	if (iAttrSlotIndex >= 0 && iAttrSlotIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM)
	{
		const TShopItemData * c_pItemData;
		if (CPythonShop::Instance().GetItemData(iIndex, &c_pItemData))
			return Py_BuildValue("ii", c_pItemData->aAttr[iAttrSlotIndex].bType, c_pItemData->aAttr[iAttrSlotIndex].sValue);
	}

	return Py_BuildValue("ii", 0, 0);
}

PyObject * shopClearPrivateShopStock(PyObject * poSelf, PyObject * poArgs)
{
	CPythonShop::Instance().ClearPrivateShopStock();
	return Py_BuildNone();
}
PyObject * shopAddPrivateShopItemStock(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();
	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();
	int iDisplaySlotIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iDisplaySlotIndex))
		return Py_BuildException();
	long long iPrice;
	if (!PyTuple_GetLongLong(poArgs, 3, &iPrice))
		return Py_BuildException();

	CPythonShop::Instance().AddPrivateShopItemStock(TItemPos(bItemWindowType, wItemSlotIndex), iDisplaySlotIndex, iPrice);
	return Py_BuildNone();
}
PyObject * shopDelPrivateShopItemStock(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();
	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	CPythonShop::Instance().DelPrivateShopItemStock(TItemPos(bItemWindowType, wItemSlotIndex));
	return Py_BuildNone();
}
PyObject * shopGetPrivateShopItemPrice(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();
	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	long long iValue = CPythonShop::Instance().GetPrivateShopItemPrice(TItemPos(bItemWindowType, wItemSlotIndex));
	return Py_BuildValue("L", iValue);
}
PyObject * shopBuildPrivateShop(PyObject * poSelf, PyObject * poArgs)
{
	char * szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	CPythonShop::Instance().BuildPrivateShop(szName);
	return Py_BuildNone();
}

PyObject * shopGetTabCount(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonShop::instance().GetTabCount());
}

PyObject * shopGetTabName(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bTabIdx;
	if (!PyTuple_GetInteger(poArgs, 0, &bTabIdx))
		return Py_BuildException();

	return Py_BuildValue("s", CPythonShop::instance().GetTabName(bTabIdx));
}

PyObject * shopGetTabCoinType(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bTabIdx;
	if (!PyTuple_GetInteger(poArgs, 0, &bTabIdx))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonShop::instance().GetTabCoinType(bTabIdx));
}

#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
#include "../GameLib/ItemManager.h"
void CPythonShop::ClearShopSearchData()
{
	for (auto obj : vShopSearch)
		delete obj;
	vShopSearch.clear();
	ShopSearchChangePage(1);
}

void CPythonShop::ShopSearchChangePage(int iPage)
{
	iShopSearchPage = iPage;
}

void CPythonShop::SetShopSearchItemData(ShopSearchData* sShopData)
{
	vShopSearch.push_back(sShopData);
}

ShopSearchData* CPythonShop::GetShopSearchItemData(DWORD dwIndex)
{
	dwIndex += (iShopSearchPage - 1) * 10;
	if (dwIndex >= vShopSearch.size())
		return nullptr;

	return vShopSearch.at(dwIndex);
}

void CPythonShop::SortShopSearchData()
{
	/*Sort by ASC
	1) Won
	2) Gold
	*/
	std::sort(vShopSearch.begin(), vShopSearch.end(), [](const ShopSearchData* a, const ShopSearchData* b)
		{
#if defined(ENABLE_CHEQUE_SYSTEM)
			return (a->item.byChequePrice < b->item.byChequePrice) || (a->item.byChequePrice == b->item.byChequePrice && a->item.price < b->item.price);
#else
			return (a->item.price < b->item.price);
#endif
		});
}

PyObject* shopGetPrivateShopSearchResult(PyObject* poSelf, PyObject* poArgs)
{
	int iLine;
	if (!PyTuple_GetInteger(poArgs, 0, &iLine))
		return Py_BuildException();

	auto ShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iLine);
	if (ShopSearchData)
	{
		CItemData* pItemData;
		if (CItemManager::Instance().GetItemDataPointer(ShopSearchData->item.vnum, &pItemData))
#if defined(ENABLE_CHEQUE_SYSTEM)
			return Py_BuildValue("ssiLii", pItemData->GetName(), ShopSearchData->name.c_str(), ShopSearchData->item.count, ShopSearchData->item.price, ShopSearchData->item.byChequePrice, ShopSearchData->dwShopPID);
#else
			return Py_BuildValue("ssiLi", pItemData->GetName(), ShopSearchData->name.c_str(), ShopSearchData->item.count, ShopSearchData->item.price, ShopSearchData->dwShopPID);
#endif
	}

#if defined(ENABLE_CHEQUE_SYSTEM)
	return Py_BuildValue("ssiiii", "", "", 0, 0, 0, 0);
#else
	return Py_BuildValue("ssiii", "", "", 0, 0, 0);
#endif
}

PyObject* shopGetPrivateShopSelectItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	int iAttrSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotIndex))
		return Py_BuildException();

	if (iAttrSlotIndex >= 0 && iAttrSlotIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM)
	{
		auto ShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
		if (ShopSearchData)
			return Py_BuildValue("ii", ShopSearchData->item.aAttr[iAttrSlotIndex].bType, ShopSearchData->item.aAttr[iAttrSlotIndex].sValue);
	}

	return Py_BuildValue("ii", 0, 0);
}

PyObject* shopGetPrivateShopSelectItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BuildException();

	if (iMetinSocketIndex >= 0 && iMetinSocketIndex < ITEM_SOCKET_SLOT_MAX_NUM)
	{
		auto ShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
		if (ShopSearchData)
			return Py_BuildValue("i", ShopSearchData->item.alSockets[iMetinSocketIndex]);
	}

	return Py_BuildValue("i", 0);
}

PyObject* shopGetPrivateShopSelectItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	auto ShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
	if (ShopSearchData)
		return Py_BuildValue("i", ShopSearchData->item.vnum);

	return Py_BuildValue("i", 0);
}

PyObject* shopGetPrivateShopSearchResultMaxCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonShop::Instance().GetShopSearchResultCount());
}

PyObject* shopGetPrivateShopSearchResultPage(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonShop::Instance().GetShopSearchPage());
}

#if defined(__BL_TRANSMUTATION__)
PyObject* shopGetPrivateShopItemChangeLookVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	auto ShopSearchData = CPythonShop::Instance().GetShopSearchItemData(iIndex);
	if (ShopSearchData)
		return Py_BuildValue("i", ShopSearchData->item.dwTransmutationVnum);

	return Py_BuildValue("i", 0);
}
#endif
#endif

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
PyObject* shopIsOfflineShop(PyObject* poSelf, PyObject* poArgs)
{
	CPythonShop& rkShop = CPythonShop::Instance();
	return Py_BuildValue("i", rkShop.IsOfflineShop());
}

PyObject* shopGetOfflineShopItemID(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TOfflineShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetOfflineShopItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->vnum);

	return Py_BuildValue("i", 0);
}

PyObject* shopGetOfflineShopItemCount(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const TOfflineShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetOfflineShopItemData(iIndex, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->count);

	return Py_BuildValue("i", 0);
}

PyObject* shopGetOfflineShopItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const TOfflineShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetOfflineShopItemData(iIndex, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->vnum);

	return Py_BuildValue("i", 0);
}

PyObject* shopGetOfflineShopItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const TOfflineShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetOfflineShopItemData(iIndex, &c_pItemData))
		return PyLong_FromLongLong(c_pItemData->price);

	return Py_BuildValue("i", 0);
}

PyObject* shopGetOfflineShopItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BuildException();

	const TOfflineShopItemData* c_pItemData;
	if (CPythonShop::Instance().GetOfflineShopItemData(iIndex, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->alSockets[iMetinSocketIndex]);

	return Py_BuildValue("i", 0);
}

PyObject* shopGetOfflineShopItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iAttrSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotIndex))
		return Py_BuildException();

	if (iAttrSlotIndex >= 0 && iAttrSlotIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM)
	{
		const TOfflineShopItemData* c_pItemData;
		if (CPythonShop::Instance().GetOfflineShopItemData(iIndex, &c_pItemData))
			return Py_BuildValue("ii", c_pItemData->aAttr[iAttrSlotIndex].bType, c_pItemData->aAttr[iAttrSlotIndex].sValue);
	}

	return Py_BuildValue("ii", 0, 0);
}

PyObject* shopClearOfflineShopStock(PyObject* poSelf, PyObject* poArgs)
{
	CPythonShop::Instance().ClearOfflineShopStock();
	return Py_BuildNone();
}

PyObject* shopAddOfflineShopItemStock(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	int iDisplaySlotIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iDisplaySlotIndex))
		return Py_BuildException();

	long long iPrice;
	if (!PyTuple_GetLongLong(poArgs, 3, &iPrice))
		return Py_BuildException();

	if (iPrice > 125000000000000)
		iPrice = 125000000000000;
	else if (iPrice <= 0)
		iPrice = 1;

	CPythonShop::Instance().AddOfflineShopItemStock(TItemPos(bItemWindowType, wItemSlotIndex), iDisplaySlotIndex, iPrice);
	return Py_BuildNone();
}

PyObject* shopDelOfflineShopItemStock(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	CPythonShop::Instance().DelOfflineShopItemStock(TItemPos(bItemWindowType, wItemSlotIndex));
	return Py_BuildNone();
}

PyObject* shopGetOfflineShopItemPriceReal(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	long long iValue = CPythonShop::Instance().GetOfflineShopItemPrice(TItemPos(bItemWindowType, wItemSlotIndex));
	return PyLong_FromLongLong(iValue);
}

PyObject* shopGetOfflineShopItemGetStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	return Py_BuildValue("i", 0);
}

PyObject* shopGetOfflineShopItemGetBuyerName(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	return Py_BuildValue("s", "None");
}

PyObject* shopBuildOfflineShop(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	BYTE bNpcType;
	if (!PyTuple_GetInteger(poArgs, 1, &bNpcType))
		return Py_BuildException();

	BYTE bBoardStyle;
	if (!PyTuple_GetInteger(poArgs, 2, &bBoardStyle))
		return Py_BuildException();

	CPythonShop::Instance().BuildOfflineShop(szName, bNpcType, bBoardStyle);
	return Py_BuildNone();
}
#endif

void initshop()
{
	static PyMethodDef s_methods[] =
	{
		// Shop
		{ "Open",						shopOpen,						METH_VARARGS },
		{ "Close",						shopClose,						METH_VARARGS },
		{ "IsOpen",						shopIsOpen,						METH_VARARGS },
		{ "IsPrivateShop",				shopIsPrviateShop,				METH_VARARGS },
		{ "IsMainPlayerPrivateShop",	shopIsMainPlayerPrivateShop,	METH_VARARGS },
		{ "GetItemID",					shopGetItemID,					METH_VARARGS },
		{ "GetItemCount",				shopGetItemCount,				METH_VARARGS },
		{ "GetItemPrice",				shopGetItemPrice,				METH_VARARGS },
#ifdef ENABLE_MULTISHOP
		{ "GetBuyWithItem",				shopGetBuyWithItem,				METH_VARARGS },
		{ "GetBuyWithItemCount",		shopGetBuyWithItemCount,		METH_VARARGS },
#endif
		{ "GetItemMetinSocket",			shopGetItemMetinSocket,			METH_VARARGS },
		{ "GetItemAttribute",			shopGetItemAttribute,			METH_VARARGS },
		{ "GetTabCount",				shopGetTabCount,				METH_VARARGS },
		{ "GetTabName",					shopGetTabName,					METH_VARARGS },
		{ "GetTabCoinType",				shopGetTabCoinType,				METH_VARARGS },

		// Private Shop
		{ "ClearPrivateShopStock",		shopClearPrivateShopStock,		METH_VARARGS },
		{ "AddPrivateShopItemStock",	shopAddPrivateShopItemStock,	METH_VARARGS },
		{ "DelPrivateShopItemStock",	shopDelPrivateShopItemStock,	METH_VARARGS },
		{ "GetPrivateShopItemPrice",	shopGetPrivateShopItemPrice,	METH_VARARGS },
		{ "BuildPrivateShop",			shopBuildPrivateShop,			METH_VARARGS },
#if defined(ENABLE_RENEWAL_SHOPEX)
		{ "GetItemPriceType",			shopGetItemPriceType,			METH_VARARGS },
		{ "GetItemPriceVnum",			shopGetItemPriceVnum,			METH_VARARGS },
#endif
#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
		{ "GetPrivateShopSearchResult",	shopGetPrivateShopSearchResult,	METH_VARARGS },
		{ "GetPrivateShopSelectItemAttribute",	shopGetPrivateShopSelectItemAttribute,	METH_VARARGS },
		{ "GetPrivateShopSelectItemMetinSocket",	shopGetPrivateShopSelectItemMetinSocket,	METH_VARARGS },
		{ "GetPrivateShopSelectItemVnum",	shopGetPrivateShopSelectItemVnum,	METH_VARARGS },
		{ "GetPrivateShopSearchResultMaxCount",	shopGetPrivateShopSearchResultMaxCount,	METH_VARARGS },
		{ "GetPrivateShopSearchResultPage",	shopGetPrivateShopSearchResultPage,	METH_VARARGS },
#if defined(__BL_TRANSMUTATION__)
		{ "GetPrivateShopItemChangeLookVnum", shopGetPrivateShopItemChangeLookVnum, METH_VARARGS },
#endif
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		// Offline Shop
		{ "IsOfflineShop",						shopIsOfflineShop,						METH_VARARGS },
		{ "GetOfflineShopItemID",				shopGetOfflineShopItemID,				METH_VARARGS },
		{ "GetOfflineShopItemCount",			shopGetOfflineShopItemCount,			METH_VARARGS },
		{ "GetOfflineShopItemVnum",				shopGetOfflineShopItemVnum,				METH_VARARGS },
		{ "GetOfflineShopItemPrice",			shopGetOfflineShopItemPrice,			METH_VARARGS },
		{ "GetOfflineShopItemMetinSocket",		shopGetOfflineShopItemMetinSocket,		METH_VARARGS },
		{ "GetOfflineShopItemAttribute",		shopGetOfflineShopItemAttribute,		METH_VARARGS },

		{ "ClearOfflineShopStock",				shopClearOfflineShopStock,				METH_VARARGS },
		{ "AddOfflineShopItemStock",			shopAddOfflineShopItemStock,			METH_VARARGS },
		{ "DelOfflineShopItemStock",			shopDelOfflineShopItemStock,			METH_VARARGS },
		{ "GetOfflineShopItemPriceReal",		shopGetOfflineShopItemPriceReal,		METH_VARARGS },
		{ "GetOfflineShopItemStatus",			shopGetOfflineShopItemGetStatus,		METH_VARARGS },
		{ "GetOfflineShopItemBuyerName",		shopGetOfflineShopItemGetBuyerName,		METH_VARARGS },
		{ "BuildOfflineShop",					shopBuildOfflineShop,					METH_VARARGS },
#endif
		{ NULL,							NULL,							NULL },
	};
	PyObject * poModule = Py_InitModule("shop", s_methods);

	PyModule_AddIntConstant(poModule, "SHOP_SLOT_COUNT", SHOP_HOST_ITEM_MAX_NUM);
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	PyModule_AddIntConstant(poModule, "OFFLINE_SHOP_SLOT_COUNT", OFFLINE_SHOP_HOST_ITEM_MAX_NUM);
#endif
	PyModule_AddIntConstant(poModule, "SHOP_COIN_TYPE_GOLD", SHOP_COIN_TYPE_GOLD);
	PyModule_AddIntConstant(poModule, "SHOP_COIN_TYPE_SECONDARY_COIN", SHOP_COIN_TYPE_SECONDARY_COIN);
#if defined(ENABLE_RENEWAL_SHOPEX)
	PyModule_AddIntConstant(poModule, "SHOPEX_GOLD", SHOPEX_GOLD);
	PyModule_AddIntConstant(poModule, "SHOPEX_SECONDCOIN", SHOPEX_SECONDARY);
	PyModule_AddIntConstant(poModule, "SHOPEX_ITEM", SHOPEX_ITEM);
	PyModule_AddIntConstant(poModule, "SHOPEX_EXP", SHOPEX_EXP);
	PyModule_AddIntConstant(poModule, "SHOPEX_MAX", SHOPEX_MAX);
#endif
}
//martysama0134's 2022
