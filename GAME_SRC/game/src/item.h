#ifndef __INC_METIN_II_GAME_ITEM_H__
#define __INC_METIN_II_GAME_ITEM_H__

#include "entity.h"

#ifdef RENEWAL_BOOK_NAME
#include "skill.h"
#include "mob_manager.h"
#include "unique_item.h"
#endif

class CItem : public CEntity
{
	protected:
		// override methods from ENTITY class
		virtual void	EncodeInsertPacket(LPENTITY entity);
		virtual void	EncodeRemovePacket(LPENTITY entity);

	public:
		CItem(DWORD dwVnum);
		virtual ~CItem();

		int			GetLevelLimit();

		bool		CheckItemUseLevel(int nLevel);

		bool		IsPCBangItem();

		long		FindApplyValue(BYTE bApplyType);

		bool		IsStackable()		{ return (GetFlag() & ITEM_FLAG_STACKABLE)?true:false; }

		void		Initialize();
		void		Destroy();

		void		Save();

		void		SetWindow(BYTE b)	{ m_bWindow = b; }
		BYTE		GetWindow()		{ return m_bWindow; }

		void		SetID(DWORD id)		{ m_dwID = id;	}
		DWORD		GetID()			{ return m_dwID; }

		void			SetProto(const TItemTable * table);
		TItemTable const *	GetProto()	{ return m_pProto; }

		long long	GetGold();
		long long		GetShopBuyPrice();
		const char *	GetName()
		{ 
#ifdef RENEWAL_BOOK_NAME
			if (!m_pProto)
				return "";

			static char buf[64];
			switch (m_dwVnum)
			{
			case 70104:
				snprintf(buf, sizeof(buf), "%s %s", CMobManager::instance().GetName(GetSocket(0)), m_pProto->szLocaleName);
				return buf;
			case 50300:
			case ITEM_SKILLFORGET_VNUM:
				snprintf(buf, sizeof(buf), "%s %s", CSkillManager::instance().GetName(GetSocket(0)), m_pProto->szLocaleName);
				return buf;
			}

			return m_pProto->szLocaleName;
#else
			return m_pProto ? m_pProto->szLocaleName : "";
#endif
		}
		const char *	GetBaseName()		{ return m_pProto ? m_pProto->szName : NULL; }
		BYTE		GetSize()		{ return m_pProto ? m_pProto->bSize : 0;	}

		void		SetFlag(long flag)	{ m_lFlag = flag;	}
		long		GetFlag()		{ return m_lFlag;	}

		void		AddFlag(long bit);
		void		RemoveFlag(long bit);

		DWORD		GetWearFlag()		{ return m_pProto ? m_pProto->dwWearFlags : 0; }
		DWORD		GetAntiFlag()		{ return m_pProto ? m_pProto->dwAntiFlags : 0; }
		DWORD		GetImmuneFlag()		{ return m_pProto ? m_pProto->dwImmuneFlag : 0; }

		void		SetVID(DWORD vid)	{ m_dwVID = vid;	}
		DWORD		GetVID()		{ return m_dwVID;	}

		bool		SetCount(DWORD count);
		DWORD		GetCount();




		//
		DWORD		GetVnum() const		{ return m_dwMaskVnum ? m_dwMaskVnum : m_dwVnum;	}
		DWORD		GetOriginalVnum() const		{ return m_dwVnum;	}
		BYTE		GetType() const		{ return m_pProto ? m_pProto->bType : 0;	}
		BYTE		GetSubType() const	{ return m_pProto ? m_pProto->bSubType : 0;	}
		BYTE		GetLimitType(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].bType : 0;	}
		long		GetLimitValue(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].lValue : 0;	}

		long		GetValue(DWORD idx);

		void		SetCell(LPCHARACTER ch, WORD pos)	{ m_pOwner = ch, m_wCell = pos;	}
		WORD		GetCell()				{ return m_wCell;	}

		LPITEM		RemoveFromCharacter();
		bool		AddToCharacter(LPCHARACTER ch, TItemPos Cell);
		LPCHARACTER	GetOwner()		{ return m_pOwner; }

		LPITEM		RemoveFromGround();
		bool		AddToGround(long lMapIndex, const PIXEL_POSITION & pos, bool skipOwnerCheck = false);

		int			FindEquipCell(LPCHARACTER ch, int bCandidateCell = -1);
		bool		IsEquipped() const		{ return m_bEquipped;	}
		bool		EquipTo(LPCHARACTER ch, BYTE bWearCell);
		bool		IsEquipable() const;

		bool		CanUsedBy(LPCHARACTER ch);

		bool		DistanceValid(LPCHARACTER ch);

		void		UpdatePacket();
		void		UsePacketEncode(LPCHARACTER ch, LPCHARACTER victim, struct packet_item_use * packet);

		void		SetExchanging(bool isOn = true);
		bool		IsExchanging()		{ return m_bExchanging;	}

		bool		IsTwohanded();

		bool		IsPolymorphItem();

		void		ModifyPoints(bool bAdd);

		bool		CreateSocket(BYTE bSlot, BYTE bGold);
		const long *	GetSockets()		{ return &m_alSockets[0];	}
		long		GetSocket(int i)	{ return m_alSockets[i];	}

		void		SetSockets(const long * al);
		void		SetSocket(int i, long v, bool bLog = true);

		int		GetSocketCount();
		bool		AddSocket();

		const TPlayerItemAttribute* GetAttributes()		{ return m_aAttr;	}
		const TPlayerItemAttribute& GetAttribute(int i)	{ return m_aAttr[i];	}

		BYTE		GetAttributeType(int i)	{ return m_aAttr[i].bType;	}
		short		GetAttributeValue(int i){ return m_aAttr[i].sValue;	}

		void		SetAttributes(const TPlayerItemAttribute* c_pAttribute);

		int		FindAttribute(BYTE bType);
		bool		RemoveAttributeAt(int index);
		bool		RemoveAttributeType(BYTE bType);

		bool		HasAttr(BYTE bApply);
		bool		HasRareAttr(BYTE bApply);
#ifdef WJ_ELDER_ATTRIBUTE_SYSTEM
		bool		HasApply(BYTE bApply);
		bool		HasOnlyAttr(BYTE bApply);
#endif

		void		SetDestroyEvent(LPEVENT pkEvent);
		void		StartDestroyEvent(int iSec=40);

		DWORD		GetRefinedVnum()	{ return m_pProto ? m_pProto->dwRefinedVnum : 0; }
		DWORD		GetRefineFromVnum();
		int		GetRefineLevel();

		void		SetSkipSave(bool b)	{ m_bSkipSave = b; }
		bool		GetSkipSave()		{ return m_bSkipSave; }

		bool		IsOwnership(LPCHARACTER ch);
		void		SetOwnership(LPCHARACTER ch, int iSec = 30);
		void		SetOwnershipEvent(LPEVENT pkEvent);

		DWORD		GetLastOwnerPID()	{ return m_dwLastOwnerPID; }
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
		void		SetLastOwnerPID(DWORD pid) { m_dwLastOwnerPID = pid; }
#endif

		int		GetAttributeSetIndex();
		void		AlterToMagicItem();
		void		AlterToSocketItem(int iSocketCount);

		WORD		GetRefineSet()		{ return m_pProto ? m_pProto->wRefineSet : 0;	}

		void		StartUniqueExpireEvent();
		void		SetUniqueExpireEvent(LPEVENT pkEvent);

		void		StartTimerBasedOnWearExpireEvent();
		void		SetTimerBasedOnWearExpireEvent(LPEVENT pkEvent);

		void		StartRealTimeExpireEvent();
#ifdef __NEW_POTION__
		BYTE		GetApplyType(BYTE bIdx) { return m_pProto  ? m_pProto->aApplies[bIdx].bType : 0; }
		long		GetApplyValue(BYTE bIdx) { return m_pProto ? m_pProto->aApplies[bIdx].lValue : 0; }
#endif
		bool		IsRealTimeItem();

		void		StopUniqueExpireEvent();
		void		StopTimerBasedOnWearExpireEvent();
		void		StopAccessorySocketExpireEvent();


		int			GetDuration();

		int		GetAttributeCount();
		void		ClearAttribute();
		void		ChangeAttribute(const int* aiChangeProb=NULL);
		void		AddAttribute();
		void		AddAttribute(BYTE bType, short sValue);

		void		ApplyAddon(int iAddonType);

		int		GetSpecialGroup() const;
		bool	IsSameSpecialGroup(const LPITEM item) const;

		// ACCESSORY_REFINE

		bool		IsAccessoryForSocket();

		int		GetAccessorySocketGrade();
		int		GetAccessorySocketMaxGrade();
		int		GetAccessorySocketDownGradeTime();

		void		SetAccessorySocketGrade(int iGrade);
		void		SetAccessorySocketMaxGrade(int iMaxGrade);
		void		SetAccessorySocketDownGradeTime(DWORD time);

		void		AccessorySocketDegrade();


		void		StartAccessorySocketExpireEvent();
		void		SetAccessorySocketExpireEvent(LPEVENT pkEvent);

		bool		CanPutInto(LPITEM item);
		// END_OF_ACCESSORY_REFINE

		void		CopyAttributeTo(LPITEM pItem);
		void		CopySocketTo(LPITEM pItem);

		int			GetRareAttrCount();
		bool		AddRareAttribute();
		bool		ChangeRareAttribute();

		void		AttrLog();

		void		Lock(bool f) { m_isLocked = f; }
		bool		isLocked() const { return m_isLocked; }

	private :
		void		SetAttribute(int i, BYTE bType, short sValue);
	public:
		void		SetForceAttribute(int i, BYTE bType, short sValue);
#ifdef WJ_ELDER_ATTRIBUTE_SYSTEM
		void		AddNewAttr(BYTE index, BYTE bApply, BYTE bLevel);
#endif

	protected:
		bool		EquipEx(bool is_equip);
		bool		Unequip();

		void		AddAttr(BYTE bApply, BYTE bLevel);
		void		PutAttribute(const int * aiAttrPercentTable);
		void		PutAttributeWithLevel(BYTE bLevel);

	public:
		bool		AddRareAttribute2(const int * aiAttrPercentTable = NULL);
		bool		ChangeRareAttribute2();
	protected:
		void		AddRareAttr(BYTE bApply, BYTE bLevel);
		void		PutRareAttribute(const int * aiAttrPercentTable);
		void		PutRareAttributeWithLevel(BYTE bLevel);

	protected:
		friend class CInputDB;
		bool		OnAfterCreatedItem();

	public:
		bool		IsRideItem();
		bool		IsRamadanRing();

		void		ClearMountAttributeAndAffect();
		bool		IsNewMountItem();
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
		bool		IsMountItem();
#endif
		void		SetMaskVnum(DWORD vnum)	{	m_dwMaskVnum = vnum; }
		DWORD		GetMaskVnum()			{	return m_dwMaskVnum; }
		bool		IsMaskedItem()	{	return m_dwMaskVnum != 0;	}


		bool		IsDragonSoul();
		int		GiveMoreTime_Per(float fPercent);
		int		GiveMoreTime_Fix(DWORD dwTime);

	private:
		TItemTable const * m_pProto;

		DWORD		m_dwVnum;
		LPCHARACTER	m_pOwner;

		BYTE		m_bWindow;
		DWORD		m_dwID;
		bool		m_bEquipped;
		DWORD		m_dwVID;		// VID
		WORD		m_wCell;
		DWORD		m_dwCount;
		long		m_lFlag;
		DWORD		m_dwLastOwnerPID;

		bool		m_bExchanging;

		long		m_alSockets[ITEM_SOCKET_MAX_NUM];
		TPlayerItemAttribute	m_aAttr[ITEM_ATTRIBUTE_MAX_NUM];

		LPEVENT		m_pkDestroyEvent;
		LPEVENT		m_pkExpireEvent;
		LPEVENT		m_pkUniqueExpireEvent;
		LPEVENT		m_pkTimerBasedOnWearExpireEvent;
		LPEVENT		m_pkRealTimeExpireEvent;
		LPEVENT		m_pkAccessorySocketExpireEvent;
		LPEVENT		m_pkOwnershipEvent;

		DWORD		m_dwOwnershipPID;

		bool		m_bSkipSave;

		bool		m_isLocked;

		DWORD		m_dwMaskVnum;
		DWORD		m_dwSIGVnum;
#ifdef ENABLE_SOULBIND_SYSTEM
	public:
		long		GetSealBindTime() const { return m_lSealbind;}
		void		SetSealBind(long sb = -1);
		bool		IsSealed() const { return m_lSealbind != 0;}
		void		StartUnSealBindTimerExpireEvent();
	private:
		long		m_lSealbind;
		LPEVENT		m_pkUnSealTimerExpireEvent;
#endif
	public:
		void SetSIGVnum(DWORD dwSIG)
		{
			m_dwSIGVnum = dwSIG;
		}
		DWORD	GetSIGVnum() const
		{
			return m_dwSIGVnum;
		}
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	public:
		int32_t GetAcceVnum() {
			int32_t vnum = GetVnum();
			if (GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
				vnum += ACCE_EFFECT_VNUM;
			return vnum;
		}
		int32_t CalcAcceBonus(int32_t value) {
			return CalcAcceBonus(value, GetSocket(ACCE_ABSORPTION_SOCKET));
		}
		static int32_t CalcAcceBonus(int32_t value, uint32_t pct) {
			if (!pct || pct > 100)
				return 0;
			auto ret = (double)value * (double(pct) / 100.0);
			if (value > 0) {
				ret += 0.5;
				if (ret < 1)
					ret = 1;
			} else if (value < 0) {
				ret -= 0.5;
				if (ret > -1)
					ret = -1;
			}
			return (int32_t)ret;
		}
#endif
};

EVENTINFO(item_event_info)
{
	LPITEM item;
	char szOwnerName[CHARACTER_NAME_MAX_LEN];

	item_event_info()
	: item( 0 )
	{
		::memset( szOwnerName, 0, CHARACTER_NAME_MAX_LEN );
	}
};

EVENTINFO(item_vid_event_info)
{
	DWORD item_vid;

	item_vid_event_info()
	: item_vid( 0 )
	{
	}
};

#endif
//martysama0134's 2022
