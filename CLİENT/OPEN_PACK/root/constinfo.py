# EXTRA BEGIN
# loads 5 (B,M,G,P,F) skills .mse
ENABLE_NEW_LEVELSKILL_SYSTEM = 0
# don't set a random channel when you open the client
ENABLE_RANDOM_CHANNEL_SEL = 0
# don't remove id&pass if the login attempt fails
ENABLE_CLEAN_DATA_IF_FAIL_LOGIN = 0
# ctrl+v will now work
ENABLE_PASTE_FEATURE = 0
# display all the bonuses added by a stone instead of the first one
ENABLE_FULLSTONE_DETAILS = 1
# enable successfulness % in the refine dialog
ENABLE_REFINE_PCT = 0
# extra ui features
EXTRA_UI_FEATURE = 1
#
LOAD_CURTAIN = 0
import item
import os
import app
if app.__AUTO_HUNT__:
	autoHuntAutoLoginDict = {
		"status" : 0,
		"leftTime" : 0,
		"id" : "",
		"pwd" : "",
		"addr" : "",
		"port" : 0,
		"account_addr" : "",
		"account_port" : 0,
		"slot" : -1,
		"newOptions" : None,
		"slotStatus" : {},
		"skillDict" : None,
	}

import app
if app.ENABLE_REFINE_RENEWAL:
	IS_AUTO_REFINE = False
	AUTO_REFINE_TYPE = 0
	AUTO_REFINE_DATA = {
		"ITEM" : [-1, -1],
		"NPC" : [0, -1, -1, 0]
	}

ENABLE_POTIONS_AFFECTSHOWER = 1 ## 0=Disable / 1=Enable
yesilefsunbotu = 0
uideleteitem = 0
uinewbiolog = 0
auto_pick_item = 1
auto_pick_yang = 1

IS_BONUS_CHANGER = FALSE

if app.ENABLE_HIDE_COSTUME_SYSTEM:
	HIDDEN_BODY_COSTUME = 0
	HIDDEN_HAIR_COSTUME = 0

_interface_instance = None
def GetInterfaceInstance():
	global _interface_instance
	return _interface_instance
def SetInterfaceInstance(instance):
	global _interface_instance
	if _interface_instance:
		del _interface_instance
	_interface_instance = instance


NEW_678TH_SKILL_ENABLE = 1
# EXTRA END
import app

if app.ENABLE_SKILL_SELECT_FEATURE:
	ARE_ENABLED_6TH_SKILLS = 1


if app.ENABLE_SEND_TARGET_INFO:
	MONSTER_INFO_DATA = {}

if app.AUTO_SHOUT:
	auto_shout_status = 0
	auto_shout_text = ""

BOSS_TRACKING = 0
bosstracking = 0
CREATE_PRIVATE_SHOP_EFFECT = (0,False,app.GetTime())

CLIENT_YOL = "lib/"

import app
if app.NEW_SALES_SYSTEM:
	SalesList = {}
	Sales_Temp_ItemList = []
	Sales_Temp_Index = 0
	sarirenk="|cffFDD017|H|h"
	normalrenk="|h|r"
	kirmizirenk="|cffff0000|Hemp|h"
	yesilrenk="|cff00ff00|H|h"
	kapaliyesilrenk="|cff00CC00|Hemp|h"

if app.ENABLE_OFFLINE_SHOP_SYSTEM:
	CREATE_PRIVATE_SHOP_EFFECT = (0,False,app.GetTime())
	OFFLINE_SHOP_SALES = []
	OFFLINE_SHOP_ITEM_PRICE_DIC = {}
	def MapNameByIndex(idx):
		# import localeInfo

		# MAP_INDEX_DICT = {
			# 0 : localeInfo.MAP_NONE,
			# 1 : localeInfo.MAP_A1,
			# 21 : localeInfo.MAP_B1,
			# 41 : localeInfo.MAP_C1,
			# 61 : localeInfo.MAP_N_SNOWM_01,
			# 62 : localeInfo.MAP_N_FLAME_01,
			# 63 : localeInfo.MAP_N_DESERT_01,
			# 64 : localeInfo.MAP_N_THREEWAY,
			# 65 : localeInfo.MAP_MILGYO,
			# 66 : localeInfo.MAP_DEVILTOWER1,
			# 72 : localeInfo.MAP_SKIPIA_DUNGEON_01,
			# 73 : localeInfo.MAP_SKIPIA_DUNGEON_02,
			# 208 : localeInfo.MAP_SKIPIA_DUNGEON_BOSS,
			# 216 : localeInfo.MAP_DEVILCATACOMB,
			# 217 : localeInfo.MAP_SPIDERDUNGEON_03,
			# 321 : localeInfo.MAP_DAWNMIST_DUNGEON_01,
			# 322 : localeInfo.MAP_MT_TH_DUNGEON_01,
			# 351 : localeInfo.MAP_N_FLAME_DUNGEON_01,
			# 352 : localeInfo.MAP_N_SNOW_DUNGEON_01,
			
			# 109 : "Cave dos Macacos",
			# 357 : "Esconderijo dos Orcs",
			# 30 : "Gruta da Praga",
			# 57 : "Caverna do Drag�o",
			# 13 : "Catacumbas de Azrael",
			# 56 : "Ru�nas de Ganesha",
			# 364 : "T�mulo da Sphinx",
			
		# }

		# if idx in MAP_INDEX_DICT: return MAP_INDEX_DICT[idx]
		# else: return MAP_INDEX_DICT[0]
		return "Map ID: %d" % (idx)
	def SetOfflineShopItemPrice(itemVNum, itemPrice, itemCount):
		import math
		real_item_price = int(math.ceil(itemPrice/itemCount))
		OFFLINE_SHOP_ITEM_PRICE_DIC[int(itemVNum)] = real_item_price

	def GetOfflineShopItemPrice(itemVNum, itemCount):
		try:
			return long(OFFLINE_SHOP_ITEM_PRICE_DIC[itemVNum] * itemCount)
		except KeyError:
			return 0
	
	def ExistSavedItemPrice(itemVNum):
		return OFFLINE_SHOP_ITEM_PRICE_DIC.has_key(int(itemVNum))

# enable save account
ENABLE_SAVE_ACCOUNT = True
if ENABLE_SAVE_ACCOUNT:
	class SAB:
		ST_CACHE, ST_FILE, ST_REGISTRY = xrange(3)
		slotCount = 5
		storeType = ST_REGISTRY # 0 cache, 1 file, 2 registry
		btnName = {
			"Kaydet": "SaveAccountButton_Save_%02d",
			"Onay": "SaveAccountButton_Access_%02d",
			"Sil": "SaveAccountButton_Remove_%02d",
		}
		accData = {}
		regPath = r"SOFTWARE\Umay2Games"
		regName = "slot%02d_%s"
		regValueId = "id"
		regValuePwd = "pwd"
		fileExt = ".do.not.share.it.txt"
def CreateSABDataFolder(filePath):
	import os
	folderPath = os.path.split(filePath)[0]
	if not os.path.exists(folderPath):
		os.makedirs(folderPath)
def IsExistSABDataFile(filePath):
	import os
	return os.path.exists(filePath)
def GetSABDataFile(idx):
	import os
	filePath = "%s\\Metin2\\" % os.getenv('appdata')
	filePath += SAB.regName % (idx, SAB.regValueId)
	filePath += SAB.fileExt
	return filePath
def DelJsonSABData(idx):
	import os
	filePath = GetSABDataFile(idx)
	if IsExistSABDataFile(filePath):
		os.remove(filePath)
def GetJsonSABData(idx):
	(id, pwd) = ("", "")
	filePath = GetSABDataFile(idx)
	if not IsExistSABDataFile(filePath):
		return (id, pwd)
	with old_open(filePath) as data_file:
		try:
			import json
			(id, pwd) = json.load(data_file)
			id = str(id) # unicode to ascii
			pwd = str(pwd) # unicode to ascii
		except ValueError:
			pass
	return (id, pwd)

def SetJsonSABData(idx, slotData):
	filePath = GetSABDataFile(idx)
	CreateSABDataFolder(filePath)
	with open(filePath, "w", encoding="utf-8") as data_file:
		import json
		json.dump(slotData, data_file)
def DelWinRegKeyValue(keyPath, keyName):
	try:
		import _winreg
		_winreg.CreateKey(_winreg.HKEY_CURRENT_USER, keyPath)
		_tmpKey = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, keyPath, 0, _winreg.KEY_WRITE)
		_winreg.DeleteValue(_tmpKey, keyName)
		_winreg.CloseKey(_tmpKey)
		return True
	except WindowsError:
		return False
def GetWinRegKeyValue(keyPath, keyName):
	try:
		import _winreg
		_tmpKey = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, keyPath, 0, _winreg.KEY_READ)
		keyValue, keyType = _winreg.QueryValueEx(_tmpKey, keyName)
		_winreg.CloseKey(_tmpKey)
		return str(keyValue) # unicode to ascii
	except WindowsError:
		return None
def SetWinRegKeyValue(keyPath, keyName, keyValue):
	try:
		import _winreg
		_winreg.CreateKey(_winreg.HKEY_CURRENT_USER, keyPath)
		_tmpKey = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, keyPath, 0, _winreg.KEY_WRITE)
		_winreg.SetValueEx(_tmpKey, keyName, 0, _winreg.REG_SZ, keyValue)
		_winreg.CloseKey(_tmpKey)
		return True
	except WindowsError:
		return False
# classic minmax def
def minmax(tmin, tmid, tmax):
	if tmid < tmin:
		return tmin
	elif tmid > tmax:
		return tmax
	return tmid
# EXTRA END

# option
IN_GAME_SHOP_ENABLE = 1
CONSOLE_ENABLE = 0
Vectors = ''
YangDrop = 1
maintenanceinfo = {
					"time" : 0,
					"active" : 0,
					"duration" : 0,
					}
if app.ENABLE_ITEM_DELETE_SYSTEM:
	silme = 0

if app.ENABLE_ITEM_DELETE_SYSTEM:
	ITEM_DELETE_LIST = {}

PVPMODE_ENABLE = 1
PVPMODE_TEST_ENABLE = 0
PVPMODE_ACCELKEY_ENABLE = 1
PVPMODE_ACCELKEY_DELAY = 0.5
PVPMODE_PROTECTED_LEVEL = 15

FOG_LEVEL0 = 4800.0
MuzikKontrol = False
FOG_LEVEL1 = 9600.0
FOG_LEVEL2 = 12800.0
FOG_LEVEL = FOG_LEVEL0
FOG_LEVEL_LIST=[FOG_LEVEL0, FOG_LEVEL1, FOG_LEVEL2]

CAMERA_MAX_DISTANCE_SHORT = 2500.0
CAMERA_MAX_DISTANCE_LONG = 3500.0
CAMERA_MAX_DISTANCE_LIST=[CAMERA_MAX_DISTANCE_SHORT, CAMERA_MAX_DISTANCE_LONG]
CAMERA_MAX_DISTANCE = CAMERA_MAX_DISTANCE_SHORT

CHRNAME_COLOR_INDEX = 0

ENVIRONMENT_NIGHT="d:/ymir work/environment/moonlight04.msenv"

Night = 0

# constant
HIGH_PRICE = 500000
MIDDLE_PRICE = 50000
ERROR_METIN_STONE = 28960
SUB2_LOADING_ENABLE = 1
EXPANDED_COMBO_ENABLE = 1
CONVERT_EMPIRE_LANGUAGE_ENABLE = 0
USE_ITEM_WEAPON_TABLE_ATTACK_BONUS = 0
ADD_DEF_BONUS_ENABLE = 0
LOGIN_COUNT_LIMIT_ENABLE = 0

USE_SKILL_EFFECT_UPGRADE_ENABLE = 1

VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = 1
GUILD_MONEY_PER_GSP = 100
GUILD_WAR_TYPE_SELECT_ENABLE = 1
TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE = 10

HAIR_COLOR_ENABLE = 1
ARMOR_SPECULAR_ENABLE = 1
WEAPON_SPECULAR_ENABLE = 1
SEQUENCE_PACKET_ENABLE = 1
KEEP_ACCOUNT_CONNETION_ENABLE = 1
MINIMAP_POSITIONINFO_ENABLE = 1

isItemQuestionDialog = 0

def GET_ITEM_QUESTION_DIALOG_STATUS():
	global isItemQuestionDialog
	return isItemQuestionDialog

def SET_ITEM_QUESTION_DIALOG_STATUS(flag):
	global isItemQuestionDialog
	isItemQuestionDialog = flag

import app
import net

########################

def SET_DEFAULT_FOG_LEVEL():
	global FOG_LEVEL
	app.SetMinFog(FOG_LEVEL)

def SET_FOG_LEVEL_INDEX(index):
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	try:
		FOG_LEVEL=FOG_LEVEL_LIST[index]
	except IndexError:
		FOG_LEVEL=FOG_LEVEL_LIST[0]
	app.SetMinFog(FOG_LEVEL)

def GET_FOG_LEVEL_INDEX():
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	return FOG_LEVEL_LIST.index(FOG_LEVEL)

########################

def SET_DEFAULT_CAMERA_MAX_DISTANCE():
	global CAMERA_MAX_DISTANCE
	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def SET_CAMERA_MAX_DISTANCE_INDEX(index):
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	try:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[index]
	except:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[0]

	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def GET_CAMERA_MAX_DISTANCE_INDEX():
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	return CAMERA_MAX_DISTANCE_LIST.index(CAMERA_MAX_DISTANCE)

########################

import chrmgr
import player
import app

def SET_DEFAULT_CHRNAME_COLOR():
	global CHRNAME_COLOR_INDEX
	chrmgr.SetEmpireNameMode(CHRNAME_COLOR_INDEX)

def SET_CHRNAME_COLOR_INDEX(index):
	global CHRNAME_COLOR_INDEX
	CHRNAME_COLOR_INDEX=index
	chrmgr.SetEmpireNameMode(index)

def GET_CHRNAME_COLOR_INDEX():
	global CHRNAME_COLOR_INDEX
	return CHRNAME_COLOR_INDEX

def SET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD(index):
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = index

def GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD():
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	return VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD

def SET_DEFAULT_CONVERT_EMPIRE_LANGUAGE_ENABLE():
	global CONVERT_EMPIRE_LANGUAGE_ENABLE
	net.SetEmpireLanguageMode(CONVERT_EMPIRE_LANGUAGE_ENABLE)

def SET_DEFAULT_USE_ITEM_WEAPON_TABLE_ATTACK_BONUS():
	global USE_ITEM_WEAPON_TABLE_ATTACK_BONUS
	player.SetWeaponAttackBonusFlag(USE_ITEM_WEAPON_TABLE_ATTACK_BONUS)

def SET_DEFAULT_USE_SKILL_EFFECT_ENABLE():
	global USE_SKILL_EFFECT_UPGRADE_ENABLE
	app.SetSkillEffectUpgradeEnable(USE_SKILL_EFFECT_UPGRADE_ENABLE)

def SET_TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE():
	global TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE
	app.SetTwoHandedWeaponAttSpeedDecreaseValue(TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE)

########################
import item

ACCESSORY_MATERIAL_LIST = [50623, 50624, 50625, 50626, 50627, 50628, 50629, 50630, 50631, 50632, 50633, 50634, 50635, 50636, 50637, 50638, 50639]
JewelAccessoryInfos = [
		# jewel		wrist	neck	ear
		[ 50634,	14420,	16220,	17220 ],
		[ 50635,	14500,	16500,	17500 ],
		[ 50636,	14520,	16520,	17520 ],
		[ 50637,	14540,	16540,	17540 ],
		[ 50638,	14560,	16560,	17560 ],
		[ 50639,	14570,	16570,	17570 ],
	]
def GET_ACCESSORY_MATERIAL_VNUM(vnum, subType):
	ret = vnum
	item_base = (vnum / 10) * 10
	for info in JewelAccessoryInfos:
		if item.ARMOR_WRIST == subType:
			if info[1] == item_base:
				return info[0]
		elif item.ARMOR_NECK == subType:
			if info[2] == item_base:
				return info[0]
		elif item.ARMOR_EAR == subType:
			if info[3] == item_base:
				return info[0]

	if vnum >= 16210 and vnum <= 16219:
		return 50625

	if item.ARMOR_WRIST == subType:
		WRIST_ITEM_VNUM_BASE = 14000
		ret -= WRIST_ITEM_VNUM_BASE
	elif item.ARMOR_NECK == subType:
		NECK_ITEM_VNUM_BASE = 16000
		ret -= NECK_ITEM_VNUM_BASE
	elif item.ARMOR_EAR == subType:
		EAR_ITEM_VNUM_BASE = 17000
		ret -= EAR_ITEM_VNUM_BASE

	type = ret/20

	if type<0 or type>=len(ACCESSORY_MATERIAL_LIST):
		type = (ret-170) / 20
		if type<0 or type>=len(ACCESSORY_MATERIAL_LIST):
			return 0

	return ACCESSORY_MATERIAL_LIST[type]

##################################################################

def GET_BELT_MATERIAL_VNUM(vnum, subType = 0):
	return 18900

##################################################################

def IS_AUTO_POTION(itemVnum):
	return IS_AUTO_POTION_HP(itemVnum) or IS_AUTO_POTION_SP(itemVnum)

def IS_AUTO_POTION_HP(itemVnum):
	if 72723 <= itemVnum and 72726 >= itemVnum:
		return 1
	elif itemVnum >= 76021 and itemVnum <= 76022:
		return 1
	elif itemVnum == 79012:
		return 1
	elif itemVnum == 72048:
		return 1
	if app.__NEW_POTION__:
		if itemVnum >= 50770 and itemVnum <= 50799:
			return 1

	return 0

def IS_AUTO_POTION_SP(itemVnum):
	if 72727 <= itemVnum and 72730 >= itemVnum:
		return 1
	elif itemVnum >= 76004 and itemVnum <= 76005:
		return 1
	elif itemVnum == 79013:
		return 1

	return 0

exInventory_isToggle = True

ACCOUNT_NAME = "Umay2Games"
WOLF_MAN = "DISABLED"
WOLF_WOMEN = "DISABLED"

