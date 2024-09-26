import wndInfo as selfs
import os
import app
import dbg
import grp
import item
import background
import chr
import chrmgr
import player
import snd
import chat
import textTail
import snd
import net
import effect
import wndMgr
import fly
import systemSetting
import quest
import guild
import skill
import messenger
import localeInfo
import constInfo
import exchange
import ime
import time
import uimaintenance
import ui
import uiCommon
import uiPhaseCurtain
import uiMapNameShower
import uiAffectShower
import uiPlayerGauge
import uiCharacter
import uiTarget
import uibosstracking
# PRIVATE_SHOP_PRICE_LIST
import uiPrivateShopBuilder
# END_OF_PRIVATE_SHOP_PRICE_LIST
import mouseModule
import consoleModule
import localeInfo
import uiWhisperAdmin
import playerSettingModule
import interfaceModule

import musicInfo
import debugInfo
import stringCommander

from _weakref import proxy

if app.ENABLE_OFFLINE_SHOP_SYSTEM:
	import uiOfflineShopBuilder
	import uiOfflineShop

if app.ENABLE_SKILL_SELECT_FEATURE:
	import uiselectskill

if app.ENABLE_EVENT_MANAGER:
	import uiEventCalendarNew

# SCREENSHOT_CWDSAVE
SCREENSHOT_CWDSAVE = True
SCREENSHOT_DIR = None

cameraDistance = 1550.0
cameraPitch = 27.0
cameraRotation = 0.0
cameraHeight = 100.0

testAlignment = 0

class GameWindow(ui.ScriptWindow):
	def __init__(self, stream):
		ui.ScriptWindow.__init__(self, "GAME")
		self.SetWindowName("game")
		net.SetPhaseWindow(net.PHASE_WINDOW_GAME, self)
		player.SetGameWindow(self)
		if app.ENABLE_SKILL_SELECT_FEATURE:
			self.skillSelect = None

		self.quickSlotPageIndex = 0
		self.lastPKModeSendedTime = 0
		self.pressNumber = None
		if app.ENABLE_CAOS_EVENT:
			self.CaosEventQuestDialog = None
		self.guildWarQuestionDialog = None
		if app.AUTO_SHOUT:
			self.shouttime = 0
		selfs.wndInterface = None
		self.targetBoard = None
		self.console = None
		self.mapNameShower = None
		self.affectShower = None
		self.playerGauge = None
		self.UiSaplingSwitchbot = None
		if app.ENABLE_SKILL_SELECT_FEATURE:
			self.skillSelect = uiselectskill.SkillSelectWindow()
			self.skillSelect.Hide()
		self.stream=stream
		selfs.wndInterface = interfaceModule.Interface()
		if app.ENABLE_MULTI_FARM_BLOCK:
			constInfo.SetInterfaceInstance(selfs.wndInterface)

		selfs.wndInterface.MakeInterface()
		selfs.wndInterface.ShowDefaultWindows()
		constInfo.SetInterfaceInstance(selfs.wndInterface)

		self.curtain = uiPhaseCurtain.PhaseCurtain()
		self.curtain.speed = 0.03
		self.curtain.Hide()

		self.targetBoard = uiTarget.TargetBoard()
		self.targetBoard.SetWhisperEvent(ui.__mem_func__(selfs.wndInterface.OpenWhisperDialog))
		self.targetBoard.Hide()

		self.console = consoleModule.ConsoleWindow()
		self.console.BindGameClass(self)
		self.console.SetConsoleSize(wndMgr.GetScreenWidth(), 200)
		self.console.Hide()

		self.mapNameShower = uiMapNameShower.MapNameShower()
		self.affectShower = uiAffectShower.AffectShower()

		self.playerGauge = uiPlayerGauge.PlayerGauge(self)
		self.playerGauge.Hide()

		self.whisperAdmin = uiWhisperAdmin.AdminWhisperManager()
		self.whisperAdmin.Close()

		self.bosstracking = uibosstracking.BossTrackingSystemWindow()
		self.bosstracking.Hide()


		self.itemDropQuestionDialog = None

		self.__SetQuickSlotMode()

		self.__ServerCommand_Build()
		self.__ProcessPreservedServerCommand()
		self.timeLine = ui.TextLine()
		self.timeLine.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		self.timeLine.SetFontColor(10, 10, 0)
		self.timeLine.SetPosition((wndMgr.GetScreenWidth() - 130) / 2, 175)
		self.isCameraMoving = False
		self.cameraMovementProgress = 0.0

	def __del__(self):
		player.SetGameWindow(0)
		net.ClearPhaseWindow(net.PHASE_WINDOW_GAME, self)
		ui.ScriptWindow.__del__(self)

	def Open(self):
		app.SetFrameSkip(1)

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())

		self.quickSlotPageIndex = 0
		self.PickingCharacterIndex = -1
		self.PickingItemIndex = -1
		self.consoleEnable = False
		self.isShowDebugInfo = False
		self.ShowNameFlag = False

		self.enableXMasBoom = False
		self.enableXMasMuzik = False
		self.startTimeXMasBoom = 0.0
		self.startTimeXMasMuzik = 0.0
		self.indexXMasBoom = 0
		self.indexXMasMuzik = 0

		global cameraDistance, cameraPitch, cameraRotation, cameraHeight

		app.SetCamera(cameraDistance, cameraPitch, cameraRotation, cameraHeight)

		constInfo.SET_DEFAULT_CAMERA_MAX_DISTANCE()
		constInfo.SET_DEFAULT_CHRNAME_COLOR()
		constInfo.SET_DEFAULT_FOG_LEVEL()
		constInfo.SET_DEFAULT_CONVERT_EMPIRE_LANGUAGE_ENABLE()
		constInfo.SET_DEFAULT_USE_ITEM_WEAPON_TABLE_ATTACK_BONUS()
		constInfo.SET_DEFAULT_USE_SKILL_EFFECT_ENABLE()

		# TWO_HANDED_WEAPON_ATTACK_SPEED_UP
		constInfo.SET_TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE()
		# END_OF_TWO_HANDED_WEAPON_ATTACK_SPEED_UP

		import event
		event.SetLeftTimeString(localeInfo.UI_LEFT_TIME)

		textTail.EnablePKTitle(constInfo.PVPMODE_ENABLE)
		maintenance = uimaintenance.MaintenanceWindow()
		maintenance.Open(constInfo.maintenanceinfo["time"],constInfo.maintenanceinfo["duration"],constInfo.maintenanceinfo["active"])
		self.maintenance = maintenance


		if constInfo.PVPMODE_TEST_ENABLE:
			self.testPKMode = ui.TextLine()
			self.testPKMode.SetFontName(localeInfo.UI_DEF_FONT)
			self.testPKMode.SetPosition(0, 15)
			self.testPKMode.SetWindowHorizontalAlignCenter()
			self.testPKMode.SetHorizontalAlignCenter()
			self.testPKMode.SetFeather()
			self.testPKMode.SetOutline()
			self.testPKMode.Show()

			self.testAlignment = ui.TextLine()
			self.testAlignment.SetFontName(localeInfo.UI_DEF_FONT)
			self.testAlignment.SetPosition(0, 35)
			self.testAlignment.SetWindowHorizontalAlignCenter()
			self.testAlignment.SetHorizontalAlignCenter()
			self.testAlignment.SetFeather()
			self.testAlignment.SetOutline()
			self.testAlignment.Show()

		self.__BuildKeyDict()
		self.__BuildDebugInfo()

		# PRIVATE_SHOP_PRICE_LIST
		uiPrivateShopBuilder.Clear()
		if app.ENABLE_OFFLINE_SHOP_SYSTEM: uiOfflineShopBuilder.Clear()
		# END_OF_PRIVATE_SHOP_PRICE_LIST

		# UNKNOWN_UPDATE
		exchange.InitTrading()
		# END_OF_UNKNOWN_UPDATE


		## Sound
		snd.SetMusicVolume(systemSetting.GetMusicVolume()*net.GetFieldMusicVolume())
		snd.SetSoundVolume(systemSetting.GetSoundVolume())

		netFieldMusicFileName = net.GetFieldMusicFileName()
		if netFieldMusicFileName:
			snd.FadeInMusic("BGM/" + netFieldMusicFileName)
		elif musicInfo.fieldMusic != "":
			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)

		self.__SetQuickSlotMode()
		self.__SelectQuickPage(self.quickSlotPageIndex)

		self.SetFocus()
		self.Show()
		app.ShowCursor()

		net.SendEnterGamePacket()

		# START_GAME_ERROR_EXIT
		try:
			self.StartGame()
		except:
			import exception
			exception.Abort("GameWindow.Open")
		# END_OF_START_GAME_ERROR_EXIT

		# ex) cubeInformation[20383] = [ {"rewordVNUM": 72723, "rewordCount": 1, "materialInfo": "101,1&102,2", "price": 999 }, ... ]
		if app.AUTO_SHOUT:
			self.shouttime = app.GetTime()+15 # kac saniyede bir ataca?ı
		self.cubeInformation = {}
		self.currentCubeNPC = 0
		self.isCameraMoving = True
		self.cameraMovementProgress = 0.0

		if systemSetting.GetNightMode():
			systemSetting.SetNightMode(True)
			background.RegisterEnvironmentData(1, constInfo.ENVIRONMENT_NIGHT)
			background.SetEnvironmentData(1)
		else:
			systemSetting.SetNightMode(False)
			background.SetEnvironmentData(0)

		for i in xrange(10):
			if systemSetting.IsSnowTexturesMode():
				if background.GetCurrentMapName():
					snow_maps = [
						"metin2_map_a1",
						"metin2_map_b1",
						"metin2_map_c1"
					]
					snow_maps_textures = {
						"metin2_map_a1" : "textureset\metin2_a1_snow.txt",
						"metin2_map_b1" : "textureset\metin2_b1_snow.txt",
						"metin2_map_c1" : "textureset\metin2_c1_snow.txt", }
					if str(background.GetCurrentMapName()) in snow_maps:
						background.TextureChange(snow_maps_textures[str(background.GetCurrentMapName())])
						
						

		if app.ENABLE_FOG_FIX:
			if systemSetting.IsFogMode():
				background.SetEnvironmentFog(True)
			else:
				background.SetEnvironmentFog(False)	

	if app.ENABLE_NEW_BIOLOG:
		def Binary_BioUpdate(self, needitem, soulitem, givecount, state, reqcount, aff_type, aff_value, aff_type2, aff_value2, aff_type3, aff_value3, aff_type4, aff_value4, chance, time):
			selfs.wndInterface.UpdateBiologInfo(needitem, soulitem, givecount, state, reqcount, aff_type, aff_value, aff_type2, aff_value2, aff_type3, aff_value3, aff_type4, aff_value4, chance, time)

		def SendBiologRequest(self):
			net.SendChatPacket("/open_biopnl")

		if app.ENABLE_MULTI_FARM_BLOCK:
			app.SetMultiFarmExeIcon(0)


	def Close(self):
		self.Hide()

		global cameraDistance, cameraPitch, cameraRotation, cameraHeight
		(cameraDistance, cameraPitch, cameraRotation, cameraHeight) = app.GetCamera()

		if musicInfo.fieldMusic != "":
			snd.FadeOutMusic("BGM/"+ musicInfo.fieldMusic)

		self.onPressKeyDict = None
		self.onClickKeyDict = None
		if app.ENABLE_SKILL_SELECT_FEATURE and self.skillSelect:
			self.skillSelect.Destroy()
			self.skillSelect = None

		chat.Close()
		snd.StopAllSound()
		grp.InitScreenEffect()
		chr.Destroy()
		textTail.Clear()
		quest.Clear()
		background.Destroy()
		guild.Destroy()
		messenger.Destroy()
		skill.ClearSkillData()
		wndMgr.Unlock()
		mouseModule.mouseController.DeattachObject()

		if self.guildWarQuestionDialog:
			self.guildWarQuestionDialog.Close()

		self.guildNameBoard = None
		self.partyRequestQuestionDialog = None
		self.partyInviteQuestionDialog = None
		self.guildInviteQuestionDialog = None
		self.guildWarQuestionDialog = None
		self.messengerAddFriendQuestion = None

		# UNKNOWN_UPDATE
		self.itemDropQuestionDialog = None
		# END_OF_UNKNOWN_UPDATE

		# QUEST_CONFIRM
		self.confirmDialog = None
		# END_OF_QUEST_CONFIRM
		self.maintenance.Close()
		self.maintenance = None
		self.PrintCoord = None
		self.FrameRate = None
		self.Pitch = None
		self.Splat = None
		self.TextureNum = None
		self.ObjectNum = None
		self.ViewDistance = None
		self.PrintMousePos = None

		self.ClearDictionary()

		self.playerGauge = None
		self.whisperAdmin = None
		self.mapNameShower = None
		self.affectShower = None

		if self.console:
			self.console.BindGameClass(0)
			self.console.Close()
			self.console=None

		if self.bosstracking:
			self.bosstracking.Destroy()
			self.bosstracking = None

		if self.targetBoard:
			self.targetBoard.Destroy()
			self.targetBoard = None

		if selfs.wndInterface:
			selfs.wndInterface.HideAllWindows()
			selfs.wndInterface.Close()
			selfs.wndInterface=None

		player.ClearSkillDict()
		player.ResetCameraRotation()

		self.KillFocus()
		if app.ENABLE_MULTI_FARM_BLOCK:
			constInfo.SetInterfaceInstance(None)

		constInfo.SetInterfaceInstance(None)
		app.HideCursor()

		print "---------------------------------------------------------------------------- CLOSE GAME WINDOW"
	def Maintenancegui(self,time,duration,active):
		constInfo.maintenanceinfo["time"] = int(time)
		constInfo.maintenanceinfo["duration"] = int(duration)
		constInfo.maintenanceinfo["active"] = int(active)

		if constInfo.maintenanceinfo["active"] == 1:
			self.maintenance.Open(constInfo.maintenanceinfo["time"],constInfo.maintenanceinfo["duration"],constInfo.maintenanceinfo["active"])
		else:
			self.maintenance.Close()

	def __BuildKeyDict(self):
		onPressKeyDict = {}


		onPressKeyDict[app.DIK_1]	= lambda : self.__PressNumKey(1)
		onPressKeyDict[app.DIK_2]	= lambda : self.__PressNumKey(2)
		onPressKeyDict[app.DIK_3]	= lambda : self.__PressNumKey(3)
		onPressKeyDict[app.DIK_4]	= lambda : self.__PressNumKey(4)
		onPressKeyDict[app.DIK_5]	= lambda : self.__PressNumKey(5)
		onPressKeyDict[app.DIK_6]	= lambda : self.__PressNumKey(6)
		onPressKeyDict[app.DIK_7]	= lambda : self.__PressNumKey(7)
		onPressKeyDict[app.DIK_8]	= lambda : self.__PressNumKey(8)
		onPressKeyDict[app.DIK_9]	= lambda : self.__PressNumKey(9)
		onPressKeyDict[app.DIK_F1]	= lambda : self.__PressQuickSlot(4)
		onPressKeyDict[app.DIK_F2]	= lambda : self.__PressQuickSlot(5)
		onPressKeyDict[app.DIK_F3]	= lambda : self.__PressQuickSlot(6)
		onPressKeyDict[app.DIK_F4]	= lambda : self.__PressQuickSlot(7)
		if app.__AUTO_HUNT__:
			onPressKeyDict[app.DIK_F5]	= lambda : self.interface.OpenAutoHunt()

#		if app.ENABLE_NEW_RANKING:
#			onPressKeyDict[app.DIK_F6]	= lambda : selfs.wndInterface.OpenRank()
#		#
#		if app.ENABLE_NEW_MISSIONS:
#			onPressKeyDict[app.DIK_F7]	= lambda : self.SendOpenMissionPanelRequest()
		#onPressKeyDict[app.DIK_F7]	= lambda : self.whisperAdmin.OpenWindow()
		#onPressKeyDict[app.DIK_F12]	= lambda : selfs.maintenanceadminopen()
		# onPressKeyDict[app.DIK_F5]	= lambda : self.EventCalendar()
		# onPressKeyDict[app.DIK_F6]	= lambda : selfs.wndInterface.OpenDeleteItem()
		#onPressKeyDict[app.DIK_F7]	= lambda : self.BossTracking()
		# onPressKeyDict[app.DIK_F8]	= lambda : selfs.wndInterface.OpenBiologWindow()

		onPressKeyDict[app.DIK_LALT]		= lambda : self.ShowName()
		onPressKeyDict[app.DIK_LCONTROL]	= lambda : self.ShowMouseImage()
		onPressKeyDict[app.DIK_SYSRQ]		= lambda : self.SaveScreen()
		onPressKeyDict[app.DIK_SPACE]		= lambda : self.StartAttack()

		onPressKeyDict[app.DIK_UP]			= lambda : self.MoveUp()
		onPressKeyDict[app.DIK_DOWN]		= lambda : self.MoveDown()
		onPressKeyDict[app.DIK_LEFT]		= lambda : self.MoveLeft()
		onPressKeyDict[app.DIK_RIGHT]		= lambda : self.MoveRight()
		onPressKeyDict[app.DIK_W]			= lambda : self.MoveUp()
		onPressKeyDict[app.DIK_S]			= lambda : self.MoveDown()
		onPressKeyDict[app.DIK_A]			= lambda : self.MoveLeft()
		onPressKeyDict[app.DIK_D]			= lambda : self.MoveRight()

		onPressKeyDict[app.DIK_E]			= lambda: app.RotateCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_R]			= lambda: app.ZoomCamera(app.CAMERA_TO_NEGATIVE)
		#onPressKeyDict[app.DIK_F]			= lambda: app.ZoomCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_T]			= lambda: app.PitchCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_G]			= self.__PressGKey
		onPressKeyDict[app.DIK_Q]			= self.__PressQKey

		onPressKeyDict[app.DIK_NUMPAD9]		= lambda: app.MovieResetCamera()
		onPressKeyDict[app.DIK_NUMPAD4]		= lambda: app.MovieRotateCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_NUMPAD6]		= lambda: app.MovieRotateCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_PGUP]		= lambda: app.MovieZoomCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_PGDN]		= lambda: app.MovieZoomCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_NUMPAD8]		= lambda: app.MoviePitchCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_NUMPAD2]		= lambda: app.MoviePitchCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_GRAVE]		= lambda : self.PickUpItem()
		onPressKeyDict[app.DIK_Z]			= lambda : self.PickUpItem()
		onPressKeyDict[app.DIK_C]			= lambda state = "STATUS": selfs.wndInterface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_V]			= lambda state = "SKILL": selfs.wndInterface.ToggleCharacterWindow(state)
		#onPressKeyDict[app.DIK_B]			= lambda state = "EMOTICON": selfs.wndInterface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_N]			= lambda state = "QUEST": selfs.wndInterface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_I]			= lambda : selfs.wndInterface.ToggleInventoryWindow()
		onPressKeyDict[app.DIK_K]			= lambda : selfs.wndInterface.ToggleExInventoryWindow()
		#onPressKeyDict[app.DIK_O]			= lambda : selfs.wndInterface.ToggleDragonSoulWindowWithNoInfo()
		onPressKeyDict[app.DIK_M]			= lambda : selfs.wndInterface.PressMKey()
		#onPressKeyDict[app.DIK_H]			= lambda : selfs.wndInterface.OpenHelpWindow()
		onPressKeyDict[app.DIK_ADD]			= lambda : selfs.wndInterface.MiniMapScaleUp()
		onPressKeyDict[app.DIK_SUBTRACT]	= lambda : selfs.wndInterface.MiniMapScaleDown()
		onPressKeyDict[app.DIK_L]			= lambda : selfs.wndInterface.ToggleChatLogWindow()
		onPressKeyDict[app.DIK_COMMA]		= lambda : self.ShowConsole()		# "`" key
		onPressKeyDict[app.DIK_LSHIFT]		= lambda : self.__SetQuickPageMode()

		onPressKeyDict[app.DIK_J]			= lambda : self.__PressJKey()
		onPressKeyDict[app.DIK_H]			= lambda : self.__PressHKey()
		onPressKeyDict[app.DIK_B]			= lambda : self.__PressBKey()
		onPressKeyDict[app.DIK_F]			= lambda : self.__PressFKey()

		# CUBE_TEST
		#onPressKeyDict[app.DIK_K]			= lambda : selfs.wndInterface.OpenCubeWindow()
		# CUBE_TEST_END

		self.onPressKeyDict = onPressKeyDict

		onClickKeyDict = {}
		onClickKeyDict[app.DIK_UP] = lambda : self.StopUp()
		onClickKeyDict[app.DIK_DOWN] = lambda : self.StopDown()
		onClickKeyDict[app.DIK_LEFT] = lambda : self.StopLeft()
		onClickKeyDict[app.DIK_RIGHT] = lambda : self.StopRight()
		onClickKeyDict[app.DIK_SPACE] = lambda : self.EndAttack()

		onClickKeyDict[app.DIK_W] = lambda : self.StopUp()
		onClickKeyDict[app.DIK_S] = lambda : self.StopDown()
		onClickKeyDict[app.DIK_A] = lambda : self.StopLeft()
		onClickKeyDict[app.DIK_D] = lambda : self.StopRight()
		onClickKeyDict[app.DIK_Q] = lambda: app.RotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_E] = lambda: app.RotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_R] = lambda: app.ZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_F] = lambda: app.ZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_T] = lambda: app.PitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_G] = lambda: self.__ReleaseGKey()
		onClickKeyDict[app.DIK_NUMPAD4] = lambda: app.MovieRotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD6] = lambda: app.MovieRotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_PGUP] = lambda: app.MovieZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_PGDN] = lambda: app.MovieZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD8] = lambda: app.MoviePitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD2] = lambda: app.MoviePitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_LALT] = lambda: self.HideName()
		onClickKeyDict[app.DIK_LCONTROL] = lambda: self.HideMouseImage()
		onClickKeyDict[app.DIK_LSHIFT] = lambda: self.__SetQuickSlotMode()

		#if constInfo.PVPMODE_ACCELKEY_ENABLE:
		#	onClickKeyDict[app.DIK_B] = lambda: self.ChangePKMode()

		self.onClickKeyDict=onClickKeyDict

	def __PressNumKey(self,num):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):

			if num >= 1 and num <= 9:
				if(chrmgr.IsPossibleEmoticon(-1)):
					chrmgr.SetEmoticon(-1,int(num)-1)
					net.SendEmoticon(int(num)-1)
		else:
			if num >= 1 and num <= 4:
				self.pressNumber(num-1)

	def __ClickBKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			return
		else:
			if constInfo.PVPMODE_ACCELKEY_ENABLE:
				self.ChangePKMode()


	def	__PressJKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			if player.IsMountingHorse():
				net.SendChatPacket("/unmount")
			else:
				#net.SendChatPacket("/user_horse_ride")
				if app.ENABLE_OFFLINE_SHOP_SYSTEM:
					if not uiPrivateShopBuilder.IsBuildingPrivateShop() and not uiOfflineShopBuilder.IsBuildingOfflineShop():
						for i in xrange(player.INVENTORY_PAGE_SIZE):
							if player.GetItemIndex(i) in (71114, 71116, 71118, 71120):
								net.SendItemUsePacket(i)
								break
				else:
					if not uiPrivateShopBuilder.IsBuildingPrivateShop():
						for i in xrange(player.INVENTORY_PAGE_SIZE*player.INVENTORY_PAGE_COUNT):
							if player.GetItemIndex(i) in (71114, 71116, 71118, 71120):
								net.SendItemUsePacket(i)
								break
	def	__PressHKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_ride")
		else:
			selfs.wndInterface.OpenHelpWindow()

	def	__PressBKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_back")
		else:
			state = "EMOTICON"
			selfs.wndInterface.ToggleCharacterWindow(state)

	def	__PressFKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_feed")
		else:
			app.ZoomCamera(app.CAMERA_TO_POSITIVE)

	def __PressGKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/ride")
		else:
			if self.ShowNameFlag:
				selfs.wndInterface.ToggleGuildWindow()
			else:
				app.PitchCamera(app.CAMERA_TO_POSITIVE)

	def	__ReleaseGKey(self):
		app.PitchCamera(app.CAMERA_STOP)

	def __PressQKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			if 0==interfaceModule.IsQBHide:
				interfaceModule.IsQBHide = 1
				selfs.wndInterface.HideAllQuestButton()
			else:
				interfaceModule.IsQBHide = 0
				selfs.wndInterface.ShowAllQuestButton()
		else:
			app.RotateCamera(app.CAMERA_TO_NEGATIVE)

	def __SetQuickSlotMode(self):
		self.pressNumber=ui.__mem_func__(self.__PressQuickSlot)

	def __SetQuickPageMode(self):
		self.pressNumber=ui.__mem_func__(self.__SelectQuickPage)

	def __PressQuickSlot(self, localSlotIndex):
		if localeInfo.IsARABIC():
			if 0 <= localSlotIndex and localSlotIndex < 4:
				player.RequestUseLocalQuickSlot(3-localSlotIndex)
			else:
				player.RequestUseLocalQuickSlot(11-localSlotIndex)
		else:
			player.RequestUseLocalQuickSlot(localSlotIndex)

	def __SelectQuickPage(self, pageIndex):
		self.quickSlotPageIndex = pageIndex
		player.SetQuickPage(pageIndex)

	def ToggleDebugInfo(self):
		self.isShowDebugInfo = not self.isShowDebugInfo

		if self.isShowDebugInfo:
			self.PrintCoord.Show()
			self.FrameRate.Show()
			self.Pitch.Show()
			self.Splat.Show()
			self.TextureNum.Show()
			self.ObjectNum.Show()
			self.ViewDistance.Show()
			self.PrintMousePos.Show()
		else:
			self.PrintCoord.Hide()
			self.FrameRate.Hide()
			self.Pitch.Hide()
			self.Splat.Hide()
			self.TextureNum.Hide()
			self.ObjectNum.Hide()
			self.ViewDistance.Hide()
			self.PrintMousePos.Hide()

	def __BuildDebugInfo(self):
		## Character Position Coordinate
		self.PrintCoord = ui.TextLine()
		self.PrintCoord.SetFontName(localeInfo.UI_DEF_FONT)
		self.PrintCoord.SetPosition(wndMgr.GetScreenWidth() - 270, 0)

		## Frame Rate
		self.FrameRate = ui.TextLine()
		self.FrameRate.SetFontName(localeInfo.UI_DEF_FONT)
		self.FrameRate.SetPosition(wndMgr.GetScreenWidth() - 270, 20)

		## Camera Pitch
		self.Pitch = ui.TextLine()
		self.Pitch.SetFontName(localeInfo.UI_DEF_FONT)
		self.Pitch.SetPosition(wndMgr.GetScreenWidth() - 270, 40)

		## Splat
		self.Splat = ui.TextLine()
		self.Splat.SetFontName(localeInfo.UI_DEF_FONT)
		self.Splat.SetPosition(wndMgr.GetScreenWidth() - 270, 60)

		##
		self.PrintMousePos = ui.TextLine()
		self.PrintMousePos.SetFontName(localeInfo.UI_DEF_FONT)
		self.PrintMousePos.SetPosition(wndMgr.GetScreenWidth() - 270, 80)

		# TextureNum
		self.TextureNum = ui.TextLine()
		self.TextureNum.SetFontName(localeInfo.UI_DEF_FONT)
		self.TextureNum.SetPosition(wndMgr.GetScreenWidth() - 270, 100)

		self.ObjectNum = ui.TextLine()
		self.ObjectNum.SetFontName(localeInfo.UI_DEF_FONT)
		self.ObjectNum.SetPosition(wndMgr.GetScreenWidth() - 270, 120)

		self.ViewDistance = ui.TextLine()
		self.ViewDistance.SetFontName(localeInfo.UI_DEF_FONT)
		self.ViewDistance.SetPosition(0, 0)
		self.timeLine.SetWindowHorizontalAlignCenter()
		self.timeLine.SetHorizontalAlignCenter()
		self.timeLine.SetFeather()
		self.timeLine.SetOutline()
		self.timeLine.Show()

	def __NotifyError(self, msg):
		chat.AppendChat(chat.CHAT_TYPE_INFO, msg)

	def ChangePKMode(self):

		if not app.IsPressed(app.DIK_LCONTROL):
			return

		if player.GetStatus(player.LEVEL)<constInfo.PVPMODE_PROTECTED_LEVEL:
			self.__NotifyError(localeInfo.OPTION_PVPMODE_PROTECT % (constInfo.PVPMODE_PROTECTED_LEVEL))
			return

		curTime = app.GetTime()
		if curTime - self.lastPKModeSendedTime < constInfo.PVPMODE_ACCELKEY_DELAY:
			return

		self.lastPKModeSendedTime = curTime

		curPKMode = player.GetPKMode()
		nextPKMode = curPKMode + 1
		if nextPKMode == player.PK_MODE_PROTECT:
			if 0 == player.GetGuildID():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_CANNOT_SET_GUILD_MODE)
				nextPKMode = 0
			else:
				nextPKMode = player.PK_MODE_GUILD

		elif nextPKMode == player.PK_MODE_MAX_NUM:
			nextPKMode = 0

		net.SendChatPacket("/PKMode " + str(nextPKMode))
		print "/PKMode " + str(nextPKMode)

	def OnChangePKMode(self):

		selfs.wndInterface.OnChangePKMode()

		try:
			self.__NotifyError(localeInfo.OPTION_PVPMODE_MESSAGE_DICT[player.GetPKMode()])
		except KeyError:
			print "UNKNOWN PVPMode[%d]" % (player.GetPKMode())

		if constInfo.PVPMODE_TEST_ENABLE:
			curPKMode = player.GetPKMode()
			alignment, grade = chr.testGetPKData()
			self.pkModeNameDict = { 0 : "PEACE", 1 : "REVENGE", 2 : "FREE", 3 : "PROTECT", }
			self.testPKMode.SetText("Current PK Mode : " + self.pkModeNameDict.get(curPKMode, "UNKNOWN"))
			self.testAlignment.SetText("Current Alignment : " + str(alignment) + " (" + localeInfo.TITLE_NAME_LIST[grade] + ")")

	###############################################################################################
	###############################################################################################
	## Game Callback Functions

	# Start
	def StartGame(self):
		self.RefreshInventory()
		self.RefreshEquipment()
		self.RefreshCharacter()
		self.RefreshSkill()
		self.OyuncuAdiGonder()

	def OyuncuAdiGonder(self):
		id = player.GetName()
		kaydet = open("Lib/player.cfg", "w")
		kaydet.write("NAME						" + id)
		kaydet.close()

	# Refresh
	def CheckGameButton(self):
		if selfs.wndInterface:
			selfs.wndInterface.CheckGameButton()

	def RefreshAlignment(self):
		selfs.wndInterface.RefreshAlignment()

	if app.WJ_SHOW_ALL_CHANNEL:
		def BINARY_OnChannelPacket(self, channel):
			import net
			dict = {'name' : 'Umay2Games'} # Replace with your server name.
			net.SetServerInfo((localeInfo.TEXT_CHANNEL % (dict['name'], channel)).strip())
			if selfs.wndInterface:
				selfs.wndInterface.wndMiniMap.serverInfo.SetText(net.GetServerInfo())

	def RefreshStatus(self):
		self.CheckGameButton()

		if selfs.wndInterface:
			selfs.wndInterface.RefreshStatus()

		if self.playerGauge:
			self.playerGauge.RefreshGauge()

	def RefreshStamina(self):
		selfs.wndInterface.RefreshStamina()

	def RefreshSkill(self):
		self.CheckGameButton()
		if selfs.wndInterface:
			selfs.wndInterface.RefreshSkill()

	def RefreshQuest(self):
		selfs.wndInterface.RefreshQuest()

	def RefreshMessenger(self):
		selfs.wndInterface.RefreshMessenger()

	def RefreshGuildInfoPage(self):
		selfs.wndInterface.RefreshGuildInfoPage()

	def RefreshGuildBoardPage(self):
		selfs.wndInterface.RefreshGuildBoardPage()

	def RefreshGuildMemberPage(self):
		selfs.wndInterface.RefreshGuildMemberPage()

	def RefreshGuildMemberPageGradeComboBox(self):
		selfs.wndInterface.RefreshGuildMemberPageGradeComboBox()

	def RefreshGuildSkillPage(self):
		selfs.wndInterface.RefreshGuildSkillPage()

	def RefreshGuildGradePage(self):
		selfs.wndInterface.RefreshGuildGradePage()

	def RefreshMobile(self):
		if selfs.wndInterface:
			selfs.wndInterface.RefreshMobile()

	def OnMobileAuthority(self):
		selfs.wndInterface.OnMobileAuthority()

	def OnBlockMode(self, mode):
		selfs.wndInterface.OnBlockMode(mode)

	def OpenQuestWindow(self, skin, idx):
		selfs.wndInterface.OpenQuestWindow(skin, idx)

	def AskGuildName(self):

		guildNameBoard = uiCommon.InputDialog()
		guildNameBoard.SetTitle(localeInfo.GUILD_NAME)
		guildNameBoard.SetAcceptEvent(ui.__mem_func__(self.ConfirmGuildName))
		guildNameBoard.SetCancelEvent(ui.__mem_func__(self.CancelGuildName))
		guildNameBoard.Open()

		self.guildNameBoard = guildNameBoard

	def ConfirmGuildName(self):
		guildName = self.guildNameBoard.GetText()
		if not guildName:
			return

		if net.IsInsultIn(guildName):
			self.PopupMessage(localeInfo.GUILD_CREATE_ERROR_INSULT_NAME)
			return

		net.SendAnswerMakeGuildPacket(guildName)
		self.guildNameBoard.Close()
		self.guildNameBoard = None
		return True

	def CancelGuildName(self):
		self.guildNameBoard.Close()
		self.guildNameBoard = None
		return True

	## Refine
	def PopupMessage(self, msg):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, 0, localeInfo.UI_OK)

	def OpenRefineDialog(self, targetItemPos, nextGradeItemVnum, cost, prob, type=0):
		selfs.wndInterface.OpenRefineDialog(targetItemPos, nextGradeItemVnum, cost, prob, type)

	def AppendMaterialToRefineDialog(self, vnum, count):
		selfs.wndInterface.AppendMaterialToRefineDialog(vnum, count)

	def RunUseSkillEvent(self, slotIndex, coolTime):
		selfs.wndInterface.OnUseSkill(slotIndex, coolTime)

	def ClearAffects(self):
		self.affectShower.ClearAffects()

	def SetAffect(self, affect):
		self.affectShower.SetAffect(affect)

	def ResetAffect(self, affect):
		self.affectShower.ResetAffect(affect)

	# UNKNOWN_UPDATE
	def BINARY_NEW_AddAffect(self, type, pointIdx, value, duration):
		if app.__NEW_POTION__:
			if type == chr.NEW_AFFECT_NEW_POTION:
				type += 50770 + pointIdx

		self.affectShower.BINARY_NEW_AddAffect(type, pointIdx, value, duration)
		if app.__AUTO_HUNT__:
			if constInfo.autoHuntAutoLoginDict["status"] == 1 and constInfo.autoHuntAutoLoginDict["leftTime"] == -2:
				constInfo.autoHuntAutoLoginDict["leftTime"] = app.GetGlobalTimeStamp() + 2

	def BINARY_NEW_RemoveAffect(self, type, pointIdx):
		if app.__NEW_POTION__:
			if type == chr.NEW_AFFECT_NEW_POTION:
				type += 50770 + pointIdx
		self.affectShower.BINARY_NEW_RemoveAffect(type, pointIdx)
		if app.__AUTO_HUNT__:
			if type == chr.NEW_AFFECT_AUTO_HUNT:
				net.SendChatPacket("/auto_hunt end")

	# END_OF_UNKNOWN_UPDATE

	def ActivateSkillSlot(self, slotIndex):
		if selfs.wndInterface:
			selfs.wndInterface.OnActivateSkill(slotIndex)

	def DeactivateSkillSlot(self, slotIndex):
		if selfs.wndInterface:
			selfs.wndInterface.OnDeactivateSkill(slotIndex)

	def RefreshEquipment(self):
		if selfs.wndInterface:
			selfs.wndInterface.RefreshInventory()

	def RefreshInventory(self):
		if selfs.wndInterface:
			selfs.wndInterface.RefreshInventory()

	def RefreshCharacter(self):
		if selfs.wndInterface:
			selfs.wndInterface.RefreshCharacter()

	def OnGameOver(self):
		self.CloseTargetBoard()
		self.OpenRestartDialog()

	def OpenRestartDialog(self):
		selfs.wndInterface.OpenRestartDialog()

	def ChangeCurrentSkill(self, skillSlotNumber):
		selfs.wndInterface.OnChangeCurrentSkill(skillSlotNumber)

	## TargetBoard
	def SetPCTargetBoard(self, vid, name):
		self.targetBoard.Open(vid, name)

		if app.IsPressed(app.DIK_LCONTROL):

			if not player.IsSameEmpire(vid):
				return

			if player.IsMainCharacterIndex(vid):
				return
			elif chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(vid):
				return

			selfs.wndInterface.OpenWhisperDialog(name)


	def RefreshTargetBoardByVID(self, vid):
		self.targetBoard.RefreshByVID(vid)

	def RefreshTargetBoardByName(self, name):
		self.targetBoard.RefreshByName(name)

	def __RefreshTargetBoard(self):
		self.targetBoard.Refresh()
	if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
		def SetHPTargetBoard(self, vid, hpPercentage, iMinHP, iMaxHP):
			if vid != self.targetBoard.GetTargetVID():
				self.targetBoard.ResetTargetBoard()
				self.targetBoard.SetEnemyVID(vid)
			
			self.targetBoard.SetHP(hpPercentage, iMinHP, iMaxHP)
			self.targetBoard.Show()
	else:
		def SetHPTargetBoard(self, vid, hpPercentage):
			if vid != self.targetBoard.GetTargetVID():
				self.targetBoard.ResetTargetBoard()
				self.targetBoard.SetEnemyVID(vid)

			self.targetBoard.SetHP(hpPercentage)
			self.targetBoard.Show()

	def CloseTargetBoardIfDifferent(self, vid):
		if vid != self.targetBoard.GetTargetVID():
			self.targetBoard.Close()

	def CloseTargetBoard(self):
		self.targetBoard.Close()

	## View Equipment
	def OpenEquipmentDialog(self, vid):
		selfs.wndInterface.OpenEquipmentDialog(vid)

	def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count):
		selfs.wndInterface.SetEquipmentDialogItem(vid, slotIndex, vnum, count)

	def SetEquipmentDialogSocket(self, vid, slotIndex, socketIndex, value):
		selfs.wndInterface.SetEquipmentDialogSocket(vid, slotIndex, socketIndex, value)

	def SetEquipmentDialogAttr(self, vid, slotIndex, attrIndex, type, value):
		selfs.wndInterface.SetEquipmentDialogAttr(vid, slotIndex, attrIndex, type, value)

	# SHOW_LOCAL_MAP_NAME
	def ShowMapName(self, mapName, x, y):

		if self.mapNameShower:
			self.mapNameShower.ShowMapName(mapName, x, y)

		if selfs.wndInterface:
			selfs.wndInterface.SetMapName(mapName)
	# END_OF_SHOW_LOCAL_MAP_NAME

	if app.ENABLE_EXTENDED_BATTLE_PASS:
		def BINARY_ExtOpenBattlePass(self):
			if selfs.wndInterface:
				selfs.wndInterface.ReciveOpenExtBattlePass()
			
		def BINARY_ExtBattlePassAddGeneralInfo(self, BattlePassType, BattlePassName, BattlePassID, battlePassStartTime, battlePassEndTime):
			if selfs.wndInterface:
				selfs.wndInterface.AddExtendedBattleGeneralInfo(BattlePassType, BattlePassName, BattlePassID, battlePassStartTime, battlePassEndTime)
				
		def BINARY_ExtBattlePassAddMission(self, battlepassType, battlepassID, missionIndex, missionType, missionInfo1, missionInfo2, missionInfo3):
			if selfs.wndInterface:
				selfs.wndInterface.AddExtendedBattlePassMission(battlepassType, battlepassID, missionIndex, missionType, missionInfo1, missionInfo2, missionInfo3)

		def BINARY_ExtBattlePassAddMissionReward(self, battlepassType, battlepassID, missionIndex, missionType, itemVnum, itemCount):
			if selfs.wndInterface:
				selfs.wndInterface.AddExtendedBattlePassMissionReward(battlepassType, battlepassID, missionIndex, missionType, itemVnum, itemCount)

		def BINARY_ExtBattlePassUpdate(self, battlepassType, missionIndex, missionType, newProgress):
			if selfs.wndInterface:
				selfs.wndInterface.UpdateExtendedBattlePassMission(battlepassType, missionIndex, missionType, newProgress)

		def BINARY_ExtBattlePassAddReward(self, battlepassType, battlepassID, itemVnum, itemCount):
			if selfs.wndInterface:
				selfs.wndInterface.AddExtendedBattlePassReward(battlepassType, battlepassID, itemVnum, itemCount)
		
		def BINARY_ExtBattlePassAddRanklistEntry(self, playername, battlepassType, battlepassID, startTime, endTime):
			if selfs.wndInterface:
				selfs.wndInterface.AddExtBattlePassRanklistEntry(playername, battlepassType, battlepassID, startTime, endTime)


	def BINARY_OpenAtlasWindow(self):
		selfs.wndInterface.BINARY_OpenAtlasWindow()

	## Chat
	def OnRecvWhisper(self, mode, name, line):

		if os.path.exists(str(constInfo.CLIENT_YOL)+"block_"+str(player.GetName())+".kf") and open(str(constInfo.CLIENT_YOL)+"block_"+str(player.GetName())+".kf", "r").read().find("#"+str(name)+"#") != -1:
			net.SendWhisperPacket(name, "#Sizi engelledim. Bu nedenle bana mesaj gonderemezsiniz.#"+str(player.GetStatus(player.LEVEL))+"#1#")
			return
		else:
			pass

		if line.find("Sizi engelledim. Bu nedenle bana mesaj gonderemezsiniz.") != -1:
			bol = line.split("#")
			chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, "Sizi engellemis, Bu kisiye mesaj atamassiniz.")

			return

		if mode == chat.WHISPER_TYPE_GM:
			selfs.wndInterface.RegisterGameMasterName(name)
		chat.AppendWhisper(mode, name, line)
		selfs.wndInterface.RecvWhisper(name)


	def OnRecvWhisperSystemMessage(self, mode, name, line):
		chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, line)
		selfs.wndInterface.RecvWhisper(name)

	def OnRecvWhisperError(self, mode, name, line):
		if localeInfo.WHISPER_ERROR.has_key(mode):
			chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, localeInfo.WHISPER_ERROR[mode](name))
		else:
			chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, "Whisper Unknown Error(mode=%d, name=%s)" % (mode, name))
		selfs.wndInterface.RecvWhisper(name)

	def RecvWhisper(self, name):
		selfs.wndInterface.RecvWhisper(name)

	def BINARY_OnRecvBulkWhisper(self, content):
		content = content.replace("$", " ")

		selfs.wndInterface.RegisterGameMasterName("[SYSTEM]")
		chat.AppendWhisper(chat.WHISPER_TYPE_CHAT, "[SYSTEM]", content)
		
		selfs.wndInterface.RecvWhisper("[SYSTEM]")


	#YangDrop
	def OnPickMoney(self, money):
		if constInfo.YangDrop == 1:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GAME_PICK_MONEY % (money))
		else:
			return
	#YangDrop

	def OnShopError(self, type):
		try:
			self.PopupMessage(localeInfo.SHOP_ERROR_DICT[type])
		except KeyError:
			self.PopupMessage(localeInfo.SHOP_ERROR_UNKNOWN % (type))

	def OnSafeBoxError(self):
		self.PopupMessage(localeInfo.SAFEBOX_ERROR)

	def OnFishingSuccess(self, isFish, fishName):
		chat.AppendChatWithDelay(chat.CHAT_TYPE_INFO, localeInfo.FISHING_SUCCESS(isFish, fishName), 2000)

	# ADD_FISHING_MESSAGE
	def OnFishingNotifyUnknown(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.FISHING_UNKNOWN)

	def OnFishingWrongPlace(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.FISHING_WRONG_PLACE)
	# END_OF_ADD_FISHING_MESSAGE

	def OnFishingNotify(self, isFish, fishName):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.FISHING_NOTIFY(isFish, fishName))

	def OnFishingFailure(self):
		chat.AppendChatWithDelay(chat.CHAT_TYPE_INFO, localeInfo.FISHING_FAILURE, 2000)

	def OnCannotPickItem(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GAME_CANNOT_PICK_ITEM)

	# MINING
	def OnCannotMining(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GAME_CANNOT_MINING)
	# END_OF_MINING

	def OnCannotUseSkill(self, vid, type):
		if localeInfo.USE_SKILL_ERROR_TAIL_DICT.has_key(type):
			textTail.RegisterInfoTail(vid, localeInfo.USE_SKILL_ERROR_TAIL_DICT[type])

		if localeInfo.USE_SKILL_ERROR_CHAT_DICT.has_key(type):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_SKILL_ERROR_CHAT_DICT[type])

	def	OnCannotShotError(self, vid, type):
		textTail.RegisterInfoTail(vid, localeInfo.SHOT_ERROR_TAIL_DICT.get(type, localeInfo.SHOT_ERROR_UNKNOWN % (type)))

	## PointReset
	def StartPointReset(self):
		selfs.wndInterface.OpenPointResetDialog()

	## Shop
	def StartShop(self, vid):
		selfs.wndInterface.OpenShopDialog(vid)

	def EndShop(self):
		selfs.wndInterface.CloseShopDialog()

	def RefreshShop(self):
		selfs.wndInterface.RefreshShopDialog()

	def SetShopSellingPrice(self, Price):
		pass

	if app.ENABLE_OFFLINE_SHOP_SYSTEM:
		def StartOfflineShop(self, vid):
			shopVID, privateShopEffect, privateShopEffectStartTime = constInfo.CREATE_PRIVATE_SHOP_EFFECT
			if privateShopEffect is True and int(vid) == shopVID: background.DeletePrivateShopPos(); constInfo.CREATE_PRIVATE_SHOP_EFFECT = (0, False, app.GetTime())
			selfs.wndInterface.OpenOfflineShopDialog(vid)

		def EndOfflineShop(self):
			selfs.wndInterface.CloseOfflineShopDialog()

		def RefreshOfflineShop(self):
			selfs.wndInterface.RefreshOfflineShopDialog()

		def RefreshOfflineShopManager(self):
			selfs.wndInterface.RefreshOfflineShopDialogManager()

		def CloseOfflineShopManager(self):
			selfs.wndInterface.CloseOfflineShopDialogManager()

	## Exchange
	def StartExchange(self):
		selfs.wndInterface.StartExchange()

	def EndExchange(self):
		selfs.wndInterface.EndExchange()

	def RefreshExchange(self):
		selfs.wndInterface.RefreshExchange()

	## Party
	def RecvPartyInviteQuestion(self, leaderVID, leaderName):
		partyInviteQuestionDialog = uiCommon.QuestionDialog()
		partyInviteQuestionDialog.SetText(leaderName + localeInfo.PARTY_DO_YOU_JOIN)
		partyInviteQuestionDialog.SetAcceptEvent(lambda arg=True: self.AnswerPartyInvite(arg))
		partyInviteQuestionDialog.SetCancelEvent(lambda arg=False: self.AnswerPartyInvite(arg))
		partyInviteQuestionDialog.Open()
		partyInviteQuestionDialog.partyLeaderVID = leaderVID
		self.partyInviteQuestionDialog = partyInviteQuestionDialog

	def AnswerPartyInvite(self, answer):

		if not self.partyInviteQuestionDialog:
			return

		partyLeaderVID = self.partyInviteQuestionDialog.partyLeaderVID

		distance = player.GetCharacterDistance(partyLeaderVID)
		if distance < 0.0 or distance > 5000:
			answer = False

		net.SendPartyInviteAnswerPacket(partyLeaderVID, answer)

		self.partyInviteQuestionDialog.Close()
		self.partyInviteQuestionDialog = None

	def AddPartyMember(self, pid, name):
		selfs.wndInterface.AddPartyMember(pid, name)

	def UpdatePartyMemberInfo(self, pid):
		selfs.wndInterface.UpdatePartyMemberInfo(pid)

	def RemovePartyMember(self, pid):
		selfs.wndInterface.RemovePartyMember(pid)
		self.__RefreshTargetBoard()

	def LinkPartyMember(self, pid, vid):
		selfs.wndInterface.LinkPartyMember(pid, vid)

	def UnlinkPartyMember(self, pid):
		selfs.wndInterface.UnlinkPartyMember(pid)

	def UnlinkAllPartyMember(self):
		selfs.wndInterface.UnlinkAllPartyMember()

	def ExitParty(self):
		selfs.wndInterface.ExitParty()
		self.RefreshTargetBoardByVID(self.targetBoard.GetTargetVID())

	def ChangePartyParameter(self, distributionMode):
		selfs.wndInterface.ChangePartyParameter(distributionMode)

	## Messenger
	def OnMessengerAddFriendQuestion(self, name):
		messengerAddFriendQuestion = uiCommon.QuestionDialog2()
		messengerAddFriendQuestion.SetText1(localeInfo.MESSENGER_DO_YOU_ACCEPT_ADD_FRIEND_1 % (name))
		messengerAddFriendQuestion.SetText2(localeInfo.MESSENGER_DO_YOU_ACCEPT_ADD_FRIEND_2)
		messengerAddFriendQuestion.SetAcceptEvent(ui.__mem_func__(self.OnAcceptAddFriend))
		messengerAddFriendQuestion.SetCancelEvent(ui.__mem_func__(self.OnDenyAddFriend))
		messengerAddFriendQuestion.Open()
		messengerAddFriendQuestion.name = name
		self.messengerAddFriendQuestion = messengerAddFriendQuestion

	def OnAcceptAddFriend(self):
		name = self.messengerAddFriendQuestion.name
		net.SendChatPacket("/messenger_auth y " + name)
		self.OnCloseAddFriendQuestionDialog()
		return True

	def OnDenyAddFriend(self):
		name = self.messengerAddFriendQuestion.name
		net.SendChatPacket("/messenger_auth n " + name)
		self.OnCloseAddFriendQuestionDialog()
		return True

	def OnCloseAddFriendQuestionDialog(self):
		self.messengerAddFriendQuestion.Close()
		self.messengerAddFriendQuestion = None
		return True

	## SafeBox
	def OpenSafeboxWindow(self, size):
		selfs.wndInterface.OpenSafeboxWindow(size)

	def RefreshSafebox(self):
		selfs.wndInterface.RefreshSafebox()

	def RefreshSafeboxMoney(self):
		selfs.wndInterface.RefreshSafeboxMoney()

	# ITEM_MALL
	def OpenMallWindow(self, size):
		selfs.wndInterface.OpenMallWindow(size)

	def RefreshMall(self):
		selfs.wndInterface.RefreshMall()
	# END_OF_ITEM_MALL

	## Guild
	def RecvGuildInviteQuestion(self, guildID, guildName):
		guildInviteQuestionDialog = uiCommon.QuestionDialog()
		guildInviteQuestionDialog.SetText(guildName + localeInfo.GUILD_DO_YOU_JOIN)
		guildInviteQuestionDialog.SetAcceptEvent(lambda arg=True: self.AnswerGuildInvite(arg))
		guildInviteQuestionDialog.SetCancelEvent(lambda arg=False: self.AnswerGuildInvite(arg))
		guildInviteQuestionDialog.Open()
		guildInviteQuestionDialog.guildID = guildID
		self.guildInviteQuestionDialog = guildInviteQuestionDialog

	def AnswerGuildInvite(self, answer):

		if not self.guildInviteQuestionDialog:
			return

		guildLeaderVID = self.guildInviteQuestionDialog.guildID
		net.SendGuildInviteAnswerPacket(guildLeaderVID, answer)

		self.guildInviteQuestionDialog.Close()
		self.guildInviteQuestionDialog = None


	def DeleteGuild(self):
		selfs.wndInterface.DeleteGuild()

	## Clock
	def ShowClock(self, second):
		selfs.wndInterface.ShowClock(second)

	def HideClock(self):
		selfs.wndInterface.HideClock()

	## Emotion
	def BINARY_ActEmotion(self, emotionIndex):
		if selfs.wndInterface.wndCharacter:
			selfs.wndInterface.wndCharacter.ActEmotion(emotionIndex)

	###############################################################################################
	###############################################################################################
	## Keyboard Functions

	def CheckFocus(self):
		if False == self.IsFocus():
			if True == selfs.wndInterface.IsOpenChat():
				selfs.wndInterface.ToggleChat()

			self.SetFocus()

	def SaveScreen(self):
		print "save screen"

		# SCREENSHOT_CWDSAVE
		if SCREENSHOT_CWDSAVE:
			if not os.path.exists(os.getcwd()+os.sep+"screenshot"):
				os.mkdir(os.getcwd()+os.sep+"screenshot")

			(succeeded, name) = grp.SaveScreenShotToPath(os.getcwd()+os.sep+"screenshot"+os.sep)
		elif SCREENSHOT_DIR:
			(succeeded, name) = grp.SaveScreenShot(SCREENSHOT_DIR)
		else:
			(succeeded, name) = grp.SaveScreenShot()
		# END_OF_SCREENSHOT_CWDSAVE

		if succeeded:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "%s %s %s" % (name, localeInfo.SCREENSHOT_SAVE1, localeInfo.SCREENSHOT_SAVE2))
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SCREENSHOT_SAVE_FAILURE)

	def ShowConsole(self):
		if debugInfo.IsDebugMode() or True == self.consoleEnable:
			player.EndKeyWalkingImmediately()
			self.console.OpenWindow()

	def ShowName(self):
		self.ShowNameFlag = True
		self.playerGauge.EnableShowAlways()
		player.SetQuickPage(self.quickSlotPageIndex+1)

	# ADD_ALWAYS_SHOW_NAME
	def __IsShowName(self):

		if systemSetting.IsAlwaysShowName():
			return True

		if self.ShowNameFlag:
			return True

		return False
	# END_OF_ADD_ALWAYS_SHOW_NAME

	def HideName(self):
		self.ShowNameFlag = False
		self.playerGauge.DisableShowAlways()
		player.SetQuickPage(self.quickSlotPageIndex)

	def ShowMouseImage(self):
		selfs.wndInterface.ShowMouseImage()

	def HideMouseImage(self):
		selfs.wndInterface.HideMouseImage()

	def StartAttack(self):
		player.SetAttackKeyState(True)

	def EndAttack(self):
		player.SetAttackKeyState(False)

	def MoveUp(self):
		player.SetSingleDIKKeyState(app.DIK_UP, True)

	def MoveDown(self):
		player.SetSingleDIKKeyState(app.DIK_DOWN, True)

	def MoveLeft(self):
		player.SetSingleDIKKeyState(app.DIK_LEFT, True)

	def MoveRight(self):
		player.SetSingleDIKKeyState(app.DIK_RIGHT, True)

	def StopUp(self):
		player.SetSingleDIKKeyState(app.DIK_UP, False)

	def StopDown(self):
		player.SetSingleDIKKeyState(app.DIK_DOWN, False)

	def StopLeft(self):
		player.SetSingleDIKKeyState(app.DIK_LEFT, False)

	def StopRight(self):
		player.SetSingleDIKKeyState(app.DIK_RIGHT, False)

	def PickUpItem(self):
		player.PickCloseItemVector()

	def PickUpMoney(self):
		player.PickCloseMoney()

	###############################################################################################
	###############################################################################################
	## Event Handler

	def OnKeyDown(self, key):
		if selfs.wndInterface.wndWeb and selfs.wndInterface.wndWeb.IsShow():
			return

		if key == app.DIK_ESC:
			self.RequestDropItem(False)
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

		try:
			self.onPressKeyDict[key]()
		except KeyError:
			pass
		except:
			raise

		return True

	def OnKeyUp(self, key):
		try:
			self.onClickKeyDict[key]()
		except KeyError:
			pass
		except:
			raise

		return True

	def OnMouseLeftButtonDown(self):
		if selfs.wndInterface.BUILD_OnMouseLeftButtonDown():
			return

		if mouseModule.mouseController.isAttached():
			self.CheckFocus()
		else:
			hyperlink = ui.GetHyperlink()
			if hyperlink:
				return
			else:
				self.CheckFocus()
				player.SetMouseState(player.MBT_LEFT, player.MBS_PRESS);

		return True

	def OnMouseLeftButtonUp(self):

		if selfs.wndInterface.BUILD_OnMouseLeftButtonUp():
			return

		if mouseModule.mouseController.isAttached():

			attachedType = mouseModule.mouseController.GetAttachedType()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()
			attachedItemSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()

			## QuickSlot
			if player.SLOT_TYPE_QUICK_SLOT == attachedType:
				player.RequestDeleteGlobalQuickSlot(attachedItemSlotPos)

			## Inventory
			elif player.SLOT_TYPE_INVENTORY == attachedType:

				if player.ITEM_MONEY == attachedItemIndex:
					self.__PutMoney(attachedType, attachedItemCount, self.PickingCharacterIndex)
				else:
					self.__PutItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, self.PickingCharacterIndex)

			## DragonSoul
			elif player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedType:
				self.__PutItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, self.PickingCharacterIndex)

			mouseModule.mouseController.DeattachObject()

		else:
			hyperlink = ui.GetHyperlink()
			if hyperlink:
				if app.IsPressed(app.DIK_LALT):
					link = chat.GetLinkFromHyperlink(hyperlink)
					ime.PasteString(link)
				else:
					selfs.wndInterface.MakeHyperlinkTooltip(hyperlink)
				return
			else:
				player.SetMouseState(player.MBT_LEFT, player.MBS_CLICK)

		#player.EndMouseWalking()
		return True

	def __PutItem(self, attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, dstChrID):
		if player.SLOT_TYPE_INVENTORY == attachedType or player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedType:
			attachedInvenType = player.SlotTypeToInvenType(attachedType)
			if True == chr.HasInstance(self.PickingCharacterIndex) and player.GetMainCharacterIndex() != dstChrID:
				if player.IsEquipmentSlot(attachedItemSlotPos) and player.SLOT_TYPE_DRAGON_SOUL_INVENTORY != attachedType:
					self.stream.popupWindow.Close()
					self.stream.popupWindow.Open(localeInfo.EXCHANGE_FAILURE_EQUIP_ITEM, 0, localeInfo.UI_OK)
				else:
					if chr.IsNPC(dstChrID):
						if app.ENABLE_REFINE_RENEWAL:
							constInfo.AUTO_REFINE_TYPE = 2
							constInfo.AUTO_REFINE_DATA["NPC"][0] = dstChrID
							constInfo.AUTO_REFINE_DATA["NPC"][1] = attachedInvenType
							constInfo.AUTO_REFINE_DATA["NPC"][2] = attachedItemSlotPos
							constInfo.AUTO_REFINE_DATA["NPC"][3] = attachedItemCount
							net.SendGiveItemPacket(dstChrID, attachedInvenType, attachedItemSlotPos, attachedItemCount)
					else:
						net.SendExchangeStartPacket(dstChrID)
						net.SendExchangeItemAddPacket(attachedInvenType, attachedItemSlotPos, 0)
			else:
				if app.ENABLE_DROP_DIALOG_EXTENDED_SYSTEM:
					selfs.wndInterface.DeleteItem(attachedItemSlotPos, attachedInvenType)
				else:
					self.__DropItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount)

	def __PutMoney(self, attachedType, attachedMoney, dstChrID):
		if True == chr.HasInstance(dstChrID) and player.GetMainCharacterIndex() != dstChrID:
			net.SendExchangeStartPacket(dstChrID)
			net.SendExchangeElkAddPacket(attachedMoney)
		else:
			if not app.ENABLE_DROP_DIALOG_EXTENDED_SYSTEM:
				self.__DropMoney(attachedType, attachedMoney)

	def __DropMoney(self, attachedType, attachedMoney):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return
		# END_OF_PRIVATESHOP_DISABLE_ITEM_DROP

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if uiOfflineShopBuilder.IsBuildingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
				return

			if uiOfflineShop.IsEditingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
				return

		if attachedMoney>=1000:
			self.stream.popupWindow.Close()
			self.stream.popupWindow.Open(localeInfo.DROP_MONEY_FAILURE_1000_OVER, 0, localeInfo.UI_OK)
			return

		itemDropQuestionDialog = uiCommon.QuestionDialog()
		itemDropQuestionDialog.SetText(localeInfo.DO_YOU_DROP_MONEY % (attachedMoney))
		itemDropQuestionDialog.SetAcceptEvent(lambda arg=True: self.RequestDropItem(arg))
		itemDropQuestionDialog.SetCancelEvent(lambda arg=False: self.RequestDropItem(arg))
		itemDropQuestionDialog.Open()
		itemDropQuestionDialog.dropType = attachedType
		itemDropQuestionDialog.dropCount = attachedMoney
		itemDropQuestionDialog.dropNumber = player.ITEM_MONEY
		self.itemDropQuestionDialog = itemDropQuestionDialog

	def __DropItem(self, attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return
		# END_OF_PRIVATESHOP_DISABLE_ITEM_DROP

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if uiOfflineShopBuilder.IsBuildingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
				return

			if uiOfflineShop.IsEditingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
				return

		if player.SLOT_TYPE_INVENTORY == attachedType and player.IsEquipmentSlot(attachedItemSlotPos):
			self.stream.popupWindow.Close()
			self.stream.popupWindow.Open(localeInfo.DROP_ITEM_FAILURE_EQUIP_ITEM, 0, localeInfo.UI_OK)

		else:
			if player.SLOT_TYPE_INVENTORY == attachedType:
				dropItemIndex = player.GetItemIndex(attachedItemSlotPos)

				item.SelectItem(dropItemIndex)
				dropItemName = item.GetItemName()

				## Question Text
				questionText = localeInfo.HOW_MANY_ITEM_DO_YOU_DROP(dropItemName, attachedItemCount)

				## Dialog
				itemDropQuestionDialog = uiCommon.QuestionDialog()
				itemDropQuestionDialog.SetText(questionText)
				itemDropQuestionDialog.SetAcceptEvent(lambda arg=True: self.RequestDropItem(arg))
				itemDropQuestionDialog.SetCancelEvent(lambda arg=False: self.RequestDropItem(arg))
				itemDropQuestionDialog.Open()
				itemDropQuestionDialog.dropType = attachedType
				itemDropQuestionDialog.dropNumber = attachedItemSlotPos
				itemDropQuestionDialog.dropCount = attachedItemCount
				self.itemDropQuestionDialog = itemDropQuestionDialog

				constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
			elif player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedType:
				dropItemIndex = player.GetItemIndex(player.DRAGON_SOUL_INVENTORY, attachedItemSlotPos)

				item.SelectItem(dropItemIndex)
				dropItemName = item.GetItemName()

				## Question Text
				questionText = localeInfo.HOW_MANY_ITEM_DO_YOU_DROP(dropItemName, attachedItemCount)

				## Dialog
				itemDropQuestionDialog = uiCommon.QuestionDialog()
				itemDropQuestionDialog.SetText(questionText)
				itemDropQuestionDialog.SetAcceptEvent(lambda arg=True: self.RequestDropItem(arg))
				itemDropQuestionDialog.SetCancelEvent(lambda arg=False: self.RequestDropItem(arg))
				itemDropQuestionDialog.Open()
				itemDropQuestionDialog.dropType = attachedType
				itemDropQuestionDialog.dropNumber = attachedItemSlotPos
				itemDropQuestionDialog.dropCount = attachedItemCount
				self.itemDropQuestionDialog = itemDropQuestionDialog

				constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def RequestDropItem(self, answer):
		if not self.itemDropQuestionDialog:
			return

		if answer:
			dropType = self.itemDropQuestionDialog.dropType
			dropCount = self.itemDropQuestionDialog.dropCount
			dropNumber = self.itemDropQuestionDialog.dropNumber

			if player.SLOT_TYPE_INVENTORY == dropType:
				if dropNumber == player.ITEM_MONEY:
					net.SendGoldDropPacketNew(dropCount)
					snd.PlaySound("sound/ui/money.wav")
				else:
					# PRIVATESHOP_DISABLE_ITEM_DROP
					self.__SendDropItemPacket(dropNumber, dropCount)
					# END_OF_PRIVATESHOP_DISABLE_ITEM_DROP
			elif player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == dropType:
					# PRIVATESHOP_DISABLE_ITEM_DROP
					self.__SendDropItemPacket(dropNumber, dropCount, player.DRAGON_SOUL_INVENTORY)
					# END_OF_PRIVATESHOP_DISABLE_ITEM_DROP

		self.itemDropQuestionDialog.Close()
		self.itemDropQuestionDialog = None

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	# PRIVATESHOP_DISABLE_ITEM_DROP
	def __SendDropItemPacket(self, itemVNum, itemCount, itemInvenType = player.INVENTORY):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if uiOfflineShopBuilder.IsBuildingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
				return

			if uiOfflineShop.IsEditingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
				return

		net.SendItemDropPacketNew(itemInvenType, itemVNum, itemCount)
	# END_OF_PRIVATESHOP_DISABLE_ITEM_DROP

	def OnMouseRightButtonDown(self):

		self.CheckFocus()

		if True == mouseModule.mouseController.isAttached():
			mouseModule.mouseController.DeattachObject()

		else:
			player.SetMouseState(player.MBT_RIGHT, player.MBS_PRESS)

		return True

	def OnMouseRightButtonUp(self):
		if True == mouseModule.mouseController.isAttached():
			return True

		player.SetMouseState(player.MBT_RIGHT, player.MBS_CLICK)
		return True

	def OnMouseMiddleButtonDown(self):
		player.SetMouseMiddleButtonState(player.MBS_PRESS)

	def OnMouseMiddleButtonUp(self):
		player.SetMouseMiddleButtonState(player.MBS_CLICK)

	def Lerp(self, a, b, t):
		return (1 - t) * a + t * b

	def OnUpdate(self):
		app.UpdateGame()

		localtime = localtime = time.strftime("%d %m %Y %H:%M")
		self.timeLine.SetText(localtime)
		self.timeLine.Show()

		if self.mapNameShower.IsShow():
			self.mapNameShower.Update()

		if self.isShowDebugInfo:
			self.UpdateDebugInfo()

		if self.enableXMasBoom:
			self.__XMasBoom_Update()

		if 0 == constInfo.auto_pick_item:
			self.PickUpItem()

		if 0 == constInfo.auto_pick_yang:
			self.PickUpMoney()

		if self.isCameraMoving == False:
			self.cameraMovementProgress += 0.01
			startLoc = [5827.0, 90.0, 200.0]
			global cameraDistance, cameraPitch, cameraRotation, cameraHeight
			(ucameraDistance, ucameraPitch, ucameraRotation, ucameraHeight) = app.GetCamera()
			dist = self.Lerp(startLoc[0], cameraDistance, self.cameraMovementProgress)
			put = self.Lerp(startLoc[1], cameraPitch, self.cameraMovementProgress)
			rot = self.Lerp(startLoc[2], cameraRotation, self.cameraMovementProgress)
			app.SetCamera(dist, put, rot, ucameraHeight)
			if self.cameraMovementProgress >= 1:
				self.isCameraMoving = False
				self.cameraMovementProgress = 0.0

		if constInfo.LOAD_CURTAIN == 1:
			constInfo.LOAD_CURTAIN = 0

		if 1== constInfo.BOSS_TRACKING:
			constInfo.BOSS_TRACKING = 0
			self.BossTrackingSystemShow()

		if app.AUTO_SHOUT:
			if constInfo.auto_shout_status == 1:
				if self.shouttime <= app.GetTime():
					self.shouttime = app.GetTime()+8
					#net.SendChatPacket(str(constInfo.auto_shout_text),chat.CHAT_TYPE_SHOUT)
					net.SendChatPacket(str(constInfo.Vectors+'|cFFFF0000|HVectors:'+str(player.GetName())+'|h[PM]|h|r'+" : "+constInfo.auto_shout_text), chat.CHAT_TYPE_SHOUT)

		#if int(int(selfs.wndInterface.LastContactTimeStamp) + selfs.wndInterface.WaitTime) < int(app.GetTime()) and selfs.wndInterface.State == "Kapali":
		#	selfs.wndInterface.State = "Acik"


		if app.__AUTO_HUNT__:
			if constInfo.autoHuntAutoLoginDict["status"] == 1 and constInfo.autoHuntAutoLoginDict["leftTime"] > 0 and constInfo.autoHuntAutoLoginDict["leftTime"] < app.GetGlobalTimeStamp():
				constInfo.autoHuntAutoLoginDict["leftTime"] = 0
				selfs.wndInterface.CheckAutoLogin()

		selfs.wndInterface.BUILD_OnUpdate()


	def UpdateDebugInfo(self):
		#
		(x, y, z) = player.GetMainCharacterPosition()
		nUpdateTime = app.GetUpdateTime()
		nUpdateFPS = app.GetUpdateFPS()
		nRenderFPS = app.GetRenderFPS()
		nFaceCount = app.GetFaceCount()
		fFaceSpeed = app.GetFaceSpeed()
		nST=background.GetRenderShadowTime()
		(fAveRT, nCurRT) =  app.GetRenderTime()
		(iNum, fFogStart, fFogEnd, fFarCilp) = background.GetDistanceSetInfo()
		(iPatch, iSplat, fSplatRatio, sTextureNum) = background.GetRenderedSplatNum()
		if iPatch == 0:
			iPatch = 1

		#(dwRenderedThing, dwRenderedCRC) = background.GetRenderedGraphicThingInstanceNum()

		self.PrintCoord.SetText("Coordinate: %.2f %.2f %.2f ATM: %d" % (x, y, z, app.GetAvailableTextureMemory()/(1024*1024)))
		xMouse, yMouse = wndMgr.GetMousePosition()
		self.PrintMousePos.SetText("MousePosition: %d %d" % (xMouse, yMouse))

		self.FrameRate.SetText("UFPS: %3d UT: %3d FS %.2f" % (nUpdateFPS, nUpdateTime, fFaceSpeed))

		if fAveRT>1.0:
			self.Pitch.SetText("RFPS: %3d RT:%.2f(%3d) FC: %d(%.2f) " % (nRenderFPS, fAveRT, nCurRT, nFaceCount, nFaceCount/fAveRT))

		self.Splat.SetText("PATCH: %d SPLAT: %d BAD(%.2f)" % (iPatch, iSplat, fSplatRatio))
		#self.Pitch.SetText("Pitch: %.2f" % (app.GetCameraPitch())
		#self.TextureNum.SetText("TN : %s" % (sTextureNum))
		#self.ObjectNum.SetText("GTI : %d, CRC : %d" % (dwRenderedThing, dwRenderedCRC))
		self.ViewDistance.SetText("Num : %d, FS : %f, FE : %f, FC : %f" % (iNum, fFogStart, fFogEnd, fFarCilp))

	def OnRender(self):
		app.RenderGame()

		if self.console.Console.collision:
			background.RenderCollision()
			chr.RenderCollision()

		(x, y) = app.GetCursorPosition()

		########################
		# Picking
		########################
		textTail.UpdateAllTextTail()

		if True == wndMgr.IsPickedWindow(self.hWnd):

			self.PickingCharacterIndex = chr.Pick()

			if -1 != self.PickingCharacterIndex:
				textTail.ShowCharacterTextTail(self.PickingCharacterIndex)
			if 0 != self.targetBoard.GetTargetVID():
				textTail.ShowCharacterTextTail(self.targetBoard.GetTargetVID())

			# ADD_ALWAYS_SHOW_NAME
			if not self.__IsShowName():
				self.PickingItemIndex = item.Pick()
				if -1 != self.PickingItemIndex:
					textTail.ShowItemTextTail(self.PickingItemIndex)
			# END_OF_ADD_ALWAYS_SHOW_NAME

		## Show all name in the range

		# ADD_ALWAYS_SHOW_NAME
		if self.__IsShowName():
			textTail.ShowAllTextTail()
			self.PickingItemIndex = textTail.Pick(x, y)
		# END_OF_ADD_ALWAYS_SHOW_NAME

		textTail.UpdateShowingTextTail()
		textTail.ArrangeTextTail()
		if -1 != self.PickingItemIndex:
			textTail.SelectItemName(self.PickingItemIndex)

		grp.PopState()
		grp.SetInterfaceRenderState()

		textTail.Render()
		textTail.HideAllTextTail()

	def OnPressEscapeKey(self):
		if app.TARGET == app.GetCursor():
			app.SetCursor(app.NORMAL)

		elif True == mouseModule.mouseController.isAttached():
			mouseModule.mouseController.DeattachObject()

		else:
			selfs.wndInterface.OpenSystemDialog()

		return True

	def OnIMEReturn(self):
		if app.IsPressed(app.DIK_LSHIFT):
			selfs.wndInterface.OpenWhisperDialogWithoutTarget()
		else:
			selfs.wndInterface.ToggleChat()
		return True

	def OnPressExitKey(self):
		selfs.wndInterface.ToggleSystemDialog()
		return True

	## BINARY CALLBACK
	######################################################################################

	# EXCHANGE
	if app.WJ_ENABLE_TRADABLE_ICON:
		def BINARY_AddItemToExchange(self, inven_type, inven_pos, display_pos):
			if inven_type == player.INVENTORY:
				selfs.wndInterface.CantTradableItemExchange(display_pos, inven_pos)
	# END_OF_EXCHANGE

	# WEDDING
	def BINARY_LoverInfo(self, name, lovePoint):
		if selfs.wndInterface.wndMessenger:
			selfs.wndInterface.wndMessenger.OnAddLover(name, lovePoint)
		if self.affectShower:
			self.affectShower.SetLoverInfo(name, lovePoint)

	def BINARY_UpdateLovePoint(self, lovePoint):
		if selfs.wndInterface.wndMessenger:
			selfs.wndInterface.wndMessenger.OnUpdateLovePoint(lovePoint)
		if self.affectShower:
			self.affectShower.OnUpdateLovePoint(lovePoint)
	# END_OF_WEDDING

	if app.ENABLE_SEND_TARGET_INFO:
		def BINARY_AddTargetMonsterDropInfo(self, raceNum, itemVnum, itemCount):
			if not raceNum in constInfo.MONSTER_INFO_DATA:
				constInfo.MONSTER_INFO_DATA.update({raceNum : {}})
				constInfo.MONSTER_INFO_DATA[raceNum].update({"items" : []})
			curList = constInfo.MONSTER_INFO_DATA[raceNum]["items"]

			isUpgradeable = False
			isMetin = False
			item.SelectItem(itemVnum)
			if item.GetItemType() == item.ITEM_TYPE_WEAPON or item.GetItemType() == item.ITEM_TYPE_ARMOR:
				isUpgradeable = True
			elif item.GetItemType() == item.ITEM_TYPE_METIN:
				isMetin = True

			for curItem in curList:
				if isUpgradeable:
					if curItem.has_key("vnum_list") and curItem["vnum_list"][0] / 10 * 10 == itemVnum / 10 * 10:
						if not (itemVnum in curItem["vnum_list"]):
							curItem["vnum_list"].append(itemVnum)
						return
				elif isMetin:
					if curItem.has_key("vnum_list"):
						baseVnum = curItem["vnum_list"][0]
					if curItem.has_key("vnum_list") and (baseVnum - baseVnum%1000) == (itemVnum - itemVnum%1000):
						if not (itemVnum in curItem["vnum_list"]):
							curItem["vnum_list"].append(itemVnum)
						return
				else:
					if curItem.has_key("vnum") and curItem["vnum"] == itemVnum and curItem["count"] == itemCount:
						return

			if isUpgradeable or isMetin:
				curList.append({"vnum_list":[itemVnum], "count":itemCount})
			else:
				curList.append({"vnum":itemVnum, "count":itemCount})

		def BINARY_RefreshTargetMonsterDropInfo(self, raceNum):
			self.targetBoard.RefreshMonsterInfoBoard()

	# QUEST_CONFIRM
	def BINARY_OnQuestConfirm(self, msg, timeout, pid):
		confirmDialog = uiCommon.QuestionDialogWithTimeLimit()
		confirmDialog.Open(msg, timeout)
		confirmDialog.SetAcceptEvent(lambda answer=True, pid=pid: net.SendQuestConfirmPacket(answer, pid) or self.confirmDialog.Hide())
		confirmDialog.SetCancelEvent(lambda answer=False, pid=pid: net.SendQuestConfirmPacket(answer, pid) or self.confirmDialog.Hide())
		self.confirmDialog = confirmDialog
	# END_OF_QUEST_CONFIRM

	# GIFT command
	def Gift_Show(self):
		selfs.wndInterface.ShowGift()

	if app.BL_PRIVATESHOP_SEARCH_SYSTEM:
		def OpenPShopSearchDialogCash(self):
			selfs.wndInterface.OpenPShopSearchDialogCash()
		def RefreshPShopSearchDialog(self):
			selfs.wndInterface.RefreshPShopSearchDialog()

	# CUBE
	def BINARY_Cube_Open(self, npcVNUM):
		self.currentCubeNPC = npcVNUM

		selfs.wndInterface.OpenCubeWindow()


		if npcVNUM not in self.cubeInformation:
			net.SendChatPacket("/cube r_info")
		else:
			cubeInfoList = self.cubeInformation[npcVNUM]

			i = 0
			for cubeInfo in cubeInfoList:
				selfs.wndInterface.wndCube.AddCubeResultItem(cubeInfo["vnum"], cubeInfo["count"])

				j = 0
				for materialList in cubeInfo["materialList"]:
					for materialInfo in materialList:
						itemVnum, itemCount = materialInfo
						selfs.wndInterface.wndCube.AddMaterialInfo(i, j, itemVnum, itemCount)
					j = j + 1

				i = i + 1

			selfs.wndInterface.wndCube.Refresh()

	def BINARY_Cube_Close(self):
		selfs.wndInterface.CloseCubeWindow()

	def BINARY_Cube_UpdateInfo(self, gold, itemVnum, count):
		selfs.wndInterface.UpdateCubeInfo(gold, itemVnum, count)

	def BINARY_Cube_Succeed(self, itemVnum, count):
		print "큐브 제작 성공"
		selfs.wndInterface.SucceedCubeWork(itemVnum, count)
		pass

	def BINARY_Cube_Failed(self):
		print "큐브 제작 실패"
		selfs.wndInterface.FailedCubeWork()
		pass

	def BINARY_Cube_ResultList(self, npcVNUM, listText):
		#print listText

		if npcVNUM == 0:
			npcVNUM = self.currentCubeNPC

		self.cubeInformation[npcVNUM] = []

		try:
			for eachInfoText in listText.split("/"):
				eachInfo = eachInfoText.split(",")
				itemVnum	= int(eachInfo[0])
				itemCount	= int(eachInfo[1])

				self.cubeInformation[npcVNUM].append({"vnum": itemVnum, "count": itemCount})
				selfs.wndInterface.wndCube.AddCubeResultItem(itemVnum, itemCount)

			resultCount = len(self.cubeInformation[npcVNUM])
			requestCount = 7
			modCount = resultCount % requestCount
			splitCount = resultCount / requestCount
			for i in xrange(splitCount):
				#print("/cube r_info %d %d" % (i * requestCount, requestCount))
				net.SendChatPacket("/cube r_info %d %d" % (i * requestCount, requestCount))

			if 0 < modCount:
				#print("/cube r_info %d %d" % (splitCount * requestCount, modCount))
				net.SendChatPacket("/cube r_info %d %d" % (splitCount * requestCount, modCount))

		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

		pass

	def BINARY_Cube_MaterialInfo(self, startIndex, listCount, listText):
		# Material Text Format : 125,1|126,2|127,2|123,5&555,5&555,4/120000
		try:
			#print listText

			if 3 > len(listText):
				dbg.TraceError("Wrong Cube Material Infomation")
				return 0



			eachResultList = listText.split("@")

			cubeInfo = self.cubeInformation[self.currentCubeNPC]

			itemIndex = 0
			for eachResultText in eachResultList:
				cubeInfo[startIndex + itemIndex]["materialList"] = [[], [], [], [], []]
				materialList = cubeInfo[startIndex + itemIndex]["materialList"]

				gold = 0
				splitResult = eachResultText.split("/")
				if 1 < len(splitResult):
					gold = int(splitResult[1])

				#print "splitResult : ", splitResult
				eachMaterialList = splitResult[0].split("&")

				i = 0
				for eachMaterialText in eachMaterialList:
					complicatedList = eachMaterialText.split("|")

					if 0 < len(complicatedList):
						for complicatedText in complicatedList:
							(itemVnum, itemCount) = complicatedText.split(",")
							itemVnum = int(itemVnum)
							itemCount = int(itemCount)
							selfs.wndInterface.wndCube.AddMaterialInfo(itemIndex + startIndex, i, itemVnum, itemCount)

							materialList[i].append((itemVnum, itemCount))

					else:
						itemVnum, itemCount = eachMaterialText.split(",")
						itemVnum = int(itemVnum)
						itemCount = int(itemCount)
						selfs.wndInterface.wndCube.AddMaterialInfo(itemIndex + startIndex, i, itemVnum, itemCount)

						materialList[i].append((itemVnum, itemCount))

					i = i + 1



				itemIndex = itemIndex + 1

			selfs.wndInterface.wndCube.Refresh()


		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

		pass

	# END_OF_CUBE

	def BINARY_Highlight_Item(self, inven_type, inven_pos):
		# @fixme003 (+if selfs.wndInterface:)
		if selfs.wndInterface:
			selfs.wndInterface.Highligt_Item(inven_type, inven_pos)

	# END of DRAGON SOUL REFINE WINDOW

	def BINARY_SetBigMessage(self, message):
		selfs.wndInterface.bigBoard.SetTip(message)

	def BINARY_SetTipMessage(self, message):
		selfs.wndInterface.tipBoard.SetTip(message)

	def BINARY_AppendNotifyMessage(self, type):
		if not type in localeInfo.NOTIFY_MESSAGE:
			return
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.NOTIFY_MESSAGE[type])

#	if app.ENABLE_HIDE_COSTUME_SYSTEM:
#		def SetBodyCostumeHidden(self, hidden):
#			constInfo.HIDDEN_BODY_COSTUME = int(hidden)
#			selfs.wndInterface.RefreshVisibleCostume()
#
#		def SetHairCostumeHidden(self, hidden):
#			constInfo.HIDDEN_HAIR_COSTUME = int(hidden)
#			selfs.wndInterface.RefreshVisibleCostume()

	def BINARY_Guild_EnterGuildArea(self, areaID):
		selfs.wndInterface.BULID_EnterGuildArea(areaID)

	def BINARY_Guild_ExitGuildArea(self, areaID):
		selfs.wndInterface.BULID_ExitGuildArea(areaID)

	def BINARY_GuildWar_OnSendDeclare(self, guildID):
		pass

	def BINARY_GuildWar_OnRecvDeclare(self, guildID, warType):
		mainCharacterName = player.GetMainCharacterName()
		masterName = guild.GetGuildMasterName()
		if mainCharacterName == masterName:
			self.__GuildWar_OpenAskDialog(guildID, warType)

	def BINARY_GuildWar_OnRecvPoint(self, gainGuildID, opponentGuildID, point):
		selfs.wndInterface.OnRecvGuildWarPoint(gainGuildID, opponentGuildID, point)

	def BINARY_GuildWar_OnStart(self, guildSelf, guildOpp):
		selfs.wndInterface.OnStartGuildWar(guildSelf, guildOpp)

	def BINARY_GuildWar_OnEnd(self, guildSelf, guildOpp):
		selfs.wndInterface.OnEndGuildWar(guildSelf, guildOpp)

	def BINARY_BettingGuildWar_SetObserverMode(self, isEnable):
		selfs.wndInterface.BINARY_SetObserverMode(isEnable)

	def BINARY_BettingGuildWar_UpdateObserverCount(self, observerCount):
		selfs.wndInterface.wndMiniMap.UpdateObserverCount(observerCount)

	def __GuildWar_UpdateMemberCount(self, guildID1, memberCount1, guildID2, memberCount2, observerCount):
		guildID1 = int(guildID1)
		guildID2 = int(guildID2)
		memberCount1 = int(memberCount1)
		memberCount2 = int(memberCount2)
		observerCount = int(observerCount)

		selfs.wndInterface.UpdateMemberCount(guildID1, memberCount1, guildID2, memberCount2)
		selfs.wndInterface.wndMiniMap.UpdateObserverCount(observerCount)

	def __GuildWar_OpenAskDialog(self, guildID, warType):

		guildName = guild.GetGuildName(guildID)

		# REMOVED_GUILD_BUG_FIX
		if "Noname" == guildName:
			return
		# END_OF_REMOVED_GUILD_BUG_FIX

		import uiGuild
		questionDialog = uiGuild.AcceptGuildWarDialog()
		questionDialog.SAFE_SetAcceptEvent(self.__GuildWar_OnAccept)
		questionDialog.SAFE_SetCancelEvent(self.__GuildWar_OnDecline)
		questionDialog.Open(guildName, warType)

		self.guildWarQuestionDialog = questionDialog

	def __GuildWar_CloseAskDialog(self):
		self.guildWarQuestionDialog.Close()
		self.guildWarQuestionDialog = None

	def __GuildWar_OnAccept(self):

		guildName = self.guildWarQuestionDialog.GetGuildName()

		net.SendChatPacket("/war " + guildName)
		self.__GuildWar_CloseAskDialog()

		return 1

	def __GuildWar_OnDecline(self):

		guildName = self.guildWarQuestionDialog.GetGuildName()

		net.SendChatPacket("/nowar " + guildName)
		self.__GuildWar_CloseAskDialog()

		return 1
	## BINARY CALLBACK
	######################################################################################

	def __ServerCommand_Build(self):
		serverCommandList={
			"ConsoleEnable"			: self.__Console_Enable,
			"DayMode"				: self.__DayMode_Update,
			"PRESERVE_DayMode"		: self.__PRESERVE_DayMode_Update,
			"CloseRestartWindow"	: self.__RestartDialog_Close,
			"OpenPrivateShop"		: self.__PrivateShop_Open,
			"PartyHealReady"		: self.PartyHealReady,
			"ShowMeSafeboxPassword"	: self.AskSafeboxPassword,
			"CloseSafebox"			: self.CommandCloseSafebox,
			"selectskill_open"	: self.skillSelect.Open,

			# ITEM_MALL
			"CloseMall"				: self.CommandCloseMall,
			"ShowMeMallPassword"	: self.AskMallPassword,
			"item_mall"				: self.__ItemMall_Open,
			# END_OF_ITEM_MALL

			"RefineSuceeded"		: self.RefineSuceededMessage,
			"RefineFailed"			: self.RefineFailedMessage,
			"xmas_snow"				: self.__XMasSnow_Enable,
			"xmas_boom"				: self.__XMasBoom_Enable,
			"xmas_song"				: self.__XMasSong_Enable,
			"xmas_muzik"			: self.__MuzikCal_Enable,
			"xmas_tree"				: self.__XMasTree_Enable,
			"newyear_boom"			: self.__XMasBoom_Enable,
			"PartyRequest"			: self.__PartyRequestQuestion,
			"PartyRequestDenied"	: self.__PartyRequestDenied,
			"horse_state"			: self.__Horse_UpdateState,
			"hide_horse_state"		: self.__Horse_HideState,
			"WarUC"					: self.__GuildWar_UpdateMemberCount,
			"test_server"			: self.__EnableTestServerFlag,
			"mall"			: self.__InGameShop_Show,
			"BossTracking"		: self.GetBossTrackingInformation,
			"BossTrackingUpdatePacket" : self.BossTrackingUpdate,
			"BGM" : self.__Music,
			# WEDDING
			"lover_login"			: self.__LoginLover,
			"lover_logout"			: self.__LogoutLover,
			"lover_near"			: self.__LoverNear,
			"lover_far"				: self.__LoverFar,
			"lover_divorce"			: self.__LoverDivorce,
			"PlayMusic"				: self.__PlayMusic,
			"Maintenancegui"	: self.Maintenancegui,
			# END_OF_WEDDING

			# PRIVATE_SHOP_PRICE_LIST
			"MyShopPriceList"		: self.__PrivateShop_PriceList,
			# END_OF_PRIVATE_SHOP_PRICE_LIST
			"OpenBulkWhisperPanel"		: self.whisperAdmin.OpenWindow,
		}

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			serverCommandList.update({
				"openOffBuilder" : self.openOffBuilder,
				"BINARY_CloseOfflineShop" : self.BINARY_CloseOfflineShop,
				"OpenOfflineShop" : self.__OfflineShop_Open,
				"OpenOfflineShopSalesWindow" : self.__OpenOfflineShopSalesWindow,
				"ClearOfflineShopSales" : self.__ClearOfflineShopSalesWindow,
			})

		if app.ENABLE_NEW_BIOLOG:
			serverCommandList.update({
				"OpenBioSelect" : selfs.wndInterface.wndBioWindow.OpenSelectDialog,
				"ReminderStatus" : selfs.wndInterface.wndBioWindow.UpdateReminder,
				"OpenBioPanel" : selfs.wndInterface.OpenBiologWindow,
				"CloseBiologWindow" : selfs.wndInterface.CloseBiologWindow,
			})

		if app.NEW_SALES_SYSTEM:
			serverCommandList.update({
				"SalesClearList"			: self.SalesClearList,
				"SalesNewList"			: self.SalesNewList,
				"SalesInsertItem"			: self.SalesInsertItem,
				"SalesUpdateList"			: self.SalesUpdateList,
				"SalesShowIcon"			: selfs.wndInterface.SalesShowIcon,
			})

		if app.FATE_ROULETTE:
			serverCommandList.update({
				"RoulettePrepare"				: selfs.wndInterface.RoulettePrepare,
				"RouletteRun"					: selfs.wndInterface.RouletteRun,
				"RouletteReset"	: selfs.wndInterface.RouletteReset,
				"RouletteShowIcon"			: selfs.wndInterface.RouletteShowIcon,
				"RouletteOpen"			: selfs.wndInterface.RouletteOpen,
			})
			
			

		if app.ENABLE_NEW_MISSIONS:
			serverCommandList.update({
				"MissionGlobalInfo" : selfs.wndInterface.AppendGlobalMissionInfos,
				"MissionInfo" : selfs.wndInterface.AppendMissionInfos,
				"MissionItems" : selfs.wndInterface.UpdateMissionItem,
				"MyMissions" : selfs.wndInterface.MyMissions,
				"UpMission" : selfs.wndInterface.wndMissionPanel.UpdateMyVal,
				"AppendMissions" : selfs.wndInterface.AppendMissions,
				"OpenMissionPanel" : selfs.wndInterface.OpenMissionPanel,
			})

		serverCommandList.update({
			"botcontrol" : self.UpdateBotControlItems,
			"closebotcontrol" : self.CloseBotControl,
			"botcontrolname" : self.UpdateBotControlRealName,
			"botcontrolrof" : self.UpdateBotControlRof,
		})

#		if app.ENABLE_HIDE_COSTUME_SYSTEM:
#			serverCommandList.update({
#				"SetBodyCostumeHidden" : self.SetBodyCostumeHidden,
#				"SetHairCostumeHidden" : self.SetHairCostumeHidden,
#			})

		if app.BL_MOVE_CHANNEL:
			serverCommandList["server_info"] = self.__SeverInfo

		if app.ENABLE_CAOS_EVENT:
			serverCommandList.update({"CaosEventInfo" : selfs.wndInterface.wndCaosEvent.AppendPlayer,})
			serverCommandList.update({"RefreshCaosUI" : selfs.wndInterface.wndCaosEvent.Refresh,})
			serverCommandList.update({"ClearCaosUI" : selfs.wndInterface.wndCaosEvent.ClearInfos,})
			serverCommandList.update({"OpenCaosEventIMG" : selfs.wndInterface.wndCaosEvent.OpenLoginImage,})
			serverCommandList.update({"CaosEventRequest" : self.CaosEvent_OpenQuestDialog,})

		if app.ENABLE_REWARD_SYSTEM:
			serverCommandList.update({"RewardData" : selfs.wndInterface.RewardData})

		if app.ENABLE_AUTOMATIC_ITEM_PROCESS:
			serverCommandList.update({
				"clearItemProcessInfo" : selfs.wndInterface.wndDeleteWindow.RefreshPanel,
				"addItemProcessInfo" : selfs.wndInterface.wndDeleteWindow.AddItem,
				"setItemProcessStatus" : selfs.wndInterface.wndDeleteWindow.SetMod,
			})

		if app.__AUTO_HUNT__:
			serverCommandList.update({"AutoHuntStatus" : selfs.wndInterface.AutoHuntStatus})

		if app.ENABLE_MULTI_FARM_BLOCK:
			serverCommandList.update({"UpdateMultiFarmAffect" : self.UpdateMultiFarmAffect})
			serverCommandList.update({"UpdateMultiFarmPlayer" : self.UpdateMultiFarmPlayer})

		self.serverCommander=stringCommander.Analyzer()
		if app.ENABLE_EXTEND_INVEN_SYSTEM:
			self.serverCommander.SAFE_RegisterCallBack("ExInvenItemUseMsg", self.ExInvenItemUseMsg)	
		for serverCommandItem in serverCommandList.items():
			self.serverCommander.SAFE_RegisterCallBack(
				serverCommandItem[0], serverCommandItem[1]
			)

		if app.ITEM_SHOP:
			self.serverCommander.SAFE_RegisterCallBack("RefreshMyCash", self.RefreshCashToItemShop)

	if app.ENABLE_CAOS_EVENT:
		def CaosEvent_OpenQuestDialog(self):
			if self.CaosEventQuestDialog:
				self.CaosEventQuestDialog.Close()
				
			CaosEventQuestDialog = uiCommon.QuestionDialogCaosEvent()
			CaosEventQuestDialog.SetAcceptEvent(lambda arg=1: self.CaosEvent_Answer(arg))
			CaosEventQuestDialog.SetCancelEvent(lambda arg=0: self.CaosEvent_Answer(arg))
			CaosEventQuestDialog.Open()
			self.CaosEventQuestDialog = CaosEventQuestDialog

		def CaosEvent_Answer(self, answer):
			if not self.CaosEventQuestDialog:
				return

			if answer == 1:
				net.SendChatPacket("/warp_caosevent")

			self.CaosEventQuestDialog.Close()
			self.CaosEventQuestDialog = None

	if app.ENABLE_MULTI_FARM_BLOCK:
		def UpdateMultiFarmPlayer(self, multiFarmPlayer):
			self.affectShower.SetMultiFarmPlayer(str(multiFarmPlayer))
		def UpdateMultiFarmAffect(self, multiFarmStatus, isNewStatus):
			self.affectShower.SetMultiFarmInfo(int(multiFarmStatus))
			if int(isNewStatus) == 1:
				if int(multiFarmStatus) == 1:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MULTI_FARM_ACTIVE_CHAT)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MULTI_FARM_DEACTIVE_CHAT)
			app.SetMultiFarmExeIcon(int(multiFarmStatus))

	if app.NEW_SALES_SYSTEM:
		def SalesClearList(self):
			constInfo.SalesList = {}	
			constInfo.Sales_Temp_Index = 0	
				
		def SalesNewList(self, id, type, normal_price, sales_price, time):
			constInfo.SalesList.update({int(constInfo.Sales_Temp_Index) : {}})
			constInfo.SalesList[int(constInfo.Sales_Temp_Index)].update({"id":int(id), "type":int(type), "normal_price":int(normal_price), "sales_price":int(sales_price), "time":int(time), "items":[]})
			
			
		def SalesInsertItem(self, id, vnum, count):
			ekle = {"vnum" : int(vnum), "count" : int(count)}
			constInfo.Sales_Temp_ItemList.append(ekle.copy())

			
		def SalesUpdateList(self, id):
			constInfo.SalesList[int(constInfo.Sales_Temp_Index)]["items"].append(constInfo.Sales_Temp_ItemList)
			constInfo.Sales_Temp_ItemList = []
			constInfo.Sales_Temp_Index = int(constInfo.Sales_Temp_Index)+1
	

	if app.ENABLE_CUBE_RENEWAL_WORLDARD:
		def BINARY_CUBE_RENEWAL_OPEN(self):
			if selfs.wndInterface:
				selfs.wndInterface.BINARY_CUBE_RENEWAL_OPEN()

	def BINARY_ServerCommand_Run(self, line):
		try:
			#print " BINARY_ServerCommand_Run", line
			return self.serverCommander.Run(line)
		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

	def __ProcessPreservedServerCommand(self):
		try:
			command = net.GetPreservedServerCommand()
			while command:
				print " __ProcessPreservedServerCommand", command
				self.serverCommander.Run(command)
				command = net.GetPreservedServerCommand()
		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

	def PartyHealReady(self):
		selfs.wndInterface.PartyHealReady()

	def AskSafeboxPassword(self):
		selfs.wndInterface.AskSafeboxPassword()

	# ITEM_MALL
	def AskMallPassword(self):
		selfs.wndInterface.AskMallPassword()

	def __ItemMall_Open(self):
		selfs.wndInterface.OpenItemMall();

	def CommandCloseMall(self):
		selfs.wndInterface.CommandCloseMall()
	# END_OF_ITEM_MALL

	def __Music(self, lied):
		snd.FadeOutAllMusic()
		musicInfo.SaveLastPlayFieldMusic()
		snd.FadeInMusic("BGM/" + lied)

	def RefineSuceededMessage(self):
		self.PopupMessage(localeInfo.REFINE_SUCCESS)
		if app.ENABLE_REFINE_RENEWAL:
			selfs.wndInterface.CheckRefineDialog(False)

	def RefineFailedMessage(self):
		self.PopupMessage(localeInfo.REFINE_FAILURE)
		if app.ENABLE_REFINE_RENEWAL:
			selfs.wndInterface.CheckRefineDialog(True)

	def CommandCloseSafebox(self):
		selfs.wndInterface.CommandCloseSafebox()

	# PRIVATE_SHOP_PRICE_LIST
	def __PrivateShop_PriceList(self, itemVNum, itemPrice):
		uiPrivateShopBuilder.SetPrivateShopItemPrice(itemVNum, itemPrice)
	# END_OF_PRIVATE_SHOP_PRICE_LIST

	def __Horse_HideState(self):
		self.affectShower.SetHorseState(0, 0, 0)

	def __Horse_UpdateState(self, level, health, battery):
		self.affectShower.SetHorseState(int(level), int(health), int(battery))

	def __IsXMasMap(self):
		mapDict = ( "metin2_map_n_flame_01",
					"metin2_map_n_desert_01",
					"metin2_map_spiderdungeon",
					"metin2_map_deviltower1", )

		if background.GetCurrentMapName() in mapDict:
			return False

		return True

	def __XMasSnow_Enable(self, mode):

		self.__XMasSong_Enable(mode)

		if "1"==mode:

			if not self.__IsXMasMap():
				return

			print "XMAS_SNOW ON"
			background.EnableSnow(1)

		else:
			print "XMAS_SNOW OFF"
			background.EnableSnow(0)

	def __XMasBoom_Enable(self, mode):
		if "1"==mode:

			if not self.__IsXMasMap():
				return

			print "XMAS_BOOM ON"
			self.__DayMode_Update("dark")
			self.enableXMasBoom = True
			self.startTimeXMasBoom = app.GetTime()
		else:
			print "XMAS_BOOM OFF"
			self.__DayMode_Update("light")
			self.enableXMasBoom = False

	def __XMasTree_Enable(self, grade):

		print "XMAS_TREE ", grade
		background.SetXMasTree(int(grade))

	def __XMasSong_Enable(self, mode):
		if "1"==mode:
			print "XMAS_SONG ON"

			XMAS_BGM = "xmas.mp3"

			if app.IsExistFile("BGM/" + XMAS_BGM)==1:
				if musicInfo.fieldMusic != "":
					snd.FadeOutMusic("BGM/" + musicInfo.fieldMusic)

				musicInfo.fieldMusic=XMAS_BGM
				snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)

		else:
			print "XMAS_SONG OFF"

			if musicInfo.fieldMusic != "":
				snd.FadeOutMusic("BGM/" + musicInfo.fieldMusic)

			musicInfo.fieldMusic=musicInfo.METIN2THEMA
			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)


	def __MuzikCal_Enable(self, mode):

		if constInfo.MuzikKontrol == False:
			self.__DayMode_Update("dark")
	
		if "1"==mode:
			print "Muzik Acildi"
		
			self.enableXMasMuzik = True
			constInfo.MuzikKontrol = True
			self.startTimeXMasMuzik = app.GetTime()
		
			XMAS_BGM = "1.mp3"

			if app.IsExistFile("BGM/" + XMAS_BGM)==1:
				if musicInfo.fieldMusic != "":
					snd.FadeOutMusic("BGM/" + musicInfo.fieldMusic , 1)

				musicInfo.fieldMusic=XMAS_BGM
				musicInfo.SaveLastPlayFieldMusic()
			 
				snd.FadeInMusic("BGM/" + musicInfo.fieldMusic,1)

		elif "2"==mode:
			print "Muzik Acildi"

			self.enableXMasMuzik = True
			constInfo.MuzikKontrol = True
			self.startTimeXMasMuzik = app.GetTime()
		
			XMAS_BGM = "2.mp3"

			if app.IsExistFile("BGM/" + XMAS_BGM)==1:
				if musicInfo.fieldMusic != "":
					snd.FadeOutMusic("BGM/" + musicInfo.fieldMusic , 1)

				musicInfo.fieldMusic=XMAS_BGM
				musicInfo.SaveLastPlayFieldMusic()
			
				snd.FadeInMusic("BGM/" + musicInfo.fieldMusic,1)

		elif "3"==mode:
			print "Muzik Acildi"

			self.enableXMasMuzik = True
			constInfo.MuzikKontrol = True
			self.startTimeXMasMuzik = app.GetTime()
		
			XMAS_BGM = "3.mp3"

			if app.IsExistFile("BGM/" + XMAS_BGM)==1:
				if musicInfo.fieldMusic != "":
					snd.FadeOutMusic("BGM/" + musicInfo.fieldMusic , 1)

				musicInfo.fieldMusic=XMAS_BGM
				musicInfo.SaveLastPlayFieldMusic()
			
				snd.FadeInMusic("BGM/" + musicInfo.fieldMusic,1)

		elif "4"==mode:
			print "Muzik Acildi"

			self.enableXMasMuzik = True
			constInfo.MuzikKontrol = True
			self.startTimeXMasMuzik = app.GetTime()
		
			XMAS_BGM = "4.mp3"

			if app.IsExistFile("BGM/" + XMAS_BGM)==1:
				if musicInfo.fieldMusic != "":
					snd.FadeOutMusic("BGM/" + musicInfo.fieldMusic , 1)

				musicInfo.fieldMusic=XMAS_BGM
				musicInfo.SaveLastPlayFieldMusic()
			
				snd.FadeInMusic("BGM/" + musicInfo.fieldMusic,1)

		elif "5"==mode:
			print "Muzik Acildi"

			self.enableXMasMuzik = True
			constInfo.MuzikKontrol = True
			self.startTimeXMasMuzik = app.GetTime()
		
			XMAS_BGM = "5.mp3"

			if app.IsExistFile("BGM/" + XMAS_BGM)==1:
				if musicInfo.fieldMusic != "":
					snd.FadeOutMusic("BGM/" + musicInfo.fieldMusic , 1)

				musicInfo.fieldMusic=XMAS_BGM
				musicInfo.SaveLastPlayFieldMusic()
			
				snd.FadeInMusic("BGM/" + musicInfo.fieldMusic,1)

		else:
	
			print "MuzikCal Kapatildi"
			self.__DayMode_Update("light")
			self.enableXMasMuzik = False
			constInfo.MuzikKontrol = False

			if musicInfo.fieldMusic != "":
				snd.FadeOutMusic("BGM/" + musicInfo.fieldMusic,1)

			musicInfo.fieldMusic=musicInfo.METIN2THEMA
			musicInfo.SaveLastPlayFieldMusic()
			musicInfo.loginMusic=""

			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)


	def __RestartDialog_Close(self):
		selfs.wndInterface.CloseRestartDialog()

	def __Console_Enable(self):
		constInfo.CONSOLE_ENABLE = True
		self.consoleEnable = True
		app.EnableSpecialCameraMode()
		ui.EnablePaste(True)

	## PrivateShop
	def __PrivateShop_Open(self):
		selfs.wndInterface.OpenPrivateShopInputNameDialog()

	def BINARY_PrivateShop_Appear(self, vid, text):
		selfs.wndInterface.AppearPrivateShop(vid, text)

	def BINARY_PrivateShop_Disappear(self, vid):
		selfs.wndInterface.DisappearPrivateShop(vid)

	if app.ENABLE_OFFLINE_SHOP_SYSTEM:
		def openOffBuilder(self):
			if selfs.wndInterface: selfs.wndInterface.OpenOfflineShopCreateDialog()

		def BINARY_OfflineShopAdviseBuyItem(self, itemId, buyerName, itemPrice):
			if selfs.wndInterface and itemId > 0:
				item.SelectItem(itemId)
				itemPrice -= (3 * itemPrice) / 100
				tip_msg = localeInfo.OFFLINE_SHOP_ADVISE_PLAYER_TIP.format(item.GetItemName(), localeInfo.NumberToMoneyString(itemPrice))
				selfs.wndInterface.LoadAppLeftTip(tip_msg, "SHOP")
		def __OfflineShop_Open(self):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_SHOP_OPEN_WITH_BUTTON)
		
		def BINARY_CloseOfflineShop(self):
			if selfs.wndInterface:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_SHOP_FORCE_CLOSE)
				selfs.wndInterface.BINARY_CloseOfflineShop()
		
		def BINARY_ChangeOfflineShopName(self, shopName):
			if selfs.wndInterface:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_SHOP_CHANGE_SHOP_NAME)
				selfs.wndInterface.BINARY_ChangeOfflineShopName(shopName)

		def __OpenOfflineShopSalesWindow(self):
			selfs.wndInterface.OpenOfflineShopSalesWindow()

		def __ClearOfflineShopSalesWindow(self):
			constInfo.OFFLINE_SHOP_SALES = []

		def BINARY_OfflineShop_Appear(self, vid, text, borderStyle):
			if (chr.GetInstanceType(vid) == chr.INSTANCE_TYPE_NPC):
				selfs.wndInterface.AppearOfflineShop(vid, text, borderStyle)
		
		def BINARY_OfflineShop_ChangeSign(self, vid, text):
			if (chr.GetInstanceType(vid) == chr.INSTANCE_TYPE_NPC):
				selfs.wndInterface.ChangeOfflineShopSign(vid, text)

		def BINARY_OfflineShop_Disappear(self, vid):
			if (chr.GetInstanceType(vid) == chr.INSTANCE_TYPE_NPC):
				selfs.wndInterface.DisappearOfflineShop(vid)

		def BINARY_OfflineShop_Count(self, count):
			pass

		def BINARY_OfflineShop_Open(self, isOpen, mapIndex, shopChannel, time, isPremium, displayedCount, shopName):
			if selfs.wndInterface:
				selfs.wndInterface.ToggleOfflineShopAdminPanelWindow(isOpen, mapIndex, shopChannel, time, isPremium, displayedCount, shopName)

		def BINARY_OfflineShop_Sales(self, buyerName, itemVnum, itemCount, itemPrice, itemCheque, itemDate):
			constInfo.OFFLINE_SHOP_SALES.append([buyerName, itemVnum, itemCount, itemPrice, itemCheque, itemDate],)

	## DayMode
	def __PRESERVE_DayMode_Update(self, mode):
		if "light"==mode:
			background.SetEnvironmentData(0)
		elif "dark"==mode:

			if not self.__IsXMasMap():
				return

			background.RegisterEnvironmentData(1, constInfo.ENVIRONMENT_NIGHT)
			background.SetEnvironmentData(1)

	def __DayMode_Update(self, mode):
		if "light"==mode:
			self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToLight)
		elif "dark"==mode:

			if not self.__IsXMasMap():
				return

			self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToDark)

	def __DayMode_OnCompleteChangeToLight(self):
		background.SetEnvironmentData(0)
		self.curtain.FadeIn()

	def __DayMode_OnCompleteChangeToDark(self):
		background.RegisterEnvironmentData(1, constInfo.ENVIRONMENT_NIGHT)
		background.SetEnvironmentData(1)
		self.curtain.FadeIn()

	## XMasBoom
	def __XMasBoom_Update(self):

		self.BOOM_DATA_LIST = ( (2, 5), (5, 2), (7, 3), (10, 3), (20, 5) )
		if self.indexXMasBoom >= len(self.BOOM_DATA_LIST):
			return

		boomTime = self.BOOM_DATA_LIST[self.indexXMasBoom][0]
		boomCount = self.BOOM_DATA_LIST[self.indexXMasBoom][1]

		if app.GetTime() - self.startTimeXMasBoom > boomTime:

			self.indexXMasBoom += 1

			for i in xrange(boomCount):
				self.__XMasBoom_Boom()

	def __XMasBoom_Boom(self):
		x, y, z = player.GetMainCharacterPosition()
		randX = app.GetRandom(-150, 150)
		randY = app.GetRandom(-150, 150)

		snd.PlaySound3D(x+randX, -y+randY, z, "sound/common/etc/salute.mp3")

	def __XMasMuzik_Update(self):

		self.BOOM_DATA_LIST = ( (2, 5), (5, 2), (7, 3), (10, 3), (20, 5) )
		if self.indexXMasMuzik >= len(self.BOOM_DATA_LIST):
			return

		boomTime = self.BOOM_DATA_LIST[self.indexXMasMuzik][0]
		boomCount = self.BOOM_DATA_LIST[self.indexXMasMuzik][1]

		if app.GetTime() - self.startTimeXMasMuzik > boomTime:

			self.indexXMasMuzik += 1

			for i in xrange(boomCount):
				self.__XMasMuzik_Boom()

	def __XMasMuzik_Boom(self):
		x, y, z = player.GetMainCharacterPosition()
		randX = app.GetRandom(-150, 150)
		randY = app.GetRandom(-150, 150)

		snd.PlaySound3D(x+randX, -y+randY, z, "sound/common/etc/salute.mp3")

	def __PartyRequestQuestion(self, vid):
		vid = int(vid)
		partyRequestQuestionDialog = uiCommon.QuestionDialog()
		partyRequestQuestionDialog.SetText(chr.GetNameByVID(vid) + localeInfo.PARTY_DO_YOU_ACCEPT)
		partyRequestQuestionDialog.SetAcceptText(localeInfo.UI_ACCEPT)
		partyRequestQuestionDialog.SetCancelText(localeInfo.UI_DENY)
		partyRequestQuestionDialog.SetAcceptEvent(lambda arg=True: self.__AnswerPartyRequest(arg))
		partyRequestQuestionDialog.SetCancelEvent(lambda arg=False: self.__AnswerPartyRequest(arg))
		partyRequestQuestionDialog.Open()
		partyRequestQuestionDialog.vid = vid
		self.partyRequestQuestionDialog = partyRequestQuestionDialog

	def __AnswerPartyRequest(self, answer):
		if not self.partyRequestQuestionDialog:
			return

		vid = self.partyRequestQuestionDialog.vid

		if answer:
			net.SendChatPacket("/party_request_accept " + str(vid))
		else:
			net.SendChatPacket("/party_request_deny " + str(vid))

		self.partyRequestQuestionDialog.Close()
		self.partyRequestQuestionDialog = None

	def __PartyRequestDenied(self):
		self.PopupMessage(localeInfo.PARTY_REQUEST_DENIED)

	def __EnableTestServerFlag(self):
		app.EnableTestServerFlag()

	def __InGameShop_Show(self, url):
		selfs.wndInterface.OpenWebWindow(url)

	# WEDDING
	def __LoginLover(self):
		if selfs.wndInterface.wndMessenger:
			selfs.wndInterface.wndMessenger.OnLoginLover()

	def __LogoutLover(self):
		if selfs.wndInterface.wndMessenger:
			selfs.wndInterface.wndMessenger.OnLogoutLover()
		if self.affectShower:
			self.affectShower.HideLoverState()

	def __LoverNear(self):
		if self.affectShower:
			self.affectShower.ShowLoverState()

	def __LoverFar(self):
		if self.affectShower:
			self.affectShower.HideLoverState()

	def __LoverDivorce(self):
		if selfs.wndInterface.wndMessenger:
			selfs.wndInterface.wndMessenger.ClearLoverInfo()
		if self.affectShower:
			self.affectShower.ClearLoverState()

	def __PlayMusic(self, flag, filename):
		flag = int(flag)
		if flag:
			snd.FadeOutAllMusic()
			musicInfo.SaveLastPlayFieldMusic()
			snd.FadeInMusic("BGM/" + filename)
		else:
			snd.FadeOutAllMusic()
			musicInfo.LoadLastPlayFieldMusic()
			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)
	# END_OF_WEDDING

	def maintenanceadminopen(self):
		if not "[" in player.GetName():
			return

		self.Maintenancedialog = uimaintenance.MaintenanceDialog()
		self.Maintenancedialog.Show()

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def ActAcce(self, iAct, bWindow):
			if selfs.wndInterface:
				selfs.wndInterface.ActAcce(iAct, bWindow)

		def AlertAcce(self, bWindow):
			snd.PlaySound("sound/ui/make_soket.wav")
			if bWindow:
				self.PopupMessage(localeInfo.ACCE_DEL_SERVEITEM)
			else:
				self.PopupMessage(localeInfo.ACCE_DEL_ABSORDITEM)

	if app.ENABLE_EXTEND_INVEN_SYSTEM:	
		def ExInvenItemUseMsg(self, enough_count):
			if selfs.wndInterface:
				selfs.wndInterface.ExInvenItemUseMsg(enough_count)


	def BossTracking(self):
		import constInfo
		constInfo.BOSS_TRACKING = 1
		if selfs.wndInterface:
			selfs.wndInterface.BossTracking()

	def GetBossTrackingInformation(self, btcadi1, btcadi2, btcadi3, btcadi4, btcadi5, btcadi6, btalevkral1, btalevkral2, btalevkral3, btalevkral4, btalevkral5, btalevkral6, btkorumcek1, btkorumcek2, btkorumcek3, btkorumcek4, btkorumcek5, btkorumcek6, btsarikaplan1, btsarikaplan2, btsarikaplan3, btsarikaplan4, btsarikaplan5, btsarikaplan6, btbuzkralice1, btbuzkralice2, btbuzkralice3, btbuzkralice4, btbuzkralice5, btbuzkralice6, btdokuzk1, btdokuzk2, btdokuzk3, btdokuzk4, btdokuzk5, btdokuzk6, btcolejder1, btcolejder2, btcolejder3, btcolejder4, btcolejder5, btcolejder6, btagac1, btagac2, btagac3, btagac4, btagac5, btagac6, btkomutan1, btkomutan2, btkomutan3, btkomutan4, btkomutan5, btkomutan6, btkaranlik1, btkaranlik2, btkaranlik3, btkaranlik4, btkaranlik5, btkaranlik6):
		self.bosstracking.GetMobInformation(btcadi1, btcadi2, btcadi3, btcadi4, btcadi5, btcadi6, btalevkral1, btalevkral2, btalevkral3, btalevkral4, btalevkral5, btalevkral6, btkorumcek1, btkorumcek2, btkorumcek3, btkorumcek4, btkorumcek5, btkorumcek6, btsarikaplan1, btsarikaplan2, btsarikaplan3, btsarikaplan4, btsarikaplan5, btsarikaplan6, btbuzkralice1, btbuzkralice2, btbuzkralice3, btbuzkralice4, btbuzkralice5, btbuzkralice6, btdokuzk1, btdokuzk2, btdokuzk3, btdokuzk4, btdokuzk5, btdokuzk6, btcolejder1, btcolejder2, btcolejder3, btcolejder4, btcolejder5, btcolejder6, btagac1, btagac2, btagac3, btagac4, btagac5, btagac6, btkomutan1, btkomutan2, btkomutan3, btkomutan4, btkomutan5, btkomutan6, btkaranlik1, btkaranlik2, btkaranlik3, btkaranlik4, btkaranlik5, btkaranlik6)

	def BossTrackingUpdate(self):
		net.SendChatPacket("/bosstrackingtest")

	def BossTrackingSystemShow(self):
		self.bosstracking.Show()
		net.SendChatPacket("/bosstrackingtest")

	if app.ENABLE_SWITCHBOT:
		def RefreshSwitchbotWindow(self):
			selfs.wndInterface.RefreshSwitchbotWindow()
			
		def RefreshSwitchbotItem(self, slot):
			selfs.wndInterface.RefreshSwitchbotItem(slot)

	def EventCalendar(self):
		if selfs.wndInterface:
			selfs.wndInterface.EventCalendarGame()


	def BINARY_Cards_UpdateInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points):
		selfs.wndInterface.UpdateCardsInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points)
		
	def BINARY_Cards_FieldUpdateInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
		selfs.wndInterface.UpdateCardsFieldInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)
		
	def BINARY_Cards_PutReward(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
		selfs.wndInterface.CardsPutReward(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)
		
	def BINARY_Cards_ShowIcon(self):
		selfs.wndInterface.CardsShowIcon()
		
	def BINARY_Cards_Open(self, safemode):
		selfs.wndInterface.OpenCardsWindow(safemode)

	def CardsOpenInfoWindow(self):
		selfs.wndInterface.CardsShowInfoWindow()

	def OpenLuckyDrawDialog(self, joinItemVnum, joinItemCount, entryPrice, endTime, totalParticipants,
				maxParticipants, maxParticipantsPerPlayer, playerParticipants,
				awardItems, winner1Name, winner2Name, winner3Name, isFinished, isWon):
		selfs.wndInterface.OpenLuckyDrawDialog(joinItemVnum, joinItemCount, entryPrice, endTime, totalParticipants,
				maxParticipants, maxParticipantsPerPlayer, playerParticipants,
				awardItems, winner1Name, winner2Name, winner3Name, isFinished, isWon)

	if app.ITEM_SHOP:
		def AddCategory(self, categoryName, categoryID):
			selfs.wndInterface.AddCategory(categoryName, categoryID)
		def FillCategory(self, mainID, categoryName, categoryID):
			selfs.wndInterface.FillCategory(mainID, categoryName, categoryID)
		def AddItem(self, itemID, itemVnum, itemCount, itemPrice, categoryID, itemSockets, itemAttrs, priceType):
			selfs.wndInterface.AddItem(itemID, itemVnum, itemCount, itemPrice, categoryID, itemSockets, itemAttrs, priceType)
		def ClearList(self):
			selfs.wndInterface.ClearList()
		def RefreshCashToItemShop(self, myCash, myVC):
			selfs.wndInterface.RefreshCashToItemShop(int(str(myCash)), int(str(myVC)))


	if app.ENABLE_NEW_RANKING:
		def BINARY_RankList(self, idx, name, empire, value):
			if selfs.wndInterface:
				selfs.wndInterface.wndRank.AppendInfo(idx, name, empire, value)

		def BINARY_RankCategoryInfo(self, size):
			if selfs.wndInterface:
				selfs.wndInterface.wndRank.LoadRankButtons(size)

		def BINARY_ClearRankList(self):
			if selfs.wndInterface:
				selfs.wndInterface.wndRank.ClearRanks()

	def UpdateBotControlItems(self, data):
		vv = data.split('|')
		vv.pop()
		itemArr = [int(i) for i in vv]
		if selfs.wndInterface:
			selfs.wndInterface.wndBotControl.UpdateItems(itemArr)
			selfs.wndInterface.OpenBotControl()

	def UpdateBotControlRealName(self, name):
		if selfs.wndInterface:
			chat.AppendChat(1, "{}".format(name))
			selfs.wndInterface.wndBotControl.SetRealItemName(str(name))

	def UpdateBotControlRof(self, rof):
		if selfs.wndInterface:
			selfs.wndInterface.wndBotControl.SetROF(int(rof))

	def CloseBotControl(self):
		if selfs.wndInterface:
			selfs.wndInterface.wndBotControl.Close()

	if app.ENABLE_NEW_MISSIONS:
		def SendOpenMissionPanelRequest(self):
			net.SendChatPacket("/udt_misions")

	if app.ENABLE_EVENT_MANAGER:
		def ClearEventManager(self):
			selfs.wndInterface.ClearEventManager()
		def RefreshEventManager(self):
			selfs.wndInterface.RefreshEventManager()
		def RefreshEventStatus(self, eventID, eventStatus, eventendTime, eventEndTimeText):
			selfs.wndInterface.RefreshEventStatus(int(eventID), int(eventStatus), int(eventendTime), str(eventEndTimeText))
		def AppendEvent(self, dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart):
			selfs.wndInterface.AppendEvent(int(dayIndex),int(eventID), int(eventIndex), str(startTime), str(endTime), int(empireFlag), int(channelFlag), int(value0), int(value1), int(value2), int(value3), int(startRealTime), int(endRealTime), int(isAlreadyStart))


	if app.BL_MOVE_CHANNEL:
		def __SeverInfo(self, channelNumber, mapIndex):
			#print "__SeverInfo %s %s" % (channelNumber, mapIndex)
			
			_chNum	= int(channelNumber.strip())
			_mapIdx	= int(mapIndex.strip())
			
			if _chNum == 99 or _mapIdx >= 10000:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_NOTICE % 0)
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_NOTICE % _chNum)
				
			net.SetChannelName(_chNum)
			net.SetMapIndex(_mapIdx)
			selfs.wndInterface.RefreshServerInfo()

	if app.__RANKING_SYSTEM__:
		def LoadRankData(self, index, isFirst):
			selfs.wndInterface.LoadRankData(int(index), int(isFirst))
		def LoadRankUpdate(self, index):
			selfs.wndInterface.LoadRankUpdate(int(index))
