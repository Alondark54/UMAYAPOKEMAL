import ui
import uiScriptLocale
import net
import app
import dbg
import player
import background
import wndMgr

import localeInfo
import chrmgr
import colorInfo
import constInfo

import playerSettingModule
import stringCommander
import emotion

####################################
####################################
import uiRefine
import uiToolTip
import uiAttachMetin
import uiPickMoney
import uiChat
import uiMessenger
import uiHelp
import uiWhisper
import uiPointReset
import uiShop
import uiExchange
import uiSystem
import uiOption
import uiRestart
####################################

class LoadingWindow(ui.ScriptWindow):
	def __init__(self, stream):
		print "NEW LOADING WINDOW -------------------------------------------------------------------------------"
		ui.Window.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, self)

		self.stream=stream
		self.loadingImage=0
		self.loadingGage=0
		self.errMsg=0
		self.update=0
		self.playerX=0
		self.playerY=0
		self.loadStepList=[]

	def __del__(self):
		print "---------------------------------------------------------------------------- DELETE LOADING WINDOW"
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, 0)
		ui.Window.__del__(self)

	def Open(self):
		print "OPEN LOADING WINDOW -------------------------------------------------------------------------------"

		#app.HideCursor()

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/LoadingWindow1.py")
		except:
			import exception
			exception.Abort("LodingWindow.Open - LoadScriptFile Error")

		try:
			self.errMsg=self.GetChild("ErrorMessage")
		except:
			import exception
			exception.Abort("LodingWindow.Open - LoadScriptFile Error")

		self.errMsg.Hide()

		self.Show()
		chrSlot=self.stream.GetCharacterSlot()
		net.SendSelectCharacterPacket(chrSlot)
		app.SetFrameSkip(0)


	def Close(self):
		print "---------------------------------------------------------------------------- CLOSE LOADING WINDOW"

		app.SetFrameSkip(1)

		self.loadStepList=[]
		self.loadingImage=0
		self.loadingGage=0
		self.errMsg=0
		self.ClearDictionary()
		self.Hide()

	def OnPressEscapeKey(self):
		app.SetFrameSkip(1)
		self.stream.SetLoginPhase()
		return True

	def __SetNext(self, next):
		if next:
			self.update=ui.__mem_func__(next)
		else:
			self.update=0

	def __SetProgress(self, p):
		if self.loadingGage:
			self.loadingGage.SetPercentage(2+98*p/100, 100)

	def DEBUG_LoadData(self, playerX, playerY):
		self.playerX=playerX
		self.playerY=playerY

		self.__RegisterSkill()
		self.__RegisterTitleName()
		self.__RegisterColor()
		self.__InitData()
		self.__LoadMap()
		self.__LoadSound()
		self.__LoadEffect()
		self.__LoadWarrior()
		self.__LoadAssassin()
		self.__LoadSura()
		self.__LoadShaman()
		if app.ENABLE_WOLFMAN_CHARACTER:
			self.__LoadWolfman()
		self.__LoadSkill()
		self.__LoadEnemy()
		self.__LoadNPC()
		self.__StartGame()

	def LoadData(self, playerX, playerY):
		self.playerX=playerX
		self.playerY=playerY

		self.__RegisterDungeonMapName()
		self.__RegisterSkill()
		self.__RegisterTitleName()
		self.__RegisterColor()
		self.__RegisterEmotionIcon()
		if app.ENABLE_STREAMER_ICON:
			self.__RegisterStreamerName()
		self.loadStepList=[
			(0, ui.__mem_func__(self.__InitData)),
			(10, ui.__mem_func__(self.__LoadMap)),
			(30, ui.__mem_func__(self.__LoadSound)),
			(40, ui.__mem_func__(self.__LoadEffect)),
			(50, ui.__mem_func__(self.__LoadWarrior)),
			(60, ui.__mem_func__(self.__LoadAssassin)),
			(70, ui.__mem_func__(self.__LoadSura)),
			(80, ui.__mem_func__(self.__LoadShaman)),
			(90, ui.__mem_func__(self.__LoadSkill)),
			(93, ui.__mem_func__(self.__LoadEnemy)),
			(97, ui.__mem_func__(self.__LoadNPC)),

			# GUILD_BUILDING
			(98, ui.__mem_func__(self.__LoadGuildBuilding)),
			# END_OF_GUILD_BUILDING

			(100, ui.__mem_func__(self.__StartGame)),
		]
		if app.ENABLE_WOLFMAN_CHARACTER:
			self.loadStepList+=[(100, ui.__mem_func__(self.__LoadWolfman)),]

		self.__SetProgress(0)
		#self.__SetNext(self.__LoadMap)

	def OnUpdate(self):
		if len(self.loadStepList)>0:
			(progress, runFunc)=self.loadStepList[0]

			try:
				runFunc()
			except:
				self.errMsg.Show()
				self.loadStepList=[]


				import dbg
				dbg.TraceError(" !!! Failed to load game data : STEP [%d]" % (progress))

				#import shutil
				#import os
				#shutil.copyfile("syserr.txt", "errorlog.txt")
				#os.system("errorlog.exe")

				app.Exit()

				return

			self.loadStepList.pop(0)

			self.__SetProgress(progress)

	def __InitData(self):
		playerSettingModule.LoadGameData("INIT")

	def __RegisterDungeonMapName(self):
		background.RegisterDungeonMapName("metin2_map_spiderdungeon")
		background.RegisterDungeonMapName("metin2_map_monkeydungeon")
		background.RegisterDungeonMapName("metin2_map_monkeydungeon_02")
		background.RegisterDungeonMapName("metin2_map_monkeydungeon_03")
		background.RegisterDungeonMapName("metin2_map_deviltower1")

	def __RegisterSkill(self):

		race = net.GetMainActorRace()
		group = net.GetMainActorSkillGroup()
		empire = net.GetMainActorEmpire()

		playerSettingModule.RegisterSkill(race, group, empire)

	def __RegisterTitleName(self):
		for i in xrange(len(localeInfo.TITLE_NAME_LIST)):
			chrmgr.RegisterTitleName(i, localeInfo.TITLE_NAME_LIST[i])

		if app.ENABLE_RANKING_TITLES:
			for idx, itm in enumerate(localeInfo.RANKING_TITLE_LIST, start = 1):
				chrmgr.RegisterNewRankingTitleName(idx, itm)

	if app.ENABLE_STREAMER_ICON:
		def __RegisterStreamerName(self):
			for i in xrange(len(localeInfo.STREAMER_NAME_LIST)):
				chrmgr.RegisterStreamerName(i, localeInfo.STREAMER_NAME_LIST[i])


	def __RegisterColor(self):

		## Name
		NAME_COLOR_DICT = {
			chrmgr.NAMECOLOR_PC : colorInfo.CHR_NAME_RGB_PC,
			chrmgr.NAMECOLOR_NPC : colorInfo.CHR_NAME_RGB_NPC,
			chrmgr.NAMECOLOR_MOB : colorInfo.CHR_NAME_RGB_MOB,
			chrmgr.NAMECOLOR_PVP : colorInfo.CHR_NAME_RGB_PVP,
			chrmgr.NAMECOLOR_PK : colorInfo.CHR_NAME_RGB_PK,
			chrmgr.NAMECOLOR_PARTY : colorInfo.CHR_NAME_RGB_PARTY,
			chrmgr.NAMECOLOR_WARP : colorInfo.CHR_NAME_RGB_WARP,
			chrmgr.NAMECOLOR_WAYPOINT : colorInfo.CHR_NAME_RGB_WAYPOINT,
			chrmgr.NAMECOLOR_OFFLINESHOP : colorInfo.CHR_NAME_RGB_OFFLINESHOP,

			chrmgr.NAMECOLOR_EMPIRE_MOB : colorInfo.CHR_NAME_RGB_EMPIRE_MOB,
			chrmgr.NAMECOLOR_EMPIRE_NPC : colorInfo.CHR_NAME_RGB_EMPIRE_NPC,
			chrmgr.NAMECOLOR_EMPIRE_PC+1 : colorInfo.CHR_NAME_RGB_EMPIRE_PC_A,
			chrmgr.NAMECOLOR_EMPIRE_PC+2 : colorInfo.CHR_NAME_RGB_EMPIRE_PC_B,
			chrmgr.NAMECOLOR_EMPIRE_PC+3 : colorInfo.CHR_NAME_RGB_EMPIRE_PC_C,
		}
		for name, rgb in NAME_COLOR_DICT.items():
			chrmgr.RegisterNameColor(name, rgb[0], rgb[1], rgb[2])

		## Title
		TITLE_COLOR_DICT = (	colorInfo.TITLE_RGB_GOOD_4,
								colorInfo.TITLE_RGB_GOOD_3,
								colorInfo.TITLE_RGB_GOOD_2,
								colorInfo.TITLE_RGB_GOOD_1,
								colorInfo.TITLE_RGB_NORMAL,
								colorInfo.TITLE_RGB_EVIL_1,
								colorInfo.TITLE_RGB_EVIL_2,
								colorInfo.TITLE_RGB_EVIL_3,
								colorInfo.TITLE_RGB_EVIL_4,	)
		count = 0
		for rgb in TITLE_COLOR_DICT:
			chrmgr.RegisterTitleColor(count, rgb[0], rgb[1], rgb[2])
			count += 1
		if app.ENABLE_STREAMER_ICON:
			STREAMER_COLOR_DICT = ( colorInfo.STREAMER_COLOR_0,
									colorInfo.STREAMER_COLOR_1,
									colorInfo.STREAMER_COLOR_2,
									colorInfo.STREAMER_COLOR_3,
									colorInfo.STREAMER_COLOR_4)
			count_streamer = 0
			for rgb in STREAMER_COLOR_DICT:
				chrmgr.RegisterStreamerColor(count_streamer, rgb[0], rgb[1], rgb[2])
				count_streamer += 1
	def __RegisterEmotionIcon(self):
		emotion.RegisterEmotionIcons()

	def __LoadMap(self):
		net.Warp(self.playerX, self.playerY)

	def __LoadSound(self):
		playerSettingModule.LoadGameData("SOUND")
		constInfo.MuzikKontrol = False

	def __LoadEffect(self):
		playerSettingModule.LoadGameData("EFFECT")

	def __LoadWarrior(self):
		playerSettingModule.LoadGameData("WARRIOR")

	def __LoadAssassin(self):
		playerSettingModule.LoadGameData("ASSASSIN")

	def __LoadSura(self):
		playerSettingModule.LoadGameData("SURA")

	def __LoadShaman(self):
		playerSettingModule.LoadGameData("SHAMAN")

	if app.ENABLE_WOLFMAN_CHARACTER:
		def __LoadWolfman(self):
			playerSettingModule.LoadGameData("WOLFMAN")

	def __LoadSkill(self):
		playerSettingModule.LoadGameData("SKILL")

	def __LoadEnemy(self):
		playerSettingModule.LoadGameData("ENEMY")

	def __LoadNPC(self):
		playerSettingModule.LoadGameData("NPC")

	# GUILD_BUILDING
	def __LoadGuildBuilding(self):
		playerSettingModule.LoadGuildBuildingList(localeInfo.GUILD_BUILDING_LIST_TXT)
	# END_OF_GUILD_BUILDING

	def __StartGame(self):
		background.SetViewDistanceSet(background.DISTANCE0, 25600)
		"""
		background.SetViewDistanceSet(background.DISTANCE1, 19200)
		background.SetViewDistanceSet(background.DISTANCE2, 12800)
		background.SetViewDistanceSet(background.DISTANCE3, 9600)
		background.SetViewDistanceSet(background.DISTANCE4, 6400)
		"""
		background.SelectViewDistanceNum(background.DISTANCE0)

		app.SetGlobalCenterPosition(self.playerX, self.playerY)

		net.StartGame()
