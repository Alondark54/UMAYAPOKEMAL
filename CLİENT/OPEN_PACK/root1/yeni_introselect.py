import chr
import grp
import app
import math
import wndMgr
import snd
import net
import systemSetting
import localeInfo
import chr

import ui
import uiScriptLocale
import networkModule
import musicInfo
import playerSettingModule

####################################
# ¨¬u¢¬¡Í ¨öCCaA¡í A¡×CN ¢¬?¥ìa ¡¤I¥ìu ¨¬?¢¥a
####################################
import uiCommon					
import uiMapNameShower			 
import uiAffectShower			  
import uiPlayerGauge			   
import uiCharacter				 
import uiTarget					
import consoleModule			   

# interface moduleAI ©ö¢çA|¨ú©¬...
import interfaceModule
import uiTaskBar				   
import uiInventory
import dbg
import uitooltip
import constInfo
import event
import background
import grp
import player
###################################

LEAVE_BUTTON_FOR_POTAL = False
NOT_NEED_DELETE_CODE = False
ENABLE_ENGNUM_DELETE_CODE = False

if localeInfo.IsJAPAN():
	NOT_NEED_DELETE_CODE = True
elif localeInfo.IsHONGKONG():
	ENABLE_ENGNUM_DELETE_CODE = True
elif localeInfo.IsNEWCIBN() or localeInfo.IsCIBN10():
	ENABLE_ENGNUM_DELETE_CODE = True
elif localeInfo.IsEUROPE():
	ENABLE_ENGNUM_DELETE_CODE = True

###################################

class SelectCharacterWindow(ui.Window):
	SLOT_COUNT = 5
	CHARACTER_TYPE_COUNT = 4
	
	EMPIRE_NAME = {
		net.EMPIRE_A : localeInfo.EMPIRE_A,
		net.EMPIRE_B : localeInfo.EMPIRE_B,
		net.EMPIRE_C : localeInfo.EMPIRE_C
	}
	
	RACE_NAME = {
					playerSettingModule.RACE_WARRIOR_M : localeInfo.JOB_WARRIOR,
					playerSettingModule.RACE_WARRIOR_W : localeInfo.JOB_WARRIOR,
					playerSettingModule.RACE_ASSASSIN_M : localeInfo.JOB_ASSASSIN,
					playerSettingModule.RACE_ASSASSIN_W	: localeInfo.JOB_ASSASSIN,
					playerSettingModule.RACE_SURA_M : localeInfo.JOB_SURA,
					playerSettingModule.RACE_SURA_W : localeInfo.JOB_SURA,
					playerSettingModule.RACE_SHAMAN_M : localeInfo.JOB_SHAMAN,
					playerSettingModule.RACE_SHAMAN_W : localeInfo.JOB_SHAMAN,
	}
	
	DESCRIPTION_FILE_NAME =	{
								playerSettingModule.RACE_WARRIOR_M : uiScriptLocale.JOBDESC_WARRIOR_PATH,
								playerSettingModule.RACE_WARRIOR_W : uiScriptLocale.JOBDESC_WARRIOR_PATH,
								playerSettingModule.RACE_ASSASSIN_M : uiScriptLocale.JOBDESC_ASSASSIN_PATH,
								playerSettingModule.RACE_ASSASSIN_W	: uiScriptLocale.JOBDESC_ASSASSIN_PATH,
								playerSettingModule.RACE_SURA_M : uiScriptLocale.JOBDESC_SURA_PATH,
								playerSettingModule.RACE_SURA_W : uiScriptLocale.JOBDESC_SURA_PATH,
								playerSettingModule.RACE_SHAMAN_M : uiScriptLocale.JOBDESC_SHAMAN_PATH,
								playerSettingModule.RACE_SHAMAN_W : uiScriptLocale.JOBDESC_SHAMAN_PATH,
	}
	
	DESCRIPTION_FILE_NAME_LIMIT =	{
								playerSettingModule.RACE_WARRIOR_M : 1,
								playerSettingModule.RACE_WARRIOR_W : 1,
								playerSettingModule.RACE_ASSASSIN_M : 1,
								playerSettingModule.RACE_ASSASSIN_W	: 1,
								playerSettingModule.RACE_SURA_M : 1,
								playerSettingModule.RACE_SURA_W : 1,
								playerSettingModule.RACE_SHAMAN_M : 1,
								playerSettingModule.RACE_SHAMAN_W : 1,
	}
	
	FACE_IMAGE_DICT_1 = {
							playerSettingModule.RACE_WARRIOR_M : "d:/ymir work/ui/intro/public_intro/face/face_warrior_m_01.sub",
							playerSettingModule.RACE_WARRIOR_W : "d:/ymir work/ui/intro/public_intro/face/face_warrior_w_01.sub",
							playerSettingModule.RACE_ASSASSIN_M : "d:/ymir work/ui/intro/public_intro/face/face_assassin_m_01.sub",
							playerSettingModule.RACE_ASSASSIN_W	: "d:/ymir work/ui/intro/public_intro/face/face_assassin_w_01.sub",
							playerSettingModule.RACE_SURA_M : "d:/ymir work/ui/intro/public_intro/face/face_sura_m_01.sub",
							playerSettingModule.RACE_SURA_W : "d:/ymir work/ui/intro/public_intro/face/face_sura_w_01.sub",
							playerSettingModule.RACE_SHAMAN_M : "d:/ymir work/ui/intro/public_intro/face/face_shaman_m_01.sub",
							playerSettingModule.RACE_SHAMAN_W : "d:/ymir work/ui/intro/public_intro/face/face_shaman_w_01.sub",
						}
	
	FACE_IMAGE_DICT_2 = {
							playerSettingModule.RACE_WARRIOR_M : "d:/ymir work/ui/intro/public_intro/face/face_warrior_m_02.sub",
							playerSettingModule.RACE_WARRIOR_W : "d:/ymir work/ui/intro/public_intro/face/face_warrior_w_02.sub",
							playerSettingModule.RACE_ASSASSIN_M : "d:/ymir work/ui/intro/public_intro/face/face_assassin_m_02.sub",
							playerSettingModule.RACE_ASSASSIN_W	: "d:/ymir work/ui/intro/public_intro/face/face_assassin_w_02.sub",
							playerSettingModule.RACE_SURA_M : "d:/ymir work/ui/intro/public_intro/face/face_sura_m_02.sub",
							playerSettingModule.RACE_SURA_W : "d:/ymir work/ui/intro/public_intro/face/face_sura_w_02.sub",
							playerSettingModule.RACE_SHAMAN_M : "d:/ymir work/ui/intro/public_intro/face/face_shaman_m_02.sub",
							playerSettingModule.RACE_SHAMAN_W : "d:/ymir work/ui/intro/public_intro/face/face_shaman_w_02.sub",
						}
	
	FACE_IMAGE_DICT_3 = {
						playerSettingModule.RACE_WARRIOR_M : "icon/face/warrior_m.tga",
						playerSettingModule.RACE_WARRIOR_W : "icon/face/warrior_w.tga",
						playerSettingModule.RACE_ASSASSIN_M : "icon/face/assassin_m.tga",
						playerSettingModule.RACE_ASSASSIN_W	: "icon/face/assassin_w.tga",
						playerSettingModule.RACE_SURA_M : "icon/face/sura_m.tga",
						playerSettingModule.RACE_SURA_W : "icon/face/sura_w.tga",
						playerSettingModule.RACE_SHAMAN_M : "icon/face/shaman_m.tga",
						playerSettingModule.RACE_SHAMAN_W : "icon/face/shaman_w.tga",
					}

	class DescriptionBox(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.descIndex = 0
		def __del__(self):
			ui.Window.__del__(self)
		def SetIndex(self, index):
			self.descIndex = index
		def OnRender(self):
			event.RenderEventSet(self.descIndex)
	
	class CharacterRenderer(ui.Window):
		def OnRender(self):
			self.Refresh()
			grp.ClearDepthBuffer()
			grp.SetGameRenderState()
			grp.PushState()
			grp.SetOmniLight()

			screenWidth = wndMgr.GetScreenWidth()
			screenHeight = wndMgr.GetScreenHeight()
			newScreenWidth = float(screenWidth - 270)
			newScreenHeight = float(screenHeight)

			grp.SetViewport(270.0/screenWidth, 0.0, newScreenWidth/screenWidth, newScreenHeight/screenHeight)

			app.SetCenterPosition(0.0, 0.0, 0.0)
			app.SetCamera(1550.0, 15.0, 180.0, 95.0)
			grp.SetPerspective(10.0, newScreenWidth/newScreenHeight, 1000.0, 3000.0)

			(x, y) = app.GetCursorPosition()
			grp.SetCursorPosition(x, y)

			chr.Deform()
			chr.Render()

			grp.RestoreViewport()
			grp.PopState()
			grp.SetInterfaceRenderState()

	def __init__(self, stream):
		ui.Window.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_SELECT, self)

		self.stream=stream
		self.slot = self.stream.GetCharacterSlot()

		self.openLoadingFlag = False
		self.startIndex = -1
		self.startReservingTime = 0

		self.flagDict = {}
		self.curNameAlpha = []
		self.destNameAlpha = []
		for i in xrange(self.CHARACTER_TYPE_COUNT):
			self.curNameAlpha.append(0.0)
			self.destNameAlpha.append(0.0)

		self.curGauge = [0.0, 0.0, 0.0, 0.0]
		self.destGauge = [0.0, 0.0, 0.0, 0.0]

		self.dlgBoard = 0
		self.changeNameFlag = False
		self.nameInputBoard = None
		self.sendedChangeNamePacket = False

		self.startIndex = -1
		self.isLoad = 0
		if app.ENABLE_PLAYER_PIN_SYSTEM:
			self.pinSuccess = False
		self.isCameraMoving = False
		self.cameraMovementProgress = 0.0

	def __del__(self):
		ui.Window.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_SELECT, 0)

	def Open(self):
		if not self.__LoadBoardDialog(uiScriptLocale.LOCALE_UISCRIPT_PATH + "selectcharacterwindow.py"):
			dbg.TraceError("SelectCharacterWindow.Open - __LoadScript Error")
			return
		
		if not self.__LoadQuestionDialog("uiscript/questiondialog.py"):
			return
		self.onPressKeyDict = None
		playerSettingModule.LoadGameData("INIT")
		self.InitCharacterBoard()
		self.btnStart.Enable()
		self.btnCreate.Enable()
		self.btnDelete.Enable()
		self.btnExit.Enable()
		self.dlgBoard.Show()
		self.SetWindowName("SelectCharacterWindow")
		self.Show()
		
		if self.slot >= 0:
			self.SelectSlot(self.slot)
		
		if musicInfo.selectMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/"+musicInfo.selectMusic)

		app.SetDefaultCamera()
		app.SetCamera(1600.0, 15.0, 190.0, 110.0)
		
		self.isLoad=1
		self.Refresh()
		if self.stream.isAutoSelect:
			chrSlot = self.stream.GetCharacterSlot()
			self.SelectSlot(chrSlot)
			if app.ENABLE_PLAYER_PIN_SYSTEM:
				self.InputPinCode()
			else:
				self.StartGame()
		
		self.SetEmpire(net.GetEmpireID())
		app.ShowCursor()

	def Close(self):
		if musicInfo.selectMusic != "":
			snd.FadeOutMusic("BGM/"+musicInfo.selectMusic)

		self.stream.popupWindow.Close()

		if self.dlgBoard:
			self.dlgBoard.ClearDictionary()

		self.empireName = None
		self.flagDict = {}
		self.dlgBoard = None
		self.btnStart = None
		self.btnCreate = None
		self.btnDelete = None
		self.btnExit = None
		self.btnLeft = None
		self.btnRight = None
		self.backGround = None

		self.dlgQuestion.ClearDictionary()
		self.dlgQuestion = None
		self.dlgQuestionText = None
		self.dlgQuestionAcceptButton = None
		self.dlgQuestionCancelButton = None
		self.privateInputBoard = None
		self.nameInputBoard = None

		chr.DeleteInstance(0)
		chr.DeleteInstance(1)
		chr.DeleteInstance(2)
		chr.DeleteInstance(3)
		chr.DeleteInstance(4)

		self.Hide()
		self.KillFocus()
		self.descriptionBox.Hide()
	
		app.HideCursor()
		event.Destroy()

	def SetEmpire(self, id):
		self.empireName.SetText(self.EMPIRE_NAME.get(id, ""))
		if id == 1:
			self.empireName.SetFontColor(1.0, 0, 0)
			self.EmpireFlagA.Show()
			self.EmpireFlagB.Hide()
			self.EmpireFlagC.Hide()
		elif id == 2:
			self.empireName.SetFontColor(1.0, 1.0, 0.0)
			self.EmpireFlagA.Hide()
			self.EmpireFlagB.Show()
			self.EmpireFlagC.Hide()
		elif id == 3:
			self.empireName.SetFontColor(0.0, 0, 1.0)
			self.EmpireFlagA.Hide()
			self.EmpireFlagB.Hide()
			self.EmpireFlagC.Show()
		
	def HideAllFlag(self):
		for flag in self.flagDict.values():
			flag.Hide()

	def Refresh(self):
		if not self.isLoad:
			return
		self.SelectSlot(0)
		self.CharacterSlot_0.Hide()
		self.CharacterFace_0.Hide()
		self.CharacterSlot_0_Lv.Hide()
		self.CharacterSlot_0_Name.Hide()
		self.CharacterSlot_1.Hide()
		self.CharacterFace_1.Hide()
		self.CharacterSlot_1_Lv.Hide()
		self.CharacterSlot_1_Name.Hide()
		self.CharacterSlot_2.Hide()
		self.CharacterFace_2.Hide()
		self.CharacterSlot_2_Lv.Hide()
		self.CharacterSlot_2_Name.Hide()
		self.CharacterSlot_3.Hide()
		self.CharacterFace_3.Hide()
		self.CharacterSlot_3_Lv.Hide()
		self.CharacterSlot_3_Name.Hide()
		self.CharacterSlot_4.Hide()
		self.CharacterFace_4.Hide()
		self.CharacterSlot_4_Lv.Hide()
		self.CharacterSlot_4_Name.Hide()
		for i in xrange(self.SLOT_COUNT):
			id = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_ID)
			race = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_RACE)
			name = net.GetAccountCharacterSlotDataString(i, net.ACCOUNT_CHARACTER_SLOT_NAME)
			level = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_LEVEL)
			if id != 0:
				if i == 0:
					self.CharacterFace_0.LoadImage(self.FACE_IMAGE_DICT_2[race])
					self.CharacterFace_0.Show()
					self.CharacterSlot_0.Show()
					self.CharacterSlot_0_Name.SetText(name)
					self.CharacterSlot_0_Name.Show()
					self.CharacterSlot_0_Lv.SetText(localeInfo.INTRO_SELECT_LEVEL % (level))
					self.CharacterSlot_0_Lv.SetFontColor(30, 72, 168)
					self.CharacterSlot_0_Lv.Show()
				elif i == 1:
					self.CharacterFace_1.LoadImage(self.FACE_IMAGE_DICT_2[race])
					self.CharacterFace_1.Show()
					self.CharacterSlot_1.Show()
					self.CharacterSlot_1_Name.SetText(name)
					self.CharacterSlot_1_Name.Show()
					self.CharacterSlot_1_Lv.SetText(localeInfo.INTRO_SELECT_LEVEL % (level))
					self.CharacterSlot_1_Lv.SetFontColor(30, 72, 168)
					self.CharacterSlot_1_Lv.Show()
				elif i == 2:
					self.CharacterFace_2.LoadImage(self.FACE_IMAGE_DICT_2[race])
					self.CharacterFace_2.Show()
					self.CharacterSlot_2.Show()
					self.CharacterSlot_2_Name.SetText(name)
					self.CharacterSlot_2_Name.Show()
					self.CharacterSlot_2_Lv.SetText(localeInfo.INTRO_SELECT_LEVEL % (level))
					self.CharacterSlot_2_Lv.SetFontColor(30, 72, 168)
					self.CharacterSlot_2_Lv.Show()
				elif i == 3:
					self.CharacterFace_3.LoadImage(self.FACE_IMAGE_DICT_2[race])
					self.CharacterFace_3.Show()
					self.CharacterSlot_3.Show()
					self.CharacterSlot_3_Name.SetText(name)
					self.CharacterSlot_3_Name.Show()
					self.CharacterSlot_3_Lv.SetText(localeInfo.INTRO_SELECT_LEVEL % (level))
					self.CharacterSlot_3_Lv.SetFontColor(30, 72, 168)
					self.CharacterSlot_3_Lv.Show()
				elif i == 4:
					self.CharacterFace_4.LoadImage(self.FACE_IMAGE_DICT_2[race])
					self.CharacterFace_4.Show()
					self.CharacterSlot_4.Show()
					self.CharacterSlot_4_Name.SetText(name)
					self.CharacterSlot_4_Name.Show()
					self.CharacterSlot_4_Lv.SetText(localeInfo.INTRO_SELECT_LEVEL % (level))
					self.CharacterSlot_4_Lv.SetFontColor(30, 72, 168)
					self.CharacterSlot_4_Lv.Show()

	def GetCharacterSlotID(self, slotIndex):
		return net.GetAccountCharacterSlotDataInteger(slotIndex, net.ACCOUNT_CHARACTER_SLOT_ID)

	def __LoadQuestionDialog(self, fileName):
		self.dlgQuestion = ui.ScriptWindow()

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self.dlgQuestion, fileName)
		except:
			import exception
			exception.Abort("SelectCharacterWindow.LoadQuestionDialog.LoadScript")

		try:
			GetObject=self.dlgQuestion.GetChild
			self.dlgQuestionText=GetObject("message")
			self.dlgQuestionAcceptButton=GetObject("accept")
			self.dlgQuestionCancelButton=GetObject("cancel")
		except:
			import exception
			exception.Abort("SelectCharacterWindow.LoadQuestionDialog.BindObject")

		self.dlgQuestionText.SetText(localeInfo.SELECT_DO_YOU_DELETE_REALLY)
		self.dlgQuestionAcceptButton.SetEvent(ui.__mem_func__(self.RequestDeleteCharacter))
		self.dlgQuestionCancelButton.SetEvent(ui.__mem_func__(self.dlgQuestion.Hide))
		return 1

	def __LoadBoardDialog(self, fileName):
		self.dlgBoard = ui.ScriptWindow()
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self.dlgBoard, fileName)
		except:
			import exception
			exception.Abort("SelectCharacterWindow.LoadBoardDialog.LoadScript")
		
		try:
			GetObject=self.dlgBoard.GetChild
			self.backGroundImg1 = GetObject("BackGround")
			self.backGroundImg2 = GetObject("BackGround2")
			self.backGroundImg2.Hide()
			self.backGroundImg3 = GetObject("BackGround3")
			self.backGroundImg3.Hide()
			
			self.btnStart = GetObject("start_button")
			self.btnCreate = GetObject("create_button")
			self.btnDelete = GetObject("delete_button")
			self.btnExit = GetObject("exit_button")
			self.CharacterSlot_0 = GetObject("CharacterSlot_0")
			self.CharacterFace_0 = GetObject("CharacterFace_0")
			self.CharacterSlot_0_Lv = ui.TextLine()
			self.CharacterSlot_0_Lv.SetParent(self.CharacterSlot_0)
			self.CharacterSlot_0_Lv.SetPosition(108, 19)
			self.CharacterSlot_0_Lv.SetHorizontalAlignLeft()
			self.CharacterSlot_0_Lv.SetText("")
			self.CharacterSlot_0_Name = ui.TextLine()
			self.CharacterSlot_0_Name.SetParent(self.CharacterSlot_0)
			self.CharacterSlot_0_Name.SetPosition(140, 3)
			self.CharacterSlot_0_Name.SetHorizontalAlignRight()
			self.CharacterSlot_0_Name.SetText("")
			self.CharacterSlot_1 = GetObject("CharacterSlot_1")
			self.CharacterFace_1 = GetObject("CharacterFace_1")
			self.CharacterSlot_1_Lv = ui.TextLine()
			self.CharacterSlot_1_Lv.SetParent(self.CharacterSlot_1)
			self.CharacterSlot_1_Lv.SetPosition(108, 19)
			self.CharacterSlot_1_Lv.SetHorizontalAlignLeft()
			self.CharacterSlot_1_Lv.SetText("")
			self.CharacterSlot_1_Name = ui.TextLine()
			self.CharacterSlot_1_Name.SetParent(self.CharacterSlot_1)
			self.CharacterSlot_1_Name.SetPosition(140, 3)
			self.CharacterSlot_1_Name.SetHorizontalAlignRight()
			self.CharacterSlot_1_Name.SetText("")
			self.CharacterSlot_2 = GetObject("CharacterSlot_2")
			self.CharacterFace_2 = GetObject("CharacterFace_2")
			self.CharacterSlot_2_Lv = ui.TextLine()
			self.CharacterSlot_2_Lv.SetParent(self.CharacterSlot_2)
			self.CharacterSlot_2_Lv.SetPosition(108, 19)
			self.CharacterSlot_2_Lv.SetHorizontalAlignLeft()
			self.CharacterSlot_2_Lv.SetText("")
			self.CharacterSlot_2_Name = ui.TextLine()
			self.CharacterSlot_2_Name.SetParent(self.CharacterSlot_2)
			self.CharacterSlot_2_Name.SetPosition(140, 3)
			self.CharacterSlot_2_Name.SetHorizontalAlignRight()
			self.CharacterSlot_2_Name.SetText("")
			self.CharacterSlot_3 = GetObject("CharacterSlot_3")
			self.CharacterFace_3 = GetObject("CharacterFace_3")
			self.CharacterSlot_3_Lv = ui.TextLine()
			self.CharacterSlot_3_Lv.SetParent(self.CharacterSlot_3)
			self.CharacterSlot_3_Lv.SetPosition(108, 19)
			self.CharacterSlot_3_Lv.SetHorizontalAlignLeft()
			self.CharacterSlot_3_Lv.SetText("")
			self.CharacterSlot_3_Name = ui.TextLine()
			self.CharacterSlot_3_Name.SetParent(self.CharacterSlot_3)
			self.CharacterSlot_3_Name.SetPosition(140, 3)
			self.CharacterSlot_3_Name.SetHorizontalAlignRight()
			self.CharacterSlot_3_Name.SetText("")
			self.CharacterSlot_4 = GetObject("CharacterSlot_4")
			self.CharacterFace_4 = GetObject("CharacterFace_4")
			self.CharacterSlot_4_Lv = ui.TextLine()
			self.CharacterSlot_4_Lv.SetParent(self.CharacterSlot_4)
			self.CharacterSlot_4_Lv.SetPosition(108, 19)
			self.CharacterSlot_4_Lv.SetHorizontalAlignLeft()
			self.CharacterSlot_4_Lv.SetText("")
			self.CharacterSlot_4_Name = ui.TextLine()
			self.CharacterSlot_4_Name.SetParent(self.CharacterSlot_4)
			self.CharacterSlot_4_Name.SetPosition(140, 3)
			self.CharacterSlot_4_Name.SetHorizontalAlignRight()
			self.CharacterSlot_4_Name.SetText("")
			self.DiscFace = GetObject("DiscFace")
			self.raceName = GetObject("raceName_Text")
			
			self.my_id = GetObject("my_id")
			self.textBoard = GetObject("text_board")
			self.textBoardPrev = GetObject("prev_button")
			self.textBoardNext = GetObject("next_button")
			
			self.CharacterHTH = GetObject("hth_value")
			self.CharacterINT = GetObject("int_value")
			self.CharacterSTR = GetObject("str_value")
			self.CharacterDEX = GetObject("dex_value")
			
			self.NameList = []
			self.NameList.append(GetObject("name_warrior"))
			self.NameList.append(GetObject("name_assassin"))
			self.NameList.append(GetObject("name_sura"))
			self.NameList.append(GetObject("name_shaman"))
			
			self.GaugeList = []
			self.GaugeList.append(GetObject("hth_gauge"))
			self.GaugeList.append(GetObject("int_gauge"))
			self.GaugeList.append(GetObject("str_gauge"))
			self.GaugeList.append(GetObject("dex_gauge"))
			
			self.empireName = GetObject("EmpireName")
			self.EmpireFlagA = GetObject("EmpireFlag_A")
			self.EmpireFlagB = GetObject("EmpireFlag_B")
			self.EmpireFlagC = GetObject("EmpireFlag_C")
		except:
			import exception
			exception.Abort("SelectCharacterWindow.LoadBoardDialog.BindObject")
		
		self.toolTip = uitooltip.ToolTip()
		self.toolTip.ClearToolTip()
		
		self.descIndex = 0
		self.descriptionBox = self.DescriptionBox()
		self.descriptionBox.Show()
		self.my_id.SetText(constInfo.ACCOUNT_NAME)
		self.textBoardPrev.SetEvent(ui.__mem_func__(self.PrevDescriptionPage))
		self.textBoardNext.SetEvent(ui.__mem_func__(self.NextDescriptionPage))
		GetObject("start_button").ShowToolTip = lambda arg=1: self.OverInButton(arg)
		GetObject("start_button").HideToolTip = lambda arg=1: self.OverOutButton()
		GetObject("exit_button").ShowToolTip = lambda arg=2: self.OverInButton(arg)
		GetObject("exit_button").HideToolTip = lambda arg=2: self.OverOutButton()
		GetObject("delete_button").ShowToolTip = lambda arg=3: self.OverInButton(arg)
		GetObject("delete_button").HideToolTip = lambda arg=3: self.OverOutButton()
		GetObject("create_button").ShowToolTip = lambda arg=4: self.OverInButton(arg)
		GetObject("create_button").HideToolTip = lambda arg=4: self.OverOutButton()
		GetObject("CharacterSlot_0").ShowToolTip = lambda arg=5: self.OverInButton(arg)
		GetObject("CharacterSlot_0").HideToolTip = lambda arg=5: self.OverOutButton()
		GetObject("CharacterSlot_1").ShowToolTip = lambda arg=6: self.OverInButton(arg)
		GetObject("CharacterSlot_1").HideToolTip = lambda arg=6: self.OverOutButton()
		GetObject("CharacterSlot_2").ShowToolTip = lambda arg=7: self.OverInButton(arg)
		GetObject("CharacterSlot_2").HideToolTip = lambda arg=7: self.OverOutButton()
		GetObject("CharacterSlot_3").ShowToolTip = lambda arg=8: self.OverInButton(arg)
		GetObject("CharacterSlot_3").HideToolTip = lambda arg=8: self.OverOutButton()
		GetObject("CharacterSlot_4").ShowToolTip = lambda arg=9: self.OverInButton(arg)
		GetObject("CharacterSlot_4").HideToolTip = lambda arg=9: self.OverOutButton()
		GetObject("prev_button").ShowToolTip = lambda arg=9: self.OverInButton(arg)
		GetObject("prev_button").HideToolTip = lambda arg=9: self.OverOutButton()
		GetObject("next_button").ShowToolTip = lambda arg=10: self.OverInButton(arg)
		GetObject("next_button").HideToolTip = lambda arg=10: self.OverOutButton()
		
		for name in self.NameList:
			name.SetAlpha(0.0)
		
		if app.ENABLE_PLAYER_PIN_SYSTEM:
			self.btnStart.SetEvent(ui.__mem_func__(self.InputPinCode))
		else:
			self.btnStart.SetEvent(ui.__mem_func__(self.StartGame))
		self.btnCreate.SetEvent(ui.__mem_func__(self.CreateCharacter))
		self.btnExit.SetEvent(ui.__mem_func__(self.ExitSelect))
		
		self.CharacterSlot_0.SAFE_SetEvent(self.SelectSlot, 0)
		self.CharacterSlot_1.SAFE_SetEvent(self.SelectSlot, 1)
		self.CharacterSlot_2.SAFE_SetEvent(self.SelectSlot, 2)
		self.CharacterSlot_3.SAFE_SetEvent(self.SelectSlot, 3)
		self.CharacterSlot_4.SAFE_SetEvent(self.SelectSlot, 4)
		
		if NOT_NEED_DELETE_CODE:
			self.btnDelete.SetEvent(ui.__mem_func__(self.PopupDeleteQuestion))
		else:
			self.btnDelete.SetEvent(ui.__mem_func__(self.InputPrivateCode))


		self.backGroundImg1.Hide()

		return 1

	def LoadMap(self):
		id = net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_ID)
		if 0 == id:
			GetObject=self.dlgBoard.GetChild
			self.backGroundImg1 = GetObject("BackGround")
			self.backGroundImg1.Show()
		else:
			background.Initialize()
			MAPS_LIST = [
				{"x": 30000, "y": 40000, "map_name": "metin2_map_a1"},
				{"x": 12300, "y": 32100, "map_name": "metin2_map_n_desert_01"},
				{"x": 78300, "y": 84400, "map_name": "map_a2"},
				{"x": 78000, "y": 62000, "map_name": "map_n_snowm_01"},
				{"x": 80400, "y": 19200, "map_name": "metin2_map_b1"},
				{"x": 35800, "y": 54400, "map_name": "metin2_map_c1"},
			]
			map_random = MAPS_LIST[app.GetRandom(0,len(MAPS_LIST)-1)]
			background.LoadMap(map_random["map_name"], map_random["x"], map_random["y"], 0)
			background.SetShadowLevel(background.SHADOW_ALL)
			chr.SelectInstance(self.slot)
			player.SetMainCharacterIndex(self.slot)
			chr.SetPixelPosition(map_random["x"], map_random["y"])
			self.backGroundImg1.Hide()

	def OnRender(self):
		app.RenderGame()
		grp.PopState()
		grp.SetInterfaceRenderState()

	def SameLoginDisconnect(self):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeInfo.LOGIN_FAILURE_SAMELOGIN, self.ExitSelect, localeInfo.UI_OK)
		#self.stream.popupWindow.Open(localeInfo.LOGIN_FAILURE_ALREAY, app.Exit, localeInfo.UI_OK)

	def PrevDescriptionPage(self):
		if event.IsWait(self.descIndex) == True:
			if event.GetVisibleStartLine(self.descIndex)-14 >= 0:
				event.SetVisibleStartLine(self.descIndex, event.GetVisibleStartLine(self.descIndex)-14)
				event.Skip(self.descIndex)
		else:
			event.Skip(self.descIndex)

	def NextDescriptionPage(self):
		if event.IsWait(self.descIndex) == True:
			event.SetVisibleStartLine(self.descIndex, event.GetVisibleStartLine(self.descIndex)+14)
			event.Skip(self.descIndex)
		else:
			event.Skip(self.descIndex)

	def MakeCharacter(self, index, id, name, race, form, hair):
		if 0 == id:
			return
		
		chr.CreateInstance(index)
		chr.SelectInstance(index)
		chr.SetVirtualID(index)
		chr.SetInstanceType(chr.INSTANCE_TYPE_PLAYER)
		chr.SetNameString(name)
		
		chr.SetRace(race)
		chr.SetArmor(form)
		chr.SetHair(hair)
		chr.Refresh()
		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)
		chr.SetRotation(0.0)
		chr.SetPixelPosition(30000, 40000, 30)
		self.DiscFace.LoadImage(self.FACE_IMAGE_DICT_3[race])
		self.DiscFace.Show()
		self.raceName.SetText(self.RACE_NAME[race])
		self.raceName.Show()

	## Manage Character
	def StartGame(self):
		self.toolTip.Hide()
		
		if self.sendedChangeNamePacket:
			return
		
		if self.changeNameFlag:
			self.OpenChangeNameDialog()
			return
		
		if -1 != self.startIndex:
			return
		
		if musicInfo.selectMusic != "":
			snd.FadeLimitOutMusic("BGM/"+musicInfo.selectMusic, systemSetting.GetMusicVolume()*0.05)
		
		self.btnStart.SetUp()
		self.btnCreate.SetUp()
		self.btnDelete.SetUp()
		self.btnExit.SetUp()
		
		self.btnStart.Disable()
		self.btnCreate.Disable()
		self.btnDelete.Disable()
		self.btnExit.Disable()
		self.dlgQuestion.Hide()
		
		self.stream.SetCharacterSlot(self.slot)
		self.startIndex = self.slot
		self.startReservingTime = app.GetTime()
		for i in xrange(self.SLOT_COUNT):
			if False == chr.HasInstance(i):
				continue
			
			chr.SelectInstance(i)
			if i == self.slot:
				self.slot = self.slot
				chr.PushOnceMotion(chr.MOTION_INTRO_SELECTED, 0.1)
				continue
			
			chr.PushOnceMotion(chr.MOTION_INTRO_NOT_SELECTED, 0.1)

	def OpenChangeNameDialog(self):
		import uiCommon
		nameInputBoard = uiCommon.InputDialogWithDescription()
		nameInputBoard.SetTitle(localeInfo.SELECT_CHANGE_NAME_TITLE)
		nameInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputName))
		nameInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputName))
		nameInputBoard.SetMaxLength(chr.PLAYER_NAME_MAX_LEN)
		nameInputBoard.SetBoardWidth(200)
		nameInputBoard.SetDescription(localeInfo.SELECT_INPUT_CHANGING_NAME)
		nameInputBoard.Open()
		nameInputBoard.slot = self.slot
		self.nameInputBoard = nameInputBoard

	def OnChangeName(self, id, name):
		self.SelectSlot(id)
		self.sendedChangeNamePacket = False
		self.PopupMessage(localeInfo.SELECT_CHANGED_NAME)

	def AcceptInputName(self):
		changeName = self.nameInputBoard.GetText()
		if not changeName:
			return

		self.sendedChangeNamePacket = True
		net.SendChangeNamePacket(self.nameInputBoard.slot, changeName)
		return self.CancelInputName()

	def CancelInputName(self):
		self.nameInputBoard.Close()
		self.nameInputBoard = None
		return True

	def OnCreateFailure(self, type):
		self.sendedChangeNamePacket = False
		if 0 == type:
			self.PopupMessage(localeInfo.SELECT_CHANGE_FAILURE_STRANGE_NAME)
		elif 1 == type:
			self.PopupMessage(localeInfo.SELECT_CHANGE_FAILURE_ALREADY_EXIST_NAME)
		elif 100 == type:
			self.PopupMessage(localeInfo.SELECT_CHANGE_FAILURE_STRANGE_INDEX)

	def CreateCharacter(self):
		self.toolTip.Hide()
		for i in xrange(self.SLOT_COUNT):
			id = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_ID)
			if id == 0:
				self.stream.SetCharacterSlot(i)
				EMPIRE_MODE = 1
				if EMPIRE_MODE:
					if self.__AreAllSlotEmpty():
						self.stream.SetReselectEmpirePhase()
					else:
						self.stream.SetCreateCharacterPhase()
				else:
					self.stream.SetCreateCharacterPhase()
				
				break

	def __AreAllSlotEmpty(self):
		for iSlot in xrange(self.SLOT_COUNT):
			if 0!=net.GetAccountCharacterSlotDataInteger(iSlot, net.ACCOUNT_CHARACTER_SLOT_ID):
				return 0
		return 1

	def PopupDeleteQuestion(self):
		id = self.GetCharacterSlotID(self.slot)
		if 0 == id:
			return

		self.dlgQuestion.Show()
		self.dlgQuestion.SetTop()

	def RequestDeleteCharacter(self):
		self.dlgQuestion.Hide()

		id = self.GetCharacterSlotID(self.slot)
		if 0 == id:
			self.PopupMessage(localeInfo.SELECT_EMPTY_SLOT)
			return
		self.PopupMessage(localeInfo.SELECT_DELEING)

	def InputPrivateCode(self):
		
		import uiCommon
		privateInputBoard = uiCommon.InputDialogWithDescription()
		privateInputBoard.SetTitle(localeInfo.INPUT_PRIVATE_CODE_DIALOG_TITLE)
		privateInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPrivateCode))
		privateInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPrivateCode))

		if ENABLE_ENGNUM_DELETE_CODE:
			pass
		else:
			privateInputBoard.SetNumberMode()

		privateInputBoard.SetSecretMode()
		privateInputBoard.SetMaxLength(7)
			
		privateInputBoard.SetBoardWidth(250)
		privateInputBoard.SetDescription(localeInfo.INPUT_PRIVATE_CODE_DIALOG_DESCRIPTION)
		privateInputBoard.Open()
		self.privateInputBoard = privateInputBoard

	def AcceptInputPrivateCode(self):
		privateCode = self.privateInputBoard.GetText()
		if not privateCode:
			return

		id = self.GetCharacterSlotID(self.slot)
		if 0 == id:
			self.PopupMessage(localeInfo.SELECT_EMPTY_SLOT)
			return

		net.SendDestroyCharacterPacket(self.slot, privateCode)
		self.PopupMessage(localeInfo.SELECT_DELEING)

		self.CancelInputPrivateCode()
		return True

	def CancelInputPrivateCode(self):
		self.privateInputBoard = None
		return True

	def OnDeleteSuccess(self, slot):
		self.PopupMessage(localeInfo.SELECT_DELETED)
		self.DeleteCharacter(slot)

	def OnDeleteFailure(self):
		self.PopupMessage(localeInfo.SELECT_CAN_NOT_DELETE)

	if app.ENABLE_PLAYER_PIN_SYSTEM:
		def InputPinCode(self):
			if self.pinSuccess == True:
				return

			import uiCommon
			pinCodeInputBoard = uiCommon.InputPinCodeDialog()
			pinCodeInputBoard.SetTitle(localeInfo.INPUT_PIN_CODE_DIALOG_TITLE)
			pinCodeInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPinCode))
			pinCodeInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPinCode))

			pinCodeInputBoard.SetNumberMode()
			pinCodeInputBoard.SetSecretMode()

			pinCodeInputBoard.Open()
			self.pinCodeInputBoard = pinCodeInputBoard

		def AcceptInputPinCode(self):
			pinCode = self.pinCodeInputBoard.GetText()
			if not pinCode:
				return

			id = self.GetCharacterSlotID(self.slot)
			if 0 == id:
				self.PopupMessage(localeInfo.SELECT_EMPTY_SLOT)
				return

			net.SendCharacterPinCodePacket(self.slot, pinCode)

			self.CancelInputPinCode()
			return True

		def CancelInputPinCode(self):
			self.pinCodeInputBoard = None
			return True

		def OnPinCodeSuccess(self):
			self.pinSuccess = True
			self.StartGame()

		def OnPinCodeFailure(self):
			self.pinSuccess = False

			import introLogin
			self.disconectDlg = introLogin.ConnectingDialog()
			self.disconectDlg.Open(3.0)
			self.disconectDlg.SetText(localeInfo.SELECT_WRONG_PIN_CODE)
			self.disconectDlg.SAFE_SetTimeOverEvent(self.ExitPhase)

		def ExitPhase(self):
			self.Hide()

	def DeleteCharacter(self, index):
		chr.DeleteInstance(index)
		self.SelectSlot(0)
		self.Refresh()

	def ExitSelect(self):
		self.dlgQuestion.Hide()
	
		if LEAVE_BUTTON_FOR_POTAL:
			if app.loggined:
				self.stream.SetPhaseWindow(0)
			else:
				self.stream.setloginphase()
		else:
			self.stream.SetLoginPhase()

		self.Hide()

	def GetSlotIndex(self):
		return self.slot

	def DecreaseSlotIndex(self):
		slotIndex = (self.GetSlotIndex() - 1 + self.SLOT_COUNT) % self.SLOT_COUNT
		self.SelectSlot(slotIndex)

	def IncreaseSlotIndex(self):
		slotIndex = (self.GetSlotIndex() + 1) % self.SLOT_COUNT
		self.SelectSlot(slotIndex)

	def SelectSlot(self, index):

		# if key == app.DIK_ESC:
			# self.RequestDropItem(False)
		#player.SetSingleDIKKeyState(app.DIK_RIGHT, True) #dondurme komutu python

		self.InitCharacterBoard()
		if index < 0:
			return
		
		if index >= self.SLOT_COUNT:
			return
		
		snd.PlaySound("sound/ui/click.wav")
		chr.DeleteInstance(0)
		chr.DeleteInstance(1)
		chr.DeleteInstance(2)
		chr.DeleteInstance(3)
		chr.DeleteInstance(4)
		self.DiscFace.Hide()
		self.raceName.Hide()
		id = net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_ID)
		race = net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_RACE)
		form = net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_FORM)
		name = net.GetAccountCharacterSlotDataString(index, net.ACCOUNT_CHARACTER_SLOT_NAME)
		hair = net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_HAIR)
		#acce = net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_ACCE)
		#acce_spec = net.GetAccountCharacterSlotDataInteger(index, net.ACCOUNT_CHARACTER_SLOT_ACCE_SPEC)
		if id != 0:
			#self.MakeCharacter(index, id, name, race, form, hair, acce, acce_spec)
			self.MakeCharacter(index, id, name, race, form, hair)
		
		if index == 0:
			self.CharacterSlot_0.Down()
			self.CharacterSlot_0_Name.SetFontColor(1.0, 0.6000, 0.3500)
			self.CharacterFace_0.LoadImage(self.FACE_IMAGE_DICT_1[race])
			self.CharacterSlot_1.SetUp()
			self.CharacterSlot_2.SetUp()
			self.CharacterSlot_3.SetUp()
			self.CharacterSlot_4.SetUp()
			self.CharacterSlot_1_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_2_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_3_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_4_Name.SetFontColor(0.8549, 0.8549, 0.8549)
		elif index == 1:
			self.CharacterSlot_1.Down()
			self.CharacterSlot_1_Name.SetFontColor(1.0, 0.6000, 0.3500)
			self.CharacterFace_1.LoadImage(self.FACE_IMAGE_DICT_1[race])
			self.CharacterSlot_0.SetUp()
			self.CharacterSlot_2.SetUp()
			self.CharacterSlot_3.SetUp()
			self.CharacterSlot_4.SetUp()
			self.CharacterSlot_0_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_2_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_3_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_4_Name.SetFontColor(0.8549, 0.8549, 0.8549)
		elif index == 2:
			self.CharacterSlot_2.Down()
			self.CharacterSlot_2_Name.SetFontColor(1.0, 0.6000, 0.3500)
			self.CharacterFace_2.LoadImage(self.FACE_IMAGE_DICT_1[race])
			self.CharacterSlot_0.SetUp()
			self.CharacterSlot_1.SetUp()
			self.CharacterSlot_3.SetUp()
			self.CharacterSlot_4.SetUp()
			self.CharacterSlot_0_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_1_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_3_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_4_Name.SetFontColor(0.8549, 0.8549, 0.8549)
		elif index == 3:
			self.CharacterSlot_3.Down()
			self.CharacterSlot_3_Name.SetFontColor(1.0, 0.6000, 0.3500)
			self.CharacterFace_3.LoadImage(self.FACE_IMAGE_DICT_1[race])
			self.CharacterSlot_0.SetUp()
			self.CharacterSlot_1.SetUp()
			self.CharacterSlot_2.SetUp()
			self.CharacterSlot_4.SetUp()
			self.CharacterSlot_0_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_1_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_2_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_4_Name.SetFontColor(0.8549, 0.8549, 0.8549)
		elif index == 4:
			self.CharacterSlot_4.Down()
			self.CharacterSlot_4_Name.SetFontColor(1.0, 0.6000, 0.3500)
			self.CharacterFace_4.LoadImage(self.FACE_IMAGE_DICT_1[race])
			self.CharacterSlot_0.SetUp()
			self.CharacterSlot_1.SetUp()
			self.CharacterSlot_2.SetUp()
			self.CharacterSlot_3.SetUp()
			self.CharacterSlot_0_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_1_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_2_Name.SetFontColor(0.8549, 0.8549, 0.8549)
			self.CharacterSlot_3_Name.SetFontColor(0.8549, 0.8549, 0.8549)
		
		self.slot = index
		self.LoadMap()
		if self.__AreAllSlotEmpty():
			self.isCameraMoving = False
		else:
			self.isCameraMoving = True
			self.cameraMovementProgress = 0.0

		for i in xrange(self.CHARACTER_TYPE_COUNT):
			self.destNameAlpha[i] = 0.0
		
		self.destGauge = [0.0, 0.0, 0.0, 0.0]
		id=net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_ID)
		if 0 != id:
			self.btnStart.Show()
			self.btnDelete.Show()
			self.btnCreate.Hide()
			for i in xrange(self.SLOT_COUNT):
				id_check = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_ID)
				if id_check == 0:
					self.btnCreate.Show()
			
			playTime=net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_PLAYTIME)
			level=net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_LEVEL)
			race=net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_RACE)
			valueHTH=net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_HTH)
			valueINT=net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_INT)
			valueSTR=net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_STR)
			valueDEX=net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_DEX)
			name=net.GetAccountCharacterSlotDataString(self.slot, net.ACCOUNT_CHARACTER_SLOT_NAME)
			guildID=net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_GUILD_ID)
			guildName=net.GetAccountCharacterSlotDataString(self.slot, net.ACCOUNT_CHARACTER_SLOT_GUILD_NAME)
			self.changeNameFlag=net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG)
			
			job = chr.RaceToJob(race)
			if job >= 0 and job < self.CHARACTER_TYPE_COUNT:
				self.destNameAlpha[job] = 1.0
			
			self.CharacterHTH.SetText(str(valueHTH))
			self.CharacterINT.SetText(str(valueINT))
			self.CharacterSTR.SetText(str(valueSTR))
			self.CharacterDEX.SetText(str(valueDEX))
			
			statesSummary = float(valueHTH + valueINT + valueSTR + valueDEX)
			if statesSummary > 0.0:
				self.destGauge = [
										((float(valueHTH) * 10) / 90) / 10,
										((float(valueINT) * 10) / 90) / 10,
										((float(valueSTR) * 10) / 90) / 10,
										((float(valueDEX) * 10) / 90) / 10
									]
			
			event.ClearEventSet(self.descIndex)
			self.descIndex = event.RegisterEventSet(self.DESCRIPTION_FILE_NAME[race])
			if self.DESCRIPTION_FILE_NAME_LIMIT[race] == 1:
				self.textBoardPrev.Show()
				self.textBoardNext.Show()
			else:
				self.textBoardPrev.Hide()
				self.textBoardNext.Hide()
		else:
			self.textBoardPrev.Hide()
			self.textBoardNext.Hide()

	def InitCharacterBoard(self):
		self.btnStart.Hide()
		self.btnDelete.Hide()
		self.btnCreate.Show()
		
		self.CharacterHTH.SetText("")
		self.CharacterINT.SetText("")
		self.CharacterSTR.SetText("")
		self.CharacterDEX.SetText("")
		
	## Event
	def OnKeyDown(self, key):

		# if key == app.DIK_ESC:
			# self.RequestDropItem(False)
			# player.SetSingleDIKKeyState(app.DIK_RIGHT, True)

		if 1 == key:
			self.ExitSelect()
		if 2 == key:
			self.SelectSlot(0)
		if 3 == key:
			self.SelectSlot(1)
		if 4 == key:
			self.SelectSlot(2)
		if 5 == key:
			self.SelectSlot(3)

		if 28 == key:

			if app.ENABLE_PLAYER_PIN_SYSTEM:
				id = net.GetAccountCharacterSlotDataInteger(self.slot, net.ACCOUNT_CHARACTER_SLOT_ID)
				if 0 == id:
					self.CreateCharacter()
				else:
					if app.ENABLE_PLAYER_PIN_SYSTEM:
						self.InputPinCode()
					else:
						self.StartGame()

		if 203 == key:
			self.slot = (self.GetSlotIndex() - 1 + self.SLOT_COUNT) % self.SLOT_COUNT
			self.SelectSlot(self.slot)
		if 205 == key:
			self.slot = (self.GetSlotIndex() + 1) % self.SLOT_COUNT
			self.SelectSlot(self.slot)

		return True

	def Lerp(self, a, b, t):
		return (1 - t) * a + t * b

	def OnUpdate(self):
		app.UpdateGame()
		if self.isCameraMoving == True:
			self.cameraMovementProgress += 0.01
			startLoc = [5827.0, 90.0, 350.0]
			endLoc = [300.0, 10.0, 180.0]
			dist = self.Lerp(startLoc[0], endLoc[0], self.cameraMovementProgress)
			put = self.Lerp(startLoc[1], endLoc[1], self.cameraMovementProgress)
			rot = self.Lerp(startLoc[2], endLoc[2], self.cameraMovementProgress)
			app.SetCamera(dist, put, rot, 150.0)
			if self.cameraMovementProgress >= 1:
				self.isCameraMoving = False
				self.cameraMovementProgress = 0.0
		#chr.Update()
		(xposEventSet, yposEventSet) = self.textBoard.GetGlobalPosition()
		event.UpdateEventSet(self.descIndex, xposEventSet+7, -(yposEventSet+7))
		self.descriptionBox.SetIndex(self.descIndex)
		
		for i in xrange(4):
			self.curGauge[i] += (self.destGauge[i] - self.curGauge[i]) / 10.0
			if abs(self.curGauge[i] - self.destGauge[i]) < 0.005:
				self.curGauge[i] = self.destGauge[i]
			
			self.GaugeList[i].SetPercentage(self.curGauge[i], 1.0)
		for i in xrange(self.CHARACTER_TYPE_COUNT):
			self.curNameAlpha[i] += (self.destNameAlpha[i] - self.curNameAlpha[i]) / 10.0
			self.NameList[i].SetAlpha(self.curNameAlpha[i])
		for i in xrange(self.SLOT_COUNT):
			if False == chr.HasInstance(i):
				continue
		if -1 != self.startIndex:
			if app.GetTime() - self.startReservingTime > 3.0:
				if False == self.openLoadingFlag:
					chrSlot=self.stream.GetCharacterSlot()
					net.DirectEnter(chrSlot)
					self.openLoadingFlag = True
					import chat
					chat.Clear()

	def OverInButton(self, stat):
		if stat == 1:
			self.toolTip.ClearToolTip()
			self.toolTip.AlignHorizonalCenter()
			self.toolTip.AutoAppendNewTextLine(uiScriptLocale.SELECT_SELECT, grp.GenerateColor(1.0, 1.0, 0.0, 1.0))
			self.toolTip.Show()
		elif stat == 2:
			self.toolTip.ClearToolTip()
			self.toolTip.AlignHorizonalCenter()
			self.toolTip.AutoAppendNewTextLine(uiScriptLocale.SELECT_EXIT, grp.GenerateColor(1.0, 1.0, 0.0, 1.0))
			self.toolTip.Show()
		elif stat == 3:
			self.toolTip.ClearToolTip()
			self.toolTip.AlignHorizonalCenter()
			self.toolTip.AutoAppendNewTextLine(uiScriptLocale.SELECT_DELETE, grp.GenerateColor(1.0, 1.0, 0.0, 1.0))
			self.toolTip.Show()
		elif stat == 4:
			self.toolTip.ClearToolTip()
			self.toolTip.AlignHorizonalCenter()
			self.toolTip.AutoAppendNewTextLine(uiScriptLocale.SELECT_CREATE, grp.GenerateColor(1.0, 1.0, 0.0, 1.0))
			self.toolTip.Show()
		elif stat >= 5 and stat <= 8:
			playTime = net.GetAccountCharacterSlotDataInteger(stat-5, net.ACCOUNT_CHARACTER_SLOT_PLAYTIME)
			guildName = net.GetAccountCharacterSlotDataString(stat-5, net.ACCOUNT_CHARACTER_SLOT_GUILD_NAME)
			
			self.toolTip.ClearToolTip()
			self.toolTip.AlignHorizonalCenter()
			if guildName:
				self.toolTip.AutoAppendNewTextLine(uiScriptLocale.SELECT_YES_GUILD % (guildName), grp.GenerateColor(1.0, 1.0, 0.0, 1.0), True)
			else:
				self.toolTip.AutoAppendNewTextLine(uiScriptLocale.SELECT_NO_GUILD, grp.GenerateColor(1.0, 1.0, 0.0, 1.0), True)

			self.toolTip.AutoAppendNewTextLine(uiScriptLocale.SELECT_PLAYTIME % (playTime), grp.GenerateColor(1.0, 1.0, 0.0, 1.0))
			self.toolTip.Show()
		elif stat == 9:
			self.toolTip.ClearToolTip()
			self.toolTip.AlignHorizonalCenter()
			self.toolTip.AutoAppendNewTextLine(uiScriptLocale.CREATE_PREV, grp.GenerateColor(1.0, 1.0, 0.0, 1.0))
			self.toolTip.Show()
		elif stat == 10:
			self.toolTip.ClearToolTip()
			self.toolTip.AlignHorizonalCenter()
			self.toolTip.AutoAppendNewTextLine(uiScriptLocale.CREATE_NEXT, grp.GenerateColor(1.0, 1.0, 0.0, 1.0))
			self.toolTip.Show()

	def OverOutButton(self):
		self.toolTip.Hide()

	def EmptyFunc(self):
		pass

	def PopupMessage(self, msg, func=0):
		if not func:
			func=self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeInfo.UI_OK)

	def OnPressExitKey(self):
		self.ExitSelect()
		return True

