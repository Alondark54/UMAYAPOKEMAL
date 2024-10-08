
class COfflineShopManager : public singleton<COfflineShopManager>
{
	public:
		typedef std::map<DWORD, COfflineShop *> TShopMap;
		typedef std::map<DWORD, DWORD> TOfflineShopMap;

	public:
		COfflineShopManager();
		~COfflineShopManager();

		bool StartShopping(LPCHARACTER pkChr, LPCHARACTER pkChrShopKeeper);
		void StopShopping(LPCHARACTER ch);

		void Buy(LPCHARACTER ch, BYTE bPos);

		LPOFFLINESHOP	CreateOfflineShop(LPCHARACTER npc, DWORD dwOwnerPID, TOfflineShopItemTable * pTable = NULL, short bItemCount = 0, int32_t lMapIndex = 0, int32_t iTime = 0, const char * szSign = "\0", BYTE bBoardStyle = 0);
		LPOFFLINESHOP	FindOfflineShop(DWORD dwVID);
		void			ChangeOfflineShopTime(LPCHARACTER ch, int32_t bTime);
		void			DestroyOfflineShop(LPCHARACTER ch, DWORD dwVID, bool bDestroyAll = false);

		void			AddItem(LPCHARACTER ch, BYTE bDisplayPos, TItemPos bPos, long long iPrice);
		void			RemoveItem(LPCHARACTER ch, BYTE bPos);
		void			Refresh(LPCHARACTER ch);
		void			RefreshMoney(LPCHARACTER ch);

		DWORD			FindMyOfflineShop(DWORD dwPID);
		void			WithdrawMoney(LPCHARACTER ch, long long llRequiredMoney);
		BYTE			LeftItemCount(LPCHARACTER ch);
		int32_t			GetShopCountChannel() const { return m_Map_pkOfflineShopByNPC2.size(); }

		void			RefreshUnsoldItems(LPCHARACTER ch);
		void			TakeItem(LPCHARACTER ch, BYTE bPos);
		bool			HasOfflineShop(LPCHARACTER ch);
		LPOFFLINESHOP	FindShop(DWORD dwVID);
		bool			AddGuest(LPCHARACTER ch, DWORD dwVID);
		LPCHARACTER		GetOfflineShopNPC(DWORD dwVID);
		
		int32_t			GetMapIndex(DWORD dwPID);
		bool			GetOfflineShopCoords(DWORD dwPID, int32_t* map_index, int32_t* x, int32_t* y);
		bool			GetOfflineShopCoordsP2P(LPCHARACTER ch, int32_t* map_index, int32_t* x, int32_t* y);
		int32_t			GetLeftTime(DWORD dwPID);
		const char*		GetOfflineShopSign(DWORD dwPID);
		bool			CanGetOfflineShop(DWORD dwPID);
		void			RefreshP2P(DWORD dwPID);
		void			AddItemP2P(DWORD dwPID, TOfflineShopItem pkItem, BYTE bPos, long long iPrice);
		void			RemoveItemP2P(DWORD dwPID, BYTE bPos);
		void			GiveItemP2P(DWORD dwPID, TPlayerItem pkItem);
		bool			CanCloseOfflineShopP2P(DWORD dwPID);
		void			ChangeOfflineShopName(DWORD dwPID, const char *shopName);
		bool			PlayerIsOnline(DWORD dwPID);
		void			AdviseItemSell(LPCHARACTER ch, DWORD itemVnum, long long itemPrice, const char * buyerName);
		DWORD			GetOfflineShopVID(DWORD dwPID);
		void			ChangeOfflineShopItemPrice(LPCHARACTER ch, BYTE bPos, long long llPrice);
		void			ChangeOfflineShopItemPriceP2P(DWORD dwPID, BYTE bPos, long long llPrice);
	private:
		TOfflineShopMap	m_Map_pkOfflineShopByNPC2;
		TShopMap		m_map_pkOfflineShopByNPC;
};
#pragma once