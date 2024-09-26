import ui, app, chat, random, re, localeInfo, net
class BotControlWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.items = []
		self.time = 0
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.items = []
		self.time = 0

	def Open(self):
		self.Refresh()
		self.time = app.GetTime()+5
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	# def OnPressEscapeKey(self):
		# self.Close()

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/botcontrol.py")
		except:
			import exception
			exception.Abort("BotControlWindow.LoadWindow.__LoadScript")
		try:
			# self.titleBar = self.GetChild("titlebar")
			self.itemSlot = self.GetChild("ItemSlot")
			self.itemName = self.GetChild("itemName")
			self.rofText = self.GetChild("rofText")
		except:
			import exception
			exception.Abort("BotControlWindow.LoadWindow.__BindObject")

		# self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		self.itemSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.ClickSlot))

	def ClickSlot(self, selectedSlotPos):
		for i in range(self.itemSlot.GetSlotCount()):
			self.itemSlot.DeactivateSlot(i)
		self.itemSlot.ActivateSlot(selectedSlotPos)
		net.SendChatPacket("/b0t_c4tr9 %d"%int(self.items[selectedSlotPos]))
		# chat.AppendChat(1, "napi10 %d"%self.items[selectedSlotPos])

	def UpdateItems(self, vnums):
		self.items = []
		self.items.extend(vnums)

	def ShuffleArray(self):
		random.shuffle(self.items)

	def SetRealItemName(self, txt):

		def generate_color(text):
			color = '|cff{:02x}{:02x}{:02x}'.format(*map(lambda x: random.randint(0, 255), range(3)))
			return color + text


		tmp_name = re.sub(r"[0-9]","",txt).replace('+', '').replace('_', '')
		new_name = '-'.join(tmp_name[i:i + 1] for i in range(0, len(tmp_name)))
		lowUp_name = "".join( random.choice([generate_color(k.upper()), k ]) for k in new_name )
		# colored_name = "".join( random.choice([generate_color(), k ]) for k in new_name )

		self.itemName.SetText(lowUp_name)

	def SetROF(self, rof):
		self.rofText.SetText("Kalan: %d"%rof)

	def Refresh(self):
		for i in range(self.itemSlot.GetSlotCount()):
			self.itemSlot.ClearSlot(i)

		for i in range(len(self.items)):
			setItemVNum=self.itemSlot.SetItemSlot
			getItemVNum=self.items[i]
			setItemVNum(i, getItemVNum, 0)
		self.itemSlot.RefreshSlot()


	def OnUpdate(self):
		if self.time <= app.GetTime():
			self.ShuffleArray()
			self.Refresh()
			self.time = app.GetTime()+5