#pragma once

#include "Packet.h"


typedef enum
{
	SHOP_COIN_TYPE_GOLD, // DEFAULT VALUE
	SHOP_COIN_TYPE_SECONDARY_COIN,
} EShopCoinType;

class CPythonShop : public CSingleton<CPythonShop>
{
	public:
		CPythonShop(void);
		virtual ~CPythonShop(void);

		void Clear();

		void SetItemData(DWORD dwIndex, const TShopItemData & c_rShopItemData);
		BOOL GetItemData(DWORD dwIndex, const TShopItemData ** c_ppItemData);

		void SetItemData(BYTE tabIdx, DWORD dwSlotPos, const TShopItemData & c_rShopItemData);
		BOOL GetItemData(BYTE tabIdx, DWORD dwSlotPos, const TShopItemData ** c_ppItemData);
#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
		void SetShopSearchItemData(ShopSearchData* sShopData);
		ShopSearchData* GetShopSearchItemData(DWORD dwIndex);
		void ClearShopSearchData();
		void SortShopSearchData();
		void ShopSearchChangePage(int iPage);
		size_t GetShopSearchResultCount() const { return vShopSearch.size(); }
		int GetShopSearchPage() const { return iShopSearchPage; }
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		void SetOfflineShopItemData(DWORD dwIndex, const TOfflineShopItemData& c_rShopItemData);
		BOOL GetOfflineShopItemData(DWORD dwIndex, const TOfflineShopItemData** c_ppItemData);

		void SetOfflineShopItemData(BYTE tabIdx, DWORD dwSlotPos, const TOfflineShopItemData& c_rShopItemData);
		BOOL GetOfflineShopItemData(BYTE tabIdx, DWORD dwSlotPos, const TOfflineShopItemData** c_ppItemData);
#endif

		void SetTabCount(BYTE bTabCount) { m_bTabCount = bTabCount; }
		BYTE GetTabCount() { return m_bTabCount; }

		void SetTabCoinType(BYTE tabIdx, BYTE coinType);
		BYTE GetTabCoinType(BYTE tabIdx);

		void SetTabName(BYTE tabIdx, const char* name);
		const char* GetTabName(BYTE tabIdx);

		//BOOL GetSlotItemID(DWORD dwSlotPos, DWORD* pdwItemID);

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		void Open(BOOL isPrivateShop, BOOL isMainPrivateShop, BOOL isOfflineShop);
#else
		void Open(BOOL isPrivateShop, BOOL isMainPrivateShop);
#endif
		void Close();
		BOOL IsOpen();
		BOOL IsPrivateShop();
		BOOL IsMainPlayerPrivateShop();
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		BOOL IsOfflineShop();
#endif

		void ClearPrivateShopStock();
		void AddPrivateShopItemStock(TItemPos ItemPos, BYTE byDisplayPos, long long dwPrice);
		void DelPrivateShopItemStock(TItemPos ItemPos);
		long long GetPrivateShopItemPrice(TItemPos ItemPos);
		void BuildPrivateShop(const char * c_szName);

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		void ClearOfflineShopStock();
		void AddOfflineShopItemStock(TItemPos ItemPos, BYTE byDisplayPos, long long dwPrice);
		void DelOfflineShopItemStock(TItemPos ItemPos);
		long long GetOfflineShopItemPrice(TItemPos ItemPos);
		BYTE GetOfflineShopItemStatus(TItemPos ItemPos);
		void BuildOfflineShop(const char* c_szName, BYTE bNpcType, BYTE bBoardStyle);
#endif

	protected:
		BOOL	CheckSlotIndex(DWORD dwIndex);

	protected:
		BOOL				m_isShoping;
		BOOL				m_isPrivateShop;
		BOOL				m_isMainPlayerPrivateShop;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		BOOL				m_isOfflineShop;
#endif

		struct ShopTab
		{
			ShopTab()
			{
				coinType = SHOP_COIN_TYPE_GOLD;
			}
			BYTE				coinType;
			std::string			name;
			TShopItemData		items[SHOP_HOST_ITEM_MAX_NUM];
		};

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		struct OfflineShopTab
		{
			OfflineShopTab()
			{
				coinType = SHOP_COIN_TYPE_GOLD;
			}
			BYTE				coinType;
			std::string			name;
			TOfflineShopItemData items[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
		};
#endif

		BYTE m_bTabCount;
		ShopTab m_aShoptabs[SHOP_TAB_COUNT_MAX];
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		OfflineShopTab m_aOfflineShoptabs[SHOP_TAB_COUNT_MAX];
#endif

		typedef std::map<TItemPos, TShopItemTable> TPrivateShopItemStock;
		TPrivateShopItemStock	m_PrivateShopItemStock;
#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
		std::vector<ShopSearchData*> vShopSearch;
		int iShopSearchPage;
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		typedef std::map<TItemPos, TOfflineShopItemTable> TOfflineShopItemStock;
		TOfflineShopItemStock	m_OfflineShopItemStock;
#endif
};
//martysama0134's 2022
