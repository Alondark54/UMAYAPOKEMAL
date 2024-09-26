#pragma once

#include "ItemData.h"

#include "../UserInterface/Locale_inc.h"
#if defined(__BL_CHEST_DROP_INFO__)
#include <unordered_map>
#endif

class CItemManager : public CSingleton<CItemManager>
{
	public:
		enum EItemDescCol
		{
			ITEMDESC_COL_VNUM,
			ITEMDESC_COL_NAME,
			ITEMDESC_COL_DESC,
			ITEMDESC_COL_SUMM,
			ITEMDESC_COL_NUM,
		};

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		enum EItemScaleColumn
		{
			ITEMSCALE_VNUM,
			ITEMSCALE_JOB,
			ITEMSCALE_SEX,
			ITEMSCALE_SCALE_X,
			ITEMSCALE_SCALE_Y,
			ITEMSCALE_SCALE_Z,
			ITEMSCALE_POSITION_X, // facultative
			ITEMSCALE_POSITION_Y, // facultative
			ITEMSCALE_POSITION_Z, // facultative
			ITEMSCALE_NUM,
			ITEMSCALE_REQ = ITEMSCALE_SCALE_Z + 1,
			ITEMSCALE_AURA_NUM = ITEMSCALE_POSITION_X + 1,
		};
#endif

	public:
		typedef std::map<DWORD, CItemData*> TItemMap;
		typedef std::map<std::string, CItemData*> TItemNameMap;

	public:
		CItemManager();
		virtual ~CItemManager();

		void			Destroy();

		BOOL			SelectItemData(DWORD dwIndex);
		CItemData *		GetSelectedItemDataPointer();

		BOOL			GetItemDataPointer(DWORD dwItemID, CItemData ** ppItemData);

		/////
		bool			LoadItemDesc(const char* c_szFileName);
		bool			LoadItemList(const char* c_szFileName);
		bool			LoadItemTable(const char* c_szFileName);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool			LoadItemScale(const char* c_szFileName);
#endif
		CItemData *		MakeItemData(DWORD dwIndex);

#if defined(__BL_CHEST_DROP_INFO__)
		struct SDropItemInfo
		{
			DWORD	dwDropVnum;
			int		iCount;
		};

		using							TChestDropItemInfoVec = std::vector<SDropItemInfo>;
		using							TChestDropItemInfoMap = std::unordered_map<DWORD, TChestDropItemInfoVec>;

		bool							LoadChestDropInfo(const char* c_szFileName);
		
		TChestDropItemInfoVec*			GetItemDropInfoVec(const DWORD dwVnum);
		TChestDropItemInfoVec*			GetBaseItemDropInfoVec(const DWORD dwVnum);
#endif

	protected:
		TItemMap m_ItemMap;
		std::vector<CItemData*>  m_vec_ItemRange;
		CItemData * m_pSelectedItemData;
#if defined(__BL_CHEST_DROP_INFO__)
		TChestDropItemInfoMap m_ItemDropInfoMap;
		TChestDropItemInfoMap m_BaseItemDropInfoMap;
#endif

};
//martysama0134's 2022
