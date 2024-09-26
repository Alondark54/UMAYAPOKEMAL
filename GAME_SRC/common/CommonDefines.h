#ifndef __INC_METIN2_COMMON_DEFINES_H__
#define __INC_METIN2_COMMON_DEFINES_H__
#pragma once
//////////////////////////////////////////////////////////////////////////
// ### Standard Features ###
#define _IMPROVED_PACKET_ENCRYPTION_
#define __PET_SYSTEM__
#define ENABLE_MULTI_FARM_BLOCK
#define __UDP_BLOCK__
//#define ENABLE_QUEST_CATEGORY

#define ENABLE_MULTI_FARM_BLOCK

#define __AUTO_HUNT__


// ### END Standard Features ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New Features ###
#define ENABLE_D_NJGUILD
#define ENABLE_LUCKY_DRAW
#define ENABLE_FULL_NOTICE
//#define MANGO_LISANS_SISTEMI
#define ENABLE_NEWSTUFF
#define ENABLE_PORT_SECURITY
#define	ENABLE_FIREWORK_STUN
#define ENABLE_BELT_INVENTORY_EX
#define	SOHBET_DURDUR
#define ENABLE_CMD_WARP_IN_DUNGEON
#define ENABLE_MANGO_POTIONS
// #define ENABLE_ITEM_ATTR_COSTUME
// #define ENABLE_SEQUENCE_SYSTEM
#define DBONE_EFFECTS
#define ENABLE_PLAYER_PER_ACCOUNT5
//#define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM
#define ENABLE_MOUNT_COSTUME_SYSTEM
#define ENABLE_WEAPON_COSTUME_SYSTEM
#define ENABLE_QUEST_DIE_EVENT
#define ENABLE_QUEST_BOOT_EVENT
#define ENABLE_QUEST_DND_EVENT
#define ENABLE_AUTOMATIC_ITEM_PROCESS
#ifdef ENABLE_AUTOMATIC_ITEM_PROCESS
	#define AUTOMATIC_ITEM_PROCESS_MAX 80 // max item count
#endif
#define	__RANKING_SYSTEM__
//#define RUNIK_ULTIMATE
//#define SOHBET_DURDUR
#define ENABLE_MULTISHOP
#define ENABLE_EVENT_MANAGER
//#define ENABLE_NEW_RANKING
//#ifdef ENABLE_NEW_RANKING
//	using ULL = unsigned long long;
//	#define ENABLE_RANKING_TITLES // -> deactive
//#endif

enum eCommonDefines {
	MAP_ALLOW_LIMIT = 32, // 32 default
};

#define ENABLE_WOLFMAN_CHARACTER
#ifdef ENABLE_WOLFMAN_CHARACTER
#define USE_MOB_BLEEDING_AS_POISON
#define __NEW_POTION__
#define USE_MOB_CLAW_AS_DAGGER
// #define USE_ITEM_BLEEDING_AS_POISON
// #define USE_ITEM_CLAW_AS_DAGGER
// #define USE_WOLFMAN_STONES
// #define USE_WOLFMAN_BOOKS
#endif

// #define ENABLE_MAGIC_REDUCTION_SYSTEM
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
// #define USE_MAGIC_REDUCTION_STONES
#endif

// ### END New Features ###
//////////////////////////////////////////////////////////////////////////

#define __SEND_TARGET_INFO__
#define __VIEW_TARGET_PLAYER_HP__
#define __VIEW_TARGET_DECIMAL_HP__
#define ENABLE_DROP_DIALOG_EXTENDED_SYSTEM
//#define ENABLE_EXTEND_INVEN_SYSTEM
#define ENABLE_AFFECT_POLYMORPH_REMOVE
#define WJ_ENABLE_TRADABLE_ICON
#define	ENABLE_ADD_STONE_SLOT
#define WJ_ELDER_ATTRIBUTE_SYSTEM
//#define ENABLE_BOSS_TRACKING
#define BL_REMOTE_SHOP
#define	RENEWAL_BOOK_NAME
//#define ENABLE_UNLIMITED_SKILL_SCROLL
#define ENABLE_SWITCHBOT
#define __HIDE_COSTUME_SYSTEM__ // Kostüm Gizleme
#define	TOPLU_TAS_FIX
#define ENABLE_NEW_BIOLOG
#define NEW_SALES_SYSTEM
#define FIX_HEADER_CG_MARK_LOGIN
#define ENABLE_GIVE_BASIC_ITEM
#define ENABLE_SKILL_SELECT_FEATURE
#define BL_PRIVATESHOP_SEARCH_SYSTEM
#define FATE_ROULETTE
#define ENABLE_REWARD_SYSTEM
//#define ENABLE_RENEWAL_SHOPEX
#define ENABLE_FISH_EVENT
#define ENABLE_DROP_FROM_TABLE
#define ENABLE_CAOS_EVENT
//#define ENABLE_AUTO_EVENTS
#define ENABLE_NEW_PASSIVE_SKILL
#define __BL_MOVE_CHANNEL__
//#define	URIEL_ANTI_CHEAT
#define ENABLE_RING_OF_SECRETS
#define ENABLE_SPLIT
#define ENABLE_DUNGEON_JOIN_CORDS
#define THIRD_HAND
#define ANTI_MOB_RANGE_ITEM
#define ENABLE_SOULBIND_SYSTEM
#define ENABLE_MINING_EVENT
#define __NEW_EXCHANGE_WINDOW__
#define ENABLE_CUBE_RENEWAL_WORLDARD
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	//#define ENABLE_CUBE_RENEWAL_GEM_WORLDARD
	#define ENABLE_CUBE_RENEWAL_COPY_WORLDARD
#endif
//#define ENABLE_NEW_MISSIONS
//#define YONETICI_PM
//#define __PLAYER_PIN_SYSTEM__ // Player Pin Code System
#define ENABLE_FIX_READ_ETC_DROP_ITEM_FILE_BY_VNUM
//#define ENABLE_BOT_CONTROL
#define ENABLE_EXTENDED_BATTLE_PASS 	// Extended Battlepass-System by Aslan
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	#define RESTRICT_COMMAND_GET_INFO					GM_LOW_WIZARD
	#define RESTRICT_COMMAND_SET_MISSION				GM_IMPLEMENTOR
	#define RESTRICT_COMMAND_PREMIUM_ACTIVATE	GM_IMPLEMENTOR	
#endif
//////////////////////////////////////////////////////////////////////////
// ### Ex Features ###
//#define DISABLE_STOP_RIDING_WHEN_DIE //	if DISABLE_TOP_RIDING_WHEN_DIE is defined, the player doesn't lose the horse after dying
#define ENABLE_ACCE_COSTUME_SYSTEM //fixed version
// #define USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS //enable only positive bonus in acce absorb
#define ENABLE_HIGHLIGHT_NEW_ITEM //if you want to see highlighted a new item when dropped or when exchanged
#define __ENABLE_KILL_EVENT_FIX__ //if you want to fix the 0 exp problem about the when kill lua event (recommended)
// #define ENABLE_SYSLOG_PACKET_SENT // debug purposes

// ### END Ex Features ###
//////////////////////////////////////////////////////////////////////////
#define __OFFLINE_PRIVATE_SHOP_SYSTEM__
#define ITEM_SHOP
#ifdef ITEM_SHOP
	#define SHOP_CHAR_MAX_NUM 24
	#define SHOP_CAT_MAX_NUM 256
#endif

#endif
//martysama0134's 2022
