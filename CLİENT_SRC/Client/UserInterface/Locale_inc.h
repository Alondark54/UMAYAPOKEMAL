
#pragma once

//////////////////////////////////////////////////////////////////////////
// ### Default Ymir Macros ###
#define LOCALE_SERVICE_EUROPE
#define ENABLE_COSTUME_SYSTEM
#define ENABLE_ENERGY_SYSTEM
#define ENABLE_DRAGON_SOUL_SYSTEM
#define ENABLE_NEW_EQUIPMENT_SYSTEM
// ### Default Ymir Macros ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New From LocaleInc ###
#define ENABLE_PACK_GET_CHECK
//////#define ENABLE_CANSEEHIDDENTHING_FOR_GM
#define ENABLE_PROTOSTRUCT_AUTODETECT
#define __AUTO_HUNT__
#define ENABLE_PLAYER_PER_ACCOUNT5
#define ENABLE_LEVEL_IN_TRADE
//#define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM
#define ENABLE_LVL115_ARMOR_EFFECT
#define ENABLE_SLOT_WINDOW_EX
#define ENABLE_TEXT_LEVEL_REFRESH
#define ENABLE_USE_COSTUME_ATTR
#define ENABLE_DISCORD_RPC
#define ENABLE_MANGO_POTIONS
#define ENABLE_MULTI_FARM_BLOCK
#define LINK_IN_CHAT
#define ENABLE_LUCKY_DRAW

// #define __COOL_REFRESH_EDITLINE__

#define WJ_SHOW_MOB_INFO
#ifdef WJ_SHOW_MOB_INFO
#define ENABLE_SHOW_MOBAIFLAG
#define ENABLE_SHOW_MOBLEVEL
#endif
// ### New From LocaleInc ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### From GameLib ###
//#define ENABLE_WOLFMAN_CHARACTER

// #define ENABLE_MAGIC_REDUCTION_SYSTEM
#define ENABLE_MOUNT_COSTUME_SYSTEM
#define ENABLE_WEAPON_COSTUME_SYSTEM
// ### From GameLib ###
//////////////////////////////////////////////////////////////////////////

#define ENABLE_CUBE_RENEWAL_WORLDARD
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	//#define ENABLE_CUBE_RENEWAL_GEM_WORLDARD
	#define ENABLE_CUBE_RENEWAL_COPY_WORLDARD
#endif

/*
	###		New System Defines - Extended Version		###
*/

// if is define ENABLE_ACCE_COSTUME_SYSTEM the players can use shoulder sash
// if you want to use object scaling function you must defined ENABLE_OBJ_SCALLING
#define ENABLE_ACCE_COSTUME_SYSTEM
#define ENABLE_OBJ_SCALLING
// #define USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS

// if you want use SetMouseWheelScrollEvent or you want use mouse wheel to move the scrollbar
#define ENABLE_MOUSEWHEEL_EVENT

//if you want to see highlighted a new item when dropped or when exchanged
#define ENABLE_HIGHLIGHT_NEW_ITEM

// it shows emojis in the textlines
#define ENABLE_EMOJI_SYSTEM
#define ENABLE_NEW_BIOLOG							//Biyolog Sistemi
#define ENABLE_DETAILS_UI							//Bonus bilgisi sistemi
#define ENABLE_SEND_TARGET_INFO						//Mob Target Info
#define ENABLE_VIEW_TARGET_PLAYER_HP				//Oyuncu HP Görme
#define ENABLE_VIEW_TARGET_DECIMAL_HP				//Mob HP Görme
//#define REFRESH_MONEY_SLEEP							//Para Eksilme Animasyonu
#define ENABLE_DROP_DIALOG_EXTENDED_SYSTEM			//Yere Eşya At, Sil, Sat Modülü
#define ENABLE_ITEM_DELETE_SYSTEM					//Toplu İtem Sil ve Sat Modülü
#define ENABLE_FAST_INVENTORY						//Envanter Yanı Hızlı Menü
#define ENABLE_EXTEND_INVEN_SYSTEM					//Envanter Genişletme
#define AUTO_SHOUT									//Otomatik Bağırma Sistemi
#define WJ_ENABLE_TRADABLE_ICON						//GF Slot İşaretleme Sistemi
#define WJ_ELDER_ATTRIBUTE_SYSTEM					//Kadim Efsun ve Kadim Küre Sistemi
#define __BL_CHEST_DROP_INFO__						//Sandık Aynası (Clientside)
#define BL_REMOTE_SHOP								//Uzaktan Market Sistemi
#define ENABLE_REWARD_SYSTEM
#define ENABLE_SWITCHBOT							//Efsun Botu Svside
#define DBONE_EFFECTS
//#define ENABLE_FPS									//Yuksek FPS Modu
//#define ENABLE_FIX_MOBS_LAG							//Mob Lag Fix
#define NEW_SALES_SYSTEM							//Kampanya Sistemi
#define ENABLE_SKILL_SELECT_FEATURE					//Hızlı Beceri
//#define ENABLE_FOG_FIX								//Gölge Fix
#define BL_PRIVATESHOP_SEARCH_SYSTEM				//Pazar Arama
#define FATE_ROULETTE								//Çark
#define ENABLE_FOV_OPTION							//Görüş Açısı
#define ENABLE_AUTOMATIC_ITEM_PROCESS				//Sat
#define ENABLE_CAOS_EVENT							//Kaos Eventi
#define ENABLE_SHOW_NIGHT_SYSTEM					//Gece Gündüz
#define ENABLE_SOULBIND_SYSTEM
#define __NEW_POTION__
#define ENABLE_NEW_EXCHANGE_WINDOW					//Ticaret Penceresi
#define ENABLE_EXTENDED_BATTLE_PASS
#define ENABLE_NEW_PASSIVE_SKILL					//Pasif Beceriler
#define __RANKING_SYSTEM__
#define ENABLE_NPC_WEAR_ITEM
#define ENABLE_EVENT_MANAGER
#define ENABLE_REFINE_RENEWAL
//#define ENABLE_RENEWAL_SHOPEX
#define ENABLE_MULTISHOP
//#define ENABLE_EXTENDED_BLEND
//#define ENABLE_PLAYER_PIN_SYSTEM // Player Pin Code System
#define __BL_MOVE_CHANNEL__							//CH_Degistirme
#define WJ_SHOW_ALL_CHANNEL							//CH_Gozukmeme_Fix
#define	ENABLE_NEW_ATTR
#define	ENABLE_HIDE_COSTUME_SYSTEM
#define CEF_BROWSER // CEF Browser
//#if defined(ENABLE_FIX_MOBS_LAG)
//	// -> The define ENABLE_FIX_MOBS_LAG have problems in device reseting.
//	// -> With this new define all this problems are fixed.
//	#define FIX_MOBS_LAG_FIX
//#endif
/*
	###		New Debugging Defines
*/
// #define ENABLE_PRINT_RECV_PACKET_DEBUG
//martysama0134's 2022
#define ANTI_MOB_RANGE_ITEM
#define ENABLE_OFFLINE_SHOP_SYSTEM
#define ENABLE_AFFECT_POLYMORPH_REMOVE
#define ENABLE_FS
#define ITEM_SHOP
#ifdef ITEM_SHOP
	#define SHOP_CHAR_MAX_NUM 24
	#define SHOP_CAT_MAX_NUM 256
	#define IS_ITEM_SOCKET_MAX 4
	#define IS_ITEM_ATTR_MAX 7
#endif
//#define RUNIK_ULTIMATE				// Runik Anti Cheat Ultimate BETA İÇİN KAPATMIŞTIM
#define ENABLE_NO_PICKUP_LIMIT
//#define METIN2PM

//#define ENABLE_NEW_MISSIONS