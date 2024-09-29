import wndMgr, uiCommon, ui, time, playerSettingModule, localeInfo, snd, mouseModule, constInfo, uiScriptLocale, interfacemodule, dbg
import app, chr, chrmgr, player, net, pack, background, chat, textTail, event, effect, systemSetting, quest, guild, skill, messenger, ime, item

class MaintenanceDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/maintenanceadmin.py")
		except:
			import exception
			exception.Abort("MaintenanceDialog.__LoadDialog.LoadObject")

		try:
			getObject = self.GetChild
			self.titleName = self.GetChild("TitleName")
			self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.OnCancel))
			self.cancelButton = self.GetChild("cancel_button")
			self.bakimButton = self.GetChild("bakimyap")
			self.bakimiptalButton = self.GetChild("bakimiptal")
			self.inputSlot = getObject("InputSlot")
			self.inputValue = getObject("InputValue")
			self.inputSlot2 = getObject("InputSlot2")
			self.inputValue2 = getObject("InputValue2")

		except:
			import exception
			exception.Abort("MaintenanceDialog.__LoadDialog.BindObject")

		self.SetCenterPosition()
		self.SetTop()
		self.cancelButton.SetEvent(ui.__mem_func__(self.OnCancel))
		self.bakimButton.SetEvent(ui.__mem_func__(self.Bakimyap))
		self.bakimiptalButton.SetEvent(ui.__mem_func__(self.Bakimiptal))
		self.cancelButton.Hide()

	def Destroy(self):
		self.ClearDictionary()
		self.titleName = None
		self.cancelButton = None
		self.inputSlot = None
		self.inputValue = None
		self.inputSlot2 = None
		self.inputValue2 = None

	def SetTitle(self, title):
		self.titleName.SetText(title)

	def Bakimyap(self):
		self.Hide()
		time = self.inputValue.GetText()
		duration = self.inputValue2.GetText()
		net.SendChatPacket("/main %d %d 1" % (int(time), int(duration)))

	def Bakimiptal(self):
		self.Hide()
		net.SendChatPacket("/mainclose")

	def OnCancel(self):
		self.Hide()
		return True

class MaintenanceWindow(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.LoadWindow()
		self.maintime = 0
		self.status = 0

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/maintenance.py")
		except:
			import exception
			exception.Abort("MaintenanceWindow.LoadWindow.LoadObject")
		try:
			self.board = self.GetChild("Thinboard")
			self.textLine1 = self.GetChild("message1")
			self.textLine2 = self.GetChild("message2")
			self.textLine3 = self.GetChild("message3")
			self.textLine4 = self.GetChild("message4")
		except:
			import exception
			exception.Abort("MaintenanceWindow.LoadWindow.LoadObject")

	def Open(self, time, duration, status):
		self.SetTop()
		self.Show()
		self.textLine1.SetText("Oyunumuz 21:00'da aktif hale gelecektir.")
		self.textLine3.SetText("Tüm sorularýnýz için Discord Kanalýmýzdan bize ulaþabilirsiniz.")
		self.textLine4.SetText("Þimdiden keyifli oyunlar dileriz.")
		self.maintime = time
		self.status = status

	def Close(self):
		self.Hide()

	def SecondToHM(self, time):
		second = int(time % 60)
		minute = int((time / 60) % 60)
		hour = int((time / 60) / 60) % 24

		if hour <= 0:
			return "%d Dakika %02d Saniye" % (minute, second)
		else:
			return "%d Saat %02d Dakika %02d Saniye" % (hour, minute, second)

	def OnUpdate(self):
		remain_time = self.maintime - app.GetGlobalTimeStamp()
		if self.status == 1:
			self.textLine2.SetText("Beklerken diðer oyuncularýmýzla haritada sohbet edebilirsiniz.")
		else:
			self.Hide()