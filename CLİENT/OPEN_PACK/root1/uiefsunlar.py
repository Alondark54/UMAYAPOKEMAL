import dbg
import ui
import snd
import localeInfo
import constInfo
import musicInfo
from _weakref import proxy
import uiswitchbot
import uiefsunbotyesil
import uiSelectMusic
import grp
import app
import chrmgr
import player
import net
import background
import chat

MUSIC_FILENAME_MAX_LEN = 25
blockMode = 0

#########################################################
COLOR_BG = grp.GenerateColor(0.0, 0.0, 0.0, 0.5)
COLOR_INACTIVE = grp.GenerateColor(1.0, 0.0, 0.0, 0.2)
COLOR_ACTIVE   = grp.GenerateColor(1.0, 0.6, 0.1, 0.2)
COLOR_FINISHED = grp.GenerateColor(0.0, 1.0, 0.0, 0.2)

COLOR_INACTIVE_RARE = grp.GenerateColor(1.0, 0.2, 0.0, 0.2)
COLOR_ACTIVE_RARE   = grp.GenerateColor(1.0, 0.7, 0.2, 0.2)

COLOR_HIGHLIGHT_RARE = grp.GenerateColor(1.0, 0.2, 0.2, 0.05)

COLOR_PIN_HINT = grp.GenerateColor(0.0, 0.5, 1.0, 0.3)


COLOR_CHECKBOX_NOT_SELECTED = grp.GenerateColor(1.0, 0.3, 0.0, 0.1)
COLOR_CHECKBOX_SELECTED = grp.GenerateColor(0.3, 1.0, 1.0, 0.3)
#########################################################

class Bar(ui.Bar):
	def __init__(self,layer = "UI"):
		ui.Bar.__init__(self,layer)
	def SetColor(self,color):
		wndMgr.SetColor(self.hWnd, color)
		self.color = color

class OptionDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__Load()
		self.uiswitchbot.Show()
		self.uiefsunbotyesil.Show()

		self.uiswitchbot.Hide()
		self.uiefsunbotyesil.Hide()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		print " -------------------------------------- DELETE SYSTEM OPTION DIALOG"

	def __Initialize(self):
		self.tilingMode = 0
		self.titleBar = 0
		self.changeMusicButton = 0
		self.changeMusicButton2 = 0
		self.wndInterface = 0
		
	def Destroy(self):
		self.ClearDictionary()

		self.__Initialize()
		print " -------------------------------------- DESTROY SYSTEM OPTION DIALOG"

	def __Load_LoadScript(self, fileName):
		try:
			pyScriptLoader = ui.PythonScriptLoader()
			pyScriptLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("System.OptionDialog.__Load_LoadScript")

	def __Load_BindObject(self):
		try:
			GetObject = self.GetChild
			self.titleBar = GetObject("titlebar")
			self.changeMusicButton = GetObject("bgm_button")
			self.changeMusicButton2 = GetObject("bgm_button2")
			#self.changeMusicButton3 = GetObject("bgm_button3")
			#self.changeMusicButton4 = GetObject("bgm_button4")
			
			self.StatusBar2 = self.GetChild("yesil_title")
			self.StatusBar2.SetColor(COLOR_ACTIVE_RARE)
			self.StatusBar2.AddFlag("not_pick")
			self.StatusBar2.Show()

			self.StatusBar3 = self.GetChild("mavi_title")
			self.StatusBar3.SetColor(COLOR_ACTIVE_RARE)
			self.StatusBar3.AddFlag("not_pick")
			self.StatusBar3.Show()

		except:
			import exception
			exception.Abort("OptionDialog.__Load_BindObject")

	def __Load(self):
		self.__Load_LoadScript("uiscript/efsunbotgui.py")
		self.__Load_BindObject()

		self.SetCenterPosition()
		
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		self.changeMusicButton.SAFE_SetEvent(self.__OnClickChangeMusicButton)
		self.changeMusicButton2.SAFE_SetEvent(self.__OnClickChangeMusicButton2)
		#self.changeMusicButton3.SAFE_SetEvent(self.__OnClickChangeMusicButton3)
		#self.changeMusicButton4.SAFE_SetEvent(self.__OnClickChangeMusicButton4)
	
	def BindInterface(self, wInterface):
		self.wndInterface = wInterface
	
	def __OnClickChangeMusicButton(self):
		self.wndInterface.ToggleSwitchbotWindow()
		self.Hide()
		# if not constInfo.normalefsunbotu:
			# execfile('uiswitchbot.py',{})
			# constInfo.normalefsunbotu = 1
		# else:
			# chat.AppendChat(1, "Normal Efsun botu zaten aktif.")

	def __OnClickChangeMusicButton2(self):
		self.Hide()
		if not constInfo.yesilefsunbotu:
			execfile('uiefsunbotyesil.py',{})
			constInfo.yesilefsunbotu = 1
		else:
			chat.AppendChat(1, "Yeþil Efsun botu zaten aktif.")

	def __OnClickChangeMusicButton3(self):
		self.Hide()
		if self.uiswitchbot.sunRise_bot_shown == 1:
			self.uiswitchbot.Hide()
		else:
			self.uiswitchbot.Show()

	def __OnClickChangeMusicButton4(self):
		self.Hide()
		if self.uiefsunbotyesil.sunRise_bot_shown == 1:
			self.uiefsunbotyesil.Hide()
		else:
			self.uiefsunbotyesil.Show()

	def OnCloseInputDialog(self):
		self.inputDialog.Close()
		self.inputDialog = None
		return True

	def OnCloseQuestionDialog(self):
		self.questionDialog.Close()
		self.questionDialog = None
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True
	
	def Show(self):
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def __NotifyChatLine(self, text):
		chat.AppendChat(chat.CHAT_TYPE_INFO, text)
