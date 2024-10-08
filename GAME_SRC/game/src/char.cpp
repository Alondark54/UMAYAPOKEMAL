#include "stdafx.h"

#include "../../common/teen_packet.h"
#include "../../common/VnumHelper.h"

#include "char.h"

#include "config.h"
#include "utils.h"
#include "crc32.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "item_manager.h"
#include "motion.h"
#include "vector.h"
#include "packet.h"
#include "cmd.h"
#include "fishing.h"
#include "exchange.h"
#include "battle.h"
#include "affect.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "pvp.h"
#include "party.h"
#include "start_position.h"
#include "questmanager.h"
#include "log.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "unique_item.h"
#include "priv_manager.h"
#include "war_map.h"
#include "xmas_event.h"
#include "banword.h"
#include "target.h"
#include "wedding.h"
#include "mob_manager.h"
#include "mining.h"
#include "monarch.h"
#include "castle.h"
#include "arena.h"
#include "dev_log.h"
#include "horsename_manager.h"
#include "pcbang.h"
#include "gm.h"
#include "map_location.h"
#include "BlueDragon_Binder.h"
#include "HackShield.h"
#include "skill_power.h"
#include "XTrapManager.h"
#include "buff_on_attributes.h"

#ifdef ENABLE_CAOS_EVENT
	#include "NewCaosEvent.h"
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
#include "MountSystem.h"
#endif
#ifdef __PET_SYSTEM__
#include "PetSystem.h"
#endif
#include "DragonSoul.h"
#ifdef ENABLE_SWITCHBOT
#include "new_switchbot.h"
#endif
#ifdef __SEND_TARGET_INFO__
#include <algorithm>
#include <iterator>
using namespace std;
#endif
#include "../../common/CommonDefines.h"

#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
#include "offline_shop.h"
#include "offlineshop_manager.h"
#endif

#ifdef ENABLE_EXTENDED_BATTLE_PASS
#include "battlepass_manager.h"
#endif


#ifdef ENABLE_NEW_MISSIONS
#include "NewMissions.hxx"
#endif

extern const BYTE g_aBuffOnAttrPoints;
extern bool RaceToJob(unsigned race, unsigned *ret_job);

extern bool IS_SUMMONABLE_ZONE(int map_index); // char_item.cpp
bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index);

bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index)
{
	switch (map_index)
	{
	case 301:
	case 302:
	case 303:
	case 304:
		if (ch->GetLevel() < 90)
			return false;
	}
	return true;
}

#ifdef NEW_ICEDAMAGE_SYSTEM
const DWORD CHARACTER::GetNoDamageRaceFlag()
{
	return m_dwNDRFlag;
}

void CHARACTER::SetNoDamageRaceFlag(DWORD dwRaceFlag)
{
	if (dwRaceFlag>=MAIN_RACE_MAX_NUM) return;
	if (IS_SET(m_dwNDRFlag, 1<<dwRaceFlag)) return;
	SET_BIT(m_dwNDRFlag, 1<<dwRaceFlag);
}


void CHARACTER::UnsetNoDamageRaceFlag(DWORD dwRaceFlag)
{
	if (dwRaceFlag>=MAIN_RACE_MAX_NUM) return;
	if (!IS_SET(m_dwNDRFlag, 1<<dwRaceFlag)) return;
	REMOVE_BIT(m_dwNDRFlag, 1<<dwRaceFlag);
}

void CHARACTER::ResetNoDamageRaceFlag()
{
	m_dwNDRFlag = 0;
}

const std::set<DWORD> & CHARACTER::GetNoDamageAffectFlag()
{
	return m_setNDAFlag;
}

void CHARACTER::SetNoDamageAffectFlag(DWORD dwAffectFlag)
{
	m_setNDAFlag.insert(dwAffectFlag);
}

void CHARACTER::UnsetNoDamageAffectFlag(DWORD dwAffectFlag)
{
	m_setNDAFlag.erase(dwAffectFlag);
}

void CHARACTER::ResetNoDamageAffectFlag()
{
	m_setNDAFlag.clear();
}
#endif

// <Factor> DynamicCharacterPtr member function definitions

LPCHARACTER DynamicCharacterPtr::Get() const {
	LPCHARACTER p = NULL;
	if (is_pc) {
		p = CHARACTER_MANAGER::instance().FindByPID(id);
	} else {
		p = CHARACTER_MANAGER::instance().Find(id);
	}
	return p;
}

DynamicCharacterPtr& DynamicCharacterPtr::operator=(LPCHARACTER character) {
	if (character == NULL) {
		Reset();
		return *this;
	}
	if (character->IsPC()) {
		is_pc = true;
		id = character->GetPlayerID();
	} else {
		is_pc = false;
		id = character->GetVID();
	}
	return *this;
}

CHARACTER::CHARACTER()
{
	m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateIdle, &CHARACTER::EndStateEmpty);
	m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
	m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateBattle, &CHARACTER::EndStateEmpty);

	Initialize();
}

CHARACTER::~CHARACTER()
{
	Destroy();
}

void CHARACTER::Initialize()
{
	CEntity::Initialize(ENTITY_CHARACTER);

	m_bNoOpenedShop = true;

	m_bOpeningSafebox = false;

	m_fSyncTime = get_float_time()-3;
	m_dwPlayerID = 0;
	m_dwKillerPID = 0;
#ifdef __SEND_TARGET_INFO__
	dwLastTargetInfoPulse = 0;
#endif
#ifdef ANTI_MOB_RANGE_ITEM
	m_antimobrange = false;
#endif
#ifdef ENABLE_LUCKY_DRAW
	m_iLDJoinTime = 0;
	m_iLDReqTime = 0;
#endif
	m_iMoveCount = 0;

	m_pkRegen = NULL;
#ifdef __AUTO_HUNT__
	m_bAutoHuntStatus = false;
#endif
	regen_id_ = 0;
	m_posRegen.x = m_posRegen.y = m_posRegen.z = 0;
	m_posStart.x = m_posStart.y = 0;
	m_posDest.x = m_posDest.y = 0;
	m_fRegenAngle = 0.0f;

	m_pkMobData		= NULL;
	m_pkMobInst		= NULL;

	m_pkShop		= NULL;
	m_pkChrShopOwner	= NULL;
	m_pkMyShop		= NULL;
	m_pkExchange	= NULL;
	m_pkParty		= NULL;
	m_pkPartyRequestEvent = NULL;

#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
	m_pkOfflineShop = nullptr;
	m_pkChrOfflineShopOwner = nullptr;
	m_pkOfflineShopUpdateEvent = nullptr;
	m_iMyOfflineShopTime = 0;
#endif

	m_pGuild = NULL;

	m_pkChrTarget = NULL;

	m_pkMuyeongEvent = NULL;

	m_pkWarpNPCEvent = NULL;
	m_pkDeadEvent = NULL;
	m_pkStunEvent = NULL;
	m_pkSaveEvent = NULL;
	m_pkRecoveryEvent = NULL;
	m_pkTimedEvent = NULL;
	m_pkFishingEvent = NULL;
	m_pkWarpEvent = NULL;

	// MINING
	m_pkMiningEvent = NULL;
	// END_OF_MINING

	m_pkPoisonEvent = NULL;
#ifdef ENABLE_WOLFMAN_CHARACTER
	m_pkBleedingEvent = NULL;
#endif
	m_pkFireEvent = NULL;
	m_pkCheckSpeedHackEvent	= NULL;
	m_speed_hack_count	= 0;

	m_pkAffectEvent = NULL;
	m_afAffectFlag = TAffectFlag(0, 0);

	m_pkDestroyWhenIdleEvent = NULL;

	m_pkChrSyncOwner = NULL;

	m_points = {};
	m_pointsInstant = {};
	memset(&m_quickslot, 0, sizeof(m_quickslot));
#ifdef ENABLE_AUTOMATIC_ITEM_PROCESS
	memset(&m_automaticItemProcess, 0, sizeof(m_automaticItemProcess));
	//ClearAutomaticItemProcess();
#endif
	memset(&character_cards, 0, sizeof(character_cards));
	memset(&randomized_cards, 0, sizeof(randomized_cards));
	m_bCharType = CHAR_TYPE_MONSTER;

	SetPosition(POS_STANDING);

	m_dwPlayStartTime = m_dwLastMoveTime = get_dword_time();

	GotoState(m_stateIdle);
	m_dwStateDuration = 1;

	m_dwLastAttackTime = get_dword_time() - 20000;

	m_bAddChrState = 0;

	m_pkChrStone = NULL;

	m_pkSafebox = NULL;
	m_iSafeboxSize = -1;
	m_iSafeboxLoadTime = 0;

	m_pkMall = NULL;
	m_iMallLoadTime = 0;

	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;
	m_lWarpMapIndex = 0;

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;

	m_pSkillLevels = NULL;

	m_dwMoveStartTime = 0;
	m_dwMoveDuration = 0;

	m_dwFlyTargetID = 0;

	m_dwNextStatePulse = 0;

	m_dwLastDeadTime = get_dword_time()-180000;

	m_bSkipSave = false;

	m_bItemLoaded = false;

	m_bHasPoisoned = false;
#ifdef ENABLE_WOLFMAN_CHARACTER
	m_bHasBled = false;
#endif
	m_pkDungeon = NULL;
	m_iEventAttr = 0;

	m_kAttackLog.dwVID = 0;
	m_kAttackLog.dwTime = 0;

	m_bNowWalking = m_bWalking = false;
	ResetChangeAttackPositionTime();

	m_bDetailLog = false;
	m_bMonsterLog = false;

	m_bDisableCooltime = false;

	m_iAlignment = 0;
	m_iRealAlignment = 0;

	m_iKillerModePulse = 0;
	m_bPKMode = PK_MODE_PEACE;

	m_dwQuestNPCVID = 0;
	m_dwQuestByVnum = 0;
	m_pQuestItem = NULL;

	m_szMobileAuth[0] = '\0';

	m_dwUnderGuildWarInfoMessageTime = get_dword_time()-60000;

	m_bUnderRefine = false;

#ifdef __RANKING_SYSTEM__
	m_ranking_d_s= 0;
	m_ranking_k_m= 0;
	m_ranking_k_b= 0;
	m_ranking_c_d= 0;
	m_ranking_g= 0;
	m_ranking_c_f= 0;
	m_ranking_o_c= 0;
#endif

	// REFINE_NPC
	m_dwRefineNPCVID = 0;
	// END_OF_REFINE_NPC

	m_dwPolymorphRace = 0;

	m_bStaminaConsume = false;

	ResetChainLightningIndex();

	m_dwMountVnum = 0;
	m_chHorse = NULL;
	m_chRider = NULL;

	m_pWarMap = NULL;
#ifdef FATE_ROULETTE
	m_fatereward = 0;
	m_faterewardcount = 0;
#endif
	m_pWeddingMap = NULL;
	m_bChatCounter = 0;
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	m_bMountCounter = 0;
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
	m_pkBiologManager = NULL;
#endif
	ResetStopTime();

	m_dwLastVictimSetTime = get_dword_time() - 3000;
	m_iMaxAggro = -100;

	m_bSendHorseLevel = 0;
	m_bSendHorseHealthGrade = 0;
	m_bSendHorseStaminaGrade = 0;

	m_dwLoginPlayTime = 0;

	m_pkChrMarried = NULL;

	m_posSafeboxOpen.x = -1000;
	m_posSafeboxOpen.y = -1000;

	// EQUIP_LAST_SKILL_DELAY
	m_dwLastSkillTime = get_dword_time();
	// END_OF_EQUIP_LAST_SKILL_DELAY

	// MOB_SKILL_COOLTIME
	memset(m_adwMobSkillCooltime, 0, sizeof(m_adwMobSkillCooltime));
	// END_OF_MOB_SKILL_COOLTIME

	m_isinPCBang = false;

	// ARENA
	m_pArena = NULL;
	m_nPotionLimit = quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count");
	// END_ARENA

	//PREVENT_TRADE_WINDOW
	m_isOpenSafebox = 0;
	//END_PREVENT_TRADE_WINDOW

	//PREVENT_REFINE_HACK
	m_iRefineTime = 0;
	//END_PREVENT_REFINE_HACK

	//RESTRICT_USE_SEED_OR_MOONBOTTLE
	m_iSeedTime = 0;
	//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
	//PREVENT_PORTAL_AFTER_EXCHANGE
	m_iExchangeTime = 0;
	//END_PREVENT_PORTAL_AFTER_EXCHANGE
	//
	m_iSafeboxLoadTime = 0;

	m_iMyShopTime = 0;

	InitMC();

	m_deposit_pulse = 0;

	SET_OVER_TIME(this, OT_NONE);

	m_strNewName = "";

	m_known_guild.clear();

	m_dwLogOffInterval = 0;

#ifdef ENABLE_REWARD_SYSTEM
	m_bRewardLoaded = false;
#endif

	m_bComboSequence = 0;
	m_dwLastComboTime = 0;
	m_bComboIndex = 0;
	m_iComboHackCount = 0;
	m_dwSkipComboAttackByTime = 0;
#if defined(BL_REMOTE_SHOP)
	dwLastRemoteTime = 0;
#endif
	m_dwMountTime = 0;
#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
	bPrivateShopSearchState = SHOP_SEARCH_OFF;
#endif
	m_dwLastGoldDropTime = 0;
#ifdef ENABLE_NEWSTUFF
	m_dwLastItemDropTime = 0;
	m_dwLastBoxUseTime = 0;
	m_dwLastBuySellTime = 0;
#endif

	m_HackShieldCheckEvent = NULL;
	m_HackShieldCheckMode = false;

	m_bIsLoadedAffect = false;
	cannot_dead = false;
#ifdef SOHBET_DURDUR
	m_chat_durdur = 0;
#endif

#ifdef ENABLE_FISH_EVENT
	bFishChance = 0;
#endif
#ifdef __PET_SYSTEM__
	m_petSystem = 0;
	m_bIsPet = false;
#endif
#ifdef NEW_ICEDAMAGE_SYSTEM
	m_dwNDRFlag = 0;
	m_setNDAFlag.clear();
#endif

	m_fAttMul = 1.0f;
	m_fDamMul = 1.0f;

	m_pointsInstant.iDragonSoulActiveDeck = -1;
#ifdef ENABLE_ANTI_CMD_FLOOD
	m_dwCmdAntiFloodCount = 0;
	m_dwCmdAntiFloodPulse = 0;
#endif
	memset(&m_tvLastSyncTime, 0, sizeof(m_tvLastSyncTime));
	m_iSyncHackCount = 0;
#ifdef __HIDE_COSTUME_SYSTEM__
	m_bHideBodyCostume = false;
	m_bHideHairCostume = false;
#endif
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	m_listExtBattlePass.clear();
	m_bIsLoadedExtBattlePass = false;
	m_dwLastReciveExtBattlePassInfoTime = 0;
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	m_mountSystem = 0;
	m_bIsMount = false;
	m_bMountCounter = 0;
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	m_bAcceCombination	= false;
	m_bAcceAbsorption	= false;
#endif
#ifdef ENABLE_BOT_CONTROL
	m_bBotControl = false;
	m_bBotControlGUI = false;
#endif
}

void CHARACTER::Create(const char * c_pszName, DWORD vid, bool isPC)
{
	static int s_crc = 172814;

	char crc_string[128+1];
	snprintf(crc_string, sizeof(crc_string), "%s%p%d", c_pszName, this, ++s_crc);
	m_vid = VID(vid, GetCRC32(crc_string, strlen(crc_string)));

	if (isPC)
		m_stName = c_pszName;
}

void CHARACTER::Destroy()
{
	CloseMyShop();

	if (m_pkRegen)
	{
		if (m_pkDungeon) {
			// Dungeon regen may not be valid at this point
			if (m_pkDungeon->IsValidRegen(m_pkRegen, regen_id_)) {
				--m_pkRegen->count;
			}
		} else {
			// Is this really safe? NO IT ISNT! F*CK THAT SH!T!
			if (is_valid_regen(m_pkRegen))
				--m_pkRegen->count;
		}
		m_pkRegen = NULL;
	}
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (m_mountSystem)
	{
		m_mountSystem->Destroy();
		delete m_mountSystem;
		m_mountSystem = 0;
	}
	if (GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}
	HorseSummon(false);
#endif
	if (m_pkDungeon)
	{
		SetDungeon(NULL);
	}

#ifdef __PET_SYSTEM__
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;

		m_petSystem = 0;
	}
#endif

	HorseSummon(false);

	if (GetRider())
		GetRider()->ClearHorseInfo();

	if( IsPC() )
	{
		if (isHackShieldEnable)
		{
			CHackShieldManager::instance().DeleteClientHandle(GetPlayerID());
		}
	}

	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
//		BindDesc(NULL);
	}

	if (m_pkExchange)
		m_pkExchange->Cancel();

	SetVictim(NULL);

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	ClearStone();
	ClearSync();
	ClearTarget();

	if (NULL == m_pkMobData)
	{
		DragonSoul_CleanUp();
		ClearItem();
	}

	// <Factor> m_pkParty becomes NULL after CParty destructor call!
	LPPARTY party = m_pkParty;
	if (party)
	{
		if (party->GetLeaderPID() == GetVID() && !IsPC())
		{
			M2_DELETE(party);
		}
		else
		{
			party->Unlink(this);

			if (!IsPC())
				party->Quit(GetVID());
		}

		SetParty(NULL);
	}

	if (m_pkMobInst)
	{
		M2_DELETE(m_pkMobInst);
		m_pkMobInst = NULL;
	}
#ifdef ENABLE_MULTI_FARM_BLOCK
	m_bmultiFarmStatus = false;
#endif

	m_pkMobData = NULL;

	if (m_pkSafebox)
	{
		M2_DELETE(m_pkSafebox);
		m_pkSafebox = NULL;
	}

	if (m_pkMall)
	{
		M2_DELETE(m_pkMall);
		m_pkMall = NULL;
	}

	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin();  it != m_map_buff_on_attrs.end(); it++)
	{
		if (NULL != it->second)
		{
			M2_DELETE(it->second);
		}
	}
	m_map_buff_on_attrs.clear();

	m_set_pkChrSpawnedBy.clear();

	StopMuyeongEvent();
	event_cancel(&m_pkWarpNPCEvent);
	event_cancel(&m_pkRecoveryEvent);
	event_cancel(&m_pkDeadEvent);
	event_cancel(&m_pkSaveEvent);
	event_cancel(&m_pkTimedEvent);
	event_cancel(&m_pkStunEvent);
	event_cancel(&m_pkFishingEvent);
	event_cancel(&m_pkPoisonEvent);
#ifdef ENABLE_WOLFMAN_CHARACTER
	event_cancel(&m_pkBleedingEvent);
#endif
	event_cancel(&m_pkFireEvent);
	event_cancel(&m_pkPartyRequestEvent);
	//DELAYED_WARP
	event_cancel(&m_pkWarpEvent);
	event_cancel(&m_pkCheckSpeedHackEvent);
	//END_DELAYED_WARP
#ifdef ENABLE_NEW_BIOLOG
	event_cancel(&m_pkBiologNtfEvent);
#endif
	// RECALL_DELAY
	//event_cancel(&m_pkRecallEvent);
	// END_OF_RECALL_DELAY

	// MINING
	event_cancel(&m_pkMiningEvent);
	// END_OF_MINING

	StopHackShieldCheckCycle();

	for (itertype(m_mapMobSkillEvent) it = m_mapMobSkillEvent.begin(); it != m_mapMobSkillEvent.end(); ++it)
	{
		LPEVENT pkEvent = it->second;
		event_cancel(&pkEvent);
	}
	m_mapMobSkillEvent.clear();

	//event_cancel(&m_pkAffectEvent);
	ClearAffect();

	event_cancel(&m_pkDestroyWhenIdleEvent);

	if (m_pSkillLevels)
	{
		M2_DELETE_ARRAY(m_pSkillLevels);
		m_pSkillLevels = NULL;
	}

	CEntity::Destroy();

	if (GetSectree())
		GetSectree()->RemoveEntity(this);

	if (m_bMonsterLog)
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
}

const char * CHARACTER::GetName() const
{
	return m_stName.empty() ? (m_pkMobData ? m_pkMobData->m_table.szLocaleName : "") : m_stName.c_str();
}

void CHARACTER::OpenMyShop(const char * c_pszSign, TShopItemTable * pTable, BYTE bItemCount)
{
	if (!CanHandleItem()) // @fixme149
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use a private shop now."));
		return;
	}

#ifndef ENABLE_OPEN_SHOP_WITH_ARMOR
	if (GetPart(PART_MAIN) > 2)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("To open a shop, you have to take off the armor."));
		return;
	}
#endif

	if (GetMyShop())
	{
		CloseMyShop();
		return;
	}


	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());


	if (pPC->IsRunning())
		return;

	if (bItemCount == 0)
		return;

	long long nTotalMoney = 0;

	for (int n = 0; n < bItemCount; ++n)
	{
		nTotalMoney += static_cast<long long>((pTable+n)->price);
	}

	nTotalMoney += static_cast<long long>(GetGold());

	if (GOLD_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] Overflow (GOLD_MAX) id %u name %s", GetPlayerID(), GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("20? ?? ???? ??? ??? ????"));
		return;
	}

	char szSign[SHOP_SIGN_MAX_LEN+1];
	strlcpy(szSign, c_pszSign, sizeof(szSign));

	m_stShopSign = szSign;

	if (m_stShopSign.length() == 0)
		return;

	if (CBanwordManager::instance().CheckString(m_stShopSign.c_str(), m_stShopSign.length()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't name your Warehouse with an invalid name."));
		return;
	}

	// MYSHOP_PRICE_LIST
	std::map<DWORD, DWORD> itemkind;
	// END_OF_MYSHOP_PRICE_LIST

	std::set<TItemPos> cont;
	for (BYTE i = 0; i < bItemCount; ++i)
	{
		if (cont.find((pTable + i)->pos) != cont.end())
		{
			sys_err("MYSHOP: duplicate shop item detected! (name: %s)", GetName());
			return;
		}

		// ANTI_GIVE, ANTI_MYSHOP check
		LPITEM pkItem = GetItem((pTable + i)->pos);

		if (pkItem)
		{
			const TItemTable * item_table = pkItem->GetProto();

			if (item_table && (IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Error: can't be sold in Warehouse. Please report to GM."));
				return;
			}

			if (pkItem->IsEquipped() == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell items you have equipped."));
				return;
			}

			if (true == pkItem->isLocked())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell items you have equipped."));
				return;
			}

#ifdef ENABLE_SOULBIND_SYSTEM 
			if (pkItem->IsSealed()){
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Can't trade with sealed item."));
				return;
			}
#endif

			// MYSHOP_PRICE_LIST
			itemkind[pkItem->GetVnum()] = (pTable + i)->price / pkItem->GetCount();
			// END_OF_MYSHOP_PRICE_LIST
		}

		cont.insert((pTable + i)->pos);
	}

	// MYSHOP_PRICE_LIST

	if (CountSpecifyItem(71049)) {

		// @fixme403 BEGIN
		TItemPriceListTable header;
		memset(&header, 0, sizeof(TItemPriceListTable));

		header.dwOwnerID = GetPlayerID();
		header.byCount = itemkind.size();

		size_t idx=0;
		for (itertype(itemkind) it = itemkind.begin(); it != itemkind.end(); ++it)
		{
			header.aPriceInfo[idx].dwVnum = it->first;
			header.aPriceInfo[idx].dwPrice = it->second;
			idx++;
		}

		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_UPDATE, GetDesc()->GetHandle(), &header, sizeof(TItemPriceListTable));
		// @fixme403 END
	}
	// END_OF_MYSHOP_PRICE_LIST
	else if (CountSpecifyItem(50200))
		RemoveSpecifyItem(50200, 1);
	else
		return;

	if (m_pkExchange)
		m_pkExchange->Cancel();

	TPacketGCShopSign p;

	p.bHeader = HEADER_GC_SHOP_SIGN;
	p.dwVID = GetVID();
	strlcpy(p.szSign, c_pszSign, sizeof(p.szSign));

	PacketAround(&p, sizeof(TPacketGCShopSign));

	m_pkMyShop = CShopManager::instance().CreatePCShop(this, pTable, bItemCount);

	if (IsPolymorphed() == true)
	{
		RemoveAffect(AFFECT_POLYMORPH);
	}

	if (GetHorse())
	{
		HorseSummon( false, true );
	}
	else if (GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}

	SetPolymorph(30000, true);

}

void CHARACTER::CloseMyShop()
{
	if (GetMyShop())
	{
		m_stShopSign.clear();
		CShopManager::instance().DestroyPCShop(this);
		m_pkMyShop = NULL;

		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		p.szSign[0] = '\0';

		PacketAround(&p, sizeof(p));
#ifdef ENABLE_WOLFMAN_CHARACTER
		SetPolymorph(m_points.job, true);
		// SetPolymorph(0, true);
#else
		SetPolymorph(GetJob(), true);
#endif
#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
		CTargetManager::instance().DeleteShopSearchTarget(static_cast<DWORD>(GetVID()));
#endif
	}
}

void EncodeMovePacket(TPacketGCMove & pack, DWORD dwVID, BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, BYTE bRot)
{
	pack.bHeader = HEADER_GC_MOVE;
	pack.bFunc   = bFunc;
	pack.bArg	= bArg;
	pack.dwVID   = dwVID;
	pack.dwTime  = dwTime ? dwTime : get_dword_time();
	pack.bRot	= bRot;
	pack.lX		= x;
	pack.lY		= y;
	pack.dwDuration	= dwDuration;
}

void CHARACTER::RestartAtSamePos()
{
	if (m_bIsObserver)
		return;

	EncodeRemovePacket(this);
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

		EncodeRemovePacket(entity);
		if (!m_bIsObserver)
			EncodeInsertPacket(entity);

		if( entity->IsType(ENTITY_CHARACTER) )
		{
			LPCHARACTER lpChar = (LPCHARACTER)entity;
			if( lpChar->IsPC() || lpChar->IsNPC() || lpChar->IsMonster() )
			{
				if (!entity->IsObserverMode())
					entity->EncodeInsertPacket(this);
			}
		}
		else
		{
			if( !entity->IsObserverMode())
			{
				entity->EncodeInsertPacket(this);
			}
		}
	}
}

// #define ENABLE_SHOWNPCLEVEL
void CHARACTER::EncodeInsertPacket(LPENTITY entity)
{

	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	LPCHARACTER ch = (LPCHARACTER) entity;
	ch->SendGuildName(GetGuild());

	TPacketGCCharacterAdd pack;

	pack.header		= HEADER_GC_CHARACTER_ADD;
	pack.dwVID		= m_vid;
	pack.bType		= GetCharType();
	pack.angle		= GetRotation();
	pack.x		= GetX();
	pack.y		= GetY();
	pack.z		= GetZ();
	pack.wRaceNum	= GetRaceNum();
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (IsPet() || IsMount())
#else
	if (IsPet())
#endif
	{
		pack.bMovingSpeed	= 150;
	}
	else
	{
		pack.bMovingSpeed	= GetLimitPoint(POINT_MOV_SPEED);
	}
	pack.bAttackSpeed	= GetLimitPoint(POINT_ATT_SPEED);
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];

	pack.bStateFlag = m_bAddChrState;

	int iDur = 0;

	if (m_posDest.x != pack.x || m_posDest.y != pack.y)
	{
		iDur = (m_dwMoveStartTime + m_dwMoveDuration) - get_dword_time();

		if (iDur <= 0)
		{
			pack.x = m_posDest.x;
			pack.y = m_posDest.y;
		}
	}

	d->Packet(&pack, sizeof(pack));

	if (IsPC() == true || m_bCharType == CHAR_TYPE_NPC)
	{
		TPacketGCCharacterAdditionalInfo addPacket;
		memset(&addPacket, 0, sizeof(TPacketGCCharacterAdditionalInfo));

		addPacket.header = HEADER_GC_CHAR_ADDITIONAL_INFO;
		addPacket.dwVID = m_vid;

		addPacket.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
		addPacket.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
		addPacket.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
		addPacket.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		addPacket.awPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
		addPacket.bPKMode = m_bPKMode;
		addPacket.dwMountVnum = GetMountVnum();
		addPacket.bEmpire = m_bEmpire;
#ifdef ENABLE_SHOWNPCLEVEL
		if (1)
#else
		if (IsPC() == true)
#endif
		{
			addPacket.dwLevel = GetLevel();
		}
		else
		{
			addPacket.dwLevel = 0;
		}

		if (false)
		{
			LPCHARACTER ch = (LPCHARACTER) entity;

			if (GetEmpire() == ch->GetEmpire() || ch->GetGMLevel() > GM_PLAYER || m_bCharType == CHAR_TYPE_NPC)
			{
				goto show_all_info;
			}
			else
			{
				memset(addPacket.name, 0, CHARACTER_NAME_MAX_LEN);
				addPacket.dwGuildID = 0;
				addPacket.sAlignment = 0;
			}
		}
		else
		{
		show_all_info:
			strlcpy(addPacket.name, GetName(), sizeof(addPacket.name));

			if (GetGuild() != NULL)
			{
				addPacket.dwGuildID = GetGuild()->GetID();
			}
			else
			{
				addPacket.dwGuildID = 0;
			}

			addPacket.sAlignment = m_iAlignment / 10;
		}

		d->Packet(&addPacket, sizeof(TPacketGCCharacterAdditionalInfo));
	}

	if (iDur)
	{
		TPacketGCMove pack;
		EncodeMovePacket(pack, GetVID(), FUNC_MOVE, 0, m_posDest.x, m_posDest.y, iDur, 0, (BYTE) (GetRotation() / 5));
		d->Packet(&pack, sizeof(pack));

		TPacketGCWalkMode p;
		p.vid = GetVID();
		p.header = HEADER_GC_WALK_MODE;
		p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

		d->Packet(&p, sizeof(p));
	}

	if (entity->IsType(ENTITY_CHARACTER) && GetDesc())
	{
		LPCHARACTER ch = (LPCHARACTER) entity;
		if (ch->IsWalking())
		{
			TPacketGCWalkMode p;
			p.vid = ch->GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = ch->m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;
			GetDesc()->Packet(&p, sizeof(p));
		}
	}

	if (GetMyShop())
	{
		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		strlcpy(p.szSign, m_stShopSign.c_str(), sizeof(p.szSign));

		d->Packet(&p, sizeof(TPacketGCShopSign));
	}

#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
	if (IsOfflineShopNPC() && (GetRaceNum() == 30000 || (GetRaceNum() >= 30002 && GetRaceNum() <= 30008))) {
		if (entity->IsType(ENTITY_CHARACTER)) {
			LPOFFLINESHOP shop = GetOfflineShop();
			if (shop) {
				TPacketGCShopSign p;
				p.bHeader = HEADER_GC_OFFLINE_SHOP_SIGN;
				p.dwVID = GetVID();
				p.bBorderStyle = shop->GetBorderStyle();
				strlcpy(p.szSign, shop->GetShopSign(), sizeof(p.szSign));
				d->Packet(&p, sizeof(TPacketGCShopSign));
			}
		}
	}
#endif

	if (entity->IsType(ENTITY_CHARACTER))
	{
		sys_log(3, "EntityInsert %s (RaceNum %d) (%d %d) TO %s",
				GetName(), GetRaceNum(), GetX() / SECTREE_SIZE, GetY() / SECTREE_SIZE, ((LPCHARACTER)entity)->GetName());
	}
}

void CHARACTER::EncodeRemovePacket(LPENTITY entity)
{
	if (entity->GetType() != ENTITY_CHARACTER)
		return;

	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	TPacketGCCharacterDelete pack;

	pack.header	= HEADER_GC_CHARACTER_DEL;
	pack.id	= m_vid;

	d->Packet(&pack, sizeof(TPacketGCCharacterDelete));

	if (entity->IsType(ENTITY_CHARACTER))
		sys_log(3, "EntityRemove %s(%d) FROM %s", GetName(), (DWORD) m_vid, ((LPCHARACTER) entity)->GetName());
}

void CHARACTER::UpdatePacket()
{
	if (GetSectree() == NULL) return;

	TPacketGCCharacterUpdate pack;
	TPacketGCCharacterUpdate pack2;

	pack.header = HEADER_GC_CHARACTER_UPDATE;
	pack.dwVID = m_vid;

	pack.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
	pack.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
	pack.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
	pack.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	pack.awPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
	pack.bMovingSpeed	= GetLimitPoint(POINT_MOV_SPEED);
	pack.bAttackSpeed	= GetLimitPoint(POINT_ATT_SPEED);
	pack.bStateFlag	= m_bAddChrState;
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];
	pack.dwGuildID	= 0;
	pack.sAlignment	= m_iAlignment / 10;
	pack.bPKMode	= m_bPKMode;

	if (GetGuild())
		pack.dwGuildID = GetGuild()->GetID();

	pack.dwMountVnum	= GetMountVnum();

	pack2 = pack;
	pack2.dwGuildID = 0;
	pack2.sAlignment = 0;

	if (false)
	{
		if (m_bIsObserver != true)
		{
			for (ENTITY_MAP::iterator iter = m_map_view.begin(); iter != m_map_view.end(); iter++)
			{
				LPENTITY pEntity = iter->first;

				if (pEntity != NULL)
				{
					if (pEntity->IsType(ENTITY_CHARACTER) == true)
					{
						if (pEntity->GetDesc() != NULL)
						{
							LPCHARACTER pChar = (LPCHARACTER)pEntity;

							if (GetEmpire() == pChar->GetEmpire() || pChar->GetGMLevel() > GM_PLAYER)
							{
								pEntity->GetDesc()->Packet(&pack, sizeof(pack));
							}
							else
							{
								pEntity->GetDesc()->Packet(&pack2, sizeof(pack2));
							}
						}
					}
					else
					{
						if (pEntity->GetDesc() != NULL)
						{
							pEntity->GetDesc()->Packet(&pack, sizeof(pack));
						}
					}
				}
			}
		}

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}
	else
	{
		PacketAround(&pack, sizeof(pack));
	}
}

LPCHARACTER CHARACTER::FindCharacterInView(const char * c_pszName, bool bFindPCOnly)
{
	ENTITY_MAP::iterator it = m_map_view.begin();

	for (; it != m_map_view.end(); ++it)
	{
		if (!it->first->IsType(ENTITY_CHARACTER))
			continue;

		LPCHARACTER tch = (LPCHARACTER) it->first;

		if (bFindPCOnly && tch->IsNPC())
			continue;

		if (!strcasecmp(tch->GetName(), c_pszName))
			return (tch);
	}

	return NULL;
}

void CHARACTER::SetPosition(int pos)
{
	if (pos == POS_STANDING)
	{
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

		event_cancel(&m_pkDeadEvent);
		event_cancel(&m_pkStunEvent);
	}
	else if (pos == POS_DEAD)
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);

	if (!IsStone())
	{
		switch (pos)
		{
			case POS_FIGHTING:
				if (!IsState(m_stateBattle))
					MonsterLog("[BATTLE] �ο�� ����");

				GotoState(m_stateBattle);
				break;

			default:
				if (!IsState(m_stateIdle))
					MonsterLog("[IDLE] ���� ����");

				GotoState(m_stateIdle);
				break;
		}
	}

	m_pointsInstant.position = pos;
}

void CHARACTER::Save()
{
	if (!m_bSkipSave)
		CHARACTER_MANAGER::instance().DelayedSave(this);
}

void CHARACTER::CreatePlayerProto(TPlayerTable & tab)
{
	memset(&tab, 0, sizeof(TPlayerTable));

	if (GetNewName().empty())
	{
		strlcpy(tab.name, GetName(), sizeof(tab.name));
	}
	else
	{
		strlcpy(tab.name, GetNewName().c_str(), sizeof(tab.name));
	}

	strlcpy(tab.ip, GetDesc()->GetHostName(), sizeof(tab.ip));

	tab.id			= m_dwPlayerID;
	tab.voice		= GetPoint(POINT_VOICE);
	tab.level		= GetLevel();
	tab.level_step	= GetPoint(POINT_LEVEL_STEP);
	tab.exp			= GetExp();
	tab.gold		= GetGold();
	tab.job			= m_points.job;
	tab.part_base	= m_pointsInstant.bBasePart;
	tab.skill_group	= m_points.skill_group;
#ifndef ENABLE_EXTEND_INVEN_SYSTEM
	tab.envanter 		= Inven_Point();
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
	tab.m_BiologActualMission = GetBiologMissions();
	tab.m_BiologCollectedItems = GetBiologCollectedItems();
	tab.m_BiologCooldownReminder = GetBiologCooldownReminder();
	tab.m_BiologCooldown = GetBiologCooldown();
#endif

	DWORD dwPlayedTime = (get_dword_time() - m_dwPlayStartTime);

	if (dwPlayedTime > 60000)
	{
#ifdef ENABLE_REWARD_SYSTEM
		CHARACTER_MANAGER::Instance().DoReward(this, REWARD_MISSION_PLAYTIME, 0, dwPlayedTime / 1000);
#endif
		if (GetSectree() && !GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		{
			if (GetRealAlignment() < 0)
			{
				if (IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_TIME))
					UpdateAlignment(120 * (dwPlayedTime / 60000));
				else
					UpdateAlignment(60 * (dwPlayedTime / 60000));
			}
			else
				UpdateAlignment(5 * (dwPlayedTime / 60000));
		}

		SetRealPoint(POINT_PLAYTIME, GetRealPoint(POINT_PLAYTIME) + dwPlayedTime / 60000);
		ResetPlayTime(dwPlayedTime % 60000);
	}

	tab.playtime = GetRealPoint(POINT_PLAYTIME);
	tab.lAlignment = m_iRealAlignment;

	if (m_posWarp.x != 0 || m_posWarp.y != 0)
	{
		tab.x = m_posWarp.x;
		tab.y = m_posWarp.y;
		tab.z = 0;
		tab.lMapIndex = m_lWarpMapIndex;
	}
	else
	{
		tab.x = GetX();
		tab.y = GetY();
		tab.z = GetZ();
		tab.lMapIndex	= GetMapIndex();
	}

	if (m_lExitMapIndex == 0)
	{
		tab.lExitMapIndex	= tab.lMapIndex;
		tab.lExitX		= tab.x;
		tab.lExitY		= tab.y;
	}
	else
	{
		tab.lExitMapIndex	= m_lExitMapIndex;
		tab.lExitX		= m_posExit.x;
		tab.lExitY		= m_posExit.y;
	}

	sys_log(0, "SAVE: %s %dx%d", GetName(), tab.x, tab.y);

	tab.st = GetRealPoint(POINT_ST);
	tab.ht = GetRealPoint(POINT_HT);
	tab.dx = GetRealPoint(POINT_DX);
	tab.iq = GetRealPoint(POINT_IQ);

	tab.stat_point = GetPoint(POINT_STAT);
	tab.skill_point = GetPoint(POINT_SKILL);
	tab.sub_skill_point = GetPoint(POINT_SUB_SKILL);
	tab.horse_skill_point = GetPoint(POINT_HORSE_SKILL);

	tab.stat_reset_count = GetPoint(POINT_STAT_RESET_COUNT);

	tab.hp = GetHP();
	tab.sp = GetSP();

	tab.stamina = GetStamina();

	tab.sRandomHP = m_points.iRandomHP;
	tab.sRandomSP = m_points.iRandomSP;

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
		tab.quickslot[i] = m_quickslot[i];

#ifdef ENABLE_AUTOMATIC_ITEM_PROCESS
	for (size_t i = 0; i < AUTOMATIC_ITEM_PROCESS_MAX; ++i)
		tab.uiAutomaticItemProcess[i] = m_automaticItemProcess[i];
#endif


	if (m_stMobile.length() && !*m_szMobileAuth)
		strlcpy(tab.szMobile, m_stMobile.c_str(), sizeof(tab.szMobile));

	thecore_memcpy(tab.parts, m_pointsInstant.parts, sizeof(tab.parts));

	// REMOVE_REAL_SKILL_LEVLES
	thecore_memcpy(tab.skills, m_pSkillLevels, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES

	tab.horse = GetHorseData();
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	tab.battle_pass_premium_id = GetExtBattlePassPremiumID();
#endif
#ifdef ENABLE_NEW_BIOLOG
	tab.bioLevel = GetBioLevel();
	tab.bioGiven = GetBioGivenCount();
	tab.bioState = GetBioState();
	tab.bioTime = GetBioTime();
	tab.bioNtf = GetBioNtf();
#endif
#ifdef __RANKING_SYSTEM__
	tab.killed_metin = GetDestroyStone();
	tab.killed_monster = GetKillMonster();
	tab.killed_boss = GetKillBoss();
	tab.success_dungeon = GetCompletedDungeon();
	tab.gaya = GetPoint(POINT_GOLD);
	tab.caught_fishes = GetCaughtFishes();
	tab.open_chest = GetOpenedChest();
#endif
}


void CHARACTER::SaveReal()
{
	if (m_bSkipSave)
		return;

	if (!GetDesc())
	{
		sys_err("Character::Save : no descriptor when saving (name: %s)", GetName());
		return;
	}

	TPlayerTable table;
	CreatePlayerProto(table);

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_SAVE, GetDesc()->GetHandle(), &table, sizeof(TPlayerTable));

	quest::PC * pkQuestPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	if (!pkQuestPC)
		sys_err("CHARACTER::Save : null quest::PC pointer! (name %s)", GetName());
	else
	{
		pkQuestPC->Save();
	}

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
	if (pMarriage)
		pMarriage->Save();
}

void CHARACTER::FlushDelayedSaveItem()
{
	LPITEM item;

	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
		if ((item = GetInventoryItem(i)))
			ITEM_MANAGER::instance().FlushDelayedSave(item);
}

void CHARACTER::Disconnect(const char * c_pszReason)
{
	assert(GetDesc() != NULL);

	sys_log(0, "DISCONNECT: %s (%s)", GetName(), c_pszReason ? c_pszReason : "unset" );
#ifdef ENABLE_REWARD_SYSTEM
	SaveRewardData();
#endif
	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
	if (GetOfflineShop()) {
		GetOfflineShop()->RemoveGuest(this);
		SetOfflineShop(nullptr);
	}
#endif

	if (GetArena() != NULL)
	{
		GetArena()->OnDisconnect(GetPlayerID());
	}

	if (GetParty() != NULL)
	{
		GetParty()->UpdateOfflineState(GetPlayerID());
	}

	marriage::CManager::instance().Logout(this);
#ifdef ENABLE_MULTI_FARM_BLOCK
	//CHARACTER_MANAGER::Instance().CheckMultiFarmAccount(GetDesc() ? GetDesc()->GetHostName() : "", GetPlayerID(),GetName(), false);
	CHARACTER_MANAGER::Instance().RemoveMultiFarm(GetDesc() ? GetDesc()->GetHostName() : "", GetPlayerID(), false);
#endif
	// P2P Logout
	TPacketGGLogout p;
	p.bHeader = HEADER_GG_LOGOUT;
	strlcpy(p.szName, GetName(), sizeof(p.szName));
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogout));
	LogManager::instance().CharLog(this, 0, "LOGOUT", "");

#ifdef ENABLE_PCBANG_FEATURE // @warme006
	{
		long playTime = GetRealPoint(POINT_PLAYTIME) - m_dwLoginPlayTime;
		LogManager::instance().LoginLog(false, GetDesc()->GetAccountTable().id, GetPlayerID(), GetLevel(), GetJob(), playTime);

		if (0)
			CPCBangManager::instance().Log(GetDesc()->GetHostName(), GetPlayerID(), playTime);
	}
#endif

	if (m_pWarMap)
		SetWarMap(NULL);

	if (m_pWeddingMap)
	{
		SetWeddingMap(NULL);
	}

	if (GetGuild())
		GetGuild()->LogoutMember(this);

	quest::CQuestManager::instance().LogoutPC(this);

#ifdef ENABLE_EXTENDED_BATTLE_PASS
	ListExtBattlePassMap::iterator itext = m_listExtBattlePass.begin();
	while (itext != m_listExtBattlePass.end())
	{
		TPlayerExtBattlePassMission* pkMission = *itext++;

		if (!pkMission->bIsUpdated)
			continue;

		db_clientdesc->DBPacket(HEADER_GD_SAVE_EXT_BATTLE_PASS, 0, pkMission, sizeof(TPlayerExtBattlePassMission));
	}
	m_bIsLoadedExtBattlePass = false;
#endif

	if (GetParty())
		GetParty()->Unlink(this);

	if (IsStun() || IsDead())
	{
		DeathPenalty(0);
		PointChange(POINT_HP, 50 - GetHP());
	}


	if (!CHARACTER_MANAGER::instance().FlushDelayedSave(this))
	{
		SaveReal();
	}

	FlushDelayedSaveItem();

	SaveAffect();
	m_bIsLoadedAffect = false;

#ifdef __ENABLE_BIOLOG_SYSTEM__
	if (m_pkBiologManager)
	{
		delete m_pkBiologManager;
		m_pkBiologManager = NULL;
	}
#endif

	m_bSkipSave = true;

	quest::CQuestManager::instance().DisconnectPC(this);

	CloseSafebox();

	CloseMall();

	CPVPManager::instance().Disconnect(this);

	CTargetManager::instance().Logout(GetPlayerID());

	MessengerManager::instance().Logout(GetName());

	if (g_TeenDesc)
	{
		int		offset = 0;
		char	buf[245] = {0};

		buf[0] = HEADER_GT_LOGOUT;
		offset += 1;

		memset(buf+offset, 0x00, 2);
		offset += 2;

		TAccountTable	&acc_table = GetDesc()->GetAccountTable();
		memcpy(buf+offset, &acc_table.id, 4);
		offset += 4;

		g_TeenDesc->Packet(buf, offset);
	}
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}
#endif
	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
//		BindDesc(NULL);
	}

	CXTrapManager::instance().DestroyClientSession(this);

	M2_DESTROY_CHARACTER(this);
}

bool CHARACTER::Show(long lMapIndex, long x, long y, long z, bool bShowSpawnMotion/* = false */)
{
	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "cannot find sectree by %dx%d mapindex %d", x, y, lMapIndex);
		return false;
	}

	SetMapIndex(lMapIndex);

	bool bChangeTree = false;

	if (!GetSectree() || GetSectree() != sectree)
		bChangeTree = true;

	if (bChangeTree)
	{
		if (GetSectree())
			GetSectree()->RemoveEntity(this);

		ViewCleanup();
	}

	if (!IsNPC())
	{
		sys_log(0, "SHOW: %s %dx%dx%d", GetName(), x, y, z);
		if (GetStamina() < GetMaxStamina())
			StartAffectEvent();
	}
	else if (m_pkMobData)
	{
		m_pkMobInst->m_posLastAttacked.x = x;
		m_pkMobInst->m_posLastAttacked.y = y;
		m_pkMobInst->m_posLastAttacked.z = z;
	}

	if (bShowSpawnMotion)
	{
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
		m_afAffectFlag.Set(AFF_SPAWN);
	}

	SetXYZ(x, y, z);

	m_posDest.x = x;
	m_posDest.y = y;
	m_posDest.z = z;

	m_posStart.x = x;
	m_posStart.y = y;
	m_posStart.z = z;

	if (bChangeTree)
	{
		EncodeInsertPacket(this);
		sectree->InsertEntity(this);

		UpdateSectree();
	}
	else
	{
		ViewReencode();
		sys_log(0, "	  in same sectree");
	}

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	SetValidComboInterval(0);
	return true;
}

// BGM_INFO
struct BGMInfo
{
	std::string	name;
	float		vol;
};

typedef std::map<unsigned, BGMInfo> BGMInfoMap;

static BGMInfoMap 	gs_bgmInfoMap;
static bool		gs_bgmVolEnable = false;

void CHARACTER_SetBGMVolumeEnable()
{
	gs_bgmVolEnable = true;
	sys_log(0, "bgm_info.set_bgm_volume_enable");
}

void CHARACTER_AddBGMInfo(unsigned mapIndex, const char* name, float vol)
{
	BGMInfo newInfo;
	newInfo.name = name;
	newInfo.vol = vol;

	gs_bgmInfoMap[mapIndex] = newInfo;

	sys_log(0, "bgm_info.add_info(%d, '%s', %f)", mapIndex, name, vol);
}

const BGMInfo& CHARACTER_GetBGMInfo(unsigned mapIndex)
{
	BGMInfoMap::iterator f = gs_bgmInfoMap.find(mapIndex);
	if (gs_bgmInfoMap.end() == f)
	{
		static BGMInfo s_empty = {"", 0.0f};
		return s_empty;
	}
	return f->second;
}

bool CHARACTER_IsBGMVolumeEnable()
{
	return gs_bgmVolEnable;
}
// END_OF_BGM_INFO

void CHARACTER::MainCharacterPacket()
{
	const unsigned mapIndex = GetMapIndex();
	const BGMInfo& bgmInfo = CHARACTER_GetBGMInfo(mapIndex);
	int MuzikDeger = quest::CQuestManager::instance().GetEventFlag("xmas_muzik");

	// SUPPORT_BGM
	if (!bgmInfo.name.empty())
	{
		if (CHARACTER_IsBGMVolumeEnable())
		{
			sys_log(1, "bgm_info.play_bgm_vol(%d, name='%s', vol=%f)", mapIndex, bgmInfo.name.c_str(), bgmInfo.vol);
			TPacketGCMainCharacter4_BGM_VOL mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER4_BGM_VOL;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));

			mainChrPacket.fBGMVol = bgmInfo.vol;
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			if(MuzikDeger == 0){
				strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			}else if(MuzikDeger == 1){
				strlcpy(mainChrPacket.szBGMName, "1.mp3", sizeof(mainChrPacket.szBGMName));
			}else if(MuzikDeger == 2){
				strlcpy(mainChrPacket.szBGMName, "2.mp3", sizeof(mainChrPacket.szBGMName));
			}else if(MuzikDeger == 3){
				strlcpy(mainChrPacket.szBGMName, "3.mp3", sizeof(mainChrPacket.szBGMName));
			}else if(MuzikDeger == 4){
				strlcpy(mainChrPacket.szBGMName, "4.mp3", sizeof(mainChrPacket.szBGMName));
			}else if(MuzikDeger == 4){
				strlcpy(mainChrPacket.szBGMName, "5.mp3", sizeof(mainChrPacket.szBGMName));
			}
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter4_BGM_VOL));
		}
		else
		{
			sys_log(1, "bgm_info.play(%d, '%s')", mapIndex, bgmInfo.name.c_str());
			TPacketGCMainCharacter3_BGM mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER3_BGM;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			if(MuzikDeger == 0){
				strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			}else if(MuzikDeger == 1){
				strlcpy(mainChrPacket.szBGMName, "1.mp3", sizeof(mainChrPacket.szBGMName));
			}else if(MuzikDeger == 2){
				strlcpy(mainChrPacket.szBGMName, "2.mp3", sizeof(mainChrPacket.szBGMName));
			}else if(MuzikDeger == 3){
				strlcpy(mainChrPacket.szBGMName, "3.mp3", sizeof(mainChrPacket.szBGMName));
			}else if(MuzikDeger == 4){
				strlcpy(mainChrPacket.szBGMName, "4.mp3", sizeof(mainChrPacket.szBGMName));
			}else if(MuzikDeger == 4){
				strlcpy(mainChrPacket.szBGMName, "5.mp3", sizeof(mainChrPacket.szBGMName));
			}
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter3_BGM));
		}
		//if (m_stMobile.length())
		//		ChatPacket(CHAT_TYPE_COMMAND, "sms");
	}
	// END_OF_SUPPORT_BGM
	else
	{
		sys_log(0, "bgm_info.play(%d, DEFAULT_BGM_NAME)", mapIndex);

		TPacketGCMainCharacter pack;
		pack.header = HEADER_GC_MAIN_CHARACTER;
		pack.dwVID = m_vid;
		pack.wRaceNum = GetRaceNum();
		pack.lx = GetX();
		pack.ly = GetY();
		pack.lz = GetZ();
		pack.empire = GetDesc()->GetEmpire();
		pack.skill_group = GetSkillGroup();
		strlcpy(pack.szName, GetName(), sizeof(pack.szName));
		GetDesc()->Packet(&pack, sizeof(TPacketGCMainCharacter));

		if (m_stMobile.length())
			ChatPacket(CHAT_TYPE_COMMAND, "sms");
	}
}

void CHARACTER::PointsPacket()
{
	if (!GetDesc())
		return;

	TPacketGCPoints pack;

	pack.header	= HEADER_GC_CHARACTER_POINTS;

	pack.points[POINT_LEVEL]		= GetLevel();
	pack.points[POINT_EXP]		= GetExp();
	pack.points[POINT_NEXT_EXP]		= GetNextExp();
	pack.points[POINT_HP]		= GetHP();
	pack.points[POINT_MAX_HP]		= GetMaxHP();
	pack.points[POINT_SP]		= GetSP();
	pack.points[POINT_MAX_SP]		= GetMaxSP();
	pack.points[POINT_GOLD]		= GetGold();
	pack.points[POINT_STAMINA]		= GetStamina();
	pack.points[POINT_MAX_STAMINA]	= GetMaxStamina();
#ifndef ENABLE_EXTEND_INVEN_SYSTEM
	pack.points[POINT_INVEN]		= Inven_Point();
#endif
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	pack.points[POINT_BATTLE_PASS_PREMIUM_ID] = GetExtBattlePassPremiumID();
#endif

	for (int i = POINT_ST; i < POINT_MAX_NUM; ++i)
		pack.points[i] = GetPoint(i);

	GetDesc()->Packet(&pack, sizeof(TPacketGCPoints));
}

bool CHARACTER::ChangeSex()
{
	int src_race = GetRaceNum();

	switch (src_race)
	{
		case MAIN_RACE_WARRIOR_M:
			m_points.job = MAIN_RACE_WARRIOR_W;
			break;

		case MAIN_RACE_WARRIOR_W:
			m_points.job = MAIN_RACE_WARRIOR_M;
			break;

		case MAIN_RACE_ASSASSIN_M:
			m_points.job = MAIN_RACE_ASSASSIN_W;
			break;

		case MAIN_RACE_ASSASSIN_W:
			m_points.job = MAIN_RACE_ASSASSIN_M;
			break;

		case MAIN_RACE_SURA_M:
			m_points.job = MAIN_RACE_SURA_W;
			break;

		case MAIN_RACE_SURA_W:
			m_points.job = MAIN_RACE_SURA_M;
			break;

		case MAIN_RACE_SHAMAN_M:
			m_points.job = MAIN_RACE_SHAMAN_W;
			break;

		case MAIN_RACE_SHAMAN_W:
			m_points.job = MAIN_RACE_SHAMAN_M;
			break;
#ifdef ENABLE_WOLFMAN_CHARACTER
		case MAIN_RACE_WOLFMAN_M:
			m_points.job = MAIN_RACE_WOLFMAN_M;
			break;
#endif
		default:
			sys_err("CHANGE_SEX: %s unknown race %d", GetName(), src_race);
			return false;
	}

	sys_log(0, "CHANGE_SEX: %s (%d -> %d)", GetName(), src_race, m_points.job);
	return true;
}

WORD CHARACTER::GetRaceNum() const
{
	if (m_dwPolymorphRace)
		return m_dwPolymorphRace;

	if (m_pkMobData)
		return m_pkMobData->m_table.dwVnum;

	return m_points.job;
}

void CHARACTER::SetRace(BYTE race)
{
	if (race >= MAIN_RACE_MAX_NUM)
	{
		sys_err("CHARACTER::SetRace(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
		return;
	}

	m_points.job = race;
}

BYTE CHARACTER::GetJob() const
{
	unsigned race = m_points.job;
	unsigned job;

	if (RaceToJob(race, &job))
		return job;

	sys_err("CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
	return JOB_WARRIOR;
}

void CHARACTER::SetLevel(BYTE level)
{
	m_points.level = level;

	if (IsPC())
	{
		if (level < PK_PROTECT_LEVEL)
			SetPKMode(PK_MODE_PROTECT);
		else if (GetGMLevel() != GM_PLAYER)
			SetPKMode(PK_MODE_PROTECT);
		else if (m_bPKMode == PK_MODE_PROTECT)
			SetPKMode(PK_MODE_PEACE);
	}
}

void CHARACTER::SetEmpire(BYTE bEmpire)
{
	m_bEmpire = bEmpire;
}

#define ENABLE_GM_FLAG_IF_TEST_SERVER
#define ENABLE_GM_FLAG_FOR_LOW_WIZARD
void CHARACTER::SetPlayerProto(const TPlayerTable * t)
{
	if (!GetDesc() || !*GetDesc()->GetHostName())
		sys_err("cannot get desc or hostname");
	else
		SetGMLevel();

	m_bCharType = CHAR_TYPE_PC;

	m_dwPlayerID = t->id;

	m_iAlignment = t->lAlignment;
	m_iRealAlignment = t->lAlignment;

	m_points.voice = t->voice;

	m_points.skill_group = t->skill_group;

	m_pointsInstant.bBasePart = t->part_base;
	SetPart(PART_HAIR, t->parts[PART_HAIR]);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	SetPart(PART_ACCE, t->parts[PART_ACCE]);
#endif
	m_points.iRandomHP = t->sRandomHP;
	m_points.iRandomSP = t->sRandomSP;

	// REMOVE_REAL_SKILL_LEVLES
	if (m_pSkillLevels)
		M2_DELETE_ARRAY(m_pSkillLevels);

	m_pSkillLevels = M2_NEW TPlayerSkill[SKILL_MAX_NUM];
	thecore_memcpy(m_pSkillLevels, t->skills, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES

#ifdef ENABLE_AUTOMATIC_ITEM_PROCESS
	for (size_t i = 0; i < AUTOMATIC_ITEM_PROCESS_MAX; ++i)
	{
		m_automaticItemProcess[i] = t->uiAutomaticItemProcess[i];
	}
#endif

	if (t->lMapIndex >= 10000)
	{
		m_posWarp.x = t->lExitX;
		m_posWarp.y = t->lExitY;
		m_lWarpMapIndex = t->lExitMapIndex;
	}

	SetRealPoint(POINT_PLAYTIME, t->playtime);
	m_dwLoginPlayTime = t->playtime;
	SetRealPoint(POINT_ST, t->st);
	SetRealPoint(POINT_HT, t->ht);
	SetRealPoint(POINT_DX, t->dx);
	SetRealPoint(POINT_IQ, t->iq);

	SetPoint(POINT_ST, t->st);
	SetPoint(POINT_HT, t->ht);
	SetPoint(POINT_DX, t->dx);
	SetPoint(POINT_IQ, t->iq);

	SetPoint(POINT_STAT, t->stat_point);
	SetPoint(POINT_SKILL, t->skill_point);
	SetPoint(POINT_SUB_SKILL, t->sub_skill_point);
	SetPoint(POINT_HORSE_SKILL, t->horse_skill_point);

	SetPoint(POINT_STAT_RESET_COUNT, t->stat_reset_count);

	SetPoint(POINT_LEVEL_STEP, t->level_step);
	SetRealPoint(POINT_LEVEL_STEP, t->level_step);

	SetRace(t->job);

	SetLevel(t->level);
	SetExp(t->exp);
	SetGold(t->gold);
#ifndef ENABLE_EXTEND_INVEN_SYSTEM
	Set_Inventory_Point(t->envanter);
#endif
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	SetExtBattlePassPremiumID(t->battle_pass_premium_id);
#endif
	SetMapIndex(t->lMapIndex);
	SetXYZ(t->x, t->y, t->z);
#ifdef __RANKING_SYSTEM__
	SetDestroyStone(t->killed_metin);
	SetKillMonster(t->killed_monster);
	SetKillBoss(t->killed_boss);
	SetCompletedDungeon(t->success_dungeon);
	SetGaya(GetPoint(POINT_GOLD));//change like your point for example POINT_GAYA etc.
	SetCaughtFishes(t->caught_fishes);
	SetOpenedChest(t->open_chest);
#endif
#ifdef ENABLE_NEW_BIOLOG
	SetBioLevel(t->bioLevel);
	SetBioGivenCount(t->bioGiven);
	SetBioState(t->bioState);
	SetBioTime(t->bioTime);
	SetBioNtf(t->bioNtf);
#endif
	ComputePoints();

	SetHP(t->hp);
	SetSP(t->sp);
	SetStamina(t->stamina);

#ifndef ENABLE_GM_FLAG_IF_TEST_SERVER
	if (!test_server)
#endif
	{
#ifdef ENABLE_GM_FLAG_FOR_LOW_WIZARD
		if (GetGMLevel() > GM_PLAYER)
#else
		if (GetGMLevel() > GM_LOW_WIZARD)
#endif
		{
			m_afAffectFlag.Set(AFF_YMIR);
			m_bPKMode = PK_MODE_PROTECT;
		}
	}

	if (GetLevel() < PK_PROTECT_LEVEL)
		m_bPKMode = PK_MODE_PROTECT;

	m_stMobile = t->szMobile;

	SetHorseData(t->horse);

	if (GetHorseLevel() > 0)
		UpdateHorseDataByLogoff(t->logoff_interval);

	thecore_memcpy(m_aiPremiumTimes, t->aiPremiumTimes, sizeof(t->aiPremiumTimes));

	m_dwLogOffInterval = t->logoff_interval;

	sys_log(0, "PLAYER_LOAD: %s PREMIUM %d %d, LOGGOFF_INTERVAL %u PTR: %p", t->name, m_aiPremiumTimes[0], m_aiPremiumTimes[1], t->logoff_interval, this);

	if (GetGMLevel() != GM_PLAYER)
	{
		LogManager::instance().CharLog(this, GetGMLevel(), "GM_LOGIN", "");
		sys_log(0, "GM_LOGIN(gmlevel=%d, name=%s(%d), pos=(%d, %d)", GetGMLevel(), GetName(), GetPlayerID(), GetX(), GetY());
	}
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (m_mountSystem)
	{
		m_mountSystem->Destroy();
		delete m_mountSystem;
	}
	m_mountSystem = M2_NEW CMountSystem(this);
#endif
#ifdef __PET_SYSTEM__
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;
	}

	m_petSystem = M2_NEW CPetSystem(this);
#endif
#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
	SetOfflineShopVID(COfflineShopManager::instance().FindMyOfflineShop(GetPlayerID()));
#endif
}

#ifdef __ENABLE_BIOLOG_SYSTEM__
	if (m_pkBiologManager)
	{
		sys_err("Biolog manager already exist for owner %u %s", GetPlayerID(), GetName());
		delete m_pkBiologManager;
	}

	m_pkBiologManager = M2_NEW CBiologSystem(this);

	SetBiologMissions(t->m_BiologActualMission);
	SetBiologCollectedItems(t->m_BiologCollectedItems);
	SetBiologCooldownReminder(t->m_BiologCooldownReminder);
	SetBiologCooldown(t->m_BiologCooldown);
#endif

EVENTFUNC(kill_ore_load_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "kill_ore_load_even> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}

	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::SetProto(const CMob * pkMob)
{
	if (m_pkMobInst)
		M2_DELETE(m_pkMobInst);

	m_pkMobData = pkMob;
	m_pkMobInst = M2_NEW CMobInstance;

	m_bPKMode = PK_MODE_FREE;

	const TMobTable * t = &m_pkMobData->m_table;

	m_bCharType = t->bType;

	SetLevel(t->bLevel);
	SetEmpire(t->bEmpire);

	SetExp(t->dwExp);
	SetRealPoint(POINT_ST, t->bStr);
	SetRealPoint(POINT_DX, t->bDex);
	SetRealPoint(POINT_HT, t->bCon);
	SetRealPoint(POINT_IQ, t->bInt);

	ComputePoints();

	SetHP(GetMaxHP());
	SetSP(GetMaxSP());

	////////////////////
	m_pointsInstant.dwAIFlag = t->dwAIFlag;
	SetImmuneFlag(t->dwImmuneFlag);

	AssignTriggers(t);

	ApplyMobAttribute(t);

	if (IsStone())
	{
		DetermineDropMetinStone();
	}

	if (IsWarp() || IsGoto())
	{
		StartWarpNPCEvent();
	}

	CHARACTER_MANAGER::instance().RegisterRaceNumMap(this);

	// XXX X-mas santa hardcoding
	if (GetRaceNum() == xmas::MOB_SANTA_VNUM)
	{
		SetPoint(POINT_ATT_GRADE_BONUS, 10);
		SetPoint(POINT_DEF_GRADE_BONUS, 6);

		//m_dwPlayStartTime = get_dword_time() + 10 * 60 * 1000;
		m_dwPlayStartTime = get_dword_time() + 30 * 1000;
		if (test_server)
			m_dwPlayStartTime = get_dword_time() + 30 * 1000;
	}

	// XXX CTF GuildWar hardcoding
	if (warmap::IsWarFlag(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
	}

	if (warmap::IsWarFlagBase(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
	}

	if (m_bCharType == CHAR_TYPE_HORSE ||
			GetRaceNum() == 20101 ||
			GetRaceNum() == 20102 ||
			GetRaceNum() == 20103 ||
			GetRaceNum() == 20104 ||
			GetRaceNum() == 20105 ||
			GetRaceNum() == 20106 ||
			GetRaceNum() == 20107 ||
			GetRaceNum() == 20108 ||
			GetRaceNum() == 20109
			|| IsMount()
	  )
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
	}

	// MINING
	if (mining::IsVeinOfOre (GetRaceNum()))
	{
		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = this;

		m_pkMiningEvent = event_create(kill_ore_load_event, info, PASSES_PER_SEC(number(7 * 60, 15 * 60)));
	}
	// END_OF_MINING
}

const TMobTable & CHARACTER::GetMobTable() const
{
	return m_pkMobData->m_table;
}

bool CHARACTER::IsRaceFlag(DWORD dwBit) const
{
	return m_pkMobData ? IS_SET(m_pkMobData->m_table.dwRaceFlag, dwBit) : 0;
}

DWORD CHARACTER::GetMobDamageMin() const
{
	return m_pkMobData->m_table.dwDamageRange[0];
}

DWORD CHARACTER::GetMobDamageMax() const
{
	return m_pkMobData->m_table.dwDamageRange[1];
}

float CHARACTER::GetMobDamageMultiply() const
{
	float fDamMultiply = GetMobTable().fDamMultiply;

	if (IsBerserk())
		fDamMultiply = fDamMultiply * 2.0f;

	return fDamMultiply;
}

DWORD CHARACTER::GetMobDropItemVnum() const
{
	return m_pkMobData->m_table.dwDropItemVnum;
}

bool CHARACTER::IsSummonMonster() const
{
	return GetSummonVnum() != 0;
}

DWORD CHARACTER::GetSummonVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwSummonVnum : 0;
}

DWORD CHARACTER::GetPolymorphItemVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwPolymorphItemVnum : 0;
}

DWORD CHARACTER::GetMonsterDrainSPPoint() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwDrainSP : 0;
}

BYTE CHARACTER::GetMobRank() const
{
	if (!m_pkMobData)
		return MOB_RANK_KNIGHT;

	return m_pkMobData->m_table.bRank;
}

BYTE CHARACTER::GetMobSize() const
{
	if (!m_pkMobData)
		return MOBSIZE_MEDIUM;

	return m_pkMobData->m_table.bSize;
}

WORD CHARACTER::GetMobAttackRange() const
{
	switch (GetMobBattleType())
	{
		case BATTLE_TYPE_RANGE:
		case BATTLE_TYPE_MAGIC:
			return m_pkMobData->m_table.wAttackRange + GetPoint(POINT_BOW_DISTANCE);
		default:
			return m_pkMobData->m_table.wAttackRange;
	}
}

BYTE CHARACTER::GetMobBattleType() const
{
	if (!m_pkMobData)
		return BATTLE_TYPE_MELEE;

	return (m_pkMobData->m_table.bBattleType);
}

void CHARACTER::ComputeBattlePoints()
{
	if (IsPolymorphed())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
		int iAtt = 0;
		int iDef = 0;

		if (pMob)
		{
			iAtt = GetLevel() * 2 + GetPolymorphPoint(POINT_ST) * 2;
			// lev + con
			iDef = GetLevel() + GetPolymorphPoint(POINT_HT) + pMob->m_table.wDef;
		}

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
	else if (IsPC())
	{
		SetPoint(POINT_ATT_GRADE, 0);
		SetPoint(POINT_DEF_GRADE, 0);
		SetPoint(POINT_CLIENT_DEF_GRADE, 0);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));

		//
		// ATK = 2lev + 2str
		//
		int iAtk = GetLevel() * 2;
		int iStatAtk = 0;

		switch (GetJob())
		{
			case JOB_WARRIOR:
			case JOB_SURA:
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;

			case JOB_ASSASSIN:
				iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_DX)) / 3;
				break;

			case JOB_SHAMAN:
				iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_IQ)) / 3;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case JOB_WOLFMAN:
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;
#endif
			default:
				sys_err("invalid job %d", GetJob());
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;
		}

		if (GetMountVnum() && iStatAtk < 2 * GetPoint(POINT_ST))
			iStatAtk = (2 * GetPoint(POINT_ST));

		iAtk += iStatAtk;

		if (GetMountVnum())
		{
			if (GetJob() == JOB_SURA && GetSkillGroup() == 1)
			{
				iAtk += (iAtk * GetHorseLevel()) / 60;
			}
			else
			{
				iAtk += (iAtk * GetHorseLevel()) / 30;
			}
		}

		//
		// ATK Setting
		//
		iAtk += GetPoint(POINT_ATT_GRADE_BONUS);

		PointChange(POINT_ATT_GRADE, iAtk);

		// DEF = LEV + CON + ARMOR
		int iShowDef = GetLevel() + GetPoint(POINT_HT);
		int iDef = GetLevel() + (int) (GetPoint(POINT_HT) / 1.25); // For Other
		int iArmor = 0;

		LPITEM pkItem;

		for (int i = 0; i < WEAR_MAX_NUM; ++i)
			if ((pkItem = GetWear(i)) && pkItem->GetType() == ITEM_ARMOR)
			{
				if (pkItem->GetSubType() == ARMOR_BODY || pkItem->GetSubType() == ARMOR_HEAD || pkItem->GetSubType() == ARMOR_FOOTS || pkItem->GetSubType() == ARMOR_SHIELD)
				{
					iArmor += pkItem->GetValue(1);
					iArmor += (2 * pkItem->GetValue(5));
				}
			}

		if( true == IsHorseRiding() )
		{
			if (iArmor < GetHorseArmor())
				iArmor = GetHorseArmor();

			const char* pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());

			if (pHorseName != NULL && strlen(pHorseName))
			{
				iArmor += 20;
			}
		}

		iArmor += GetPoint(POINT_DEF_GRADE_BONUS);
		iArmor += GetPoint(POINT_PARTY_DEFENDER_BONUS);

		// INTERNATIONAL_VERSION
		PointChange(POINT_DEF_GRADE, iDef + iArmor);
		PointChange(POINT_CLIENT_DEF_GRADE, (iShowDef + iArmor) - GetPoint(POINT_DEF_GRADE));
		// END_OF_INTERNATIONAL_VERSION

		PointChange(POINT_MAGIC_ATT_GRADE, GetLevel() * 2 + GetPoint(POINT_IQ) * 2 + GetPoint(POINT_MAGIC_ATT_GRADE_BONUS));
		PointChange(POINT_MAGIC_DEF_GRADE, GetLevel() + (GetPoint(POINT_IQ) * 3 + GetPoint(POINT_HT)) / 3 + iArmor / 2 + GetPoint(POINT_MAGIC_DEF_GRADE_BONUS));
	}
	else
	{
		// 2lev + str * 2
		int iAtt = GetLevel() * 2 + GetPoint(POINT_ST) * 2;
		// lev + con
		int iDef = GetLevel() + GetPoint(POINT_HT) + GetMobTable().wDef;

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
}

void CHARACTER::ComputePoints()
{
	long lStat = GetPoint(POINT_STAT);
	long lStatResetCount = GetPoint(POINT_STAT_RESET_COUNT);
	long lSkillActive = GetPoint(POINT_SKILL);
	long lSkillSub = GetPoint(POINT_SUB_SKILL);
	long lSkillHorse = GetPoint(POINT_HORSE_SKILL);
	long lLevelStep = GetPoint(POINT_LEVEL_STEP);

	long lAttackerBonus = GetPoint(POINT_PARTY_ATTACKER_BONUS);
	long lTankerBonus = GetPoint(POINT_PARTY_TANKER_BONUS);
	long lBufferBonus = GetPoint(POINT_PARTY_BUFFER_BONUS);
	long lSkillMasterBonus = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
	long lHasteBonus = GetPoint(POINT_PARTY_HASTE_BONUS);
	long lDefenderBonus = GetPoint(POINT_PARTY_DEFENDER_BONUS);

	long lHPRecovery = GetPoint(POINT_HP_RECOVERY);
	long lSPRecovery = GetPoint(POINT_SP_RECOVERY);
#ifndef ENABLE_EXTEND_INVEN_SYSTEM
	long envanterim = Inven_Point();
#endif

	memset(m_pointsInstant.points, 0, sizeof(m_pointsInstant.points));
	BuffOnAttr_ClearAll();
	m_SkillDamageBonus.clear();

	SetPoint(POINT_STAT, lStat);
	SetPoint(POINT_SKILL, lSkillActive);
	SetPoint(POINT_SUB_SKILL, lSkillSub);
	SetPoint(POINT_HORSE_SKILL, lSkillHorse);
	SetPoint(POINT_LEVEL_STEP, lLevelStep);
	SetPoint(POINT_STAT_RESET_COUNT, lStatResetCount);

	SetPoint(POINT_ST, GetRealPoint(POINT_ST));
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetPart(PART_MAIN, GetOriginalPart(PART_MAIN));
	SetPart(PART_WEAPON, GetOriginalPart(PART_WEAPON));
	SetPart(PART_HEAD, GetOriginalPart(PART_HEAD));
	SetPart(PART_HAIR, GetOriginalPart(PART_HAIR));
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	SetPart(PART_ACCE, GetOriginalPart(PART_ACCE));
#endif

	SetPoint(POINT_PARTY_ATTACKER_BONUS, lAttackerBonus);
	SetPoint(POINT_PARTY_TANKER_BONUS, lTankerBonus);
	SetPoint(POINT_PARTY_BUFFER_BONUS, lBufferBonus);
	SetPoint(POINT_PARTY_SKILL_MASTER_BONUS, lSkillMasterBonus);
	SetPoint(POINT_PARTY_HASTE_BONUS, lHasteBonus);
	SetPoint(POINT_PARTY_DEFENDER_BONUS, lDefenderBonus);

	SetPoint(POINT_HP_RECOVERY, lHPRecovery);
	SetPoint(POINT_SP_RECOVERY, lSPRecovery);

	// PC_BANG_ITEM_ADD
	SetPoint(POINT_PC_BANG_EXP_BONUS, 0);
	SetPoint(POINT_PC_BANG_DROP_BONUS, 0);
	// END_PC_BANG_ITEM_ADD

#ifndef ENABLE_EXTEND_INVEN_SYSTEM
	SetPoint(POINT_INVEN, envanterim);
#endif


	int iMaxHP, iMaxSP;
	int iMaxStamina;

#ifdef ENABLE_NEW_PASSIVE_SKILL
	CSkillProto* pkSk;
	int ekle = 0;
#endif
	if (IsPC())
	{
		iMaxHP = JobInitialPoints[GetJob()].max_hp + m_points.iRandomHP + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].hp_per_ht;
		iMaxSP = JobInitialPoints[GetJob()].max_sp + m_points.iRandomSP + GetPoint(POINT_IQ) * JobInitialPoints[GetJob()].sp_per_iq;
		iMaxStamina = JobInitialPoints[GetJob()].max_stamina + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].stamina_per_con;

		{
			CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_ADD_HP);

			if (NULL != pkSk)
			{
				pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_ADD_HP) / 100.0f);

				iMaxHP += static_cast<int>(pkSk->kPointPoly.Eval());
			}
		}

#ifdef ENABLE_NEW_PASSIVE_SKILL
		{
			pkSk = CSkillManager::instance().Get(SKILL_MONSTER_BONUS);

			if (NULL != pkSk)
			{
				pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_MONSTER_BONUS) / 100.0f);

				ekle = static_cast<int>(pkSk->kPointPoly.Eval());
				PointChange(POINT_ATTBONUS_MONSTER,ekle);
			}
		}

		{
			pkSk = CSkillManager::instance().Get(SKILL_STONE_BONUS);

			if (NULL != pkSk)
			{
				pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_STONE_BONUS) / 100.0f);

				ekle = static_cast<int>(pkSk->kPointPoly.Eval());
				PointChange(POINT_RESIST_CLAW,ekle);
			}
		}

		{
			pkSk = CSkillManager::instance().Get(SKILL_BOSS_BONUS);

			if (NULL != pkSk)
			{
				pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_BOSS_BONUS) / 100.0f);

				ekle = static_cast<int>(pkSk->kPointPoly.Eval());
				PointChange(POINT_RESIST_WOLFMAN,ekle);
			}
		}
#endif

		SetPoint(POINT_MOV_SPEED,	100);
		SetPoint(POINT_ATT_SPEED,	100);
		PointChange(POINT_ATT_SPEED, GetPoint(POINT_PARTY_HASTE_BONUS));
		SetPoint(POINT_CASTING_SPEED,	100);
	}
	else
	{
		iMaxHP = m_pkMobData->m_table.dwMaxHP;
		iMaxSP = 0;
		iMaxStamina = 0;

		SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		SetPoint(POINT_MOV_SPEED, m_pkMobData->m_table.sMovingSpeed);
		SetPoint(POINT_CASTING_SPEED, m_pkMobData->m_table.sAttackSpeed);
	}

	if (IsPC())
	{
		if (GetMountVnum())
		{
			if (GetHorseST() > GetPoint(POINT_ST))
				PointChange(POINT_ST, GetHorseST() - GetPoint(POINT_ST));

			if (GetHorseDX() > GetPoint(POINT_DX))
				PointChange(POINT_DX, GetHorseDX() - GetPoint(POINT_DX));

			if (GetHorseHT() > GetPoint(POINT_HT))
				PointChange(POINT_HT, GetHorseHT() - GetPoint(POINT_HT));

			if (GetHorseIQ() > GetPoint(POINT_IQ))
				PointChange(POINT_IQ, GetHorseIQ() - GetPoint(POINT_IQ));
		}

	}

	ComputeBattlePoints();

	if (iMaxHP != GetMaxHP())
	{
		SetRealPoint(POINT_MAX_HP, iMaxHP);
	}

	PointChange(POINT_MAX_HP, 0);

	if (iMaxSP != GetMaxSP())
	{
		SetRealPoint(POINT_MAX_SP, iMaxSP);
	}

	PointChange(POINT_MAX_SP, 0);

	SetMaxStamina(iMaxStamina);
	// @fixme118 part1
	int iCurHP = this->GetHP();
	int iCurSP = this->GetSP();

	m_pointsInstant.dwImmuneFlag = 0;

	for (int i = 0 ; i < WEAR_MAX_NUM; i++)
	{
		LPITEM pItem = GetWear(i);
		if (pItem)
		{
			pItem->ModifyPoints(true);
			SET_BIT(m_pointsInstant.dwImmuneFlag, GetWear(i)->GetImmuneFlag());
		}
	}

#ifdef ENABLE_EVENT_MANAGER
	CHARACTER_MANAGER::Instance().CheckBonusEvent(this);
#endif

	if (DragonSoul_IsDeckActivated())
	{
		for (int i = WEAR_MAX_NUM + DS_SLOT_MAX * DragonSoul_GetActiveDeck();
			i < WEAR_MAX_NUM + DS_SLOT_MAX * (DragonSoul_GetActiveDeck() + 1); i++)
		{
			LPITEM pItem = GetWear(i);
			if (pItem)
			{
				if (DSManager::instance().IsTimeLeftDragonSoul(pItem))
					pItem->ModifyPoints(true);
			}
		}
	}

	if (GetHP() > GetMaxHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetSP() > GetMaxSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());

	ComputeSkillPoints();

	RefreshAffect();

	CPetSystem * pPetSystem = GetPetSystem();
	if (NULL != pPetSystem)
		pPetSystem->RefreshBuff();

	// @fixme118 part2 (after petsystem stuff)
	if (IsPC())
	{
		if (this->GetHP() != iCurHP)
			this->PointChange(POINT_HP, iCurHP-this->GetHP());
		if (this->GetSP() != iCurSP)
			this->PointChange(POINT_SP, iCurSP-this->GetSP());
	}

	UpdatePacket();
}

void CHARACTER::ResetPlayTime(DWORD dwTimeRemain)
{
	m_dwPlayStartTime = get_dword_time() - dwTimeRemain;
}

const int aiRecoveryPercents[10] = { 1, 5, 5, 5, 5, 5, 5, 5, 5, 5 };

EVENTFUNC(recovery_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "recovery_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (!ch->IsPC())
	{
		if (ch->IsAffectFlag(AFF_POISON))
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
#ifdef ENABLE_WOLFMAN_CHARACTER
		if (ch->IsAffectFlag(AFF_BLEEDING))
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
#endif
		if (2493 == ch->GetMobTable().dwVnum)
		{
			int regenPct = BlueDragon_GetRangeFactor("hp_regen", ch->GetHPPct());
			regenPct += ch->GetMobTable().bRegenPercent;

			for (int i=1 ; i <= 4 ; ++i)
			{
				if (REGEN_PECT_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap( ch->GetMapIndex(), dwDragonStoneID );

					regenPct += (val*cnt);

					break;
				}
			}

			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * regenPct) / 100));
		}
		else if (!ch->IsDoor())
		{
			ch->MonsterLog("HP_REGEN +%d", MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
		}

		if (ch->GetHP() >= ch->GetMaxHP())
		{
			ch->m_pkRecoveryEvent = NULL;
			return 0;
		}

		if (2493 == ch->GetMobTable().dwVnum)
		{
			for (int i=1 ; i <= 4 ; ++i)
			{
				if (REGEN_TIME_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap( ch->GetMapIndex(), dwDragonStoneID );

					return PASSES_PER_SEC(MAX(1, (ch->GetMobTable().bRegenCycle - (val*cnt))));
				}
			}
		}

		return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
	}
	else
	{
		ch->CheckTarget();
		ch->UpdateKillerMode();

		if (ch->IsAffectFlag(AFF_POISON) == true)
		{
			return 3;
		}
#ifdef ENABLE_WOLFMAN_CHARACTER
		if (ch->IsAffectFlag(AFF_BLEEDING))
			return 3;
#endif
		int iSec = (get_dword_time() - ch->GetLastMoveTime()) / 3000;

		ch->DistributeSP(ch);

		if (ch->GetMaxHP() <= ch->GetHP())
			return PASSES_PER_SEC(3);

		int iPercent = 0;
		int iAmount = 0;

		{
			iPercent = aiRecoveryPercents[MIN(9, iSec)];
			iAmount = 15 + (ch->GetMaxHP() * iPercent) / 100;
		}

		iAmount += (iAmount * ch->GetPoint(POINT_HP_REGEN)) / 100;

		sys_log(1, "RECOVERY_EVENT: %s %d HP_REGEN %d HP +%d", ch->GetName(), iPercent, ch->GetPoint(POINT_HP_REGEN), iAmount);

		ch->PointChange(POINT_HP, iAmount, false);
		return PASSES_PER_SEC(3);
	}
}

void CHARACTER::StartRecoveryEvent()
{
	if (m_pkRecoveryEvent)
		return;

	if (IsDead() || IsStun())
		return;

	if (IsNPC() && GetHP() >= GetMaxHP())
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	int iSec = IsPC() ? 3 : (MAX(1, GetMobTable().bRegenCycle));
	m_pkRecoveryEvent = event_create(recovery_event, info, PASSES_PER_SEC(iSec));
}

void CHARACTER::Standup()
{
	struct packet_position pack_position;

	if (!IsPosition(POS_SITTING))
		return;

	SetPosition(POS_STANDING);

	sys_log(1, "STANDUP: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_GENERAL;

	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::Sitdown(int is_ground)
{
	struct packet_position pack_position;

	if (IsPosition(POS_SITTING))
		return;

	SetPosition(POS_SITTING);
	sys_log(1, "SITDOWN: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_SITTING_GROUND;
	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::SetRotation(float fRot)
{
	m_pointsInstant.fRot = fRot;
}

void CHARACTER::SetRotationToXY(long x, long y)
{
	SetRotation(GetDegreeFromPositionXY(GetX(), GetY(), x, y));
}

bool CHARACTER::CannotMoveByAffect() const
{
	return (IsAffectFlag(AFF_STUN));
}

bool CHARACTER::CanMove() const
{
	if (CannotMoveByAffect())
		return false;

	if (GetMyShop())
		return false;

	/*
	   if (get_float_time() - m_fSyncTime < 0.2f)
	   return false;
	 */
	return true;
}

bool CHARACTER::Sync(long x, long y)
{
	if (!GetSectree())
		return false;

	LPSECTREE new_tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), x, y);

	if (!new_tree)
	{
		if (GetDesc())
		{
			sys_err("cannot find tree at %d %d (name: %s)", x, y, GetName());
			GetDesc()->SetPhase(PHASE_CLOSE);
		}
		else
		{
			sys_err("no tree: %s %d %d %d", GetName(), x, y, GetMapIndex());
			Dead();
		}

		return false;
	}

	SetRotationToXY(x, y);
	SetXYZ(x, y, 0);

	if (GetDungeon())
	{
		int iLastEventAttr = m_iEventAttr;
		m_iEventAttr = new_tree->GetEventAttribute(x, y);

		if (m_iEventAttr != iLastEventAttr)
		{
			if (GetParty())
			{
				quest::CQuestManager::instance().AttrOut(GetParty()->GetLeaderPID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetParty()->GetLeaderPID(), this, m_iEventAttr);
			}
			else
			{
				quest::CQuestManager::instance().AttrOut(GetPlayerID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetPlayerID(), this, m_iEventAttr);
			}
		}
	}

	if (GetSectree() != new_tree)
	{
		if (!IsNPC())
		{
			SECTREEID id = new_tree->GetID();
			SECTREEID old_id = GetSectree()->GetID();

			const float fDist = DISTANCE_SQRT(id.coord.x - old_id.coord.x, id.coord.y - old_id.coord.y);
			sys_log(0, "SECTREE DIFFER: %s %dx%d was %dx%d dist %.1fm",
					GetName(),
					id.coord.x,
					id.coord.y,
					old_id.coord.x,
					old_id.coord.y,
					fDist);
		}

		new_tree->InsertEntity(this);
	}

	return true;
}

void CHARACTER::Stop()
{
	if (!IsState(m_stateIdle))
		MonsterLog("[IDLE] ����");

	GotoState(m_stateIdle);

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
}

bool CHARACTER::Goto(long x, long y)
{
	if (GetX() == x && GetY() == y)
		return false;

	if (m_posDest.x == x && m_posDest.y == y)
	{
		if (!IsState(m_stateMove))
		{
			m_dwStateDuration = 4;
			GotoState(m_stateMove);
		}
		return false;
	}

	m_posDest.x = x;
	m_posDest.y = y;

	CalculateMoveDuration();

	m_dwStateDuration = 4;


	if (!IsState(m_stateMove))
	{
		MonsterLog("[MOVE] %s", GetVictim() ? "�������" : "�׳��̵�");

		if (GetVictim())
		{
			//MonsterChat(MONSTER_CHAT_CHASE);
			MonsterChat(MONSTER_CHAT_ATTACK);
		}
	}

	GotoState(m_stateMove);

	return true;
}


DWORD CHARACTER::GetMotionMode() const
{
	DWORD dwMode = MOTION_MODE_GENERAL;

	if (IsPolymorphed())
		return dwMode;

	LPITEM pkItem;

	if ((pkItem = GetWear(WEAR_WEAPON)))
	{
		switch (pkItem->GetProto()->bSubType)
		{
			case WEAPON_SWORD:
				dwMode = MOTION_MODE_ONEHAND_SWORD;
				break;

			case WEAPON_TWO_HANDED:
				dwMode = MOTION_MODE_TWOHAND_SWORD;
				break;

			case WEAPON_DAGGER:
				dwMode = MOTION_MODE_DUALHAND_SWORD;
				break;

			case WEAPON_BOW:
				dwMode = MOTION_MODE_BOW;
				break;

			case WEAPON_BELL:
				dwMode = MOTION_MODE_BELL;
				break;

			case WEAPON_FAN:
				dwMode = MOTION_MODE_FAN;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case WEAPON_CLAW:
				dwMode = MOTION_MODE_CLAW;
				break;
#endif
		}
	}
	return dwMode;
}

float CHARACTER::GetMoveMotionSpeed() const
{
	DWORD dwMode = GetMotionMode();

	const CMotion * pkMotion = NULL;

	if (!GetMountVnum())
		pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(dwMode, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	else
	{
		pkMotion = CMotionManager::instance().GetMotion(GetMountVnum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));

		if (!pkMotion)
			pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_HORSE, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	}

	if (pkMotion)
		return -pkMotion->GetAccumVector().y / pkMotion->GetDuration();
	else
	{
		sys_err("cannot find motion (name %s race %d mode %d)", GetName(), GetRaceNum(), dwMode);
		return 300.0f;
	}
}

float CHARACTER::GetMoveSpeed() const
{
	return GetMoveMotionSpeed() * 10000 / CalculateDuration(GetLimitPoint(POINT_MOV_SPEED), 10000);
}

void CHARACTER::CalculateMoveDuration()
{
	m_posStart.x = GetX();
	m_posStart.y = GetY();

	float fDist = DISTANCE_SQRT(m_posStart.x - m_posDest.x, m_posStart.y - m_posDest.y);

	float motionSpeed = GetMoveMotionSpeed();

	m_dwMoveDuration = CalculateDuration(GetLimitPoint(POINT_MOV_SPEED),
			(int) ((fDist / motionSpeed) * 1000.0f));

	if (IsNPC())
		sys_log(1, "%s: GOTO: distance %f, spd %u, duration %u, motion speed %f pos %d %d -> %d %d",
				GetName(), fDist, GetLimitPoint(POINT_MOV_SPEED), m_dwMoveDuration, motionSpeed,
				m_posStart.x, m_posStart.y, m_posDest.x, m_posDest.y);

	m_dwMoveStartTime = get_dword_time();
}

bool CHARACTER::Move(long x, long y)
{
	if (GetX() == x && GetY() == y)
		return true;

	if (test_server)
		if (m_bDetailLog)
			sys_log(0, "%s position %u %u", GetName(), x, y);

	OnMove();
	return Sync(x, y);
}

void CHARACTER::SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, int iRot)
{
	TPacketGCMove pack;

	if (bFunc == FUNC_WAIT)
	{
		x = m_posDest.x;
		y = m_posDest.y;
		dwDuration = m_dwMoveDuration;
	}

	EncodeMovePacket(pack, GetVID(), bFunc, bArg, x, y, dwDuration, dwTime, iRot == -1 ? (int) GetRotation() / 5 : iRot);
	PacketView(&pack, sizeof(TPacketGCMove), this);
}


int CHARACTER::GetRealPoint(BYTE type) const
{
	return m_points.points[type];
}

void CHARACTER::SetRealPoint(BYTE type, int val)
{
	m_points.points[type] = val;
}

int CHARACTER::GetPolymorphPoint(BYTE type) const
{
	if (IsPolymorphed() && !IsPolyMaintainStat())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
		int iPower = GetPolymorphPower();

		if (pMob)
		{
			switch (type)
			{
				case POINT_ST:
					if ((GetJob() == JOB_SHAMAN) || ((GetJob() == JOB_SURA) && (GetSkillGroup() == 2)))
						return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_IQ);
					return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_ST);

				case POINT_HT:
					return pMob->m_table.bCon * iPower / 100 + GetPoint(POINT_HT);

				case POINT_IQ:
					return pMob->m_table.bInt * iPower / 100 + GetPoint(POINT_IQ);

				case POINT_DX:
					return pMob->m_table.bDex * iPower / 100 + GetPoint(POINT_DX);
			}
		}
	}

	return GetPoint(type);
}

long long CHARACTER::GetPoint(BYTE type) const
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

	long long val = m_pointsInstant.points[type];
	long long max_val = LLONG_MAX;

	switch (type)
	{
		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
			max_val = 50;
			break;
		case POINT_GOLD:
			max_val = GOLD_MAX;
			break;		
	}

	if (val > max_val)
		sys_err("POINT_ERROR: %s type %d val %lld (max: %lld)", GetName(), val, max_val);

	return (val);
}	

int CHARACTER::GetLimitPoint(BYTE type) const
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

	int val = m_pointsInstant.points[type];
	int max_val = INT_MAX;
	int limit = INT_MAX;
	int min_limit = -INT_MAX;

	switch (type)
	{
		case POINT_ATT_SPEED:
			min_limit = 0;

			if (IsPC())
				limit = 170;
			else
				limit = 250;
			break;

		case POINT_MOV_SPEED:
			min_limit = 0;

			if (IsPC())
				limit = 200;
			else
				limit = 250;
			break;

		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
			limit = 50;
			max_val = 50;
			break;

		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
			limit = 20;
			max_val = 50;
			break;
	}

	if (val > max_val)
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), val, max_val);

	if (val > limit)
		val = limit;

	if (val < min_limit)
		val = min_limit;

	return (val);
}

void CHARACTER::SetPoint(BYTE type, long long val)
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return;
	}

	m_pointsInstant.points[type] = val;

	if (type == POINT_MOV_SPEED && get_dword_time() < m_dwMoveStartTime + m_dwMoveDuration)
	{
		CalculateMoveDuration();
	}
}

long long CHARACTER::GetAllowedGold() const
{
		return 50000000000000000;
}

void CHARACTER::CheckMaximumPoints()
{
	if (GetMaxHP() < GetHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetMaxSP() < GetSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());
}

void CHARACTER::PointChange(BYTE type, long long amount, bool bAmount, bool bBroadcast)
{
	long long val = 0;

	//sys_log(0, "PointChange %d %d | %d -> %d cHP %d mHP %d", type, amount, GetPoint(type), GetPoint(type)+amount, GetHP(), GetMaxHP());

	switch (type)
	{
		case POINT_NONE:
			return;

		case POINT_LEVEL:
			if ((GetLevel() + amount) > gPlayerMaxLevel)
				return;

			SetLevel(GetLevel() + amount);
			val = GetLevel();

#ifdef ENABLE_NEW_MISSIONS
			if (CNewMissions::instance().GetGlobalMissionValue(CNewMissions::eGlobalMissionType::FIRST_75_LEVEL) != -1) {
				CNewMissions::instance().UpdateGlobalMissions(this, val, CNewMissions::eGlobalMissionType::FIRST_75_LEVEL);
			}
			if (CNewMissions::instance().GetGlobalMissionValue(CNewMissions::eGlobalMissionType::FIRST_99_LEVEL) != -1) {
				CNewMissions::instance().UpdateGlobalMissions(this, val, CNewMissions::eGlobalMissionType::FIRST_99_LEVEL);
			}
#endif

			sys_log(0, "LEVELUP: %s %d NEXT EXP %d", GetName(), GetLevel(), GetNextExp());
#ifdef ENABLE_WOLFMAN_CHARACTER
			if (GetJob() == JOB_WOLFMAN)
			{
				if ((5 <= val) && (GetSkillGroup()!=1))
				{
					ClearSkill();
					// set skill group
					SetSkillGroup(1);
					// set skill points
					SetRealPoint(POINT_SKILL, GetLevel()-1);
					SetPoint(POINT_SKILL, GetRealPoint(POINT_SKILL));
					PointChange(POINT_SKILL, 0);
					// update points (not required)
					// ComputePoints();
					// PointsPacket();
				}
			}
#endif
			PointChange(POINT_NEXT_EXP,	GetNextExp(), false);

			if (amount)
			{
				quest::CQuestManager::instance().LevelUp(GetPlayerID());

#ifdef ENABLE_REWARD_SYSTEM
				CHARACTER_MANAGER::Instance().DoReward(this, REWARD_MISSION_LEVEL_UP, GetLevel(), 1);
#endif

				LogManager::instance().LevelLog(this, val, GetRealPoint(POINT_PLAYTIME) + (get_dword_time() - m_dwPlayStartTime) / 60000);

				if (GetGuild())
				{
					GetGuild()->LevelChange(GetPlayerID(), GetLevel());
				}

				if (GetParty())
				{
					GetParty()->RequestSetMemberLevel(GetPlayerID(), GetLevel());
				}
			}
			break;

		case POINT_NEXT_EXP:
			val = GetNextExp();
			bAmount = false;
			break;

		case POINT_EXP:
			{
				DWORD exp = GetExp();
				DWORD next_exp = GetNextExp();

				if (g_bChinaIntoxicationCheck)
				{
					if (IsOverTime(OT_NONE))
					{
						dev_log(LOG_DEB0, "<EXP_LOG> %s = NONE", GetName());
					}
					else if (IsOverTime(OT_3HOUR))
					{
						amount = (amount / 2);
						dev_log(LOG_DEB0, "<EXP_LOG> %s = 3HOUR", GetName());
					}
					else if (IsOverTime(OT_5HOUR))
					{
						amount = 0;
						dev_log(LOG_DEB0, "<EXP_LOG> %s = 5HOUR", GetName());
					}
				}

				if ((amount < 0) && (exp < (DWORD)(-amount)))
				{
					sys_log(1, "%s AMOUNT < 0 %d, CUR EXP: %d", GetName(), -amount, exp);
					amount = -exp;

					SetExp(exp + amount);
					val = GetExp();
				}
				else
				{
					if (gPlayerMaxLevel <= GetLevel())
						return;

					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, "You have gained %d exp.", amount);

					DWORD iExpBalance = 0;

					if (exp + amount >= next_exp)
					{
						iExpBalance = (exp + amount) - next_exp;
						amount = next_exp - exp;

						SetExp(0);
						exp = next_exp;
					}
					else
					{
						SetExp(exp + amount);
						exp = GetExp();
					}

					DWORD q = DWORD(next_exp / 4.0f);
					int iLevStep = GetRealPoint(POINT_LEVEL_STEP);

					if (iLevStep >= 4)
					{
						sys_err("%s LEVEL_STEP bigger than 4! (%d)", GetName(), iLevStep);
						iLevStep = 4;
					}

					if (exp >= next_exp && iLevStep < 4)
					{
						for (int i = 0; i < 4 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q * 3 && iLevStep < 3)
					{
						for (int i = 0; i < 3 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q * 2 && iLevStep < 2)
					{
						for (int i = 0; i < 2 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q && iLevStep < 1)
						PointChange(POINT_LEVEL_STEP, 1);

					if (iExpBalance)
					{
						PointChange(POINT_EXP, iExpBalance);
					}

					val = GetExp();
				}
			}
			break;

		case POINT_LEVEL_STEP:
			if (amount > 0)
			{
				val = GetPoint(POINT_LEVEL_STEP) + amount;

				switch (val)
				{
					case 1:
					case 2:
					case 3:
						if ((GetLevel() <= g_iStatusPointGetLevelLimit) &&
							(GetLevel() <= gPlayerMaxLevel) ) // @fixme104
							PointChange(POINT_STAT, 1);
						break;

					case 4:
						{
							int iHP = number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end);
							int iSP = number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end);

							m_points.iRandomHP += iHP;
							m_points.iRandomSP += iSP;

							if (GetSkillGroup())
							{
								if (GetLevel() >= 5)
									PointChange(POINT_SKILL, 1);

								if (GetLevel() >= 9)
									PointChange(POINT_SUB_SKILL, 1);
							}

							PointChange(POINT_MAX_HP, iHP);
							PointChange(POINT_MAX_SP, iSP);
							PointChange(POINT_LEVEL, 1, false, true);

							val = 0;
						}
						break;
				}

				if (GetLevel() <= 10)
					AutoGiveItem(27001, 2);
				else if (GetLevel() <= 30)
					AutoGiveItem(27002, 2);
				else
				{
					AutoGiveItem(27002, 2);
//					AutoGiveItem(27003, 2);
				}

				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				PointChange(POINT_STAMINA, GetMaxStamina() - GetStamina());

				SetPoint(POINT_LEVEL_STEP, val);
				SetRealPoint(POINT_LEVEL_STEP, val);

				Save();
			}
			else
				val = GetPoint(POINT_LEVEL_STEP);

			break;

		case POINT_HP:
			{
				if (IsDead() || IsStun())
					return;

				int prev_hp = GetHP();

				amount = MIN(GetMaxHP() - GetHP(), amount);
				SetHP(GetHP() + amount);
				val = GetHP();

				BroadcastTargetPacket();

				if (GetParty() && IsPC() && val != prev_hp)
					GetParty()->SendPartyInfoOneToAll(this);
			}
			break;

		case POINT_SP:
			{
				if (IsDead() || IsStun())
					return;

				amount = MIN(GetMaxSP() - GetSP(), amount);
				SetSP(GetSP() + amount);
				val = GetSP();
			}
			break;

		case POINT_STAMINA:
			{
				if (IsDead() || IsStun())
					return;

				int prev_val = GetStamina();
				amount = MIN(GetMaxStamina() - GetStamina(), amount);
				SetStamina(GetStamina() + amount);
				val = GetStamina();

				if (val == 0)
				{
					// Stamina
					SetNowWalking(true);
				}
				else if (prev_val == 0)
				{
					ResetWalking();
				}

				if (amount < 0 && val != 0)
					return;
			}
			break;

		case POINT_MAX_HP:
			{
				SetPoint(type, GetPoint(type) + amount);

				//SetMaxHP(GetMaxHP() + amount);
				int hp = GetRealPoint(POINT_MAX_HP);
				int add_hp = MIN(3500, hp * GetPoint(POINT_MAX_HP_PCT) / 100);
				add_hp += GetPoint(POINT_MAX_HP);
				add_hp += GetPoint(POINT_PARTY_TANKER_BONUS);

				SetMaxHP(hp + add_hp);

				val = GetMaxHP();
			}
			break;

		case POINT_MAX_SP:
			{
				SetPoint(type, GetPoint(type) + amount);

				//SetMaxSP(GetMaxSP() + amount);
				int sp = GetRealPoint(POINT_MAX_SP);
				int add_sp = MIN(800, sp * GetPoint(POINT_MAX_SP_PCT) / 100);
				add_sp += GetPoint(POINT_MAX_SP);
				add_sp += GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);

				SetMaxSP(sp + add_sp);

				val = GetMaxSP();
			}
			break;

		case POINT_MAX_HP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_MAX_HP, 0);
			break;

		case POINT_MAX_SP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_MAX_SP, 0);
			break;

		case POINT_MAX_STAMINA:
			SetMaxStamina(GetMaxStamina() + amount);
			val = GetMaxStamina();
			break;

#ifndef ENABLE_EXTEND_INVEN_SYSTEM
		case POINT_INVEN:
			{
				const int64_t Envantertoplami = static_cast<int64_t>(Inven_Point()) + static_cast<int64_t>(amount);
				if (Envantertoplami > INVENTORY_LOCK_COVER_COUNT)
				{
					sys_err("[POINT_INVEN ERROR]");
					return;
				}
				Set_Inventory_Point(Inven_Point() + amount);
				val = Inven_Point();
			}
			break;
#endif

#ifdef ENABLE_EXTENDED_BATTLE_PASS
		case POINT_BATTLE_PASS_PREMIUM_ID:
			{
				SetExtBattlePassPremiumID(amount);
				val = GetExtBattlePassPremiumID();
			}
			break;
#endif

		case POINT_GOLD:
			{
				const long long nTotalMoney = static_cast<long long>(GetGold()) + static_cast<long long>(amount);

				if (GOLD_MAX <= nTotalMoney)
				{
					sys_err("[OVERFLOW_GOLD] OriGold %lld AddedGold %lld id %u Name %s ", GetGold(), amount, GetPlayerID(), GetName());
					LogManager::instance().CharLog(this, GetGold() + amount, "OVERFLOW_GOLD", "");
					return;
				}

				if (g_bChinaIntoxicationCheck && amount > 0)
				{
					if (IsOverTime(OT_NONE))
					{
						dev_log(LOG_DEB0, "<GOLD_LOG> %s = NONE", GetName());
					}
					else if (IsOverTime(OT_3HOUR))
					{
						amount = (amount / 2);
						dev_log(LOG_DEB0, "<GOLD_LOG> %s = 3HOUR", GetName());
					}
					else if (IsOverTime(OT_5HOUR))
					{
						amount = 0;
						dev_log(LOG_DEB0, "<GOLD_LOG> %s = 5HOUR", GetName());
					}
				}

				SetGold(GetGold() + amount);
				val = GetGold();
#ifdef __RANKING_SYSTEM__
				SetRankManuel(RANK_GOLD, 0);
#endif
			}
			break;

		case POINT_SKILL:
		case POINT_STAT:
		case POINT_SUB_SKILL:
		case POINT_STAT_RESET_COUNT:
		case POINT_HORSE_SKILL:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			SetRealPoint(type, val);
			break;

		case POINT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_CLIENT_DEF_GRADE, amount);
			break;

		case POINT_CLIENT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_ST:
		case POINT_HT:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HP_REGEN:
		case POINT_SP_REGEN:
		case POINT_ATT_SPEED:
		case POINT_ATT_GRADE:
		case POINT_MOV_SPEED:
		case POINT_CASTING_SPEED:
		case POINT_MAGIC_ATT_GRADE:
		case POINT_MAGIC_DEF_GRADE:
		case POINT_BOW_DISTANCE:
		case POINT_HP_RECOVERY:
		case POINT_SP_RECOVERY:

		case POINT_ATTBONUS_HUMAN:	// 42
		case POINT_ATTBONUS_ANIMAL:	// 43
		case POINT_ATTBONUS_ORC:	// 44
		case POINT_ATTBONUS_MILGYO:	// 45
		case POINT_ATTBONUS_UNDEAD:	// 46
		case POINT_ATTBONUS_DEVIL:	// 47
		case POINT_ATTBONUS_MONSTER:
		case POINT_ATTBONUS_SURA:
		case POINT_ATTBONUS_ASSASSIN:
		case POINT_ATTBONUS_WARRIOR:
		case POINT_ATTBONUS_SHAMAN:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_ATTBONUS_WOLFMAN:
#endif

		case POINT_POISON_PCT:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_PCT:
#endif
		case POINT_STUN_PCT:
		case POINT_SLOW_PCT:

		case POINT_BLOCK:
		case POINT_DODGE:

		case POINT_CRITICAL_PCT:
		case POINT_RESIST_CRITICAL:
		case POINT_PENETRATE_PCT:
		case POINT_RESIST_PENETRATE:
		case POINT_CURSE_PCT:

		case POINT_STEAL_HP:		// 48
		case POINT_STEAL_SP:		// 49

		case POINT_MANA_BURN_PCT:	// 50
		case POINT_DAMAGE_SP_RECOVER:	// 51
		case POINT_RESIST_NORMAL_DAMAGE:
		case POINT_RESIST_SWORD:
		case POINT_RESIST_TWOHAND:
		case POINT_RESIST_DAGGER:
		case POINT_RESIST_BELL:
		case POINT_RESIST_FAN:
		case POINT_RESIST_BOW:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_CLAW:
#endif
		case POINT_RESIST_FIRE:
		case POINT_RESIST_ELEC:
		case POINT_RESIST_MAGIC:
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case POINT_ACCEDRAIN_RATE:
#endif
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
		case POINT_RESIST_MAGIC_REDUCTION:
#endif
		case POINT_RESIST_WIND:
		case POINT_RESIST_ICE:
		case POINT_RESIST_EARTH:
		case POINT_RESIST_DARK:
		case POINT_REFLECT_MELEE:	// 67
		case POINT_REFLECT_CURSE:	// 68
		case POINT_POISON_REDUCE:	// 69
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_REDUCE:
#endif
		case POINT_KILL_SP_RECOVER:	// 70
		case POINT_KILL_HP_RECOVERY:	// 75
		case POINT_HIT_HP_RECOVERY:
		case POINT_HIT_SP_RECOVERY:
		case POINT_MANASHIELD:
		case POINT_ATT_BONUS:
		case POINT_DEF_BONUS:
		case POINT_SKILL_DAMAGE_BONUS:
		case POINT_NORMAL_HIT_DAMAGE_BONUS:

			// DEPEND_BONUS_ATTRIBUTES
		case POINT_SKILL_DEFEND_BONUS:
		case POINT_NORMAL_HIT_DEFEND_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
			// END_OF_DEPEND_BONUS_ATTRIBUTES

		case POINT_PARTY_ATTACKER_BONUS:
		case POINT_PARTY_TANKER_BONUS:
		case POINT_PARTY_BUFFER_BONUS:
		case POINT_PARTY_SKILL_MASTER_BONUS:
		case POINT_PARTY_HASTE_BONUS:
		case POINT_PARTY_DEFENDER_BONUS:

		case POINT_RESIST_WARRIOR :
		case POINT_RESIST_ASSASSIN :
		case POINT_RESIST_SURA :
		case POINT_RESIST_SHAMAN :
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_WOLFMAN :
#endif

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
		case POINT_MALL_EXPBONUS:
		case POINT_MALL_ITEMBONUS:
		case POINT_MALL_GOLDBONUS:
		case POINT_MELEE_MAGIC_ATT_BONUS_PER:
			if (GetPoint(type) + amount > 350)
			{
				sys_err("MALL_BONUS exceeded over 350!! point type: %d name: %s amount %d", type, GetName(), amount);
				amount = 350 - GetPoint(type);
			}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

			// PC_BANG_ITEM_ADD
		case POINT_PC_BANG_EXP_BONUS :
		case POINT_PC_BANG_DROP_BONUS :
		case POINT_RAMADAN_CANDY_BONUS_EXP:
			SetPoint(type, amount);
			val = GetPoint(type);
			break;
			// END_PC_BANG_ITEM_ADD

		case POINT_EXP_DOUBLE_BONUS:	// 71
		case POINT_GOLD_DOUBLE_BONUS:	// 72
		case POINT_ITEM_DROP_BONUS:	// 73
		case POINT_POTION_BONUS:	// 74
			if (GetPoint(type) + amount > 350)
			{
				sys_err("BONUS exceeded over 350!! point type: %d name: %s amount %d", type, GetName(), amount);
				amount = 350 - GetPoint(type);
			}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_IMMUNE_STUN:		// 76
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				// ChatPacket(CHAT_TYPE_INFO, "IMMUNE_STUN SET_BIT type(%u) amount(%d)", type, amount);
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			}
			else
			{
				// ChatPacket(CHAT_TYPE_INFO, "IMMUNE_STUN REMOVE_BIT type(%u) amount(%d)", type, amount);
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			}
			break;

		case POINT_IMMUNE_SLOW:		// 77
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			}
			break;

		case POINT_IMMUNE_FALL:	// 78
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			}
			break;

		case POINT_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_VOICE:
		case POINT_EMPIRE_POINT:
			//sys_err("CHARACTER::PointChange: %s: point cannot be changed. use SetPoint instead (type: %d)", GetName(), type);
			val = GetRealPoint(type);
			break;

		case POINT_POLYMORPH:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			SetPolymorph(val);
			break;

		case POINT_MOUNT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			MountVnum(val);
			break;

		case POINT_ENERGY:
		case POINT_COSTUME_ATTR_BONUS:
			{
				int old_val = GetPoint(type);
				SetPoint(type, old_val + amount);
				val = GetPoint(type);
				BuffOnAttr_ValueChange(type, old_val, val);
			}
			break;

		default:
			sys_err("CHARACTER::PointChange: %s: unknown point change type %d", GetName(), type);
			return;
	}

	switch (type)
	{
		case POINT_LEVEL:
		case POINT_ST:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HT:
			ComputeBattlePoints();
			break;
		case POINT_MAX_HP:
		case POINT_MAX_SP:
		case POINT_MAX_STAMINA:
			break;
	}

	if (type == POINT_HP && amount == 0)
		return;

	if (GetDesc())
	{
		struct packet_point_change pack;

		pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
		pack.dwVID = m_vid;
		pack.type = type;
		pack.value = val;

		if (bAmount)
			pack.amount = amount;
		else
			pack.amount = 0;

		if (!bBroadcast)
			GetDesc()->Packet(&pack, sizeof(struct packet_point_change));
		else
			PacketAround(&pack, sizeof(pack));
	}
}

void CHARACTER::ApplyPoint(BYTE bApplyType, int iVal)
{
	switch (bApplyType)
	{
		case APPLY_NONE:			// 0
			break;

		case APPLY_CON:
			PointChange(POINT_HT, iVal);
			PointChange(POINT_MAX_HP, (iVal * JobInitialPoints[GetJob()].hp_per_ht));
			PointChange(POINT_MAX_STAMINA, (iVal * JobInitialPoints[GetJob()].stamina_per_con));
			break;

		case APPLY_INT:
			PointChange(POINT_IQ, iVal);
			PointChange(POINT_MAX_SP, (iVal * JobInitialPoints[GetJob()].sp_per_iq));
			break;

		case APPLY_SKILL:
			// SKILL_DAMAGE_BONUS
			{
				// 00000000 00000000 00000000 00000000

				// vnum	 ^ add	   change
				BYTE bSkillVnum = (BYTE) (((DWORD)iVal) >> 24);
				int iAdd = iVal & 0x00800000;
				int iChange = iVal & 0x007fffff;

				sys_log(1, "APPLY_SKILL skill %d add? %d change %d", bSkillVnum, iAdd ? 1 : 0, iChange);

				if (0 == iAdd)
					iChange = -iChange;

				boost::unordered_map<BYTE, int>::iterator iter = m_SkillDamageBonus.find(bSkillVnum);

				if (iter == m_SkillDamageBonus.end())
					m_SkillDamageBonus.insert(std::make_pair(bSkillVnum, iChange));
				else
					iter->second += iChange;
			}
			// END_OF_SKILL_DAMAGE_BONUS
			break;

		case APPLY_MAX_HP:
		case APPLY_MAX_HP_PCT:
			{
				int i = GetMaxHP(); if(i == 0) break;
				PointChange(aApplyInfo[bApplyType].bPointType, iVal);
				float fRatio = (float)GetMaxHP() / (float)i;
				PointChange(POINT_HP, GetHP() * fRatio - GetHP());
			}
			break;

		case APPLY_MAX_SP:
		case APPLY_MAX_SP_PCT:
			{
				int i = GetMaxSP(); if(i == 0) break;
				PointChange(aApplyInfo[bApplyType].bPointType, iVal);
				float fRatio = (float)GetMaxSP() / (float)i;
				PointChange(POINT_SP, GetSP() * fRatio - GetSP());
			}
			break;

		case APPLY_STR:
		case APPLY_DEX:
		case APPLY_ATT_SPEED:
		case APPLY_MOV_SPEED:
		case APPLY_CAST_SPEED:
		case APPLY_HP_REGEN:
		case APPLY_SP_REGEN:
		case APPLY_POISON_PCT:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case APPLY_BLEEDING_PCT:
#endif
		case APPLY_STUN_PCT:
		case APPLY_SLOW_PCT:
		case APPLY_CRITICAL_PCT:
		case APPLY_PENETRATE_PCT:
		case APPLY_ATTBONUS_HUMAN:
		case APPLY_ATTBONUS_ANIMAL:
		case APPLY_ATTBONUS_ORC:
		case APPLY_ATTBONUS_MILGYO:
		case APPLY_ATTBONUS_UNDEAD:
		case APPLY_ATTBONUS_DEVIL:
		case APPLY_ATTBONUS_WARRIOR:	// 59
		case APPLY_ATTBONUS_ASSASSIN:	// 60
		case APPLY_ATTBONUS_SURA:	// 61
		case APPLY_ATTBONUS_SHAMAN:	// 62
#ifdef ENABLE_WOLFMAN_CHARACTER
		case APPLY_ATTBONUS_WOLFMAN:
#endif
		case APPLY_ATTBONUS_MONSTER:	// 63
		case APPLY_STEAL_HP:
		case APPLY_STEAL_SP:
		case APPLY_MANA_BURN_PCT:
		case APPLY_DAMAGE_SP_RECOVER:
		case APPLY_BLOCK:
		case APPLY_DODGE:
		case APPLY_RESIST_SWORD:
		case APPLY_RESIST_TWOHAND:
		case APPLY_RESIST_DAGGER:
		case APPLY_RESIST_BELL:
		case APPLY_RESIST_FAN:
		case APPLY_RESIST_BOW:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case APPLY_RESIST_CLAW:
#endif
		case APPLY_RESIST_FIRE:
		case APPLY_RESIST_ELEC:
		case APPLY_RESIST_MAGIC:
		case APPLY_RESIST_WIND:
		case APPLY_RESIST_ICE:
		case APPLY_RESIST_EARTH:
		case APPLY_RESIST_DARK:
		case APPLY_REFLECT_MELEE:
		case APPLY_REFLECT_CURSE:
		case APPLY_ANTI_CRITICAL_PCT:
		case APPLY_ANTI_PENETRATE_PCT:
		case APPLY_POISON_REDUCE:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case APPLY_BLEEDING_REDUCE:
#endif
		case APPLY_KILL_SP_RECOVER:
		case APPLY_EXP_DOUBLE_BONUS:
		case APPLY_GOLD_DOUBLE_BONUS:
		case APPLY_ITEM_DROP_BONUS:
		case APPLY_POTION_BONUS:
		case APPLY_KILL_HP_RECOVER:
		case APPLY_IMMUNE_STUN:
		case APPLY_IMMUNE_SLOW:
		case APPLY_IMMUNE_FALL:
		case APPLY_BOW_DISTANCE:
		case APPLY_ATT_GRADE_BONUS:
		case APPLY_DEF_GRADE_BONUS:
		case APPLY_MAGIC_ATT_GRADE:
		case APPLY_MAGIC_DEF_GRADE:
		case APPLY_CURSE_PCT:
		case APPLY_MAX_STAMINA:
		case APPLY_MALL_ATTBONUS:
		case APPLY_MALL_DEFBONUS:
		case APPLY_MALL_EXPBONUS:
		case APPLY_MALL_ITEMBONUS:
		case APPLY_MALL_GOLDBONUS:
		case APPLY_SKILL_DAMAGE_BONUS:
		case APPLY_NORMAL_HIT_DAMAGE_BONUS:

			// DEPEND_BONUS_ATTRIBUTES
		case APPLY_SKILL_DEFEND_BONUS:
		case APPLY_NORMAL_HIT_DEFEND_BONUS:
			// END_OF_DEPEND_BONUS_ATTRIBUTES

		case APPLY_PC_BANG_EXP_BONUS :
		case APPLY_PC_BANG_DROP_BONUS :

		case APPLY_RESIST_WARRIOR :
		case APPLY_RESIST_ASSASSIN :
		case APPLY_RESIST_SURA :
		case APPLY_RESIST_SHAMAN :
#ifdef ENABLE_WOLFMAN_CHARACTER
		case APPLY_RESIST_WOLFMAN :
#endif
		case APPLY_ENERGY:					// 82
		case APPLY_DEF_GRADE:				// 83
		case APPLY_COSTUME_ATTR_BONUS:		// 84
		case APPLY_MAGIC_ATTBONUS_PER:		// 85
		case APPLY_MELEE_MAGIC_ATTBONUS_PER:			// 86
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case APPLY_ACCEDRAIN_RATE:			//97
#endif
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
		case APPLY_RESIST_MAGIC_REDUCTION:	//98
#endif
		case APPLY_MOUNT:	//99
			PointChange(aApplyInfo[bApplyType].bPointType, iVal);
			break;

		default:
			sys_err("Unknown apply type %d name %s", bApplyType, GetName());
			break;
	}
}

void CHARACTER::MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet)
{
	packet->header	= HEADER_GC_MOTION;
	packet->vid		= m_vid;
	packet->motion	= motion;

	if (victim)
		packet->victim_vid = victim->GetVID();
	else
		packet->victim_vid = 0;
}

void CHARACTER::Motion(BYTE motion, LPCHARACTER victim)
{
	struct packet_motion pack_motion;
	MotionPacketEncode(motion, victim, &pack_motion);
	PacketAround(&pack_motion, sizeof(struct packet_motion));
}

EVENTFUNC(save_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "save_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	sys_log(1, "SAVE_EVENT: %s", ch->GetName());
	ch->Save();
	ch->FlushDelayedSaveItem();
	return (save_event_second_cycle);
}

void CHARACTER::StartSaveEvent()
{
	if (m_pkSaveEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;
	m_pkSaveEvent = event_create(save_event, info, save_event_second_cycle);
}

void CHARACTER::MonsterLog(const char* format, ...)
{
	if (!test_server)
		return;

	if (IsPC())
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%u)", (DWORD)GetVID());

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	va_list args;

	va_start(args, format);

	int len2 = vsnprintf(chatbuf + len, sizeof(chatbuf) - len, format, args);

	if (len2 < 0 || len2 >= (int) sizeof(chatbuf) - len)
		len += (sizeof(chatbuf) - len) - 1;
	else
		len += len2;

	++len;

	va_end(args);

	TPacketGCChat pack_chat;

	pack_chat.header	= HEADER_GC_CHAT;
	pack_chat.size		= sizeof(TPacketGCChat) + len;
	pack_chat.type	  = CHAT_TYPE_TALKING;
	pack_chat.id		= (DWORD)GetVID();
	pack_chat.bEmpire	= 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(TPacketGCChat));
	buf.write(chatbuf, len);

	CHARACTER_MANAGER::instance().PacketMonsterLog(this, buf.read_peek(), buf.size());
}

void CHARACTER::ChatPacket(BYTE type, const char * format, ...)
{
	LPDESC d = GetDesc();

	if (!d || !format)
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;

	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	struct packet_chat pack_chat;

	pack_chat.header	= HEADER_GC_CHAT;
	pack_chat.size	  = sizeof(struct packet_chat) + len;
	pack_chat.type	  = type;
	pack_chat.id		= 0;
	pack_chat.bEmpire   = d->GetEmpire();

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());

	if (type == CHAT_TYPE_COMMAND && test_server)
		sys_log(0, "SEND_COMMAND %s %s", GetName(), chatbuf);
}

// MINING
void CHARACTER::mining_take()
{
	m_pkMiningEvent = NULL;
}

void CHARACTER::mining_cancel()
{
	if (m_pkMiningEvent)
	{
		sys_log(0, "XXX MINING CANCEL");
		event_cancel(&m_pkMiningEvent);
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Madencilik iptal edildi."));
	}
}

void CHARACTER::mining(LPCHARACTER chLoad)
{
	if (m_pkMiningEvent)
	{
		mining_cancel();
		return;
	}

	if (!chLoad)
		return;

	// @fixme128
	if (GetMapIndex() != chLoad->GetMapIndex() || DISTANCE_APPROX(GetX() - chLoad->GetX(), GetY() - chLoad->GetY()) > 1000)
		return;

	if (mining::GetRawOreFromLoad(chLoad->GetRaceNum()) == 0)
		return;

	LPITEM pick = GetWear(WEAR_WEAPON);

	if (!pick || pick->GetType() != ITEM_PICK)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°î±ªÀÌ¸¦ ÀåÂøÇÏ¼¼¿ä."));
		return;
	}

	int count = number(5, 15);

	TPacketGCDigMotion p;
	p.header = HEADER_GC_DIG_MOTION;
	p.vid = GetVID();
	p.target_vid = chLoad->GetVID();
	p.count = count;

	PacketAround(&p, sizeof(p));

	m_pkMiningEvent = mining::CreateMiningEvent(this, chLoad, count);
}
// END_OF_MINING

void CHARACTER::fishing()
{
	if (m_pkFishingEvent)
	{
		fishing_take();
		return;
	}

	{
		LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

		int	x = GetX();
		int y = GetY();

		LPSECTREE tree = pkSectreeMap->Find(x, y);
		DWORD dwAttr = tree->GetAttribute(x, y);

//		if (IS_SET(dwAttr, ATTR_BLOCK))
//		{
//			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot fish here."));
//			return;
//		}
	}

// fishing map block begin

	bool isBlock = false;
	const WORD	mapIDX[] = {1, 41, 61};
	for (const auto& i : mapIDX) {
		if (GetMapIndex() == i)
		{
			isBlock = true;
			break;
		}
	}

	if (isBlock) {
		ChatPacket(1, "BALIK HARITASINDAN BALIK TUTABILIRSINIZ. LUTFEN BALIKCI ILE GORUSUN!");
		return;
	}

	LPITEM rod = GetWear(WEAR_WEAPON);

	if (!rod || rod->GetType() != ITEM_ROD)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please choose a Rod."));
		return;
	}

	if (0 == rod->GetSocket(2))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("First place the Bait on the Hook."));
		return;
	}

	float fx, fy;
	GetDeltaByDegree(GetRotation(), 400.0f, &fx, &fy);

	m_pkFishingEvent = fishing::CreateFishingEvent(this);
}

void CHARACTER::fishing_take()
{
	LPITEM rod = GetWear(WEAR_WEAPON);
	if (rod && rod->GetType() == ITEM_ROD)
	{
		using fishing::fishing_event_info;
		if (m_pkFishingEvent)
		{
			struct fishing_event_info* info = dynamic_cast<struct fishing_event_info*>(m_pkFishingEvent->info);

			if (info)
				fishing::Take(info, this);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't go fishing without a Rod!"));
	}

	event_cancel(&m_pkFishingEvent);
}

bool CHARACTER::StartStateMachine(int iNextPulse)
{
	if (CHARACTER_MANAGER::instance().AddToStateList(this))
	{
		m_dwNextStatePulse = thecore_heart->pulse + iNextPulse;
		return true;
	}

	return false;
}

void CHARACTER::StopStateMachine()
{
	CHARACTER_MANAGER::instance().RemoveFromStateList(this);
}

void CHARACTER::UpdateStateMachine(DWORD dwPulse)
{
	if (dwPulse < m_dwNextStatePulse)
		return;

	if (IsDead())
		return;

	Update();
	m_dwNextStatePulse = dwPulse + m_dwStateDuration;
}

void CHARACTER::SetNextStatePulse(int iNextPulse)
{
	CHARACTER_MANAGER::instance().AddToStateList(this);
	m_dwNextStatePulse = iNextPulse;

	if (iNextPulse < 10)
		MonsterLog("�������·ξ����");
}


void CHARACTER::UpdateCharacter(DWORD dwPulse)
{
	CFSM::Update();
}

void CHARACTER::SetShop(LPSHOP pkShop)
{
	if ((m_pkShop = pkShop))
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
	else
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
		SetShopOwner(NULL);
	}
}

void CHARACTER::SetExchange(CExchange * pkExchange)
{
	m_pkExchange = pkExchange;
}

void CHARACTER::SetPart(BYTE bPartPos, DWORD wVal)
{
	assert(bPartPos < PART_MAX_NUM);
#ifdef ENABLE_CAOS_EVENT
	if (GetMapIndex() == CAOS_EVENT_MAP) { return; }
#endif
	m_pointsInstant.parts[bPartPos] = wVal;
}

DWORD CHARACTER::GetPart(BYTE bPartPos) const
{
	assert(bPartPos < PART_MAX_NUM);
#ifdef __HIDE_COSTUME_SYSTEM__
	if (bPartPos == PART_MAIN && GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden() == true)
	{
		if (const LPITEM pArmor = GetWear(WEAR_BODY))
#ifdef __CHANGE_LOOK_SYSTEM__
			return pArmor->GetTransmutation() != 0 ? pArmor->GetTransmutation() : pArmor->GetVnum();
#else
			return pArmor->GetVnum();
#endif
		else
			return 0;
	}
	else if (bPartPos == PART_HAIR && GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden() == true)
		return 0;
#endif
#ifdef ENABLE_CAOS_EVENT
	if (GetMapIndex() == CAOS_EVENT_MAP) {
		const auto& [weapon, body, hair] = CNewCaosEventManager::GetCostumes(GetRaceNum());
		if (bPartPos == PART_WEAPON) {
			return weapon;
		}
		else if (bPartPos == PART_MAIN) {
			return body;
		}
		else if (bPartPos == PART_HAIR) {
			return hair;
		}
	}
#endif
	return m_pointsInstant.parts[bPartPos];
}

DWORD CHARACTER::GetOriginalPart(BYTE bPartPos) const
{
	switch (bPartPos)
	{
	case PART_MAIN:
#ifdef __HIDE_COSTUME_SYSTEM__
		if (GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden() == true)
			if (const LPITEM pArmor = GetWear(WEAR_BODY))
				return pArmor->GetVnum();
#endif
		if (!IsPC()) 
			return GetPart(PART_MAIN);
		else
			return m_pointsInstant.bBasePart;

	case PART_HAIR:
#ifdef __HIDE_COSTUME_SYSTEM__
		if (GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden() == true)
			return 0;
#endif
		return GetPart(PART_HAIR);

#ifdef __ACCE_COSTUME_SYSTEM__
	case PART_ACCE:
#ifdef __HIDE_COSTUME_SYSTEM__
		if (GetWear(WEAR_COSTUME_ACCE) && IsAcceCostumeHidden() == true)
			return 0;
#endif
		return GetPart(PART_ACCE);
#endif

#ifdef __WEAPON_COSTUME_SYSTEM__
	case PART_WEAPON:
#ifdef __HIDE_COSTUME_SYSTEM__
		if (GetWear(WEAR_COSTUME_WEAPON) && IsWeaponCostumeHidden() == true)
			if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
				return pWeapon->GetVnum();
#endif
		return GetPart(PART_WEAPON);
#endif

	default:
		return 0;
	}
}

BYTE CHARACTER::GetCharType() const
{
	return m_bCharType;
}

bool CHARACTER::SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList)
{
	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
		return false;
	// END_OF_TRENT_MONSTER

	if (ch) // @fixme131
	{
		if (!battle_is_attackable(ch, this))
		{
			SendDamagePacket(ch, 0, DAMAGE_BLOCK);
			return false;
		}
	}

	if (ch == this)
	{
		sys_err("SetSyncOwner owner == this (%p)", this);
		return false;
	}

	if (!ch)
	{
		if (bRemoveFromList && m_pkChrSyncOwner)
		{
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
		}

		if (m_pkChrSyncOwner)
			sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());

		m_pkChrSyncOwner = NULL;
	}
	else
	{
		if (!IsSyncOwner(ch))
			return false;

		if (DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY()) > 250)
		{
			sys_log(1, "SetSyncOwner distance over than 250 %s %s", GetName(), ch->GetName());

			if (m_pkChrSyncOwner == ch)
				return true;

			return false;
		}

		if (m_pkChrSyncOwner != ch)
		{
			if (m_pkChrSyncOwner)
			{
				sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());
				m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
			}

			m_pkChrSyncOwner = ch;
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.push_back(this);

			static const timeval zero_tv = {0, 0};
			SetLastSyncTime(zero_tv);

			sys_log(1, "SetSyncOwner set %s %p to %s", GetName(), this, ch->GetName());
		}

		m_fSyncTime = get_float_time();
	}

	TPacketGCOwnership pack;

	pack.bHeader	= HEADER_GC_OWNERSHIP;
	pack.dwOwnerVID	= ch ? ch->GetVID() : 0;
	pack.dwVictimVID	= GetVID();

	PacketAround(&pack, sizeof(TPacketGCOwnership));
	return true;
}

struct FuncClearSync
{
	void operator () (LPCHARACTER ch)
	{
		assert(ch != NULL);
		ch->SetSyncOwner(NULL, false);
	}
};

void CHARACTER::ClearSync()
{
	SetSyncOwner(NULL);

	std::for_each(m_kLst_pkChrSyncOwned.begin(), m_kLst_pkChrSyncOwned.end(), FuncClearSync());
	m_kLst_pkChrSyncOwned.clear();
}

bool CHARACTER::IsSyncOwner(LPCHARACTER ch) const
{
	if (m_pkChrSyncOwner == ch)
		return true;

	if (get_float_time() - m_fSyncTime >= 3.0f)
		return true;

	return false;
}

void CHARACTER::SetParty(LPPARTY pkParty)
{
	if (pkParty == m_pkParty)
		return;

	if (pkParty && m_pkParty)
		sys_err("%s is trying to reassigning party (current %p, new party %p)", GetName(), get_pointer(m_pkParty), get_pointer(pkParty));

	sys_log(1, "PARTY set to %p", get_pointer(pkParty));

	//if (m_pkDungeon && IsPC())
	//SetDungeon(NULL);
	m_pkParty = pkParty;

	if (IsPC())
	{
		if (m_pkParty)
			SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);
		else
			REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);

		UpdatePacket();
	}
}

// PARTY_JOIN_BUG_FIX
EVENTINFO(TPartyJoinEventInfo)
{
	DWORD	dwGuestPID;
	DWORD	dwLeaderPID;

	TPartyJoinEventInfo()
	: dwGuestPID( 0 )
	, dwLeaderPID( 0 )
	{
	}
} ;

EVENTFUNC(party_request_event)
{
	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(  event->info );

	if ( info == NULL )
	{
		sys_err( "party_request_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->dwGuestPID);

	if (ch)
	{
		sys_log(0, "PartyRequestEvent %s", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
		ch->SetPartyRequestEvent(NULL);
	}

	return 0;
}

bool CHARACTER::RequestToParty(LPCHARACTER leader)
{
	if (leader->GetParty())
		leader = leader->GetParty()->GetLeaderCharacter();

	if (!leader)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot make a request because the Group leader is not online."));
		return false;
	}

	if (m_pkPartyRequestEvent)
		return false;

	if (!IsPC() || !leader->IsPC())
		return false;

	if (leader->IsBlockMode(BLOCK_PARTY_REQUEST))
		return false;

	PartyJoinErrCode errcode = IsPartyJoinableCondition(leader, this);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> The server cannot execute the Group request."));
			return false;

		case PERR_DIFFEMPIRE:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot form a Group with players from another Kingdom."));
			return false;

		case PERR_DUNGEON:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite players while you are in a dungeon."));
			return false;

		case PERR_OBSERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite players while you are in Observer Mode."));
			return false;

		case PERR_LVBOUNDARY:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> Only players with a Level difference of -30 to +30 can be invited."));
			return false;

		case PERR_LOWLEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite this player, his Level is too low."));
			return false;

		case PERR_HILEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite this player, his Level is too high."));
			return false;

		case PERR_ALREADYJOIN:
			return false;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite more players into your Group."));
			return false;

		default:
			sys_err("Do not process party join error(%d)", errcode);
			return false;
	}

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = GetPlayerID();
	info->dwLeaderPID = leader->GetPlayerID();

	SetPartyRequestEvent(event_create(party_request_event, info, PASSES_PER_SEC(10)));

	leader->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequest %u", (DWORD) GetVID());
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Do you want to join the group of %s?"), leader->GetName());
	return true;
}

void CHARACTER::DenyToParty(LPCHARACTER member)
{
	sys_log(1, "DenyToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::DenyToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

void CHARACTER::AcceptToParty(LPCHARACTER member)
{
	sys_log(1, "AcceptToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::AcceptToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	if (!GetParty())
		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The player is not in this Group."));
	else
	{
		if (GetPlayerID() != GetParty()->GetLeaderPID())
			return;

		PartyJoinErrCode errcode = IsPartyJoinableCondition(this, member);
		switch (errcode)
		{
			case PERR_NONE: 		member->PartyJoin(this); return;
			case PERR_SERVER:		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> The server cannot execute the Group request.")); break;
			case PERR_DUNGEON:		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite players while you are in a dungeon.")); break;
			case PERR_OBSERVER: 	member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite players while you are in Observer Mode.")); break;
			case PERR_LVBOUNDARY:	member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> Only players with a Level difference of -30 to +30 can be invited.")); break;
			case PERR_LOWLEVEL: 	member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite this player, his Level is too low.")); break;
			case PERR_HILEVEL: 		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite this player, his Level is too high.")); break;
			case PERR_ALREADYJOIN: 	break;
			case PERR_PARTYISFULL: {
									   ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite more players into your Group."));
									   member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot join this Group as too many members are already there."));
									   break;
								   }
			default: sys_err("Do not process party join error(%d)", errcode);
		}
	}

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

EVENTFUNC(party_invite_event)
{
	TPartyJoinEventInfo * pInfo = dynamic_cast<TPartyJoinEventInfo *>(  event->info );

	if ( pInfo == NULL )
	{
		sys_err( "party_invite_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER pchInviter = CHARACTER_MANAGER::instance().FindByPID(pInfo->dwLeaderPID);

	if (pchInviter)
	{
		sys_log(1, "PartyInviteEvent %s", pchInviter->GetName());
		pchInviter->PartyInviteDeny(pInfo->dwGuestPID);
	}

	return 0;
}

void CHARACTER::PartyInvite(LPCHARACTER pchInvitee)
{
	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You do not have the right to invite someone."));
		return;
	}
	else if (pchInvitee->IsBlockMode(BLOCK_PARTY_INVITE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> %s declines the invitation of your Group."), pchInvitee->GetName());
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> The server cannot execute the Group request."));
			return;

		case PERR_DIFFEMPIRE:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot form a Group with players from another Kingdom."));
			return;

		case PERR_DUNGEON:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite players while you are in a dungeon."));
			return;

		case PERR_OBSERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite players while you are in Observer Mode."));
			return;

		case PERR_LVBOUNDARY:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> Only players with a Level difference of -30 to +30 can be invited."));
			return;

		case PERR_LOWLEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite this player, his Level is too low."));
			return;

		case PERR_HILEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite this player, his Level is too high."));
			return;

		case PERR_ALREADYJOIN:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> %s is already in the Group."), pchInvitee->GetName());
			return;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite more players into your Group."));
			return;

		default:
			sys_err("Do not process party join error(%d)", errcode);
			return;
	}

	if (m_PartyInviteEventMap.end() != m_PartyInviteEventMap.find(pchInvitee->GetPlayerID()))
		return;

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = pchInvitee->GetPlayerID();
	info->dwLeaderPID = GetPlayerID();

	m_PartyInviteEventMap.insert(EventMap::value_type(pchInvitee->GetPlayerID(), event_create(party_invite_event, info, PASSES_PER_SEC(10))));


	TPacketGCPartyInvite p;
	p.header = HEADER_GC_PARTY_INVITE;
	p.leader_vid = GetVID();
	pchInvitee->GetDesc()->Packet(&p, sizeof(p));
}

void CHARACTER::PartyInviteAccept(LPCHARACTER pchInvitee)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(pchInvitee->GetPlayerID());

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteAccept from not invited character(%s)", pchInvitee->GetName());
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You do not have the right to invite someone."));
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableMutableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> The server cannot execute the Group request."));
			return;

		case PERR_DUNGEON:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot accept an invitation in a dungeon."));
			return;

		case PERR_OBSERVER:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite players while you are in Observer Mode."));
			return;

		case PERR_LVBOUNDARY:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> Only players with a Level difference of -30 to +30 can be invited."));
			return;

		case PERR_LOWLEVEL:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite this player, his Level is too low."));
			return;

		case PERR_HILEVEL:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite this player, his Level is too high."));
			return;

		case PERR_ALREADYJOIN:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot accept the invitation."));
			return;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot invite more players into your Group."));
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You cannot join this Group as too many members are already there."));
			return;

		default:
			sys_err("ignore party join error(%d)", errcode);
			return;
	}


	if (GetParty())
		pchInvitee->PartyJoin(this);
	else
	{
		LPPARTY pParty = CPartyManager::instance().CreateParty(this);

		pParty->Join(pchInvitee->GetPlayerID());
		pParty->Link(pchInvitee);
		pParty->SendPartyInfoAllToOne(this);
	}
}

void CHARACTER::PartyInviteDeny(DWORD dwPID)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(dwPID);

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteDeny to not exist event(inviter PID: %d, invitee PID: %d)", GetPlayerID(), dwPID);
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	LPCHARACTER pchInvitee = CHARACTER_MANAGER::instance().FindByPID(dwPID);
	if (pchInvitee)
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> %s declined your invitation."), pchInvitee->GetName());
}

void CHARACTER::PartyJoin(LPCHARACTER pLeader)
{
	pLeader->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> %s joined your Group."), GetName());
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> %s joined your Group."), pLeader->GetName());

	pLeader->GetParty()->Join(GetPlayerID());
	pLeader->GetParty()->Link(this);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (pchLeader->GetEmpire() != pchGuest->GetEmpire())
		return PERR_DIFFEMPIRE;

	return IsPartyJoinableMutableCondition(pchLeader, pchGuest);
}

static bool __party_can_join_by_level(LPCHARACTER leader, LPCHARACTER quest)
{
	int	level_limit = 30;
	return (abs(leader->GetLevel() - quest->GetLevel()) <= level_limit);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (!CPartyManager::instance().IsEnablePCParty())
		return PERR_SERVER;
	else if (pchLeader->GetDungeon())
		return PERR_DUNGEON;
	else if (pchGuest->IsObserverMode())
		return PERR_OBSERVER;
	else if (false == __party_can_join_by_level(pchLeader, pchGuest))
		return PERR_LVBOUNDARY;
	else if (pchGuest->GetParty())
		return PERR_ALREADYJOIN;
	else if (pchLeader->GetParty())
   	{
	   	if (pchLeader->GetParty()->GetMemberCount() == PARTY_MAX_MEMBER)
			return PERR_PARTYISFULL;
	}

	return PERR_NONE;
}
// END_OF_PARTY_JOIN_BUG_FIX

#ifdef NEW_SALES_SYSTEM

int CHARACTER::GetCoins() const
{
	if (!GetDesc())
		return 0;

	char epquery[1024+1];
	int my_ep = 0;
	snprintf(epquery, sizeof(epquery), 
			"SELECT cash FROM account.account WHERE id=%u", GetDesc()->GetAccountTable().id);

	std::unique_ptr<SQLMsg> epcek(DBManager::instance().DirectQuery(epquery));
	if (epcek->Get()->uiNumRows != 0)
	{
		MYSQL_ROW veri = mysql_fetch_row(epcek->Get()->pSQLResult);
		str_to_number(my_ep, veri[0]);
	}

	return my_ep;
}

bool CHARACTER::SetCoins(int ep, BYTE islem)
{ //0 siler 1 ekler

	if (!GetDesc())
		return false;

	int my_ep = 0;
	my_ep = GetCoins();
	if (islem == 0)
	{
		if (my_ep >= ep)
		{
			std::unique_ptr<SQLMsg> epis(DBManager::instance().DirectQuery("UPDATE account.account SET cash = cash - %d WHERE id = %u", ep, GetDesc()->GetAccountTable().id));
			if (epis->Get()->uiAffectedRows > 0)
			{
				return true;
			}
		}
	}
	else if (islem == 1)
	{
		std::unique_ptr<SQLMsg> epis(DBManager::instance().DirectQuery("UPDATE account.account SET cash = cash + %d WHERE id = %u", ep, GetDesc()->GetAccountTable().id));
		if (epis->Get()->uiAffectedRows > 0)
		{
			return true;				
		}
	}

	return false;
}

#endif

void CHARACTER::SetDungeon(LPDUNGEON pkDungeon)
{
	if (pkDungeon && m_pkDungeon)
		sys_err("%s is trying to reassigning dungeon (current %p, new party %p)", GetName(), get_pointer(m_pkDungeon), get_pointer(pkDungeon));

	if (m_pkDungeon == pkDungeon) {
		return;
	}

	if (m_pkDungeon)
	{
		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->DecPartyMember(GetParty(), this);
			else
				m_pkDungeon->DecMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->DecMonster();
		}
	}

	m_pkDungeon = pkDungeon;

	if (pkDungeon)
	{
		sys_log(0, "%s DUNGEON set to %p, PARTY is %p", GetName(), get_pointer(pkDungeon), get_pointer(m_pkParty));

		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->IncPartyMember(GetParty(), this);
			else
				m_pkDungeon->IncMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->IncMonster();
		}
	}
}

void CHARACTER::SetWarMap(CWarMap * pWarMap)
{
	if (m_pWarMap)
		m_pWarMap->DecMember(this);

	m_pWarMap = pWarMap;

	if (m_pWarMap)
		m_pWarMap->IncMember(this);
}

void CHARACTER::SetWeddingMap(marriage::WeddingMap* pMap)
{
	if (m_pWeddingMap)
		m_pWeddingMap->DecMember(this);

	m_pWeddingMap = pMap;

	if (m_pWeddingMap)
		m_pWeddingMap->IncMember(this);
}

void CHARACTER::SetRegen(LPREGEN pkRegen)
{
	m_pkRegen = pkRegen;
	if (pkRegen != NULL) {
		regen_id_ = pkRegen->id;
	}
	m_fRegenAngle = GetRotation();
	m_posRegen = GetXYZ();
}

bool CHARACTER::OnIdle()
{
	return false;
}

void CHARACTER::OnMove(bool bIsAttack)
{
	m_dwLastMoveTime = get_dword_time();

	if (bIsAttack)
	{
		m_dwLastAttackTime = m_dwLastMoveTime;

		if (IsAffectFlag(AFF_REVIVE_INVISIBLE))
			RemoveAffect(AFFECT_REVIVE_INVISIBLE);

		if (IsAffectFlag(AFF_EUNHYUNG))
		{
			RemoveAffect(SKILL_EUNHYUNG);
			SetAffectedEunhyung();
		}
		else
		{
			ClearAffectedEunhyung();
		}

		/*if (IsAffectFlag(AFF_JEONSIN))
		  RemoveAffect(SKILL_JEONSINBANGEO);*/
	}

	/*if (IsAffectFlag(AFF_GUNGON))
	  RemoveAffect(SKILL_GUNGON);*/

	// MINING
	mining_cancel();
	// END_OF_MINING
}

void CHARACTER::OnClick(LPCHARACTER pkChrCauser)
{
	if (!pkChrCauser)
	{
		sys_err("OnClick %s by NULL", GetName());
		return;
	}

	DWORD vid = GetVID();
	sys_log(0, "OnClick %s[vnum %d ServerUniqueID %d, pid %d] by %s", GetName(), GetRaceNum(), vid, GetPlayerID(), pkChrCauser->GetName());

	{
		if (pkChrCauser->GetMyShop() && pkChrCauser != this)
		{
			sys_err("OnClick Fail (%s->%s) - pc has shop", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	{
		if (pkChrCauser->GetExchange())
		{
			sys_err("OnClick Fail (%s->%s) - pc is exchanging", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	if (IsPC())
	{
		if (!CTargetManager::instance().GetTargetInfo(pkChrCauser->GetPlayerID(), TARGET_TYPE_VID, GetVID()))
		{
			if (GetMyShop())
			{
				if (pkChrCauser->IsDead() == true) return;

				//PREVENT_TRADE_WINDOW
				if (pkChrCauser == this)
				{
					if ((GetExchange() || IsOpenSafebox() || GetShopOwner()) || IsCubeOpen()
#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
						|| GetOfflineShopOwner()
#endif
						)
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use a private shop now."));
						return;
					}
				}
				else
				{

					if ((pkChrCauser->GetExchange() || pkChrCauser->IsOpenSafebox() || pkChrCauser->GetMyShop() || pkChrCauser->GetShopOwner()) || pkChrCauser->IsCubeOpen()
#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
						|| pkChrCauser->GetOfflineShopOwner()
#endif
						)
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use a private shop now."));
						return;
					}

					//if ((GetExchange() || IsOpenSafebox() || GetShopOwner()))
					if ((GetExchange() || IsOpenSafebox() || IsCubeOpen()
#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
						|| GetOfflineShopOwner()
#endif
						))
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This player trades already with another player."));
						return;
					}
				}
				//END_PREVENT_TRADE_WINDOW

				if (pkChrCauser->GetShop())
				{
					pkChrCauser->GetShop()->RemoveGuest(pkChrCauser);
					pkChrCauser->SetShop(NULL);
				}

				GetMyShop()->AddGuest(pkChrCauser, GetVID(), false);
				pkChrCauser->SetShopOwner(this);
#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
				if (CTargetManager::instance().GetTargetInfo(pkChrCauser->GetPlayerID(), TARGET_TYPE_VID_SHOP_SEARCH, static_cast<DWORD>(GetVID())))
					CTargetManager::instance().DeleteTarget(pkChrCauser->GetPlayerID(), SHOP_SEARCH_INDEX, "__SHOPSEARCH_TARGET__");
#endif
				return;
			}

			if (test_server)
				sys_err("%s.OnClickFailure(%s) - target is PC", pkChrCauser->GetName(), GetName());

			return;
		}
	}

	if (g_bChinaIntoxicationCheck)
	{
		if (pkChrCauser->IsOverTime(OT_3HOUR))
		{
			sys_log(0, "Teen OverTime : name = %s, hour = %d)", pkChrCauser->GetName(), 3);
			return;
		}
		else if (pkChrCauser->IsOverTime(OT_5HOUR))
		{
			sys_log(0, "Teen OverTime : name = %s, hour = %d)", pkChrCauser->GetName(), 5);
			return;
		}
	}

#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
	if (IsOfflineShopNPC()) {
		if (pkChrCauser->GetOfflineShopVID() == this->GetVID()) {
			::interpret_command(pkChrCauser, "open_offlineshop", strlen("open_offlineshop"));
			return;
		}
		else {
			if (pkChrCauser && (pkChrCauser->HaveAnotherPagesOpen())) {
				pkChrCauser->ChatPacket(CHAT_TYPE_INFO, "Acik olan pencereleri kapat ve tekrar dene.");
				return;
			}
			pkChrCauser->SetOfflineShopOwner(this);
			if (GetOfflineShop()) { GetOfflineShop()->AddGuest(pkChrCauser, this); }
			return;
		}
	}
#endif

	pkChrCauser->SetQuestNPCID(GetVID());

	if (quest::CQuestManager::instance().Click(pkChrCauser->GetPlayerID(), this))
	{
		return;
	}


	if (!IsPC())
	{
		if (!m_triggerOnClick.pFunc)
		{
			//sys_err("%s.OnClickFailure(%s) : triggerOnClick.pFunc is EMPTY(pid=%d)",
			//			pkChrCauser->GetName(),
			//			GetName(),
			//			pkChrCauser->GetPlayerID());
			return;
		}

		m_triggerOnClick.pFunc(this, pkChrCauser);
	}

}

BYTE CHARACTER::GetGMLevel() const
{
	if (test_server)
		return GM_IMPLEMENTOR;
	return m_pointsInstant.gm_level;
}

void CHARACTER::SetGMLevel()
{
	if (GetDesc())
	{
		m_pointsInstant.gm_level =  gm_get_level(GetName(), GetDesc()->GetHostName(), GetDesc()->GetAccountTable().login);
	}
	else
	{
		m_pointsInstant.gm_level = GM_PLAYER;
	}
}

BOOL CHARACTER::IsGM() const
{
	if (m_pointsInstant.gm_level != GM_PLAYER)
		return true;
	if (test_server)
		return true;
	return false;
}

void CHARACTER::SetStone(LPCHARACTER pkChrStone)
{
	m_pkChrStone = pkChrStone;

	if (m_pkChrStone)
	{
		if (pkChrStone->m_set_pkChrSpawnedBy.find(this) == pkChrStone->m_set_pkChrSpawnedBy.end())
			pkChrStone->m_set_pkChrSpawnedBy.insert(this);
	}
}

struct FuncDeadSpawnedByStone
{
	void operator () (LPCHARACTER ch)
	{
		ch->Dead(NULL);
		ch->SetStone(NULL);
	}
};

void CHARACTER::ClearStone()
{
	if (!m_set_pkChrSpawnedBy.empty())
	{
		FuncDeadSpawnedByStone f;
		std::for_each(m_set_pkChrSpawnedBy.begin(), m_set_pkChrSpawnedBy.end(), f);
		m_set_pkChrSpawnedBy.clear();
	}

	if (!m_pkChrStone)
		return;

	m_pkChrStone->m_set_pkChrSpawnedBy.erase(this);
	m_pkChrStone = NULL;
}

void CHARACTER::ClearTarget()
{
	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
		m_pkChrTarget = NULL;
	}
	
	TPacketGCTarget p;
	p.header = HEADER_GC_TARGET;
	p.dwVID = 0;
	p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
	p.iMinHP = 0;
	p.iMaxHP = 0;
#endif
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	memcpy (p.dwCostumeHideID, "", sizeof(p.dwCostumeHideID));
#endif
	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();
	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *(it++);
		pkChr->m_pkChrTarget = NULL;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}
		
		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}
	
	m_set_pkChrTargetedBy.clear();
}

void CHARACTER::SetTarget(LPCHARACTER pkChrTarget)
{
	if (m_pkChrTarget == pkChrTarget)
		return;
	
	if (IS_CASTLE_MAP(GetMapIndex()) && !IsGM())
		return;
	
	if (m_pkChrTarget)
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
	
	m_pkChrTarget = pkChrTarget;
	
	TPacketGCTarget p;
	p.header = HEADER_GC_TARGET;
	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.insert(this);
		p.dwVID	= m_pkChrTarget->GetVID();
#ifdef __VIEW_TARGET_PLAYER_HP__
		if ((m_pkChrTarget->GetMaxHP() <= 0))
		{
			p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
		if (m_pkChrTarget->IsPC()){
			char c_hide_show[255+1];
			snprintf(c_hide_show, sizeof(c_hide_show), "%d|%d|%d|%d|", m_pkChrTarget->GetQuestFlag("hide_costume_w.hide_costume_0"),m_pkChrTarget->GetQuestFlag("hide_costume_w.hide_costume_1"),m_pkChrTarget->GetQuestFlag("hide_costume_w.hide_costume_2"),m_pkChrTarget->GetQuestFlag("hide_costume_w.hide_costume_3"));
			memcpy (p.dwCostumeHideID, c_hide_show, sizeof(p.dwCostumeHideID));
		}
#endif
		}
		else if (m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed())
		{
			p.bHPPercent = MINMAX(0, (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP(), 100);
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = m_pkChrTarget->GetHP();
			p.iMaxHP = m_pkChrTarget->GetMaxHP();
#endif
#else
		if ((m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed()) || (m_pkChrTarget->GetMaxHP() <= 0))
		{
			p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
#endif
		}
		else
		{
			if (m_pkChrTarget->GetRaceNum() == 20101 || m_pkChrTarget->GetRaceNum() == 20102 || m_pkChrTarget->GetRaceNum() == 20103 || m_pkChrTarget->GetRaceNum() == 20104 || m_pkChrTarget->GetRaceNum() == 20105 || m_pkChrTarget->GetRaceNum() == 20106 || m_pkChrTarget->GetRaceNum() == 20107 || m_pkChrTarget->GetRaceNum() == 20108 || m_pkChrTarget->GetRaceNum() == 20109)
			{
				LPCHARACTER owner = m_pkChrTarget->GetVictim();
				if (owner)
				{
					int iHorseHealth = owner->GetHorseHealth();
					int iHorseMaxHealth = owner->GetHorseMaxHealth();
					if (iHorseMaxHealth)
					{
						p.bHPPercent = MINMAX(0,  iHorseHealth * 100 / iHorseMaxHealth, 100);
#ifdef __VIEW_TARGET_DECIMAL_HP__
						p.iMinHP = 100;
						p.iMaxHP = 100;
#endif
					}
					else
					{
						p.bHPPercent = 100;
#ifdef __VIEW_TARGET_DECIMAL_HP__
						p.iMinHP = 100;
						p.iMaxHP = 100;
#endif
					}
				}
				else
				{
					p.bHPPercent = 100;
#ifdef __VIEW_TARGET_DECIMAL_HP__
					p.iMinHP = 100;
					p.iMaxHP = 100;
#endif
				}
			}
			else
			{
				if (m_pkChrTarget->GetMaxHP() <= 0)
				{
					p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
					p.iMinHP = 0;
					p.iMaxHP = 0;
#endif
				}
				else
				{
					p.bHPPercent = MINMAX(0, (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP(), 100);
#ifdef __VIEW_TARGET_DECIMAL_HP__
					p.iMinHP = m_pkChrTarget->GetHP();
					p.iMaxHP = m_pkChrTarget->GetMaxHP();
#endif
				}
			}
		}
	}
	else
	{
		p.dwVID = 0;
		p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
		p.iMinHP = 0;
		p.iMaxHP = 0;
#endif
	}
	
	GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
}

void CHARACTER::BroadcastTargetPacket()
{
	if (m_set_pkChrTargetedBy.empty())
		return;
	
	TPacketGCTarget p;
	p.header = HEADER_GC_TARGET;
	p.dwVID = GetVID();
	if (GetMaxHP() <= 0)
	{
		p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
		p.iMinHP = 0;
		p.iMaxHP = 0;
#endif
	}
	else
	{
#ifdef __VIEW_TARGET_PLAYER_HP__
		p.bHPPercent = MINMAX(0, (GetHP() * 100) / GetMaxHP(), 100);
#ifdef __VIEW_TARGET_DECIMAL_HP__
		p.iMinHP = GetHP();
		p.iMaxHP = GetMaxHP();
#endif
#else
		if (IsPC())
		{
			p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
		}
		else
		{
			p.bHPPercent = MINMAX(0, (GetHP() * 100) / GetMaxHP(), 100);
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = GetHP();
			p.iMaxHP = GetMaxHP();
#endif
		}
#endif
	}
	
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
		if (IsPC()){
			char c_hide_show[255+1];
			snprintf(c_hide_show, sizeof(c_hide_show), "%d|%d|%d|%d|", GetQuestFlag("hide_costume_w.hide_costume_0"),GetQuestFlag("hide_costume_w.hide_costume_1"),GetQuestFlag("hide_costume_w.hide_costume_2"),GetQuestFlag("hide_costume_w.hide_costume_3"));
			memcpy (p.dwCostumeHideID, c_hide_show, sizeof(p.dwCostumeHideID));
		}
#endif

	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();
	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *it++;
		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}
		
		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}
}

void CHARACTER::CheckTarget()
{
	if (!m_pkChrTarget)
		return;

	if (DISTANCE_APPROX(GetX() - m_pkChrTarget->GetX(), GetY() - m_pkChrTarget->GetY()) >= 4800)
		SetTarget(NULL);
}

void CHARACTER::SetWarpLocation(long lMapIndex, long x, long y)
{
	m_posWarp.x = x * 100;
	m_posWarp.y = y * 100;
	m_lWarpMapIndex = lMapIndex;
}

void CHARACTER::SaveExitLocation()
{
	m_posExit = GetXYZ();
	m_lExitMapIndex = GetMapIndex();
}

void CHARACTER::ExitToSavedLocation()
{
	sys_log (0, "ExitToSavedLocation");
	WarpSet(m_posWarp.x, m_posWarp.y, m_lWarpMapIndex);

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;
}

#if defined(__BL_MOVE_CHANNEL__)
bool CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex, long lCustomAddr, WORD wCustomPort)
#else
bool CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex)
#endif
{
	if (!IsPC())
		return false;

	long lAddr;
	long lMapIndex;
	WORD wPort;

	if (!CMapLocation::instance().Get(x, y, lMapIndex, lAddr, wPort))
	{
		sys_err("cannot find map location index %d x %d y %d name %s", lMapIndex, x, y, GetName());
		return false;
	}

	//Send Supplementary Data Block if new map requires security packages in loading this map
	{
		long lCurAddr;
		long lCurMapIndex = 0;
		WORD wCurPort;

		CMapLocation::instance().Get(GetX(), GetY(), lCurMapIndex, lCurAddr, wCurPort);

		//do not send SDB files if char is in the same map
		if( lCurMapIndex != lMapIndex )
		{
			const TMapRegion * rMapRgn = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);
			{
				DESC_MANAGER::instance().SendClientPackageSDBToLoadMap( GetDesc(), rMapRgn->strMapName.c_str() );
			}
		}
	}

	if (lPrivateMapIndex >= 10000)
	{
		if (lPrivateMapIndex / 10000 != lMapIndex)
		{
			sys_err("Invalid map index %d, must be child of %d", lPrivateMapIndex, lMapIndex);
			return false;
		}

		lMapIndex = lPrivateMapIndex;
	}

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();

		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	sys_log(0, "WarpSet %s %d %d current map %d target map %d", GetName(), x, y, GetMapIndex(), lMapIndex);

	TPacketGCWarp p;

	p.bHeader	= HEADER_GC_WARP;
	p.lX	= x;
	p.lY	= y;
	p.lAddr	= lCustomAddr ? lCustomAddr : lAddr;
#ifdef ENABLE_NEWSTUFF
	if (!g_stProxyIP.empty())
		p.lAddr = inet_addr(g_stProxyIP.c_str());
#endif
	p.wPort	= wCustomPort ? wCustomPort : wPort;

#ifdef ENABLE_SWITCHBOT
	CSwitchbotManager::Instance().SetIsWarping(GetPlayerID(), true);

	if (p.wPort != mother_port)
	{
		CSwitchbotManager::Instance().P2PSendSwitchbot(GetPlayerID(), p.wPort);
	}
#endif

	GetDesc()->Packet(&p, sizeof(TPacketGCWarp));

	char buf[256];
	snprintf(buf, sizeof(buf), "%s MapIdx %ld DestMapIdx%ld DestX%ld DestY%ld Empire%d", GetName(), GetMapIndex(), lPrivateMapIndex, x, y, GetEmpire());
	LogManager::instance().CharLog(this, 0, "WARP", buf);

	return true;
}

#define ENABLE_GOHOME_IF_MAP_NOT_ALLOWED
void CHARACTER::WarpEnd()
{
	if (test_server)
		sys_log(0, "WarpEnd %s", GetName());

	if (m_posWarp.x == 0 && m_posWarp.y == 0)
		return;

	int index = m_lWarpMapIndex;

	if (index > 10000)
		index /= 10000;

	if (!map_allow_find(index))
	{
		sys_err("location %d %d not allowed to login this server", m_posWarp.x, m_posWarp.y);
#ifdef ENABLE_GOHOME_IF_MAP_NOT_ALLOWED
		GoHome();
#else
		GetDesc()->SetPhase(PHASE_CLOSE);
#endif
		return;
	}

	sys_log(0, "WarpEnd %s %d %u %u", GetName(), m_lWarpMapIndex, m_posWarp.x, m_posWarp.y);

	Show(m_lWarpMapIndex, m_posWarp.x, m_posWarp.y, 0);
	Stop();

	m_lWarpMapIndex = 0;
	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;

	{
		// P2P Login
		TPacketGGLogin p;

		p.bHeader = HEADER_GG_LOGIN;
		strlcpy(p.szName, GetName(), sizeof(p.szName));
		p.dwPID = GetPlayerID();
		p.bEmpire = GetEmpire();
		p.lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(GetX(), GetY());
		p.bChannel = g_bChannel;
#ifdef __MULTI_FARM_BLOCK__
		memset(p.bIpAdress, 0, sizeof(p.bIpAdress));
#endif
		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogin));
	}
}

bool CHARACTER::Return()
{
	if (!IsNPC())
		return false;

	int x, y;
	/*
	   float fDist = DISTANCE_SQRT(m_pkMobData->m_posLastAttacked.x - GetX(), m_pkMobData->m_posLastAttacked.y - GetY());
	   float fx, fy;
	   GetDeltaByDegree(GetRotation(), fDist, &fx, &fy);
	   x = GetX() + (int) fx;
	   y = GetY() + (int) fy;
	 */
	SetVictim(NULL);

	x = m_pkMobInst->m_posLastAttacked.x;
	y = m_pkMobInst->m_posLastAttacked.y;

	SetRotationToXY(x, y);

	if (!Goto(x, y))
		return false;

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	if (test_server)
		sys_log(0, "%s %p �����ϰ� ���ư���! %d %d", GetName(), this, x, y);

	if (GetParty())
		GetParty()->SendMessage(this, PM_RETURN, x, y);

	return true;
}

bool CHARACTER::Follow(LPCHARACTER pkChr, float fMinDistance)
{
	if (IsPC())
	{
		sys_err("CHARACTER::Follow : PC cannot use this method", GetName());
		return false;
	}

	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
	{
		if (pkChr->IsPC())
		{
			// If i'm in a party. I must obey party leader's AI.
			if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
			{
				if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000)
				{
					if (m_pkMobData->m_table.wAttackRange < DISTANCE_APPROX(pkChr->GetX() - GetX(), pkChr->GetY() - GetY()))
						if (Return())
							return true;
				}
			}
		}
		return false;
	}
	// END_OF_TRENT_MONSTER

	long x = pkChr->GetX();
	long y = pkChr->GetY();

	if (pkChr->IsPC())
	{
		// If i'm in a party. I must obey party leader's AI.
		if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
		{
			if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000)
			{
				if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
					if (Return())
						return true;
			}
		}
	}

	if (IsGuardNPC())
	{
		if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
			if (Return())
				return true;
	}

	if (pkChr->IsState(pkChr->m_stateMove) &&
		GetMobBattleType() != BATTLE_TYPE_RANGE &&
		GetMobBattleType() != BATTLE_TYPE_MAGIC &&
		false == IsPet()
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
		&& false == IsMount()
#endif
		)
	{
		float rot = pkChr->GetRotation();
		float rot_delta = GetDegreeDelta(rot, GetDegreeFromPositionXY(GetX(), GetY(), pkChr->GetX(), pkChr->GetY()));

		float yourSpeed = pkChr->GetMoveSpeed();
		float mySpeed = GetMoveSpeed();

		float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());
		float fFollowSpeed = mySpeed - yourSpeed * cos(rot_delta * M_PI / 180);

		if (fFollowSpeed >= 0.1f)
		{
			float fMeetTime = fDist / fFollowSpeed;
			float fYourMoveEstimateX, fYourMoveEstimateY;

			if( fMeetTime * yourSpeed <= 100000.0f )
			{
				GetDeltaByDegree(pkChr->GetRotation(), fMeetTime * yourSpeed, &fYourMoveEstimateX, &fYourMoveEstimateY);

				x += (long) fYourMoveEstimateX;
				y += (long) fYourMoveEstimateY;

				float fDistNew = sqrt(((double)x - GetX())*(x-GetX())+((double)y - GetY())*(y-GetY()));
				if (fDist < fDistNew)
				{
					x = (long)(GetX() + (x - GetX()) * fDist / fDistNew);
					y = (long)(GetY() + (y - GetY()) * fDist / fDistNew);
				}
			}
		}
	}

	SetRotationToXY(x, y);

	float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());

	if (fDist <= fMinDistance)
		return false;

	float fx, fy;

	if (IsChangeAttackPosition(pkChr) && GetMobRank() < MOB_RANK_BOSS)
	{
		SetChangeAttackPositionTime();

		int retry = 16;
		int dx, dy;
		int rot = (int) GetDegreeFromPositionXY(x, y, GetX(), GetY());

		while (--retry)
		{
			if (fDist < 500.0f)
				GetDeltaByDegree((rot + number(-90, 90) + number(-90, 90)) % 360, fMinDistance, &fx, &fy);
			else
				GetDeltaByDegree(number(0, 359), fMinDistance, &fx, &fy);

			dx = x + (int) fx;
			dy = y + (int) fy;

			LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), dx, dy);

			if (NULL == tree)
				break;

			if (0 == (tree->GetAttribute(dx, dy) & (ATTR_BLOCK | ATTR_OBJECT)))
				break;
		}

		if (!Goto(dx, dy))
			return false;
	}
	else
	{
		float fDistToGo = fDist - fMinDistance;
		GetDeltaByDegree(GetRotation(), fDistToGo, &fx, &fy);

		if (!Goto(GetX() + (int) fx, GetY() + (int) fy))
			return false;
	}

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	return true;
}

float CHARACTER::GetDistanceFromSafeboxOpen() const
{
	return DISTANCE_APPROX(GetX() - m_posSafeboxOpen.x, GetY() - m_posSafeboxOpen.y);
}

void CHARACTER::SetSafeboxOpenPosition()
{
	m_posSafeboxOpen = GetXYZ();
}

CSafebox * CHARACTER::GetSafebox() const
{
	return m_pkSafebox;
}

void CHARACTER::ReqSafeboxLoad(const char* pszPassword)
{
	if (!*pszPassword || strlen(pszPassword) > SAFEBOX_PASSWORD_MAX_LEN)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Storages> You entered a wrong password."));
		return;
	}
	else if (m_pkSafebox)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Storages> The storage is already open."));
		return;
	}

	int iPulse = thecore_pulse();

	if (iPulse - GetSafeboxLoadTime()  < PASSES_PER_SEC(10))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Storages> You have to wait 10 seconds to open the storage again."));
		return;
	}
	else if (GetDistanceFromSafeboxOpen() > 1000)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Storages> The distance between you and the storage is too far to open it."));
		return;
	}
	else if (m_bOpeningSafebox)
	{
		sys_log(0, "Overlapped safebox load request from %s", GetName());
		return;
	}

	SetSafeboxLoadTime();
	m_bOpeningSafebox = true;

	TSafeboxLoadPacket p;
	p.dwID = GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, pszPassword, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_LOAD, GetDesc()->GetHandle(), &p, sizeof(p));
}

void CHARACTER::LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(true);
	//END_PREVENT_TRADE_WINDOW

	if (m_pkSafebox)
		bLoaded = true;

	if (!m_pkSafebox)
		m_pkSafebox = M2_NEW CSafebox(this, iSize, dwGold);
	else
		m_pkSafebox->ChangeSize(iSize);

	m_iSafeboxSize = iSize;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = iSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkSafebox->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);

			if (!m_pkSafebox->Add(pItems->pos, item))
			{
				M2_DESTROY_ITEM(item);
			}
			else
				item->SetSkipSave(false);
		}
	}
}

void CHARACTER::ChangeSafeboxSize(BYTE bSize)
{
	//if (!m_pkSafebox)
	//return;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = bSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (m_pkSafebox)
		m_pkSafebox->ChangeSize(bSize);

	m_iSafeboxSize = bSize;
}

void CHARACTER::CloseSafebox()
{
	if (!m_pkSafebox)
		return;

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(false);
	//END_PREVENT_TRADE_WINDOW

	m_pkSafebox->Save();

	M2_DELETE(m_pkSafebox);
	m_pkSafebox = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseSafebox");

	SetSafeboxLoadTime();
	m_bOpeningSafebox = false;

	Save();
}

CSafebox * CHARACTER::GetMall() const
{
	return m_pkMall;
}

void CHARACTER::LoadMall(int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	if (m_pkMall)
		bLoaded = true;

	if (!m_pkMall)
		m_pkMall = M2_NEW CSafebox(this, 3 * SAFEBOX_PAGE_SIZE, 0);
	else
		m_pkMall->ChangeSize(3 * SAFEBOX_PAGE_SIZE);

	m_pkMall->SetWindowMode(MALL);

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_MALL_OPEN;
	p.bSize = 3 * SAFEBOX_PAGE_SIZE;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkMall->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);

			if (!m_pkMall->Add(pItems->pos, item))
				M2_DESTROY_ITEM(item);
			else
				item->SetSkipSave(false);
		}
	}
}

void CHARACTER::CloseMall()
{
	if (!m_pkMall)
		return;

	m_pkMall->Save();

	M2_DELETE(m_pkMall);
	m_pkMall = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseMall");
}

bool CHARACTER::BuildUpdatePartyPacket(TPacketGCPartyUpdate & out)
{
	if (!GetParty())
		return false;

	memset(&out, 0, sizeof(out));

	out.header		= HEADER_GC_PARTY_UPDATE;
	out.pid		= GetPlayerID();
	if (GetMaxHP() <= 0) // @fixme136
		out.percent_hp	= 0;
	else
		out.percent_hp	= MINMAX(0, GetHP() * 100 / GetMaxHP(), 100);
	out.role		= GetParty()->GetRole(GetPlayerID());

	sys_log(1, "PARTY %s role is %d", GetName(), out.role);

	LPCHARACTER l = GetParty()->GetLeaderCharacter();

	if (l && DISTANCE_APPROX(GetX() - l->GetX(), GetY() - l->GetY()) < PARTY_DEFAULT_RANGE)
	{
		out.affects[0] = GetParty()->GetPartyBonusExpPercent();
		out.affects[1] = GetPoint(POINT_PARTY_ATTACKER_BONUS);
		out.affects[2] = GetPoint(POINT_PARTY_TANKER_BONUS);
		out.affects[3] = GetPoint(POINT_PARTY_BUFFER_BONUS);
		out.affects[4] = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
		out.affects[5] = GetPoint(POINT_PARTY_HASTE_BONUS);
		out.affects[6] = GetPoint(POINT_PARTY_DEFENDER_BONUS);
	}

	return true;
}

int CHARACTER::GetLeadershipSkillLevel() const
{
	return GetSkillLevel(SKILL_LEADERSHIP);
}

void CHARACTER::QuerySafeboxSize()
{
	if (m_iSafeboxSize == -1)
	{
		DBManager::instance().ReturnQuery(QID_SAFEBOX_SIZE,
				GetPlayerID(),
				NULL,
				"SELECT size FROM safebox%s WHERE account_id = %u",
				get_table_postfix(),
				GetDesc()->GetAccountTable().id);
	}
}

void CHARACTER::SetSafeboxSize(int iSize)
{
	sys_log(1, "SetSafeboxSize: %s %d", GetName(), iSize);
	m_iSafeboxSize = iSize;
	DBManager::instance().Query("UPDATE safebox%s SET size = %d WHERE account_id = %u", get_table_postfix(), iSize / SAFEBOX_PAGE_SIZE, GetDesc()->GetAccountTable().id);
}

int CHARACTER::GetSafeboxSize() const
{
	return m_iSafeboxSize;
}

void CHARACTER::SetNowWalking(bool bWalkFlag)
{
	//if (m_bNowWalking != bWalkFlag || IsNPC())
	if (m_bNowWalking != bWalkFlag)
	{
		if (bWalkFlag)
		{
			m_bNowWalking = true;
			m_dwWalkStartTime = get_dword_time();
		}
		else
		{
			m_bNowWalking = false;
		}

		//if (m_bNowWalking)
		{
			TPacketGCWalkMode p;
			p.vid = GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

			PacketView(&p, sizeof(p));
		}

		if (IsNPC())
		{
			if (m_bNowWalking)
				MonsterLog("�ȴ´�");
			else
				MonsterLog("�ڴ�");
		}

		//sys_log(0, "%s is now %s", GetName(), m_bNowWalking?"walking.":"running.");
	}
}

void CHARACTER::StartStaminaConsume()
{
	if (m_bStaminaConsume)
		return;
	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = true;
	//ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
	if (IsStaminaHalfConsume())
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec / 2, GetStamina());
	else
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
}

void CHARACTER::StopStaminaConsume()
{
	if (!m_bStaminaConsume)
		return;
	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = false;
	ChatPacket(CHAT_TYPE_COMMAND, "StopStaminaConsume %d", GetStamina());
}

bool CHARACTER::IsStaminaConsume() const
{
	return m_bStaminaConsume;
}

bool CHARACTER::IsStaminaHalfConsume() const
{
	return IsEquipUniqueItem(UNIQUE_ITEM_HALF_STAMINA);
}

void CHARACTER::ResetStopTime()
{
	m_dwStopTime = get_dword_time();
}

DWORD CHARACTER::GetStopTime() const
{
	return m_dwStopTime;
}

void CHARACTER::ResetPoint(int iLv)
{
	BYTE bJob = GetJob();

	PointChange(POINT_LEVEL, iLv - GetLevel());

	SetRealPoint(POINT_ST, JobInitialPoints[bJob].st);
	SetPoint(POINT_ST, GetRealPoint(POINT_ST));

	SetRealPoint(POINT_HT, JobInitialPoints[bJob].ht);
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));

	SetRealPoint(POINT_DX, JobInitialPoints[bJob].dx);
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));

	SetRealPoint(POINT_IQ, JobInitialPoints[bJob].iq);
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetRandomHP((iLv - 1) * number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end));
	SetRandomSP((iLv - 1) * number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end));

	// @fixme104
	PointChange(POINT_STAT, (MINMAX(1, iLv, g_iStatusPointGetLevelLimit) * 3) + GetPoint(POINT_LEVEL_STEP) - GetPoint(POINT_STAT));

	ComputePoints();

	PointChange(POINT_HP, GetMaxHP() - GetHP());
	PointChange(POINT_SP, GetMaxSP() - GetSP());

	PointsPacket();

	LogManager::instance().CharLog(this, 0, "RESET_POINT", "");
}

bool CHARACTER::IsChangeAttackPosition(LPCHARACTER target) const
{
	if (!IsNPC())
		return true;

	DWORD dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_NEAR;

	if (DISTANCE_APPROX(GetX() - target->GetX(), GetY() - target->GetY()) >
		AI_CHANGE_ATTACK_POISITION_DISTANCE + GetMobAttackRange())
		dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_FAR;

	return get_dword_time() - m_dwLastChangeAttackPositionTime > dwChangeTime;
}

void CHARACTER::GiveRandomSkillBook()
{
	LPITEM item = AutoGiveItem(50300);

	if (NULL != item)
	{
		extern const DWORD GetRandomSkillVnum(BYTE bJob = JOB_MAX_NUM);
		DWORD dwSkillVnum = 0;
		// 50% of getting random books or getting one of the same player's race
		if (!number(0, 1))
			dwSkillVnum = GetRandomSkillVnum(GetJob());
		else
			dwSkillVnum = GetRandomSkillVnum();
		item->SetSocket(0, dwSkillVnum);
	}
}

void CHARACTER::ReviveInvisible(int iDur)
{
	AddAffect(AFFECT_REVIVE_INVISIBLE, POINT_NONE, 0, AFF_REVIVE_INVISIBLE, iDur, 0, true);
}

void CHARACTER::ToggleMonsterLog()
{
	m_bMonsterLog = !m_bMonsterLog;

	if (m_bMonsterLog)
	{
		CHARACTER_MANAGER::instance().RegisterForMonsterLog(this);
	}
	else
	{
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
	}
}

void CHARACTER::SetGuild(CGuild* pGuild)
{
	if (m_pGuild != pGuild)
	{
		m_pGuild = pGuild;
		UpdatePacket();
	}
}

void CHARACTER::SendGreetMessage()
{
	typeof(DBManager::instance().GetGreetMessage()) v = DBManager::instance().GetGreetMessage();

	for (itertype(v) it = v.begin(); it != v.end(); ++it)
	{
		ChatPacket(CHAT_TYPE_NOTICE, it->c_str());
	}
}

void CHARACTER::BeginStateEmpty()
{
	MonsterLog("!");
}

void CHARACTER::EffectPacket(int enumEffectType)
{
	TPacketGCSpecialEffect p;

	p.header = HEADER_GC_SEPCIAL_EFFECT;
	p.type = enumEffectType;
	p.vid = GetVID();

	PacketAround(&p, sizeof(TPacketGCSpecialEffect));
}

void CHARACTER::SpecificEffectPacket(const char filename[MAX_EFFECT_FILE_NAME])
{
	TPacketGCSpecificEffect p;

	p.header = HEADER_GC_SPECIFIC_EFFECT;
	p.vid = GetVID();
	memcpy (p.effect_file, filename, MAX_EFFECT_FILE_NAME);

	PacketAround(&p, sizeof(TPacketGCSpecificEffect));
}

void CHARACTER::MonsterChat(BYTE bMonsterChatType)
{
	if (IsPC())
		return;

	char sbuf[256+1];

	if (IsMonster())
	{
		if (number(0, 60))
			return;

		snprintf(sbuf, sizeof(sbuf),
				"(locale.monster_chat[%i] and locale.monster_chat[%i][%d] or '')",
				GetRaceNum(), GetRaceNum(), bMonsterChatType*3 + number(1, 3));
	}
	else
	{
		if (bMonsterChatType != MONSTER_CHAT_WAIT)
			return;

		if (IsGuardNPC())
		{
			if (number(0, 6))
				return;
		}
		else
		{
			if (number(0, 30))
				return;
		}

		snprintf(sbuf, sizeof(sbuf), "(locale.monster_chat[%i] and locale.monster_chat[%i][number(1, table.getn(locale.monster_chat[%i]))] or '')", GetRaceNum(), GetRaceNum(), GetRaceNum());
	}

	std::string text = quest::ScriptToString(sbuf);

	if (text.empty())
		return;

	struct packet_chat pack_chat;

	pack_chat.header	= HEADER_GC_CHAT;
	pack_chat.size	= sizeof(struct packet_chat) + text.size() + 1;
	pack_chat.type	  = CHAT_TYPE_TALKING;
	pack_chat.id		= GetVID();
	pack_chat.bEmpire	= 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(text.c_str(), text.size() + 1);

	PacketAround(buf.read_peek(), buf.size());
}

void CHARACTER::SetQuestNPCID(DWORD vid)
{
	m_dwQuestNPCVID = vid;
}

LPCHARACTER CHARACTER::GetQuestNPC() const
{
	return CHARACTER_MANAGER::instance().Find(m_dwQuestNPCVID);
}

void CHARACTER::SetQuestItemPtr(LPITEM item)
{
	m_pQuestItem = item;
}

void CHARACTER::ClearQuestItemPtr()
{
	m_pQuestItem = NULL;
}

LPITEM CHARACTER::GetQuestItemPtr() const
{
	return m_pQuestItem;
}

#ifdef ENABLE_QUEST_DND_EVENT
void CHARACTER::SetQuestDNDItemPtr(LPITEM item)
{
	m_pQuestDNDItem = item;
}

void CHARACTER::ClearQuestDNDItemPtr()
{
	m_pQuestDNDItem = NULL;
}

LPITEM CHARACTER::GetQuestDNDItemPtr() const
{
	return m_pQuestDNDItem;
}
#endif

LPDUNGEON CHARACTER::GetDungeonForce() const
{
	if (m_lWarpMapIndex > 10000)
		return CDungeonManager::instance().FindByMapIndex(m_lWarpMapIndex);

	return m_pkDungeon;
}

void CHARACTER::SetBlockMode(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;

	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);

	SetQuestFlag("game_option.block_exchange", bFlag & BLOCK_EXCHANGE ? 1 : 0);
	SetQuestFlag("game_option.block_party_invite", bFlag & BLOCK_PARTY_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_guild_invite", bFlag & BLOCK_GUILD_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_whisper", bFlag & BLOCK_WHISPER ? 1 : 0);
	SetQuestFlag("game_option.block_messenger_invite", bFlag & BLOCK_MESSENGER_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_party_request", bFlag & BLOCK_PARTY_REQUEST ? 1 : 0);
}

void CHARACTER::SetBlockModeForce(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;
	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);
}

bool CHARACTER::IsGuardNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

int CHARACTER::GetPolymorphPower() const
{
	if (test_server)
	{
		int value = quest::CQuestManager::instance().GetEventFlag("poly");
		if (value)
			return value;
	}
	return aiPolymorphPowerByLevel[MINMAX(0, GetSkillLevel(SKILL_POLYMORPH), 40)];
}

void CHARACTER::SetPolymorph(DWORD dwRaceNum, bool bMaintainStat)
{
#ifdef ENABLE_WOLFMAN_CHARACTER
	if (dwRaceNum < MAIN_RACE_MAX_NUM)
#else
	if (dwRaceNum < JOB_MAX_NUM)
#endif
	{
		dwRaceNum = 0;
		bMaintainStat = false;
	}

	if (m_dwPolymorphRace == dwRaceNum)
		return;

	m_bPolyMaintainStat = bMaintainStat;
	m_dwPolymorphRace = dwRaceNum;

	sys_log(0, "POLYMORPH: %s race %u ", GetName(), dwRaceNum);

	if (dwRaceNum != 0)
		StopRiding();

	SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	m_afAffectFlag.Set(AFF_SPAWN);

	ViewReencode();

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	if (!bMaintainStat)
	{
		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_IQ, 0);
		PointChange(POINT_HT, 0);
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputeBattlePoints();
}

int CHARACTER::GetQuestFlag(const std::string& flag) const
{
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(GetPlayerID());

	if(!pPC)
	{
		//sys_err("Nullpointer Faust in CHARACTER::GetQuestFlag %lu -> flagName: %s", GetPlayerID(), flag.c_str());
		return 0;
	}

	return pPC->GetFlag(flag);
}

void CHARACTER::SetQuestFlag(const std::string& flag, int value)
{
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(GetPlayerID());
	pPC->SetFlag(flag, value);
}

void CHARACTER::DetermineDropMetinStone()
{
#ifdef ENABLE_NEWSTUFF
	if (g_NoDropMetinStone)
	{
		m_dwDropMetinStone = 0;
		return;
	}
#endif

	static const DWORD c_adwMetin[] =
	{
#if defined(ENABLE_WOLFMAN_CHARACTER) && defined(USE_WOLFMAN_STONES)
		28012,
#endif
		28030,
		28031,
		28032,
		28033,
		28034,
		28035,
		28036,
		28037,
		28038,
		28039,
		28040,
		28041,
		28042,
		28043,
#if defined(ENABLE_MAGIC_REDUCTION_SYSTEM) && defined(USE_MAGIC_REDUCTION_STONES)
		28044,
		28045,
#endif
	};
	DWORD stone_num = GetRaceNum();
	int idx = std::lower_bound(aStoneDrop, aStoneDrop+STONE_INFO_MAX_NUM, stone_num) - aStoneDrop;
	if (idx >= STONE_INFO_MAX_NUM || aStoneDrop[idx].dwMobVnum != stone_num)
	{
		m_dwDropMetinStone = 0;
	}
	else
	{
		const SStoneDropInfo & info = aStoneDrop[idx];
		m_bDropMetinStonePct = info.iDropPct;
		{
			m_dwDropMetinStone = c_adwMetin[number(0, sizeof(c_adwMetin)/sizeof(DWORD) - 1)];
			int iGradePct = number(1, 100);
			for (int iStoneLevel = 0; iStoneLevel < STONE_LEVEL_MAX_NUM; iStoneLevel ++)
			{
				int iLevelGradePortion = info.iLevelPct[iStoneLevel];
				if (iGradePct <= iLevelGradePortion)
				{
					break;
				}
				else
				{
					iGradePct -= iLevelGradePortion;
					m_dwDropMetinStone += 100;
				}
			}
		}
	}
}

void CHARACTER::SendEquipment(LPCHARACTER ch)
{
	TPacketViewEquip p;
	p.header = HEADER_GC_VIEW_EQUIP;
	p.vid	= GetVID();
	for (int i = 0; i<WEAR_MAX_NUM; i++)
	{
		LPITEM item = GetWear(i);
		if (item)
		{
			p.equips[i].vnum = item->GetVnum();
			p.equips[i].count = item->GetCount();

			thecore_memcpy(p.equips[i].alSockets, item->GetSockets(), sizeof(p.equips[i].alSockets));
			thecore_memcpy(p.equips[i].aAttr, item->GetAttributes(), sizeof(p.equips[i].aAttr));
		}
		else
		{
			p.equips[i].vnum = 0;
		}
	}
	ch->GetDesc()->Packet(&p, sizeof(p));
}

bool CHARACTER::CanSummon(int iLeaderShip)
{
	return ((iLeaderShip >= 20) || ((iLeaderShip >= 12) && ((m_dwLastDeadTime + 180) > get_dword_time())));
}

// #define ENABLE_MOUNT_ENTITY_REFRESH
void CHARACTER::MountVnum(DWORD vnum)
{
	if (m_dwMountVnum == vnum)
		return;
	if ((m_dwMountVnum != 0)&&(vnum!=0)) //@fixme108 set recursively to 0 for eventuality
		MountVnum(0);

	m_dwMountVnum = vnum;
	m_dwMountTime = get_dword_time();

	if (m_bIsObserver)
		return;

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
#ifdef ENABLE_MOUNT_ENTITY_REFRESH
	// EncodeRemovePacket(this); // commented, otherwise it may warp you back
#endif
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

#ifdef ENABLE_MOUNT_ENTITY_REFRESH
		if (entity->IsType(ENTITY_CHARACTER))
		{
			EncodeRemovePacket(entity);
			if (!m_bIsObserver)
				EncodeInsertPacket(entity);

			if (!entity->IsObserverMode())
					entity->EncodeInsertPacket(this);
		}
		else
			EncodeInsertPacket(entity);
#else
		EncodeInsertPacket(entity);
#endif
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputePoints();
}

namespace {
	class FuncCheckWarp
	{
		public:
			FuncCheckWarp(LPCHARACTER pkWarp)
			{
				m_lTargetY = 0;
				m_lTargetX = 0;

				m_lX = pkWarp->GetX();
				m_lY = pkWarp->GetY();

				m_bInvalid = false;
				m_bEmpire = pkWarp->GetEmpire();

				char szTmp[64];

				if (3 != sscanf(pkWarp->GetName(), " %s %ld %ld ", szTmp, &m_lTargetX, &m_lTargetY))
				{
					if (number(1, 100) < 5)
						sys_err("Warp NPC name wrong : vnum(%d) name(%s)", pkWarp->GetRaceNum(), pkWarp->GetName());

					m_bInvalid = true;

					return;
				}

				m_lTargetX *= 100;
				m_lTargetY *= 100;

				m_bUseWarp = true;

				if (pkWarp->IsGoto())
				{
					LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pkWarp->GetMapIndex());
					m_lTargetX += pkSectreeMap->m_setting.iBaseX;
					m_lTargetY += pkSectreeMap->m_setting.iBaseY;
					m_bUseWarp = false;
				}
			}

			bool Valid()
			{
				return !m_bInvalid;
			}

			void operator () (LPENTITY ent)
			{
				if (!Valid())
					return;

				if (!ent->IsType(ENTITY_CHARACTER))
					return;

				LPCHARACTER pkChr = (LPCHARACTER) ent;

				if (!pkChr->IsPC())
					return;

				int iDist = DISTANCE_APPROX(pkChr->GetX() - m_lX, pkChr->GetY() - m_lY);

				if (iDist > 300)
					return;

				if (m_bEmpire && pkChr->GetEmpire() && m_bEmpire != pkChr->GetEmpire())
					return;

				if (pkChr->IsHack())
					return;

				if (!pkChr->CanHandleItem(false, true))
					return;

				if (m_bUseWarp)
					pkChr->WarpSet(m_lTargetX, m_lTargetY);
				else
				{
					pkChr->Show(pkChr->GetMapIndex(), m_lTargetX, m_lTargetY);
					pkChr->Stop();
				}
			}

			bool m_bInvalid;
			bool m_bUseWarp;

			long m_lX;
			long m_lY;
			long m_lTargetX;
			long m_lTargetY;

			BYTE m_bEmpire;
	};
}

EVENTFUNC(warp_npc_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "warp_npc_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (!ch->GetSectree())
	{
		ch->m_pkWarpNPCEvent = NULL;
		return 0;
	}

	FuncCheckWarp f(ch);
	if (f.Valid())
		ch->GetSectree()->ForEachAround(f);

	return passes_per_sec / 2;
}


void CHARACTER::StartWarpNPCEvent()
{
	if (m_pkWarpNPCEvent)
		return;

	if (!IsWarp() && !IsGoto())
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkWarpNPCEvent = event_create(warp_npc_event, info, passes_per_sec / 2);
}

void CHARACTER::SyncPacket()
{
	TEMP_BUFFER buf;

	TPacketCGSyncPositionElement elem;

	elem.dwVID = GetVID();
	elem.lX = GetX();
	elem.lY = GetY();

	TPacketGCSyncPosition pack;

	pack.bHeader = HEADER_GC_SYNC_POSITION;
	pack.wSize = sizeof(TPacketGCSyncPosition) + sizeof(elem);

	buf.write(&pack, sizeof(pack));
	buf.write(&elem, sizeof(elem));

	PacketAround(buf.read_peek(), buf.size());
}

LPCHARACTER CHARACTER::GetMarryPartner() const
{
	return m_pkChrMarried;
}

void CHARACTER::SetMarryPartner(LPCHARACTER ch)
{
	m_pkChrMarried = ch;
}

int CHARACTER::GetMarriageBonus(DWORD dwItemVnum, bool bSum)
{
	if (IsNPC())
		return 0;

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());

	if (!pMarriage)
		return 0;

	return pMarriage->GetBonus(dwItemVnum, bSum, this);
}

void CHARACTER::ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID)
{
	if (!IsPC())
		return;

	TPacketGCQuestConfirm p;

	p.header = HEADER_GC_QUEST_CONFIRM;
	p.requestPID = dwRequestPID;
	p.timeout = iTimeout;
	strlcpy(p.msg, szMsg, sizeof(p.msg));

	GetDesc()->Packet(&p, sizeof(p));
}

int CHARACTER::GetPremiumRemainSeconds(BYTE bType) const
{
	if (bType >= PREMIUM_MAX_NUM)
		return 0;

	return m_aiPremiumTimes[bType] - get_global_time();
}

bool CHARACTER::WarpToPID(DWORD dwPID)
{
	LPCHARACTER victim;
	if ((victim = (CHARACTER_MANAGER::instance().FindByPID(dwPID))))
	{
		int mapIdx = victim->GetMapIndex();
		if (IS_SUMMONABLE_ZONE(mapIdx))
		{
			if (CAN_ENTER_ZONE(this, mapIdx))
			{
				WarpSet(victim->GetX(), victim->GetY());
			}
			else
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't teleport to the player."));
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't teleport to the player."));
			return false;
		}
	}
	else
	{
		CCI * pcci = P2P_MANAGER::instance().FindByPID(dwPID);

		if (!pcci)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The player is not online."));
			return false;
		}

		if (pcci->bChannel != g_bChannel)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The player is in channel %d. You are in channel %d."), pcci->bChannel, g_bChannel);
			return false;
		}
		else if (false == IS_SUMMONABLE_ZONE(pcci->lMapIndex))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't teleport to the player."));
			return false;
		}
		else
		{
			if (!CAN_ENTER_ZONE(this, pcci->lMapIndex))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't teleport to the player."));
				return false;
			}

			TPacketGGFindPosition p;
			p.header = HEADER_GG_FIND_POSITION;
			p.dwFromPID = GetPlayerID();
			p.dwTargetPID = dwPID;
			pcci->pkDesc->Packet(&p, sizeof(TPacketGGFindPosition));

			if (test_server)
				ChatPacket(CHAT_TYPE_PARTY, "sent find position packet for teleport");
		}
	}
	return true;
}

// ADD_REFINE_BUILDING
CGuild* CHARACTER::GetRefineGuild() const
{
	LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);

	return (chRefineNPC ? chRefineNPC->GetGuild() : NULL);
}

bool CHARACTER::IsRefineThroughGuild() const
{
	return GetRefineGuild() != NULL;
}

int CHARACTER::ComputeRefineFee(int iCost, int iMultiply) const
{
	CGuild* pGuild = GetRefineGuild();
	if (pGuild)
	{
		if (pGuild == GetGuild())
			return iCost * iMultiply * 9 / 10;

		LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
		if (chRefineNPC && chRefineNPC->GetEmpire() != GetEmpire())
			return iCost * iMultiply * 3;

		return iCost * iMultiply;
	}
	else
		return iCost;
}

void CHARACTER::PayRefineFee(int iTotalMoney)
{
	int iFee = iTotalMoney / 10;
	CGuild* pGuild = GetRefineGuild();

	int iRemain = iTotalMoney;

	if (pGuild)
	{
		if (pGuild != GetGuild())
		{
			pGuild->RequestDepositMoney(this, iFee);
			iRemain -= iFee;
		}
	}

	PointChange(POINT_GOLD, -iRemain);
}
// END_OF_ADD_REFINE_BUILDING

bool CHARACTER::IsHack(bool bSendMsg, bool bCheckShopOwner, int limittime)
{
	const int iPulse = thecore_pulse();

	if (test_server)
		bSendMsg = true;

	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Depoyu kullandiktan sonra isinlanamazsin."), limittime);

		if (test_server)
			ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(limittime));
		return true;
	}
	if (bCheckShopOwner)
	{
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
		{
			if (bSendMsg)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ticaret sonrasi isinlanamazsin."));

			return true;
		}
	}
	else
	{
		if (GetExchange() || GetMyShop() || IsOpenSafebox() || IsCubeOpen())
		{
			if (bSendMsg)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ticaretten bir sure sonra isinlanamazsin"));

			return true;
		}
	}

	//PREVENT_PORTAL_AFTER_EXCHANGE
	if (iPulse - GetExchangeTime()  < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ticaretten sonra 10 saniye beklemen gerek."), limittime );
		return true;
	}
	//END_PREVENT_PORTAL_AFTER_EXCHANGE

	//PREVENT_ITEM_COPY
	if (iPulse - GetMyShopTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ticaretten sonra 10 saniye beklemen gerek."), limittime);
		return true;
	}

#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
	if (iPulse - GetMyOfflineShopTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ticaretten sonra 10 saniye beklemen gerek."), limittime);
		return true;
	}
#endif
#ifdef ENABLE_LUCKY_DRAW
	if (iPulse - GetLuckyDrawJoinTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, "Cekilise katildiktan sonra 10 saniye beklemen gerek.", limittime);
		return true;
	}
#endif
	if (iPulse - GetRefineTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ticaretten sonra 10 saniye beklemen gerek."), limittime);
		return true;
	}
	//END_PREVENT_ITEM_COPY

	return false;
}

BOOL CHARACTER::IsMonarch() const
{
	//MONARCH_LIMIT
	if (CMonarch::instance().IsMonarch(GetPlayerID(), GetEmpire()))
		return true;

	return false;

	//END_MONARCH_LIMIT
}
void CHARACTER::Say(const std::string & s)
{
	struct ::packet_script packet_script;

	packet_script.header = HEADER_GC_SCRIPT;
	packet_script.skin = 1;
	packet_script.src_size = s.size();
	packet_script.size = packet_script.src_size + sizeof(struct packet_script);

	TEMP_BUFFER buf;

	buf.write(&packet_script, sizeof(struct packet_script));
	buf.write(&s[0], s.size());

	if (IsPC())
	{
		GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}

//
// Monarch
//
void CHARACTER::InitMC()
{
	for (int n = 0; n < MI_MAX; ++n)
	{
		m_dwMonarchCooltime[n] = thecore_pulse();
	}

	m_dwMonarchCooltimelimit[MI_HEAL] = PASSES_PER_SEC(MC_HEAL);
	m_dwMonarchCooltimelimit[MI_WARP] = PASSES_PER_SEC(MC_WARP);
	m_dwMonarchCooltimelimit[MI_TRANSFER] = PASSES_PER_SEC(MC_TRANSFER);
	m_dwMonarchCooltimelimit[MI_TAX] = PASSES_PER_SEC(MC_TAX);
	m_dwMonarchCooltimelimit[MI_SUMMON] = PASSES_PER_SEC(MC_SUMMON);

	m_dwMonarchCooltime[MI_HEAL] -= PASSES_PER_SEC(GetMCL(MI_HEAL));
	m_dwMonarchCooltime[MI_WARP] -= PASSES_PER_SEC(GetMCL(MI_WARP));
	m_dwMonarchCooltime[MI_TRANSFER] -= PASSES_PER_SEC(GetMCL(MI_TRANSFER));
	m_dwMonarchCooltime[MI_TAX] -= PASSES_PER_SEC(GetMCL(MI_TAX));
	m_dwMonarchCooltime[MI_SUMMON] -= PASSES_PER_SEC(GetMCL(MI_SUMMON));
}

DWORD CHARACTER::GetMC(enum MONARCH_INDEX e) const
{
	return m_dwMonarchCooltime[e];
}

void CHARACTER::SetMC(enum MONARCH_INDEX e)
{
	m_dwMonarchCooltime[e] = thecore_pulse();
}

bool CHARACTER::IsMCOK(enum MONARCH_INDEX e) const
{
	int iPulse = thecore_pulse();

	if ((iPulse -  GetMC(e)) <  GetMCL(e))
	{
		if (test_server)
			sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

		return false;
	}

	if (test_server)
		sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

	return true;
}

DWORD CHARACTER::GetMCL(enum MONARCH_INDEX e) const
{
	return m_dwMonarchCooltimelimit[e];
}

DWORD CHARACTER::GetMCLTime(enum MONARCH_INDEX e) const
{
	int iPulse = thecore_pulse();

	if (test_server)
		sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

	return  (GetMCL(e)) / passes_per_sec   -  (iPulse - GetMC(e)) / passes_per_sec;
}

bool CHARACTER::IsSiegeNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

//------------------------------------------------
void CHARACTER::UpdateDepositPulse()
{
	m_deposit_pulse = thecore_pulse() + PASSES_PER_SEC(60*5);
}

bool CHARACTER::CanDeposit() const
{
	return (m_deposit_pulse == 0 || (m_deposit_pulse < thecore_pulse()));
}
//------------------------------------------------

ESex GET_SEX(LPCHARACTER ch)
{
	switch (ch->GetRaceNum())
	{
		case MAIN_RACE_WARRIOR_M:
		case MAIN_RACE_SURA_M:
		case MAIN_RACE_ASSASSIN_M:
		case MAIN_RACE_SHAMAN_M:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case MAIN_RACE_WOLFMAN_M:
#endif
			return SEX_MALE;

		case MAIN_RACE_ASSASSIN_W:
		case MAIN_RACE_SHAMAN_W:
		case MAIN_RACE_WARRIOR_W:
		case MAIN_RACE_SURA_W:
			return SEX_FEMALE;
	}

	/* default sex = male */
	return SEX_MALE;
}

int CHARACTER::GetHPPct() const
{
	if (GetMaxHP() <= 0) // @fixme136
		return 0;
	return (GetHP() * 100) / GetMaxHP();
}

bool CHARACTER::IsBerserk() const
{
	if (m_pkMobInst != NULL)
		return m_pkMobInst->m_IsBerserk;
	else
		return false;
}

void CHARACTER::SetBerserk(bool mode)
{
	if (m_pkMobInst != NULL)
		m_pkMobInst->m_IsBerserk = mode;
}

bool CHARACTER::IsGodSpeed() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsGodSpeed;
	}
	else
	{
		return false;
	}
}

void CHARACTER::SetGodSpeed(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsGodSpeed = mode;

		if (mode == true)
		{
			SetPoint(POINT_ATT_SPEED, 250);
		}
		else
		{
			SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		}
	}
}

bool CHARACTER::IsDeathBlow() const
{
	if (number(1, 100) <= m_pkMobData->m_table.bDeathBlowPoint)
	{
		return true;
	}
	else
	{
		return false;
	}
}

struct FFindReviver
{
	FFindReviver()
	{
		pChar = NULL;
		HasReviver = false;
	}

	void operator() (LPCHARACTER ch)
	{
		if (ch->IsMonster() != true)
		{
			return;
		}

		if (ch->IsReviver() == true && pChar != ch && ch->IsDead() != true)
		{
			if (number(1, 100) <= ch->GetMobTable().bRevivePoint)
			{
				HasReviver = true;
				pChar = ch;
			}
		}
	}

	LPCHARACTER pChar;
	bool HasReviver;
};

bool CHARACTER::HasReviverInParty() const
{
	LPPARTY party = GetParty();

	if (party != NULL)
	{
		if (party->GetMemberCount() == 1) return false;

		FFindReviver f;
		party->ForEachMemberPtr(f);
		return f.HasReviver;
	}

	return false;
}

bool CHARACTER::IsRevive() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsRevive;
	}

	return false;
}

void CHARACTER::SetRevive(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsRevive = mode;
	}
}

#define IS_SPEED_HACK_PLAYER(ch) (ch->m_speed_hack_count > SPEEDHACK_LIMIT_COUNT)

EVENTFUNC(check_speedhack_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (NULL == ch || ch->IsNPC())
		return 0;

	if (IS_SPEED_HACK_PLAYER(ch))
	{
		// write hack log
		LogManager::instance().SpeedHackLog(ch->GetPlayerID(), ch->GetX(), ch->GetY(), ch->m_speed_hack_count);

		if (g_bEnableSpeedHackCrash)
		{
			// close connection
			LPDESC desc = ch->GetDesc();

			if (desc)
			{
				DESC_MANAGER::instance().DestroyDesc(desc);
				return 0;
			}
		}
	}

	ch->m_speed_hack_count = 0;

	ch->ResetComboHackCount();
	return PASSES_PER_SEC(60);
}

void CHARACTER::StartCheckSpeedHackEvent()
{
	if (m_pkCheckSpeedHackEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkCheckSpeedHackEvent = event_create(check_speedhack_event, info, PASSES_PER_SEC(60));
}

void CHARACTER::GoHome()
{
	WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));
}

void CHARACTER::SendGuildName(CGuild* pGuild)
{
	if (NULL == pGuild) return;

	DESC	*desc = GetDesc();

	if (NULL == desc) return;
	if (m_known_guild.find(pGuild->GetID()) != m_known_guild.end()) return;

	m_known_guild.insert(pGuild->GetID());

	TPacketGCGuildName	pack;
	memset(&pack, 0x00, sizeof(pack));

	pack.header		= HEADER_GC_GUILD;
	pack.subheader	= GUILD_SUBHEADER_GC_GUILD_NAME;
	pack.size		= sizeof(TPacketGCGuildName);
	pack.guildID	= pGuild->GetID();
	memcpy(pack.guildName, pGuild->GetName(), GUILD_NAME_MAX_LEN);

	desc->Packet(&pack, sizeof(pack));
}

void CHARACTER::SendGuildName(DWORD dwGuildID)
{
	SendGuildName(CGuildManager::instance().FindGuild(dwGuildID));
}

EVENTFUNC(destroy_when_idle_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "destroy_when_idle_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (ch->GetVictim())
	{
		return PASSES_PER_SEC(300);
	}

	sys_log(1, "DESTROY_WHEN_IDLE: %s", ch->GetName());

	ch->m_pkDestroyWhenIdleEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::StartDestroyWhenIdleEvent()
{
	if (m_pkDestroyWhenIdleEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkDestroyWhenIdleEvent = event_create(destroy_when_idle_event, info, PASSES_PER_SEC(300));
}

void CHARACTER::SetComboSequence(BYTE seq)
{
	m_bComboSequence = seq;
}

BYTE CHARACTER::GetComboSequence() const
{
	return m_bComboSequence;
}

void CHARACTER::SetLastComboTime(DWORD time)
{
	m_dwLastComboTime = time;
}

DWORD CHARACTER::GetLastComboTime() const
{
	return m_dwLastComboTime;
}

void CHARACTER::SetValidComboInterval(int interval)
{
	m_iValidComboInterval = interval;
}

int CHARACTER::GetValidComboInterval() const
{
	return m_iValidComboInterval;
}

BYTE CHARACTER::GetComboIndex() const
{
	return m_bComboIndex;
}

void CHARACTER::IncreaseComboHackCount(int k)
{
	m_iComboHackCount += k;

	if (m_iComboHackCount >= 10)
	{
		if (GetDesc())
			if (GetDesc()->DelayedDisconnect(number(2, 7)))
			{
				sys_log(0, "COMBO_HACK_DISCONNECT: %s count: %d", GetName(), m_iComboHackCount);
				LogManager::instance().HackLog("Combo", this);
			}
	}
}

void CHARACTER::ResetComboHackCount()
{
	m_iComboHackCount = 0;
}

void CHARACTER::SkipComboAttackByTime(int interval)
{
	m_dwSkipComboAttackByTime = get_dword_time() + interval;
}

DWORD CHARACTER::GetSkipComboAttackByTime() const
{
	return m_dwSkipComboAttackByTime;
}

void CHARACTER::ResetChatCounter()
{
	m_bChatCounter = 0;
}

BYTE CHARACTER::IncreaseChatCounter()
{
	return ++m_bChatCounter;
}

BYTE CHARACTER::GetChatCounter() const
{
	return m_bChatCounter;
}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
BYTE CHARACTER::GetMountCounter() const
{
	return m_bMountCounter;
}

void CHARACTER::ResetMountCounter()
{
	m_bMountCounter = 0;
}

BYTE CHARACTER::IncreaseMountCounter()
{
	return ++m_bMountCounter;
}
#endif

bool CHARACTER::IsRiding() const
{
	return IsHorseRiding() || GetMountVnum();
}

bool CHARACTER::CanWarp() const
{
	const int iPulse = thecore_pulse();
	const int limit_time = PASSES_PER_SEC(g_nPortalLimitTime);

	if ((iPulse - GetSafeboxLoadTime()) < limit_time)
		return false;

	if ((iPulse - GetExchangeTime()) < limit_time)
		return false;

	if ((iPulse - GetMyShopTime()) < limit_time)
		return false;

#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
	if ((iPulse - GetMyOfflineShopTime()) < limit_time)
		return false;
#endif

	if ((iPulse - GetRefineTime()) < limit_time)
		return false;

	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
		return false;

	return true;
}

DWORD CHARACTER::GetNextExp() const
{
	if (PLAYER_MAX_LEVEL_CONST < GetLevel())
		return 2500000000u;
	else
		return exp_table[GetLevel()];
}

int	CHARACTER::GetSkillPowerByLevel(int level, bool bMob) const
{
	return CTableBySkill::instance().GetSkillPowerByLevelFromType(GetJob(), GetSkillGroup(), MINMAX(0, level, SKILL_MAX_LEVEL), bMob);
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
std::vector<LPITEM> CHARACTER::GetAcceMaterials()
{
	return std::vector<LPITEM>{ITEM_MANAGER::instance().Find(m_pointsInstant.pAcceMaterials[0].id), ITEM_MANAGER::instance().Find(m_pointsInstant.pAcceMaterials[1].id)};
}

const TItemPosEx* CHARACTER::GetAcceMaterialsInfo()
{
	return m_pointsInstant.pAcceMaterials;
}

void CHARACTER::SetAcceMaterial(int pos, LPITEM ptr)
{
	if (pos < 0 || pos >= ACCE_WINDOW_MAX_MATERIALS)
		return;
	if (!ptr)
		m_pointsInstant.pAcceMaterials[pos] = {};
	else
	{
		m_pointsInstant.pAcceMaterials[pos].id = ptr->GetID();
		m_pointsInstant.pAcceMaterials[pos].pos.cell = ptr->GetCell();
		m_pointsInstant.pAcceMaterials[pos].pos.window_type = ptr->GetWindow();
	}
}

void CHARACTER::OpenAcce(bool bCombination)
{
	if (IsAcceOpened(bCombination))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The acce window it's already opened."));
		return;
	}

	if (bCombination)
	{
		if (m_bAcceAbsorption)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Before you may close the acce absorption window."));
			return;
		}

		m_bAcceCombination = true;
	}
	else
	{
		if (m_bAcceCombination)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Before you may close the acce combine window."));
			return;
		}

		m_bAcceAbsorption = true;
	}

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_OPEN;
	sPacket.bWindow = bCombination;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));

	ClearAcceMaterials();
}

void CHARACTER::CloseAcce()
{
	if ((!m_bAcceCombination) && (!m_bAcceAbsorption))
		return;

	bool bWindow = m_bAcceCombination;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_CLOSE;
	sPacket.bWindow = bWindow;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));

	if (bWindow)
		m_bAcceCombination = false;
	else
		m_bAcceAbsorption = false;

	ClearAcceMaterials();
}

void CHARACTER::ClearAcceMaterials()
{
	auto pkItemMaterial = GetAcceMaterials();
	for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			continue;

		pkItemMaterial[i]->Lock(false);
		pkItemMaterial[i] = NULL;
		SetAcceMaterial(i, nullptr);
	}
}

bool CHARACTER::AcceIsSameGrade(long lGrade)
{
	auto pkItemMaterial = GetAcceMaterials();
	if (!pkItemMaterial[0])
		return false;
	return pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD) == lGrade;
}

DWORD CHARACTER::GetAcceCombinePrice(long lGrade)
{
	DWORD dwPrice = 0;
	switch (lGrade)
	{
	case 2:
	{
		dwPrice = ACCE_GRADE_2_PRICE;
	}
	break;
	case 3:
	{
		dwPrice = ACCE_GRADE_3_PRICE;
	}
	break;
	case 4:
	{
		dwPrice = ACCE_GRADE_4_PRICE;
	}
	break;
	default:
	{
		dwPrice = ACCE_GRADE_1_PRICE;
	}
	break;
	}

	return dwPrice;
}

BYTE CHARACTER::CheckEmptyMaterialSlot()
{
	auto pkItemMaterial = GetAcceMaterials();
	for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			return i;
	}

	return 255;
}

void CHARACTER::GetAcceCombineResult(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs)
{
	auto pkItemMaterial = GetAcceMaterials();

	if (m_bAcceCombination)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			long lVal = pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD);
			if (lVal == 4)
			{
				dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
				dwMinAbs = pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET);
				DWORD dwMaxAbsCalc = (dwMinAbs + ACCE_GRADE_4_ABS_RANGE > ACCE_GRADE_4_ABS_MAX ? ACCE_GRADE_4_ABS_MAX : (dwMinAbs + ACCE_GRADE_4_ABS_RANGE));
				dwMaxAbs = dwMaxAbsCalc;
			}
			else
			{
				DWORD dwMaskVnum = pkItemMaterial[0]->GetOriginalVnum();
				TItemTable * pTable = ITEM_MANAGER::instance().GetTable(dwMaskVnum + 1);
				if (pTable)
					dwMaskVnum += 1;

				dwItemVnum = dwMaskVnum;
				switch (lVal)
				{
				case 2:
				{
					dwMinAbs = ACCE_GRADE_3_ABS;
					dwMaxAbs = ACCE_GRADE_3_ABS;
				}
				break;
				case 3:
				{
					dwMinAbs = ACCE_GRADE_4_ABS_MIN;
					dwMaxAbs = ACCE_GRADE_4_ABS_MAX_COMB;
				}
				break;
				default:
				{
					dwMinAbs = ACCE_GRADE_2_ABS;
					dwMaxAbs = ACCE_GRADE_2_ABS;
				}
				break;
				}
			}
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
	else
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
			dwMinAbs = pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET);
			dwMaxAbs = dwMinAbs;
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
}

void CHARACTER::AddAcceMaterial(TItemPos tPos, BYTE bPos)
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
	{
		if (bPos == 255)
		{
			bPos = CheckEmptyMaterialSlot();
			if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
				return;
		}
		else
			return;
	}

	LPITEM pkItem = GetItem(tPos);
	if (!pkItem)
		return;
	else if ((pkItem->GetCell() >= INVENTORY_MAX_NUM) || (pkItem->IsEquipped()) || (tPos.IsBeltInventoryPosition()) || (pkItem->IsDragonSoul()))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (m_bAcceCombination))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (bPos == 0) && (m_bAcceAbsorption))
		return;
	else if (pkItem->isLocked())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't add locked items."));
		return;
	}
#ifdef __SOULBINDING_SYSTEM__
	else if ((pkItem->IsBind()) || (pkItem->IsUntilBind()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't add binded items."));
		return;
	}
#endif
	else if ((m_bAcceCombination) && (bPos == 1) && (!AcceIsSameGrade(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD))))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can combine just accees of same grade."));
		return;
	}
	else if ((m_bAcceCombination) && (pkItem->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_GRADE_4_ABS_MAX))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This acce got already maximum absorption chance."));
		return;
	}
	else if ((bPos == 1) && (m_bAcceAbsorption))
	{
		if ((pkItem->GetType() != ITEM_WEAPON) && (pkItem->GetType() != ITEM_ARMOR))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can absorb just the bonuses from armors and weapons."));
			return;
		}
		else if ((pkItem->GetType() == ITEM_ARMOR) && (pkItem->GetSubType() != ARMOR_BODY))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can absorb just the bonuses from armors and weapons."));
			return;
		}
	}
	else if ((pkItem->GetSubType() != COSTUME_ACCE) && (m_bAcceCombination))
		return;
	else if ((pkItem->GetSubType() != COSTUME_ACCE) && (bPos == 0) && (m_bAcceAbsorption))
		return;
	else if ((pkItem->GetSocket(ACCE_ABSORBED_SOCKET) > 0) && (bPos == 0) && (m_bAcceAbsorption))
		return;

	auto pkItemMaterial = GetAcceMaterials();
	if ((bPos == 1) && (!pkItemMaterial[0]))
		return;

	if (pkItemMaterial[bPos])
		return;

	SetAcceMaterial(bPos, pkItem);
	pkItemMaterial[bPos] = pkItem;
	pkItemMaterial[bPos]->Lock(true);

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAcceCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_ADDED;
	sPacket.bWindow = m_bAcceCombination;
	sPacket.dwPrice = GetAcceCombinePrice(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD));
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = dwItemVnum;
	sPacket.dwMinAbs = dwMinAbs;
	sPacket.dwMaxAbs = dwMaxAbs;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
}

void CHARACTER::RemoveAcceMaterial(BYTE bPos)
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
		return;

	auto pkItemMaterial = GetAcceMaterials();

	DWORD dwPrice = 0;

	if (bPos == 1)
	{
		if (pkItemMaterial[bPos])
		{
			pkItemMaterial[bPos]->Lock(false);
			pkItemMaterial[bPos] = NULL;
			SetAcceMaterial(bPos, nullptr);
		}

		if (pkItemMaterial[0])
			dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));
	}
	else
		ClearAcceMaterials();

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_REMOVED;
	sPacket.bWindow = m_bAcceCombination;
	sPacket.dwPrice = dwPrice;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
}

BYTE CHARACTER::CanRefineAcceMaterials()
{
	BYTE bReturn = 0;
	if (!GetDesc())
		return bReturn;

	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
		return bReturn;

	auto materialInfo = GetAcceMaterialsInfo();
	auto pkItemMaterial = GetAcceMaterials();
	if (!pkItemMaterial[0] || !pkItemMaterial[1])
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial null");
		return bReturn;
	}
	else if (pkItemMaterial[0]->GetOwner()!=this || pkItemMaterial[1]->GetOwner() != this)
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial different ownership");
		return bReturn;
	}
	else if (pkItemMaterial[0]->IsEquipped() || pkItemMaterial[1]->IsEquipped())
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial equipped");
		return bReturn;
	}
	else if (pkItemMaterial[0]->GetWindow() != INVENTORY || pkItemMaterial[1]->GetWindow() != INVENTORY)
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial not in INVENTORY");
		return bReturn;
	}
	else if (!materialInfo[0].id || !materialInfo[1].id)
	{
		sys_err("CanRefineAcceMaterials: materialInfo id 0");
		return bReturn;
	}
	else if (materialInfo[0].pos.cell != pkItemMaterial[0]->GetCell() || materialInfo[1].pos.cell != pkItemMaterial[1]->GetCell())
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial wrong cell");
		return bReturn;
	}
	else if (materialInfo[0].pos.window_type != pkItemMaterial[0]->GetWindow() || materialInfo[1].pos.window_type != pkItemMaterial[1]->GetWindow())
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial wrong window_type");
		return bReturn;
	}

	if (m_bAcceCombination)
	{
		if (!AcceIsSameGrade(pkItemMaterial[1]->GetValue(ACCE_GRADE_VALUE_FIELD)))
		{
			sys_err("CanRefineAcceMaterials: pkItemMaterial different acce grade");
			return bReturn;
		}

		for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
		{
			if (pkItemMaterial[i])
			{
				if ((pkItemMaterial[i]->GetType() == ITEM_COSTUME) && (pkItemMaterial[i]->GetSubType() == COSTUME_ACCE))
					bReturn = 1;
				else
				{
					bReturn = 0;
					break;
				}
			}
			else
			{
				bReturn = 0;
				break;
			}
		}
	}
	else if (m_bAcceAbsorption)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			if ((pkItemMaterial[0]->GetType() == ITEM_COSTUME) && (pkItemMaterial[0]->GetSubType() == COSTUME_ACCE))
				bReturn = 2;
			else
				bReturn = 0;

			if ((pkItemMaterial[1]->GetType() == ITEM_WEAPON) || ((pkItemMaterial[1]->GetType() == ITEM_ARMOR) && (pkItemMaterial[1]->GetSubType() == ARMOR_BODY)))
				bReturn = 2;
			else
				bReturn = 0;

			if (pkItemMaterial[0]->GetSocket(ACCE_ABSORBED_SOCKET) > 0)
				bReturn = 0;
		}
		else
			bReturn = 0;
	}

	return bReturn;
}

void CHARACTER::RefineAcceMaterials()
{
	if ((GetExchange() || IsOpenSafebox() || GetShopOwner()) || IsCubeOpen()
#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
		|| GetOfflineShopOwner()
#endif
		)
	{
		CloseAcce();
		return;
	}
	BYTE bCan = CanRefineAcceMaterials();
	if (bCan == 0)
		return;

	auto pkItemMaterial = GetAcceMaterials();

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAcceCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);
	DWORD dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));

	if (bCan == 1)
	{
		int iSuccessChance = 0;
		long lVal = pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD);
		switch (lVal)
		{
		case 2:
		{
			iSuccessChance = ACCE_COMBINE_GRADE_2;
		}
		break;
		case 3:
		{
			iSuccessChance = ACCE_COMBINE_GRADE_3;
		}
		break;
		case 4:
		{
			iSuccessChance = ACCE_COMBINE_GRADE_4;
		}
		break;
		default:
		{
			iSuccessChance = ACCE_COMBINE_GRADE_1;
		}
		break;
		}

		if (GetGold() < dwPrice)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough Yang."));
			return;
		}

		int iChance = number(1, 100);
		bool bSucces = (iChance <= iSuccessChance ? true : false);
		if (bSucces)
		{
			LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(dwItemVnum, 1, 0, false);
			if (!pkItem)
			{
				sys_err("%d can't be created.", dwItemVnum);
				return;
			}

			ITEM_MANAGER::CopyAllAttrTo(pkItemMaterial[0], pkItem);
			LogManager::instance().ItemLog(this, pkItem, "COMBINE SUCCESS", pkItem->GetName());
			DWORD dwAbs = (dwMinAbs == dwMaxAbs ? dwMinAbs : number(dwMinAbs + 1, dwMaxAbs));
			pkItem->SetSocket(ACCE_ABSORPTION_SOCKET, dwAbs);
			pkItem->SetSocket(ACCE_ABSORBED_SOCKET, pkItemMaterial[0]->GetSocket(ACCE_ABSORBED_SOCKET));

			PointChange(POINT_GOLD, -dwPrice);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);

			WORD wCell = pkItemMaterial[0]->GetCell();
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[0], "COMBINE (REFINE SUCCESS)");
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE SUCCESS)");

			pkItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
			pkItem->AttrLog();

			if (lVal == 4)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("New absorption rate: %d%"), dwAbs);
			else
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Success."));

			EffectPacket(SE_EFFECT_ACCE_SUCCEDED);
			LogManager::instance().AcceLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, pkItem->GetID(), 1, dwAbs, 1);

			ClearAcceMaterials();
		}
		else
		{
			PointChange(POINT_GOLD, -dwPrice);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);

			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE FAIL)");

			if (lVal == 4)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("New absorption rate: %d%"), pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET));
			else
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Failed."));

			LogManager::instance().AcceLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, 0, 0, 0, 0);

			pkItemMaterial[1] = NULL;
		}

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAcce sPacket;
		sPacket.header = HEADER_GC_ACCE;
		sPacket.subheader = ACCE_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bAcceCombination;
		sPacket.dwPrice = dwPrice;
		sPacket.bPos = 0;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		if (bSucces)
			sPacket.dwMaxAbs = 100;
		else
			sPacket.dwMaxAbs = 0;

		GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
	}
	else
	{
		pkItemMaterial[1]->CopyAttributeTo(pkItemMaterial[0]);
		LogManager::instance().ItemLog(this, pkItemMaterial[0], "ABSORB (REFINE SUCCESS)", pkItemMaterial[0]->GetName());
		pkItemMaterial[0]->SetSocket(ACCE_ABSORBED_SOCKET, pkItemMaterial[1]->GetOriginalVnum());
		#ifdef USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (pkItemMaterial[0]->GetAttributeValue(i) < 0)
				pkItemMaterial[0]->SetForceAttribute(i, pkItemMaterial[0]->GetAttributeType(i), 0);
		}
		#endif
		ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "ABSORBED (REFINE SUCCESS)");

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemMaterial[0]);
		pkItemMaterial[0]->AttrLog();

		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Success."));
		EffectPacket(SE_EFFECT_ACCE_SUCCEDED);

		ClearAcceMaterials();

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAcce sPacket;
		sPacket.header = HEADER_GC_ACCE;
		sPacket.subheader = ACCE_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bAcceCombination;
		sPacket.dwPrice = dwPrice;
		sPacket.bPos = 255;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		sPacket.dwMaxAbs = 1;
		GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
	}
}

bool CHARACTER::CleanAcceAttr(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Close the other windows."));
		return false;
	}
	else if ((!pkItem) || (!pkTarget))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("pkItem or pkTarget nullptr."));
		return false;
	}
	else if ((pkTarget->GetType() != ITEM_COSTUME) && (pkTarget->GetSubType() != COSTUME_ACCE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use it on non-sash items."));
		return false;
	}

	if (pkTarget->GetSocket(ACCE_ABSORBED_SOCKET) <= 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The sash item has no absorbed socket."));
		return false;
	}

	pkTarget->SetSocket(ACCE_ABSORBED_SOCKET, 0);
	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		pkTarget->SetForceAttribute(i, 0, 0);

	LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT (CLEAN ATTR)", pkTarget->GetName());
	return true;
}
#endif

#ifndef ENABLE_EXTEND_INVEN_SYSTEM
bool CHARACTER::Update_Inven()
{
	if (Inven_Point() >= INVENTORY_LOCK_COVER_COUNT) return false;
	std::vector<int> needkeys;
	for (int i = INVENTORY_NEED_KEY_START; i <= (INVENTORY_LOCKED_PAGE_COUNT*INVENTORY_NEED_KEY_INCREASE)+1; i++)
		for (int j = 0; j < 3; j++)
			needkeys.push_back(i);
	int needkey = needkeys[Inven_Point()];
	int keycount = CountSpecifyItem(INVENTORY_OPEN_KEY_VNUM) + CountSpecifyItem(INVENTORY_OPEN_KEY_VNUM2);
	if (keycount >= needkey) {
		int willdelete = INVENTORY_START_DELETE_VNUM;
		while (needkey) {
			if (!CountSpecifyItem(willdelete))
				willdelete = INVENTORY_START_DELETE_VNUM == INVENTORY_OPEN_KEY_VNUM ? INVENTORY_OPEN_KEY_VNUM2 : INVENTORY_OPEN_KEY_VNUM;
			RemoveSpecifyItem(willdelete);
			needkey--;
		}
	}	
	else {
		ChatPacket(CHAT_TYPE_COMMAND, "ExInvenItemUseMsg %d", needkey - keycount);
		return false;
	}
	PointChange(POINT_INVEN, 1, false);
	return true;
}
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
void CHARACTER::MountSummon(LPITEM mountItem)
{
	if (GetMapIndex() == 113)
		return;

	if (CWarMapManager::instance().IsWarMap(GetMapIndex()))
		return;

	CMountSystem* mountSystem = GetMountSystem();
	DWORD mobVnum = 0;

	if (!mountSystem || !mountItem)
		return;

	if (mountItem->FindApplyValue(APPLY_MOUNT) != 0)
		mobVnum = mountItem->FindApplyValue(APPLY_MOUNT);

	if (IsHorseRiding())
		StopRiding();

	if (GetHorse())
		HorseSummon(false);

	mountSystem->Summon(mobVnum, mountItem, false);
}

void CHARACTER::MountUnsummon(LPITEM mountItem)
{
	CMountSystem* mountSystem = GetMountSystem();
	DWORD mobVnum = 0;

	if (!mountSystem || !mountItem)
		return;

	if (mountItem->FindApplyValue(APPLY_MOUNT) != 0)
		mobVnum = mountItem->FindApplyValue(APPLY_MOUNT);

	if (GetMountVnum() == mobVnum)
		mountSystem->Unmount(mobVnum);

	mountSystem->Unsummon(mobVnum);
}

void CHARACTER::CheckMount()
{
	if (GetMapIndex() == 113)
		return;

	if (CWarMapManager::instance().IsWarMap(GetMapIndex()))
		return;

	CMountSystem* mountSystem = GetMountSystem();
	LPITEM mountItem = GetWear(WEAR_COSTUME_MOUNT);
	DWORD mobVnum = 0;

	if (!mountSystem || !mountItem)
		return;

	if (mountItem->FindApplyValue(APPLY_MOUNT) != 0)
		mobVnum = mountItem->FindApplyValue(APPLY_MOUNT);

	if (mountSystem->CountSummoned() == 0)
	{
		mountSystem->Summon(mobVnum, mountItem, false);
	}
}

bool CHARACTER::IsRidingMount()
{
	return (GetWear(WEAR_COSTUME_MOUNT) || FindAffect(AFFECT_MOUNT));
}
#endif
//martysama0134's 2022

#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
void CHARACTER::SetOfflineShop(LPOFFLINESHOP pkOfflineShop)
{
	m_pkOfflineShop = pkOfflineShop;
	if (m_pkOfflineShop)
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_OFFLINE_SHOP);
	else {
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_OFFLINE_SHOP);
		SetOfflineShopOwner(NULL);
	}
}
struct checkShopPosition
{
	LPCHARACTER myCH;
	bool isValidPos = true;
	checkShopPosition(LPCHARACTER ch) { myCH = ch; }
	void operator()(LPENTITY ent) {
		if (ent->IsType(ENTITY_CHARACTER)) {
			LPCHARACTER ch = (LPCHARACTER)ent;
			if (!isValidPos) { return; }
			if (ch->GetRaceNum() >= 30000 && ch->GetRaceNum() <= 30008) {
				if (DISTANCE_APPROX(ch->GetX() - myCH->GetX(), ch->GetY() - myCH->GetY()) < 250) { isValidPos = false; }
			}
		}
	}
};
void CHARACTER::OpenMyOfflineShop(const char* c_pszSign, TOfflineShopItemTable* pTable, BYTE bItemCount, BYTE bNpcType, BYTE bBoardStyle)
{
	if (thecore_pulse() - GetMyOfflineShopTime() < PASSES_PER_SEC(10)) {
		ChatPacket(CHAT_TYPE_INFO, "Lutfen 10 saniye bekle.");
		return;
	}

	SetMyOfflineShopTime();
	quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
	if (pPC && pPC->IsRunning()) return;
	if (bItemCount == 0) return;
	if (GetShop() || GetOfflineShop()) return;

	{
		auto pMsg(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM offline_shop_item%s WHERE owner_id = %u and status = 1", get_table_postfix(), GetPlayerID()));
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

		BYTE bResult = 0;
		str_to_number(bResult, row[0]);

		if (bResult) {
			ChatPacket(CHAT_TYPE_INFO, "Bir sorun olustu, lutfen DC den bizimle iletisime gec.");
			return;
		}
	}

	if (HasOfflineShop() == true) {
		ChatPacket(CHAT_TYPE_INFO, "Gazi2 Offline Pazarini tercih ettiginiz icin tesekkur ederiz.");
		return;
	}

	if (COfflineShopManager::instance().GetShopCountChannel() >= 1000) {
		ChatPacket(CHAT_TYPE_INFO, "Maksimum pazar limiti doldu. Lutfen baska kanalda pazar ac.");
		return;
	}

	if (GetLevel() < OFFLINE_SHOP_NEED_LEVEL) {
		ChatPacket(CHAT_TYPE_INFO, "Pazar olusturabilmek icin en az %d seviye olmalisiniz.", OFFLINE_SHOP_NEED_LEVEL);
		return;
	}

	LPSECTREE sectree = nullptr;
	sectree = GetSectree();
	if ((sectree = GetSectree())) {
		checkShopPosition fKontrol(this);
		sectree->ForEachAround(fKontrol);
		if (!fKontrol.isValidPos) { ChatPacket(CHAT_TYPE_INFO, "Bir baska pazarin dibine yeni bir pazar olusturamazsin. Lutfen biraz uzaklas."); return; }
	}

	char szSign[SHOP_SIGN_MAX_LEN + 1];
	strlcpy(szSign, c_pszSign, sizeof(szSign));

	if (strlen(c_pszSign) == 0) {
		ChatPacket(CHAT_TYPE_INFO, "Pazar ismi cok uzun. Lutfen 30 karakteri gecme.");
		return;
	}

	if (strstr(szSign, "%") || strstr(szSign, "'")) { ChatPacket(CHAT_TYPE_INFO, "Lutfen ozel karakter kullanma."); return; }

	m_stOfflineShopSign = szSign;
	if (CBanwordManager::instance().CheckString(m_stOfflineShopSign.c_str(), m_stOfflineShopSign.length())) { ChatPacket(CHAT_TYPE_INFO, "Lutfen ozel karakter kullanma."); return; }

	std::map<DWORD, DWORD> itemkind;
	std::set<TItemPos> cont;

	for (BYTE i = 0; i < bItemCount; ++i) {
		if (cont.find((pTable + i)->pos) != cont.end()) {
			sys_err("MY_OFFLINE_SHOP: duplicate shop item detected! (name: %s)", GetName());
			return;
		}

		LPITEM pkItem = GetItem((pTable + i)->pos);

		if (pkItem) {
			const TItemTable* item_table = pkItem->GetProto();
			if (item_table && (IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP))) {
				ChatPacket(CHAT_TYPE_INFO, "Bu esya pazara eklenemez");
				return;
			}

			if (pkItem->IsEquipped() == true) {
				ChatPacket(CHAT_TYPE_INFO, "Giyili esya pazara eklenemez.");
				return;
			}

			if (true == pkItem->isLocked()) {
				ChatPacket(CHAT_TYPE_INFO, "Kilitli esya pazara eklenemez.");
				return;
			}

#ifdef ENABLE_SOULBIND_SYSTEM 
			if (pkItem->IsSealed()){
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Can't trade with sealed item."));
				return;
			}
#endif

		}
	}

	const uint8_t openDay = 14;
	const int32_t open_time = (openDay * 60 * 60 * 24);
	int32_t npc_vnum = 30000;
	if (bNpcType != 0 && bNpcType <= 7) npc_vnum += (bNpcType + 1);
	if (bBoardStyle < 0 || bBoardStyle > 5) bBoardStyle = 0;
	LPCHARACTER npc = CHARACTER_MANAGER::instance().SpawnMob(npc_vnum, GetMapIndex(), GetX(), GetY(), GetZ(), false, -1, false, true, GetPlayerID());

	if (!npc) {
		ChatPacket(CHAT_TYPE_INFO, "[LS:3581]");
		sys_err("SpawnMob: Unkown NPC! (name: %s)", GetName());
		return;
	}

	LPOFFLINESHOP pkOfflineShop = COfflineShopManager::instance().CreateOfflineShop(npc, GetPlayerID(), pTable, bItemCount, GetMapIndex(), open_time, szSign, bBoardStyle);
	if (!pkOfflineShop) {
		ChatPacket(CHAT_TYPE_INFO, "[LS:3582]");
		sys_err("CreateOfflineShop: Unable to create offline shop! (name: %s)", GetName());
		return;
	}

	npc->SetOfflineShopTimer(open_time);
	npc->StartOfflineShopUpdateEvent();

	char szInsertQuery[512];
	snprintf(szInsertQuery, sizeof(szInsertQuery), "INSERT INTO offline_shop_npc%s (owner_id, sign, name, time, x, y, z, mapIndex, channel, npc_vnum, border_style) VALUES(%u, '%s', '%s', %d, %ld, %ld, %ld, %ld, %d, %d, %d)", get_table_postfix(), GetPlayerID(), szSign, GetName(), open_time, GetX(), GetY(), GetZ(), GetMapIndex(), g_bChannel, npc_vnum, bBoardStyle);
	DBManager::Instance().DirectQuery(szInsertQuery);

	npc->SetOfflineShop(pkOfflineShop);
	npc->SetOfflineShopChannel(g_bChannel);

	char szName[CHARACTER_NAME_MAX_LEN + 1];
	snprintf(szName, sizeof(szName), "%s", GetName());
	npc->SetName(szName);

	pkOfflineShop->SetShopSign(szSign);
	SetOfflineShopVID(npc->GetVID());
	npc->Show(GetMapIndex(), GetX(), GetY(), GetZ(), true);
	if (m_pkExchange) m_pkExchange->Cancel();
	CloseSafebox();
	TPacketGCShopSign p;
	p.bHeader = HEADER_GC_OFFLINE_SHOP_SIGN;
	p.dwVID = npc->GetVID();
	p.bBorderStyle = bBoardStyle;
	strlcpy(p.szSign, c_pszSign, sizeof(p.szSign));

	PacketAround(&p, sizeof(TPacketGCShopSign));
	//ChatPacket(CHAT_TYPE_INFO, "[LS:3583|%s]", writeTime(open_time).c_str());
	npc->ViewReencode();
	UpdatePacket();
}

EVENTFUNC(offline_shop_update_event)
{
	char_event_info* pInfo = dynamic_cast<char_event_info*>(event->info);
	if (pInfo == nullptr) { sys_err("offline_shop_update_event> <Factor> Null pointer"); return 0; }
	LPCHARACTER npc = pInfo->ch;
	if (npc == nullptr) { return 0; }

	if (false) {
	deleteShop:
		LPCHARACTER pChar = CHARACTER_MANAGER::instance().Find(npc->GetOfflineShopRealOwner());
		if (pChar) {
			pChar->ChatPacket(CHAT_TYPE_INFO, "[LS:3584]");
			COfflineShopManager::Instance().DestroyOfflineShop(pChar, npc->GetVID());
		}
		else
			COfflineShopManager::Instance().DestroyOfflineShop(nullptr, npc->GetVID());

		if (pChar) { pChar->SetOfflineShop(nullptr); pChar->SetOfflineShopOwner(nullptr); }
		M2_DESTROY_CHARACTER(npc);
		return 0;
	}

	const int32_t kalanSure = npc->GetOfflineShopTimer() - processing_time / passes_per_sec;
	if (kalanSure <= 0) { goto deleteShop; }
	// After realtimes check, check item count
	LPOFFLINESHOP pkOfflineShop = npc->GetOfflineShop();
	if (pkOfflineShop->GetLeftItemCount(npc->GetOfflineShopRealOwner()) <= 0) { goto deleteShop; }

	npc->SetOfflineShopTimer(kalanSure);
	pkOfflineShop->SetOfflineShopTime(kalanSure);
	npc->SetOfflineShopSaveTime(npc->GetOfflineShopSaveTime() + 1);
	DBManager::instance().DirectQuery("UPDATE %soffline_shop_npc SET time = %d WHERE owner_id = %u", get_table_postfix(), npc->GetOfflineShopTimer(), npc->GetOfflineShopRealOwner());
	return PASSES_PER_SEC(MIN(60, kalanSure));
}

void CHARACTER::StartOfflineShopUpdateEvent()
{
	if (m_pkOfflineShopUpdateEvent) return;
	if (IsPC() || IsMonster()) return;
	char_event_info* pInfo = AllocEventInfo<char_event_info>();
	pInfo->ch = this;
	m_pkOfflineShopUpdateEvent = event_create(offline_shop_update_event, pInfo, PASSES_PER_SEC(60));
}

void CHARACTER::StopOfflineShopUpdateEvent()
{
	m_pointsInstant.bSaveTime = 0;
	m_pointsInstant.leftTime = 0;
	event_cancel(&m_pkOfflineShopUpdateEvent);
}

void CHARACTER::SetOfflineShopSign(const char* c_szSign)
{
	char szSign[SHOP_SIGN_MAX_LEN + 1];
	strlcpy(szSign, c_szSign, sizeof(szSign));
	m_stOfflineShopSign = szSign;
}

void CHARACTER::DestroyOfflineShop()
{
	COfflineShopManager::instance().DestroyOfflineShop(nullptr, GetVID(), false);
}

bool CHARACTER::HasOfflineShop()
{
	auto pMsg(DBManager::instance().DirectQuery("SELECT owner_id FROM offline_shop_npc%s WHERE owner_id = %u", get_table_postfix(), GetPlayerID()));
	if (!pMsg || (pMsg && pMsg->Get()->uiNumRows == 0)) return false;
	return true;
}

BYTE CHARACTER::GetOfflineShopChannel()
{
	if (!HasOfflineShop()) return 0;
	auto pMsg(DBManager::instance().DirectQuery("SELECT channel FROM offline_shop_npc%s WHERE owner_id = %u", get_table_postfix(), GetPlayerID()));
	if (!pMsg || (pMsg && pMsg->Get()->uiNumRows == 0)) return 0;
	BYTE channel = 0;
	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	str_to_number(channel, row[0]);
	return channel;
}

void CHARACTER::OfflineShopPanelPacket(BYTE hasOfflineShop, const char* shopName, int32_t lMapIndex, BYTE shopChannel, int32_t iTime, DWORD displayedCount)
{
	if (!GetDesc()) return;
	TPacketGCOpenOffShopPanel packet;
	packet.bHeader = HEADER_GC_OFFLINE_SHOP_OPEN_PANEL;
	packet.hasOfflineShop = hasOfflineShop;
	packet.lMapIndex = (lMapIndex == -1 ? 0 : lMapIndex);
	packet.shopChannel = shopChannel;
	packet.iTime = (iTime == -1 ? 0 : iTime);
	packet.isPremium = static_cast<BYTE>(FindAffect(AFFECT_OFFLINESHOP_PREMIUM) ? 1 : 0);
	//packet.displayedCount = 0;
	strlcpy(packet.shopName, shopName, sizeof(packet.shopName));
	GetDesc()->Packet(&packet, sizeof(packet));
}

void CHARACTER::OpenOfflineShopPanel()
{
	quest::PC* pPC = quest::CQuestManager::Instance().GetPC(GetPlayerID());
	if (pPC->IsRunning()) return;
	if (HaveAnotherPagesOpen()) { ChatPacket(CHAT_TYPE_INFO, "Acik olan pencereleri kapat ve tekrar dene."); return; }
	BYTE shopChannel = GetOfflineShopChannel();
	if (shopChannel == 0) {
		COfflineShopManager::instance().RefreshUnsoldItems(this);
		OfflineShopPanelPacket(0);
		return;
	}
	DWORD dwOwnerPID = GetPlayerID();
	bool canGetOfflineShop = COfflineShopManager::instance().CanGetOfflineShop(dwOwnerPID);
	if (g_bChannel != shopChannel || !canGetOfflineShop) {
		TPacketGGOpenOffShopPanel searchShopData;
		searchShopData.bHeader = HEADER_GG_OFFLINE_SHOP_OPEN_PANEL;
		searchShopData.bSubHeader = OFFLINE_SHOP_PANEL_SEARCH_SHOP;
		searchShopData.shopChannel = shopChannel;
		searchShopData.dwOwnerPID = dwOwnerPID;
		P2P_MANAGER::instance().Send(&searchShopData, sizeof(TPacketGGOpenOffShopPanel));
	}
	else {
		COfflineShopManager::instance().Refresh(this);
		int32_t shop_map_index = COfflineShopManager::instance().GetMapIndex(dwOwnerPID);
		int32_t shop_left_time = COfflineShopManager::instance().GetLeftTime(dwOwnerPID);
		const char* shopName = COfflineShopManager::instance().GetOfflineShopSign(dwOwnerPID);
		DWORD displayedCount = 0;
		OfflineShopPanelPacket(static_cast<BYTE>((bool)shopChannel), shopName, shop_map_index, shopChannel, shop_left_time, displayedCount);
	}
}

void CHARACTER::TeleportToMyOfflineShop()
{
	quest::PC* pPC = quest::CQuestManager::Instance().GetPC(GetPlayerID());
	if (pPC->IsRunning()) return;
	if (IsOpenSafebox() || GetShop() || IsCubeOpen() || IsDead() || GetExchange() || GetOfflineShop() || GetMyShop()) {
		ChatPacket(CHAT_TYPE_INFO, "[LS:3586]");
		return;
	}
	BYTE shopChannel = GetOfflineShopChannel();
	if (shopChannel == 0) return;
	bool canGetOfflineShop = COfflineShopManager::instance().CanGetOfflineShop(GetPlayerID());
	if (g_bChannel != shopChannel || !canGetOfflineShop) {
		if (!CanWarp()) {
			ChatPacket(CHAT_TYPE_INFO, "[LS:3588]");
			return;
		}
		int32_t mapIdx, x, y;
		if (!COfflineShopManager::Instance().GetOfflineShopCoordsP2P(this, &mapIdx, &x, &y)) return;
		if (!GetDesc()) return;
	}
	else {
		if (!CanWarp()) {
			ChatPacket(CHAT_TYPE_INFO, "[LS:3588]");
			return;
		}

		int32_t mapIdx, x, y;
		if (!COfflineShopManager::Instance().GetOfflineShopCoords(GetPlayerID(), &mapIdx, &x, &y)) return;
		WarpSet(x, y, mapIdx);
	}
}

void CHARACTER::ChangeShopSign(DWORD shopVid, const char* newShopName, uint8_t borderStyle)
{
	TPacketGCShopSign p;
	p.bHeader = HEADER_GC_OFFLINE_CHANGE_SHOP_SIGN;
	p.dwVID = shopVid;
	strlcpy(p.szSign, newShopName, sizeof(p.szSign));
	p.bBorderStyle = borderStyle;
	PacketAround(&p, sizeof(TPacketGCShopSign));
}
#endif

#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
void CHARACTER::OpenPrivateShopSearch(DWORD dwVnum)
{
	if (GetDesc() == NULL)
		return;
	
	if (GetPrivateShopSearchState() != SHOP_SEARCH_OFF)
		return;
		
	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_CLOSE_TABS"));
		return;
	}

	TPacketGCPrivateShopSearchOpen p;
	p.header = HEADER_GC_PRIVATE_SHOP_SEARCH_OPEN;
	GetDesc()->Packet(&p, sizeof(p));

	bPrivateShopSearchState = (dwVnum == PRIVATE_SHOP_SEARCH_LOOKING_GLASS) ? SHOP_SEARCH_LOOKING : SHOP_SEARCH_TRADING;
}
#endif

bool CHARACTER::HaveAnotherPagesOpen()
{
	if (IsOpenSafebox()
		|| GetShop()
		|| IsCubeOpen()
		|| IsDead()
		|| GetExchange()
#ifdef __OFFLINE_PRIVATE_SHOP_SYSTEM__
		|| GetOfflineShop()
#endif
		|| GetMyShop()
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		|| IsAcceOpened()
#endif
		)
		return true;
	return false;
}

#if defined(__BL_MOVE_CHANNEL__)
EVENTINFO(move_channel_info)
{
	LPCHARACTER		ch;
	int				left_second;
	long			lAddr;
	WORD			wPort;

	move_channel_info(const LPCHARACTER m_ch, const int m_sec, const long m_Addr, const WORD m_Port) :
		ch(m_ch),
		left_second(m_sec),
		lAddr(m_Addr),
		wPort(m_Port)
	{}
};

EVENTFUNC(move_channel_event)
{
	move_channel_info* info = dynamic_cast<move_channel_info*>(event->info);

	if (info == nullptr)
	{
		sys_err("<move_channel_event> <Factor> Null pointer");
		return 0;
	}

	const LPCHARACTER ch = info->ch;
	if (ch == nullptr)
		return 0;

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = nullptr;
		if (ch->CanWarp())
			ch->WarpSet(ch->GetX(), ch->GetY(), 0, info->lAddr, info->wPort);
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "Kanal degistirmekten vazgectiniz.");
		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d saniye icerisinde kanal degistiriyorsunuz."), info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

void CHARACTER::MoveChannel(const TRespondMoveChannel* p)
{
	if (p == nullptr)
		return;
	
	const LPDESC d = GetDesc();
	if (d == nullptr)
		return;

	if (p->wPort == 0 
		|| p->lAddr == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, "Su an kanal degistiremezsiniz.");
		return;
	}

	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?? ?????."));
		event_cancel(&m_pkTimedEvent);
		return;
	}

	ChatPacket(CHAT_TYPE_INFO, "Lutfen bekleyin. Kanal degistiriyorsunuz.");
	if (IsHack(false, true, 10))
		return;

	m_pkTimedEvent = event_create(move_channel_event, 
		new move_channel_info(this, IsPosition(POS_FIGHTING) ? 10 : 3, p->lAddr, p->wPort), 1);
}
#endif

#ifdef ITEM_SHOP
bool CHARACTER::UpdateCoins(DWORD dwAID, long val) 
{
	if (val == 0) { sys_err("Value is empty"); return false; }
	if (!dwAID) { sys_err("dwAID = 0, returned"); return false; }

	char szQuery[256 + 1];
	long lValue = abs(val);

	if (val > 0){ sprintf(szQuery, "UPDATE account.account SET cash = cash + '%ld' WHERE id = '%u'", lValue, dwAID); }
	else{ sprintf(szQuery, "UPDATE account.account SET cash = cash - '%ld' WHERE id = '%u'", lValue, dwAID); }

	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));

	if (msg->uiSQLErrno != 0) {
		sys_err("UpdateCoins Query Failed, Error code: %ld", msg->uiSQLErrno);
		return false;}

	sys_log(0, "NAME: %s Coins Updated, Amount: %d", GetName(), val);
	return true;
}
DWORD CHARACTER::GetCoins() 
{
	if (!GetDesc()) { return 0; }
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT cash FROM account.account WHERE id = '%u'", GetDesc()->GetAccountTable().id));

	if (msg->uiSQLErrno != 0) {
		sys_err("GetCoins Query Failed, Error code: %ld", msg->uiSQLErrno);
		return 0;}
	if (!msg->Get()->uiNumRows) {
		sys_err("GetCoins Query Failed, Rows couldn't get");
		return 0;}

	MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);
	const char * cMyCoins = row[0];

	long lMyCoins = 0;
	str_to_number(lMyCoins, cMyCoins);

	return lMyCoins;
}
bool CHARACTER::UpdateVoteCoins(DWORD dwAID, long val)
{
	if (val == 0) { sys_err("Value is empty"); return false; }
	if (!dwAID) { sys_err("dwAID = 0, returned"); return false; }

	char szQuery[256 + 1];
	long lValue = abs(val);

	if (val > 0) { sprintf(szQuery, "UPDATE account.account SET mileage = mileage + '%ld' WHERE id = '%u'", lValue, dwAID); }
	else { sprintf(szQuery, "UPDATE account.account SET mileage = mileage - '%ld' WHERE id = '%u'", lValue, dwAID); }

	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));

	if (msg->uiSQLErrno != 0) {
		sys_err("UpdateCoins Query Failed, Error code: %ld", msg->uiSQLErrno);
		return false;
	}

	sys_log(0, "NAME: %s Coins Updated, Amount: %d", GetName(), val);
	return true;
}
DWORD CHARACTER::GetVoteCoins()
{
	if (!GetDesc()) { return 0; }
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT mileage FROM account.account WHERE id = '%u'", GetDesc()->GetAccountTable().id));

	if (msg->uiSQLErrno != 0) {
		sys_err("GetCoins Query Failed, Error code: %ld", msg->uiSQLErrno);
		return 0;
	}
	if (!msg->Get()->uiNumRows) {
		sys_err("GetCoins Query Failed, Rows couldn't get");
		return 0;
	}

	MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);
	const char * cMyCoins = row[0];

	long lMyCoins = 0;
	str_to_number(lMyCoins, cMyCoins);

	return lMyCoins;
}
#endif

#ifdef ENABLE_EXTENDED_BATTLE_PASS
void CHARACTER::SetLastReciveExtBattlePassInfoTime(DWORD time)
{
	m_dwLastReciveExtBattlePassInfoTime = time;
}

void CHARACTER::SetLastReciveExtBattlePassOpenRanking(DWORD time)
{
	m_dwLastExtBattlePassOpenRankingTime = time;
}

void CHARACTER::LoadExtBattlePass(DWORD dwCount, TPlayerExtBattlePassMission* data)
{
	m_bIsLoadedExtBattlePass = false;

	for (int i = 0; i < dwCount; ++i, ++data)
	{
		TPlayerExtBattlePassMission* newMission = new TPlayerExtBattlePassMission;
		newMission->dwPlayerId = data->dwPlayerId;
		newMission->dwBattlePassType = data->dwBattlePassType;
		newMission->dwMissionIndex = data->dwMissionIndex;
		newMission->dwMissionType = data->dwMissionType;
		newMission->dwBattlePassId = data->dwBattlePassId;
		newMission->dwExtraInfo = data->dwExtraInfo;
		newMission->bCompleted = data->bCompleted;
		newMission->bIsUpdated = data->bIsUpdated;

		m_listExtBattlePass.push_back(newMission);
	}

	m_bIsLoadedExtBattlePass = true;
}

DWORD CHARACTER::GetExtBattlePassMissionProgress(DWORD dwBattlePassType, BYTE bMissionIndex, BYTE bMissionType)
{
	DWORD BattlePassID;
	if (dwBattlePassType == 1)
		BattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
	else if (dwBattlePassType == 2)
		BattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
	else if (dwBattlePassType == 3)
		BattlePassID = CBattlePassManager::instance().GetEventBattlePassID();
	else {
		sys_err("Unknown BattlePassType (%d)", dwBattlePassType);
		return 0;
	}
	
	ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
	while (it != m_listExtBattlePass.end())
	{
		TPlayerExtBattlePassMission* pkMission = *it++;
		if (pkMission->dwBattlePassType == dwBattlePassType && pkMission->dwMissionIndex == bMissionIndex && pkMission->dwMissionType == bMissionType && pkMission->dwBattlePassId == BattlePassID)
			return pkMission->dwExtraInfo;
	}
	return 0;
}

bool CHARACTER::IsExtBattlePassCompletedMission(DWORD dwBattlePassType, BYTE bMissionIndex, BYTE bMissionType)
{
	DWORD BattlePassID;
	if (dwBattlePassType == 1)
		BattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
	else if (dwBattlePassType == 2)
		BattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
	else if (dwBattlePassType == 3)
		BattlePassID = CBattlePassManager::instance().GetEventBattlePassID();
	else {
		sys_err("Unknown BattlePassType (%d)", dwBattlePassType);
		return false;
	}
	ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
	while (it != m_listExtBattlePass.end())
	{
		TPlayerExtBattlePassMission* pkMission = *it++;
		if (pkMission->dwBattlePassType == dwBattlePassType && pkMission->dwMissionIndex == bMissionIndex && pkMission->dwMissionType == bMissionType && pkMission->dwBattlePassId == BattlePassID)
			return (pkMission->bCompleted ? true : false);
	}
	return false;
}

bool CHARACTER::CompleteNextExtBattlePassMission(BYTE bBattlePassType, DWORD dwMissionIndex)
{
	if (!GetDesc())
		return false;

	if (!m_bIsLoadedExtBattlePass)
		return false;

	BYTE bBattlePassID;
	if (bBattlePassType == 1)
		bBattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
	else if (bBattlePassType == 2)
		bBattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
	else if (bBattlePassType == 3)
		bBattlePassID = CBattlePassManager::instance().GetEventBattlePassID();
	else {
		sys_err("Unknown BattlePassType (%d)", bBattlePassType);
		return false;
	}

	bool missionFound = false;
	

	for (TPlayerExtBattlePassMission* pkMission : m_listExtBattlePass)
	{
		if (pkMission->dwBattlePassType == bBattlePassType &&
			pkMission->dwBattlePassId == bBattlePassID &&
			pkMission->dwMissionIndex == dwMissionIndex)
		{
			DWORD dwFirstInfo, dwSecondInfo;
			if (CBattlePassManager::instance().BattlePassMissionGetInfo(bBattlePassType, dwMissionIndex, bBattlePassID, pkMission->dwMissionType, &dwFirstInfo, &dwSecondInfo))
			{
				if (pkMission->dwExtraInfo >= dwSecondInfo) {
					return false;
				}
				pkMission->bIsUpdated = true;
				pkMission->bCompleted = true;
				pkMission->dwExtraInfo = dwSecondInfo;

				std::string stMissionName = CBattlePassManager::instance().GetMissionNameByType(pkMission->dwMissionType);
				std::string stBattlePassName = CBattlePassManager::instance().GetNormalBattlePassNameByID(pkMission->dwBattlePassId);

				CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, pkMission->dwBattlePassId, pkMission->dwMissionIndex);
				if (bBattlePassType == 1) {
					EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_NORMAL_MISSION"));
				}
				else if (bBattlePassType == 2) {
					EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_PREMIUM_MISSION"));
				}
				else if (bBattlePassType == 3) {
					EffectPacket(SE_EFFECT_BP_EVENT_MISSION_COMPLETED);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_EVENT_MISSION"));
				}

				TPacketGCExtBattlePassMissionUpdate packet;
				packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
				packet.bBattlePassType = bBattlePassType;
				packet.bMissionIndex = pkMission->dwMissionIndex;
				packet.dwNewProgress = pkMission->dwExtraInfo;
				GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
				missionFound = true;
				break;
			}
		}
	}


	if (!missionFound)
	{
		TPlayerExtBattlePassMission* newMission = new TPlayerExtBattlePassMission;
		newMission->dwPlayerId = GetPlayerID();
		newMission->dwBattlePassType = bBattlePassType;
		newMission->dwMissionType = KILL_MONSTER;
		newMission->dwBattlePassId = bBattlePassID;

		newMission->dwMissionIndex = dwMissionIndex;
		// newMission->dwExtraInfo = 1;
		// newMission->bCompleted = 0;
		// newMission->bIsUpdated = 1;
		
		
		// ChatPacket(1, "Kayit olusturuldu");


		DWORD dwFirstInfo, dwSecondInfo;
		if (CBattlePassManager::instance().BattlePassMissionGetInfo(bBattlePassType, dwMissionIndex, bBattlePassID, newMission->dwMissionType, &dwFirstInfo, &dwSecondInfo))
		{
			// if (newMission->dwExtraInfo >= dwSecondInfo) {
				// return false;
			// }
			newMission->bIsUpdated = true;
			newMission->bCompleted = true;
			newMission->dwExtraInfo = dwSecondInfo;

			std::string stMissionName = CBattlePassManager::instance().GetMissionNameByType(newMission->dwMissionType);
			std::string stBattlePassName = CBattlePassManager::instance().GetNormalBattlePassNameByID(newMission->dwBattlePassId);

			CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, newMission->dwBattlePassId, newMission->dwMissionIndex);
			if (bBattlePassType == 1) {
				EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_NORMAL_MISSION"));
			}
			else if (bBattlePassType == 2) {
				EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_PREMIUM_MISSION"));
			}
			else if (bBattlePassType == 3) {
				EffectPacket(SE_EFFECT_BP_EVENT_MISSION_COMPLETED);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_EVENT_MISSION"));
			}

			m_listExtBattlePass.push_back(newMission);
			TPacketGCExtBattlePassMissionUpdate packet;
			packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
			packet.bBattlePassType = bBattlePassType;
			packet.bMissionIndex = newMission->dwMissionIndex;
			packet.dwNewProgress = newMission->dwExtraInfo;
			GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
		}

	}

	return true;
}

bool CHARACTER::IsExtBattlePassRegistered(BYTE bBattlePassType, DWORD dwBattlePassID)
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT * FROM player.battlepass_playerindex WHERE player_id = %d and battlepass_type = %d and battlepass_id = %d", GetPlayerID(), bBattlePassType, dwBattlePassID));
	if (pMsg->Get()->uiNumRows != 0)
		return true;

	return false;
}

void CHARACTER::UpdateExtBattlePassMissionProgress(DWORD dwMissionType, DWORD dwUpdateValue, DWORD dwCondition, bool isOverride)
{
	if (!GetDesc())
		return;

	if (!m_bIsLoadedExtBattlePass)
		return;

	DWORD dwSafeCondition = dwCondition;
	for (BYTE bBattlePassType = 1; bBattlePassType <= 3 ; ++bBattlePassType)
	{
		bool foundMission = false;
		DWORD dwSaveProgress = 0;
		dwCondition = dwSafeCondition;
		
		BYTE bBattlePassID;
		BYTE bMissionIndex = CBattlePassManager::instance().GetMissionIndex(bBattlePassType, dwMissionType, dwCondition);

		if (bBattlePassType == 1)
			bBattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
		if (bBattlePassType == 2){
			bBattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
			if (bBattlePassID != GetExtBattlePassPremiumID())
				continue;
		}
		if (bBattlePassType == 3)
			bBattlePassID = CBattlePassManager::instance().GetEventBattlePassID();

		DWORD dwFirstInfo, dwSecondInfo;
		if (CBattlePassManager::instance().BattlePassMissionGetInfo(bBattlePassType, bMissionIndex, bBattlePassID, dwMissionType, &dwFirstInfo, &dwSecondInfo))
		{
			if (dwFirstInfo == 0)
				dwCondition = 0;
			
			if (dwMissionType == 2 and dwFirstInfo <= dwCondition or dwMissionType == 4 and dwFirstInfo <= dwCondition or dwMissionType == 20 and dwFirstInfo <= dwCondition)
				dwCondition = dwFirstInfo;

			if (dwFirstInfo == dwCondition && GetExtBattlePassMissionProgress(bBattlePassType, bMissionIndex, dwMissionType) < dwSecondInfo)
			{
				ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
				while (it != m_listExtBattlePass.end())
				{
					TPlayerExtBattlePassMission* pkMission = *it++;

					if (pkMission->dwBattlePassType == bBattlePassType && pkMission->dwMissionIndex == bMissionIndex && pkMission->dwMissionType == dwMissionType && pkMission->dwBattlePassId == bBattlePassID)
					{
						pkMission->bIsUpdated = 1;

						if (isOverride)
							pkMission->dwExtraInfo = dwUpdateValue;
						else
							pkMission->dwExtraInfo += dwUpdateValue;

						if (pkMission->dwExtraInfo >= dwSecondInfo)
						{
							pkMission->dwExtraInfo = dwSecondInfo;
							pkMission->bCompleted = 1;

							std::string stMissionName = CBattlePassManager::instance().GetMissionNameByType(pkMission->dwMissionType);
							std::string stBattlePassName = CBattlePassManager::instance().GetNormalBattlePassNameByID(pkMission->dwBattlePassId);

							CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, bBattlePassID, bMissionIndex);
							if (bBattlePassType == 1) {
								EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_NORMAL_MISSION"));
							}
							if (bBattlePassType == 2) {
								EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_PREMIUM_MISSION"));
							}
							if (bBattlePassType == 3) {
								EffectPacket(SE_EFFECT_BP_EVENT_MISSION_COMPLETED);
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_EVENT_MISSION"));
							}
							
							TPacketGCExtBattlePassMissionUpdate packet;
							packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
							packet.bBattlePassType = bBattlePassType;
							packet.bMissionIndex = bMissionIndex;
							packet.dwNewProgress = pkMission->dwExtraInfo;
							GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
						}

						dwSaveProgress = pkMission->dwExtraInfo;
						foundMission = true;

						if (pkMission->bCompleted != 1) {
							TPacketGCExtBattlePassMissionUpdate packet;
							packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
							packet.bBattlePassType = bBattlePassType;
							packet.bMissionIndex = bMissionIndex;
							packet.dwNewProgress = dwSaveProgress;
							GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
						}
						break;
					}
					
				}

				if (!foundMission)
				{
					if (!IsExtBattlePassRegistered(bBattlePassType, bBattlePassID))
						DBManager::instance().DirectQuery("INSERT INTO player.battlepass_playerindex SET player_id=%d, player_name='%s', battlepass_type=%d, battlepass_id=%d, start_time=NOW()", GetPlayerID(), GetName(), bBattlePassType, bBattlePassID);
					
					TPlayerExtBattlePassMission* newMission = new TPlayerExtBattlePassMission;
					newMission->dwPlayerId = GetPlayerID();
					newMission->dwBattlePassType = bBattlePassType;
					newMission->dwMissionType = dwMissionType;
					newMission->dwBattlePassId = bBattlePassID;

					if (dwUpdateValue >= dwSecondInfo)
					{
						newMission->dwMissionIndex = CBattlePassManager::instance().GetMissionIndex(bBattlePassType, dwMissionType, dwCondition);
						newMission->dwExtraInfo = dwSecondInfo;
						newMission->bCompleted = 1;

						CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, bBattlePassID, bMissionIndex);
						if (bBattlePassType == 1) {
							EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_NORMAL_MISSION"));
						}
						if (bBattlePassType == 2) {
							EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_PREMIUM_MISSION"));
						}
						if (bBattlePassType == 3) {
							EffectPacket(SE_EFFECT_BP_EVENT_MISSION_COMPLETED);
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_EVENT_MISSION"));
						}

						dwSaveProgress = dwSecondInfo;
					}
					else
					{
						newMission->dwMissionIndex = CBattlePassManager::instance().GetMissionIndex(bBattlePassType, dwMissionType, dwCondition);
						newMission->dwExtraInfo = dwUpdateValue;
						newMission->bCompleted = 0;

						dwSaveProgress = dwUpdateValue;
					}

					newMission->bIsUpdated = 1;

					m_listExtBattlePass.push_back(newMission);

					TPacketGCExtBattlePassMissionUpdate packet;
					packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
					packet.bBattlePassType = bBattlePassType;
					packet.bMissionIndex = bMissionIndex;
					packet.dwNewProgress = dwSaveProgress;
					GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
				}
			}
		}
	}
}

void CHARACTER::SetExtBattlePassMissionProgress(BYTE bBattlePassType, DWORD dwMissionIndex, DWORD dwMissionType, DWORD dwUpdateValue)
{
	if (!GetDesc())
		return;

	if (!m_bIsLoadedExtBattlePass)
		return;

	bool foundMission = false;
	DWORD dwSaveProgress = 0;
	
	BYTE bBattlePassID;
	if (bBattlePassType == 1)
		bBattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
	else if (bBattlePassType == 2)
		bBattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
	else if (bBattlePassType == 3)
		bBattlePassID = CBattlePassManager::instance().GetEventBattlePassID();
	else {
		sys_err("Unknown BattlePassType (%d)", bBattlePassType);
		return;
	}
	
	DWORD dwFirstInfo, dwSecondInfo;
	if (CBattlePassManager::instance().BattlePassMissionGetInfo(bBattlePassType, dwMissionIndex, bBattlePassID, dwMissionType, &dwFirstInfo, &dwSecondInfo))
	{
		ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
		while (it != m_listExtBattlePass.end())
		{
			TPlayerExtBattlePassMission* pkMission = *it++;

			if (pkMission->dwBattlePassType == bBattlePassType && pkMission->dwMissionIndex == dwMissionIndex && pkMission->dwMissionType == dwMissionType && pkMission->dwBattlePassId == bBattlePassID)
			{
				pkMission->bIsUpdated = 1;
				pkMission->bCompleted = 0;
				
				pkMission->dwExtraInfo = dwUpdateValue;

				if (pkMission->dwExtraInfo >= dwSecondInfo)
				{
					pkMission->dwExtraInfo = dwSecondInfo;
					pkMission->bCompleted = 1;

					std::string stMissionName = CBattlePassManager::instance().GetMissionNameByType(pkMission->dwMissionType);
					std::string stBattlePassName = CBattlePassManager::instance().GetNormalBattlePassNameByID(pkMission->dwBattlePassId);
					//ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("New Value : %d"), pkMission->dwExtraInfo);

					CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, bBattlePassID, dwMissionIndex);
					if (bBattlePassType == 1) {
						EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_NORMAL_MISSION"));
					}
					if (bBattlePassType == 2) {
						EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_PREMIUM_MISSION"));
					}
					if (bBattlePassType == 3) {
						EffectPacket(SE_EFFECT_BP_EVENT_MISSION_COMPLETED);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_EVENT_MISSION"));
					}
					
					TPacketGCExtBattlePassMissionUpdate packet;
					packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
					packet.bBattlePassType = bBattlePassType;
					packet.bMissionIndex = dwMissionIndex;
					packet.dwNewProgress = pkMission->dwExtraInfo;
					GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
				}

				dwSaveProgress = pkMission->dwExtraInfo;
				foundMission = true;

				if (pkMission->bCompleted != 1) {
					TPacketGCExtBattlePassMissionUpdate packet;
					packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
					packet.bBattlePassType = bBattlePassType;
					packet.bMissionIndex = dwMissionIndex;
					packet.dwNewProgress = dwSaveProgress;
					GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
				}
				break;
			}
		}

		if (!foundMission)
		{
			if (!IsExtBattlePassRegistered(bBattlePassType, bBattlePassID))
				DBManager::instance().DirectQuery("INSERT INTO player.battlepass_playerindex SET player_id=%d, player_name='%s', battlepass_type=%d, battlepass_id=%d, start_time=NOW()", GetPlayerID(), GetName(), bBattlePassType, bBattlePassID);

			TPlayerExtBattlePassMission* newMission = new TPlayerExtBattlePassMission;
			newMission->dwPlayerId = GetPlayerID();
			newMission->dwBattlePassType = bBattlePassType;
			newMission->dwMissionType = dwMissionType;
			newMission->dwBattlePassId = bBattlePassID;

			if (dwUpdateValue >= dwSecondInfo)
			{
				newMission->dwMissionIndex = dwMissionIndex;
				newMission->dwExtraInfo = dwSecondInfo;
				newMission->bCompleted = 1;

				CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, bBattlePassID, dwMissionIndex);
				if (bBattlePassType == 1) {
					EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_NORMAL_MISSION"));
				}
				if (bBattlePassType == 2) {
					EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_PREMIUM_MISSION"));
				}
				if (bBattlePassType == 3) {
					EffectPacket(SE_EFFECT_BP_EVENT_MISSION_COMPLETED);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLEPASS_COMPLETE_EVENT_MISSION"));
				}

				dwSaveProgress = dwSecondInfo;
			}
			else
			{
				newMission->dwMissionIndex = dwMissionIndex;
				newMission->dwExtraInfo = dwUpdateValue;
				newMission->bCompleted = 0;

				dwSaveProgress = dwUpdateValue;
			}

			newMission->bIsUpdated = 1;

			m_listExtBattlePass.push_back(newMission);

			TPacketGCExtBattlePassMissionUpdate packet;
			packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
			packet.bBattlePassType = bBattlePassType;
			packet.bMissionIndex = dwMissionIndex;
			packet.dwNewProgress = dwSaveProgress;
			GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
		}
	}
}
#endif

#ifdef __HIDE_COSTUME_SYSTEM__
void CHARACTER::SetBodyCostumeHidden(bool hidden)
{
	m_bHideBodyCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetBodyCostumeHidden %d", m_bHideBodyCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_body", m_bHideBodyCostume ? 1 : 0);
}

void CHARACTER::SetHairCostumeHidden(bool hidden)
{
	m_bHideHairCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetHairCostumeHidden %d", m_bHideHairCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_hair", m_bHideHairCostume ? 1 : 0);
}
#endif

#ifdef ENABLE_REWARD_SYSTEM
void CHARACTER::SaveRewardData()
{
	char fileName[254];
	snprintf(fileName, sizeof(fileName), "share/reward_solo/%u", GetPlayerID());
	FILE* fp;
	if ((fp = fopen(fileName, "w+")) != NULL)
	{
		for (itertype(m_mapRewardData) it = m_mapRewardData.begin(); it != m_mapRewardData.end(); ++it)
			fprintf(fp, "%u %u\n", it->first, it->second);
		fclose(fp);
	}
}
void CHARACTER::LoadRewardData()
{
	m_mapRewardData.clear();

	char fileName[254];
	snprintf(fileName, sizeof(fileName), "share/reward_solo/%u", GetPlayerID());

	FILE* fp;
	if ((fp = fopen(fileName, "r")) != NULL)
	{
		char	one_line[256];
		while (fgets(one_line, 256, fp))
		{
			if (strlen(one_line))
			{
				std::vector<std::string> vecArgs;
				split_argument(one_line, vecArgs);
				if (vecArgs.size() == 2)
				{
					BYTE bType;
					DWORD value;
					if (str_to_number(bType, vecArgs[0].c_str()) && str_to_number(value, vecArgs[1].c_str()))
						m_mapRewardData.emplace(bType, value);
				}
			}
		}
		fclose(fp);
	}
	m_bRewardLoaded = true;
}
DWORD CHARACTER::GetRewardData(const BYTE bType)
{
	if (!m_bRewardLoaded)
		LoadRewardData();

	itertype(m_mapRewardData) it = m_mapRewardData.find(bType);
	return (it != m_mapRewardData.end()) ? it->second : 0;
}
void CHARACTER::SetRewardData(const BYTE bType, const DWORD value)
{
	if (!m_bRewardLoaded)
		LoadRewardData();
	itertype(m_mapRewardData) it = m_mapRewardData.find(bType);
	if (it != m_mapRewardData.end())
		it->second = value;
	else
		m_mapRewardData.emplace(bType, value);
}
#endif


#ifdef ENABLE_AUTOMATIC_ITEM_PROCESS
void CHARACTER::SendItemProcessInfo()
{
	if (!GetDesc()) { return; }
	ChatPacket(CHAT_TYPE_COMMAND, "clearItemProcessInfo");
	for (size_t i = 0; i < AUTOMATIC_ITEM_PROCESS_MAX; ++i)
	{
		if (m_automaticItemProcess[i] == 0)
			continue;
		ChatPacket(CHAT_TYPE_COMMAND, "addItemProcessInfo %u", m_automaticItemProcess[i]);
	}
	ChatPacket(CHAT_TYPE_COMMAND, "setItemProcessStatus %d", GetQuestFlag("automatic_item_process.status"));
}

void CHARACTER::AddAutomaticItemProcess(const uint32_t vnum)
{
	const auto& getArrElmSize = [&]() -> size_t {
		size_t count = 0;
		for (size_t i = 0; i < AUTOMATIC_ITEM_PROCESS_MAX; ++i)
		{	
			if (m_automaticItemProcess[i] != 0)
				++count;
		}
		return count;
	};

	if (getArrElmSize() >= AUTOMATIC_ITEM_PROCESS_MAX)
	{
		ChatPacket(CHAT_TYPE_INFO, "Daha fazla esya ekleyemezsin.");
		return;
	}

	// check same item vnum
	for (size_t i = 0; i < AUTOMATIC_ITEM_PROCESS_MAX; ++i)
	{
		if (m_automaticItemProcess[i] == vnum)
		{
			ChatPacket(CHAT_TYPE_INFO, "Zaten bu esya ekli.");
			return;
		}
	}

	for (size_t i = 0; i < AUTOMATIC_ITEM_PROCESS_MAX; ++i)
	{
		if (m_automaticItemProcess[i] == 0)
		{
			m_automaticItemProcess[i] = vnum;
			ChatPacket(CHAT_TYPE_INFO, "Item eklendi.");
			break;
		}
	}
}

void CHARACTER::RemoveAutomaticItemProcess(const uint32_t vnum)
{
	for (size_t i = 0; i < AUTOMATIC_ITEM_PROCESS_MAX; ++i)
	{
		if (m_automaticItemProcess[i] == vnum)
		{
			m_automaticItemProcess[i] = 0;
			ChatPacket(CHAT_TYPE_INFO, "Automatic item process removed");
			// ChatPacket(CHAT_TYPE_COMMAND, "removeItemProcessInfo %u", vnum);
			break;
		}
	}
}

bool CHARACTER::IsAutomaticItemProcess(const uint32_t vnum) const
{
	bool isProcess = false;
	for (size_t i = 0; i < AUTOMATIC_ITEM_PROCESS_MAX; ++i)
	{
		if (m_automaticItemProcess[i] == vnum)
		{
			isProcess = true;
			break;
		}
	}
	return isProcess;
}
#endif

#ifdef __RANKING_SYSTEM__
void CHARACTER::SetProtectTime(const std::string& flagname, int value)
{
	itertype(m_protection_Time) it = m_protection_Time.find(flagname);
	if (it != m_protection_Time.end())
		it->second = value;
	else
		m_protection_Time.insert(make_pair(flagname, value));
}
int CHARACTER::GetProtectTime(const std::string& flagname) const
{
	const itertype(m_protection_Time) it = m_protection_Time.find(flagname);
	return (it != m_protection_Time.end()) ? it->second : 0;
}
void CHARACTER::SetRankManuel(BYTE rankIndex, long long value)
{
	switch (rankIndex)
	{
	case RANK_STONE:
		SetDestroyStone(GetDestroyStone()+value);
		break;
	case RANK_MONSTER:
		SetKillMonster(GetKillMonster()+value);
		break;
	case RANK_BOSS:
		SetKillBoss(GetKillBoss()+value);
		break;
	case RANK_DUNGEON:
		SetCompletedDungeon(GetCompletedDungeon()+value);
		break;
	case RANK_CHEST:
		SetOpenedChest(GetOpenedChest()+value);
		break;
	}
	char flag[45];
	sprintf(flag, "rank_%d",rankIndex);
	if(GetProtectTime(flag) == 1)
		SendRanks(RANK_SELF, rankIndex);
}
void CHARACTER::GetRankManuel(BYTE rankIndex, std::vector<TRanking>& list)
{
	TRanking p;
	p.empire = GetEmpire();
	p.index = 11;
	p.value = 0;
	strlcpy(p.name, GetName(), sizeof(p.name));
	switch (rankIndex)
	{
		case RANK_LEVEL:
			p.value = GetLevel();
			break;
		case RANK_STONE:
			p.value = GetDestroyStone();
			break;
		case RANK_MONSTER:
			p.value = GetKillMonster();
			break;
		case RANK_BOSS:
			p.value = GetKillBoss();
			break;
		case RANK_DUNGEON:
			p.value = GetCompletedDungeon();
			break;
		case RANK_PLAYTIME:
			p.value = GetRealPoint(POINT_PLAYTIME);
			break;
		case RANK_GOLD:
			p.value = GetGold();
			break;
		case RANK_GAYA:
			p.value = GetGaya();
			break;
		case RANK_CHEST:
			p.value = GetOpenedChest();
			break;
		default:
			return;
	}
	list.emplace_back(p);
}

void CHARACTER::SendRanks(BYTE subIndex, BYTE rankIndex)
{
	if (!GetDesc())
		return;

	TEMP_BUFFER buf;
	TPacketRank pack;
	pack.header = HEADER_GC_RANKING_UPDATE;

	char flag[45];
	sprintf(flag, "rank_%d",rankIndex);

	if (subIndex == RANK_LOAD)
	{
		//if(GetProtectTime(flag) == 1)
		//	return;

		SetProtectTime(flag, 1);

		std::vector<TRanking> rank_data;
		CHARACTER_MANAGER& chrMngr = CHARACTER_MANAGER::Instance();
		chrMngr.GetRankData(rankIndex, rank_data);
		GetRankManuel(rankIndex, rank_data);

		BYTE vec_size = rank_data.size();

		pack.size = sizeof(pack)+sizeof(BYTE)+sizeof(BYTE)+sizeof(BYTE)+(sizeof(TRanking)*vec_size);

		buf.write(&pack, sizeof(TPacketRank));
		buf.write(&subIndex, sizeof(BYTE));
		buf.write(&rankIndex, sizeof(BYTE));
		buf.write(&vec_size, sizeof(BYTE));
		if(vec_size > 0)
			buf.write(rank_data.data(), sizeof(TRanking)*vec_size);
	}
	else
	{
		if(GetProtectTime(flag) != 1)
			return;

		std::vector<TRanking> rank_data;
		GetRankManuel(rankIndex, rank_data);
		if(rank_data.size() == 0)
			return;
		else if (rank_data[0].value == 0)
			return;
		pack.size = sizeof(pack)+sizeof(BYTE)+sizeof(BYTE)+sizeof(long long);
		buf.write(&pack, sizeof(TPacketRank));
		buf.write(&subIndex, sizeof(BYTE));
		buf.write(&rankIndex, sizeof(BYTE));
		buf.write(&rank_data[0].value, sizeof(long long));
	}
	GetDesc()->Packet(buf.read_peek(), buf.size());
}
#endif

#ifdef __AUTO_HUNT__
void CHARACTER::SetAutoHuntStatus(bool bStatus, bool bMobFarm, bool bMetinFarm)
{
	m_bAutoHuntStatus = bStatus;
	RemoveAffect(AFFECT_AUTO_HUNT_AFFECT);
	if (bStatus)
		AddAffect(AFFECT_AUTO_HUNT_AFFECT, POINT_NONE, 0, AFF_AUTO_HUNT, INFINITE_AFFECT_DURATION, 0, false);
	ChatPacket(CHAT_TYPE_COMMAND, "AutoHuntStatus %d %d %d", bStatus, bMobFarm, bMetinFarm);
}
void CHARACTER::GetAutoHuntCommand(const char* szArgument)
{
	std::vector<std::string> vecArgs;
	split_argument(szArgument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "end")
	{
		SetAutoHuntStatus(false);
	}
	else if (vecArgs[1] == "start")
	{
		if (vecArgs.size() < 7) { return; }
		if (!IsAutoHuntAffectHas())
		{
			ChatPacket(CHAT_TYPE_INFO, "You don't have auto hunt affect.");
			return;
		}
		SetAutoHuntStatus(true, vecArgs[5] == "1" ? true : false, vecArgs[6] == "1" ? true : false);
	}
	ChatPacket(CHAT_TYPE_COMMAND, "auto_hunt %s", szArgument);
}
bool CHARACTER::IsAutoHuntAffectHas()
{
	return FindAffect(AFFECT_AUTO_HUNT) != NULL ? true : false;
}
#endif
