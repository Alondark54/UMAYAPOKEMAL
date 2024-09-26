enum
{
	OFFLINE_SHOP_MAX_DISTANCE = 1500,
	OFFLINE_SHOP_BUY_FEE = 3,
};

class COfflineShop
{
public:
	COfflineShop();
	~COfflineShop();

	virtual void	SetOfflineShopNPC(LPCHARACTER npc);
	virtual bool	IsOfflineShopNPC() { return m_pkOfflineShopNPC ? true : false; }
	LPCHARACTER		GetOfflineShopNPC() { return m_pkOfflineShopNPC; }

	void			SetOfflineShopItems(DWORD dwOwnerPID, TOfflineShopItemTable* pTable, BYTE bItemCount);
	void			AddItem(LPCHARACTER ch, LPITEM pkItem, BYTE bPos, long long iPrice);
	void			RemoveItem(LPCHARACTER ch, BYTE bPos);

	virtual bool	AddGuest(LPCHARACTER ch, LPCHARACTER npc);

	void			RemoveGuest(LPCHARACTER ch);
	void			RemoveAllGuest();
	void			Destroy(LPCHARACTER npc);

	virtual int32_t	Buy(LPCHARACTER ch, BYTE bPos, DWORD item_id = 0, long long llPriceCheck = 0);

	void			BroadcastUpdateItem(BYTE bPos, DWORD dwPID, bool bDestroy = false);
	void			BroadcastUpdatePrice(DWORD dwPID, BYTE bPos, long long dwPrice);
	void			Refresh(LPCHARACTER ch);

	bool			RemoveItem(DWORD dwVID, BYTE bPos);
	BYTE			GetLeftItemCount(DWORD dwPID);
	void			SetOfflineShopMapIndex(int32_t idx) { m_llMapIndex = idx; }
	int32_t			GetOfflineShopMapIndex() const { return m_llMapIndex; }
	void			SetOfflineShopTime(int32_t iTime) { m_iTime = iTime; }
	int32_t			GetOfflineShopTime() const { return m_iTime; }

	std::string shopSign;
	const char* GetShopSign() { return shopSign.c_str(); };
	void			SetShopSign(const char* c) { shopSign = c; };

	void			SetGuestMap(LPCHARACTER ch);
	void			RemoveGuestMap(LPCHARACTER ch);

	virtual void	SetOfflineShopBorderStyle(BYTE bBorderStyle);
	BYTE			GetBorderStyle() const { return m_pkOfflineShopBorderStyle; }
	void			RefreshP2P(DWORD dwPID);
	void			AddItemP2P(DWORD dwPID, TOfflineShopItem pkItem, BYTE bPos, long long iPrice);
	void			RemoveItemP2P(DWORD dwPID, BYTE bPos);
	bool			ChangeItemPrice(DWORD dwPID, BYTE bPos, long long llPrice);

protected:
	void			Broadcast(const void* data, int bytes);

private:
	int32_t				m_llMapIndex;
	int32_t				m_iTime;

	// Guest Map
	typedef std::vector<DWORD> GuestMapType;
	GuestMapType m_map_guest;
	// End Of Guest Map

	LPCHARACTER m_pkOfflineShopNPC;
	BYTE	m_pkOfflineShopBorderStyle;
};

#pragma once