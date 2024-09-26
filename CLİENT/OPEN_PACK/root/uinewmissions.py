#----------------------------------------------------------------------------
# Created By  : Larry Watterson
# Created Date: 30.03.22
# version ='1.0'
# ---------------------------------------------------------------------------

import ui, wndMgr, localeInfo, net

PATH = "missions/"

class MissionWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.Initialize()
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.Initialize()

	def Initialize(self):
		self.ToolTip = None
		self.misnDict = {}
		self.globalMissionDict = {}
		self.myInfo = {}
		self.window_dict = {}
		self.lastSelect = None

	def SetItemToolTip(self, tooltip):
		self.ToolTip = tooltip

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Open(self):
		if self.lastSelect != None:
			self.ChanegCategory(self.lastSelect)
		self.SetTop()
		self.SetCenterPosition()
		ui.ScriptWindow.Show(self)

	def Close(self):
		if self.ToolTip:
			self.ToolTip.HideToolTip()
		self.Hide()

	def LoadWindow(self):
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/newmissions.py")

		except KeyError, msg:
			import dbg
			dbg.TraceError("NewMission #1")

		try:
			self.__BindObjects()

		except KeyError, msg:
			import dbg
			dbg.TraceError("NewMission #2 - %s" % str(msg))

		try:
			self.__BindEvents()
		
		except KeyError, msg:
			import dbg
			dbg.TraceError("NewMission #3 - %s" % str(msg))

	def __BindObjects(self):
		self.window_dict = {
			"titlebar" : self.GetChild("titlebar"),
			"lst_box" : self.GetChild("ListBox"),
			"global_button" : self.GetChild("global_button"),
			"missions_button" : self.GetChild("missions_button"),
			"lst_scroll" : self.GetChild("ScrollBar"),
		}

	def __BindEvents(self):
		self.window_dict["titlebar"].SetCloseEvent(ui.__mem_func__(self.Close))

		self.window_dict["lst_box"].SetViewItemCount(3)
		self.window_dict["lst_box"].SetItemStep(100)
		self.window_dict["lst_box"].SetItemSize(444,85)



		self.window_dict['global_button'].SetEvent(ui.__mem_func__(self.ChanegCategory), "GLOBAL")
		self.window_dict['missions_button'].SetEvent(ui.__mem_func__(self.ChanegCategory), "PLAYER")
		self.window_dict['global_button'].Hide()
		self.window_dict["lst_box"].SetScrollBar(self.window_dict["lst_scroll"])


	def ChanegCategory(self, category):
		if category == "GLOBAL":
			# if self.window_dict["missions_button"].IsShow():
				# return
			self.lastSelect = "GLOBAL"
			# self.window_dict["lst_scroll"].Show()
			self.window_dict["global_button"].Hide()
			self.window_dict["missions_button"].Show()
			self.AppenMissions()
		else:
			# if self.window_dict["global_button"].IsShow():
				# return
			self.window_dict["global_button"].Show()
			self.window_dict["missions_button"].Hide()
			# self.window_dict["lst_scroll"].Hide()
			self.lastSelect = "PLAYER"
			self.AppendGlobalMissions()

	def AppendMissionInfos(self, idx, desc, needcount):
		text = str(desc).replace("#", " ")
		info = { "idx" : int(idx), "desc" : text, "needcount": int(needcount), "ms" : 0, "vnum" : [], "count" : [] }
		self.misnDict.update({ int(idx) : { } })
		self.misnDict[int(idx)].update(info)

	def AppendGlobalMissionInfos(self, idx, desc, winner, vnum, count):
		text = str(desc).replace("#", " ")
		info = { "desc" : text, "winner": str(winner), "vnum" : int(vnum), "count" : int(count) }
		self.globalMissionDict.update({ int(idx) : { } })
		self.globalMissionDict[int(idx)].update(info)

	def UpdateMissionItem(self, idx, vnum, count):
		if self.misnDict.has_key(int(idx)):
			self.misnDict[int(idx)]["vnum"].append(vnum)
			self.misnDict[int(idx)]["count"].append(count)

	def MyMissions(self, idx, value):
		if self.misnDict.has_key(int(idx)):
			self.misnDict[int(idx)]['ms'] = value

	def AppenMissions(self):
		self.window_dict["lst_box"].RemoveAllItems()
		for k,v in self.misnDict.items():
			info = MissionInfo(self, k, v["idx"], v["desc"], v["needcount"], v["ms"], v["vnum"], v["count"])
			self.window_dict["lst_box"].AppendItem(info)

	def AppendGlobalMissions(self):
		self.window_dict["lst_box"].RemoveAllItems()
		for v in self.globalMissionDict.values():
			info = GlobalMissionInfo(self, v["desc"], v["winner"], v["vnum"], v["count"])
			self.window_dict["lst_box"].AppendItem(info)

	def UpdateMyVal(self, idx, val):
		for info in self.window_dict["lst_box"].GetItems():
			if int(idx) == info.GetMissionIDX():
				info.UpdateText(val)
				return

class GlobalMissionInfo(ui.Window):
	def __init__(self, parent, desc, winner, vnum, count):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.parent = parent
		self.desc = str(desc)
		self.winner = str(winner)
		self.vnum = int(vnum)
		self.count = int(count)
		self.SetSize(444,85)
		self.LoadWindow()

	def __del__(self):
		ui.Window.__del__(self)
		del self.itemSlot
		del self.parent
		del self.descText
		del self.winnerText
		del self.desc
		del self.winner
		del self.vnum
		del self.count

	def LoadWindow(self):
		self.bg = ui.ImageBox()
		self.bg.SetParent(self)
		self.bg.LoadImage(PATH + "bg2.png")
		self.bg.SetPosition(0, 0)
		self.bg.Show()

		self.msicon = ui.ImageBox()
		self.msicon.SetParent(self.bg)
		self.msicon.LoadImage(PATH + "icon3.png")
		self.msicon.SetPosition(0, 0)
		self.msicon.Show()

		self.descText = ui.TextLine()
		self.descText.SetParent(self.bg)
		self.descText.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		# self.descText.SetPackedFontColor(0xff00e5ee)
		self.descText.SetWindowHorizontalAlignLeft()
		self.descText.SetPosition(100,17)
		self.descText.SetText(self.desc.title())
		self.descText.Show()

		self.winnerText = ui.TextLine()
		self.winnerText.SetParent(self.bg)
		self.winnerText.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		self.winnerText.SetPackedFontColor(0xff00e5ee)
		self.winnerText.SetWindowHorizontalAlignLeft()
		self.winnerText.SetPosition(100,47)
		self.winnerText.SetText(self.winner)
		self.winnerText.Show()

		self.itemSlot = ui.GridSlotWindow()
		self.itemSlot.SetParent(self.bg)
		self.itemSlot.SetPosition(325,10)
		self.itemSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.itemSlot.ArrangeSlot(0, 1, 1, 32, 32, 0, 0)
		self.itemSlot.SetItemSlot(0, self.vnum, self.count)
		self.itemSlot.RefreshSlot()
		self.itemSlot.SetSlotBaseImage("missions/slot.png", 1.0, 1.0, 1.0, 1.0)
		self.itemSlot.Show()

	def OverInItem(self, slotIndex):
		if self.parent.ToolTip:
			self.parent.ToolTip.SetItemToolTip(self.vnum)

	def OverOutItem(self):
		if self.parent.ToolTip:
			self.parent.ToolTip.HideToolTip()

class MissionInfo(ui.Window):
	def __init__(self, parent, msidx, idx, desc, needcount, ms, vnum, count):
		ui.Window.__init__(self)
		self.Initialize()
		self.SetParent(parent)
		self.parent = parent
		self.idx = int(idx) + 1
		self.desc = str(desc)
		self.needcount = int(needcount)
		self.ms = int(ms)
		self.msidx = int(msidx)
		self.SetSize(444,85)
		for i in vnum:
			self.items["vnum"].append(int(i))
		for i in count:
			self.items["count"].append(int(i))
		self.LoadWindow()

	def __del__(self):
		ui.Window.__del__(self)
		del self.items 
		del self.itemSlots 
		del self.bg 
		del self.descText 
		del self.msText 
		del self.itemSlot
		del self.parent 
		del self.idx 
		del self.desc 
		del self.needcount 
		del self.ms 
		del self.msidx 

	def Initialize(self):
		self.items = { "vnum" : [], "count" : [] }
		self.itemSlots = []

	def LoadWindow(self):
		self.bg = ui.ImageBox()
		self.bg.SetParent(self)
		self.bg.LoadImage(PATH + "bg1.png")
		self.bg.SetPosition(0, 0)
		self.bg.Show()

		self.msicon = ui.ImageBox()
		self.msicon.SetParent(self.bg)
		self.msicon.LoadImage(PATH + "icon" + str(self.idx) +".png")
		self.msicon.SetPosition(0, 0)
		self.msicon.Show()

		self.descText = ui.TextLine()
		self.descText.SetParent(self.bg)
		self.descText.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		self.descText.SetPackedFontColor(0xffFFFFFF)
		self.descText.SetWindowHorizontalAlignLeft()
		self.descText.SetPosition(100,17)
		self.descText.SetText(self.desc.title())
		self.descText.Show()

		self.msText = ui.TextLine()
		self.msText.SetParent(self.bg)
		self.msText.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
		self.msText.SetPackedFontColor(0xffFFFFFF)
		self.msText.SetWindowHorizontalAlignLeft()
		self.msText.SetPosition(100,47)
		self.msText.SetText("Ilerleme : {} / {}".format(self.ms, self.needcount))
		self.msText.Show()

		self.itemSlot = ui.GridSlotWindow()
		self.itemSlot.SetParent(self.bg)
		self.itemSlot.SetPosition(325,10)
		self.itemSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.itemSlot.ArrangeSlot(0, 3, 2, 32, 32, 0, 0)
		self.itemSlot.RefreshSlot()
		self.itemSlot.SetSlotBaseImage("missions/slot.png", 1.0, 1.0, 1.0, 1.0)
		self.itemSlot.Show()

		for i in range(len(self.items["vnum"])):
			self.itemSlot.SetItemSlot(i, self.items["vnum"][i], self.items["count"][i])

	def GetMissionIDX(self):
		return self.msidx

	def UpdateText(self, val):
		self.ms = val
		self.msText.SetText("Ilerleme : {} / {}".format(self.ms, self.needcount))

	def OverInItem(self, slotIndex):
		if self.parent.ToolTip:
			vnum = self.items["vnum"][slotIndex]
			if vnum != 0:
				self.parent.ToolTip.SetItemToolTip(vnum)

	def OverOutItem(self):
		if self.parent.ToolTip:
			self.parent.ToolTip.HideToolTip()