import ui, player, net, app
import localeInfo

IMG_DIR = "d:/ymir work/ui/game/rank/"

rank_list = {
	player.RANK_LEVEL : {
		"filename":"level.tga",
		"text":"Oyuncu\nLevel",
	},
	player.RANK_STONE : {
		"filename":"stone.tga",
		"text":"Yok Edilen\nMetinler",
	},
	player.RANK_MONSTER : {
		"filename":"monster.tga",
		"text":"Oldurulen\nCanavarlar",
	},
	player.RANK_BOSS : {
		"filename":"boss.tga",
		"text":"Oldurulen\nBosslar",
	},
	player.RANK_DUNGEON : {
		"filename":"dungeon.tga",
		"text":"Tamamlanan\nZindanlar",
	},
	player.RANK_PLAYTIME : {
		"filename":"playtime.tga",
		"text":"Oyun Suresi",
	},
	player.RANK_GOLD : {
		"filename":"yang.tga",
		"text":"Yang",
	},
	player.RANK_CHEST : {
		"filename":"chest.tga",
		"text":"Acilan\nSandiklar",
	},
}

class RankingWindow(ui.ScriptWindow):

	class rankListBoxItem(ui.Window):
		def __del__(self):
			ui.Window.__del__(self)
		def __init__(self, parent, index, name, empire, value, isMe):
			ui.Window.__init__(self)
			self.SetParent(parent)
			self.SetSize(500, 38)
			self.children = []
			self.InitItem(index, name, empire, value, isMe)
		def Destroy(self):
			self.children = []
		def InitItem(self, index, name, empire, value, isMe):
			color = ""
			if isMe:
				color = "|cffffcc00"
			elif index == 1:
				color = "|cffffcc00"
			elif index == 2:
				color = "|cFFB0C4DE"
			elif index == 3:
				color = "|cFF8B4513"
			
			playerRank = ui.TextLine()
			playerRank.SetParent(self)
			playerRank.SetPosition(15, 3)
			playerRank.SetHorizontalAlignCenter()
			if isMe and index > 10:
				playerRank.SetText("%s-"%color)
			else:
				playerRank.SetText("%s%d" % (color,index))
			playerRank.Show()
			self.children.append(playerRank)
			
			playerText = ui.TextLine()
			playerText.SetParent(self)
			playerText.SetPosition(85, 3)
			playerText.SetText("%s%s" % (color,name))
			playerText.SetHorizontalAlignCenter()
			playerText.Show()
			self.children.append(playerText)

			playerEmpire = ui.ImageBox()
			playerEmpire.SetParent(self)
			playerEmpire.LoadImage(IMG_DIR+"empire_%d.sub"%empire)
			playerEmpire.SetPosition(175, 2)
			playerEmpire.Show()
			self.children.append(playerEmpire)

			playerValue = ui.TextLine()
			playerValue.SetParent(self)
			playerValue.SetPosition(300, 3)
			playerValue.SetHorizontalAlignCenter()
			if value == "11":
				value="-"
			playerValue.SetText("%s%s" % (color,NumberToMoneyStringNEW(value)))
			playerValue.Show()
			self.children.append(playerValue)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.rankIndex = -1
		self.loadingImageRotation=0
		self.rankTimeDict={}
		self.loadingImage = None
		self.lastItem=None
		self.scrollBar = None
		self.Initializition()

	def OnRunMouseWheel(self, nLen):#it was like this
		scrollBar = self.scrollBar
		if scrollBar.IsShow():
			if nLen > 0:
				scrollBar.OnUp()
			else:
				scrollBar.OnDown()
			return True
		return False

	def Destroy(self):
		self.loadingImage = None
		if self.lastItem:
			self.lastItem.Hide()
			self.lastItem.Destroy()
			self.lastItem=None
		self.rankListBoxEx.RemoveAllItems()
		self.rankListBoxEx=None
		self.rankButtonListBoxEx.RemoveAllItems()
		self.rankButtonListBoxEx=None
		self.scrollBar=None
		self.rankIndex = 0
		self.loadingImageRotation=0
		self.rankTimeDict={}
		self.ClearDictionary()

	def Initializition(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/ranking.py")

			self.rankListBoxEx = self.GetChild("ListBoxNEW")
			self.rankButtonListBoxEx = self.GetChild("LxistBox")
			self.GetChild("EventBoardTitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

		except:
			import exception
			exception.Abort("EventInfo.LoadDialog.LoadScript")

		self.rankButtonListBoxEx.SetItemStep(46)
		self.rankButtonListBoxEx.SetItemSize(400,38)
		self.rankButtonListBoxEx.SetViewItemCount(7)

		self.rankListBoxEx.SetViewItemCount(15)
		self.rankListBoxEx.SetItemStep(26)
		self.rankListBoxEx.SetItemSize(400,38)
		self.LoadRankButtons()
		
		self.scrollBar = ScrollBarNew()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetPosition(185,45)
		self.scrollBar.SetScrollBarSize(330)
		self.scrollBar.SetScrollStep(0.4)
		self.rankButtonListBoxEx.SetScrollBar(self.scrollBar)
		self.scrollBar.Show()

		self.loadingImage = ui.ExpandedImageBox()
		self.loadingImage.SetParent(self.rankListBoxEx)
		self.loadingImage.LoadImage(IMG_DIR+"load_.tga")
		self.loadingImage.SetPosition(self.rankListBoxEx.GetWidth()/2-25,self.rankListBoxEx.GetHeight()/2-95)
		self.loadingImage.Hide()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()

	def Open(self):
		if self.rankIndex == -1:
			self.LoadInfo(0)
			self.rankButtonListBoxEx.SetBasePos(0)
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def LoadRankButtons(self):
		global rank_list
		for key, data in rank_list.items():
			rankCatoryBtn = ui.Button()
			rankCatoryBtn.SetParent(self)
			rankCatoryBtn.SetUpVisual(IMG_DIR+"btn_0.tga")
			rankCatoryBtn.SetOverVisual(IMG_DIR+"btn_0.tga")
			rankCatoryBtn.SetDownVisual(IMG_DIR+"btn_1.tga")
			rankCatoryBtn.SetEvent(lambda x = key: self.LoadInfo(x))
			rankCatoryBtn.SetPosition(1, 2)
			rankCatoryBtn.index = key
			rankCatoryBtn.Show()

			rankImage = ui.ImageBox()
			rankImage.SetParent(rankCatoryBtn)
			rankImage.LoadImage(IMG_DIR+data["filename"])
			rankImage.SetPosition(17,9)
			rankImage.Show()
			rankCatoryBtn.rankImage = rankImage

			rankText = MultiTextLine()
			rankText.SetParent(rankCatoryBtn)
			rankText.SetTextType("horizontal#left")
			rankText.SetTextRange(16)
			rankText.SetText(data["text"])
			if len(rankText.children) > 1:
				rankText.SetPosition(17+rankImage.GetWidth()+10,7)
			else:
				rankText.SetPosition(17+rankImage.GetWidth()+10,12)
			rankText.Show()
			rankCatoryBtn.rankText = rankText

			self.rankButtonListBoxEx.AppendItem(rankCatoryBtn)

			self.rankTimeDict[key] = -1

	def checkRankUpdateAlgorithm(self, index):
		isNeedPacket = False
		(playerName, playerEmpire, playerValue) = player.GetRankInfo(index,0)
		if playerName == "" or playerEmpire == 0 or playerValue == 0:
			isNeedPacket=True
		else:
			isNeedPacket=False
		if self.rankTimeDict.has_key(index):
			if app.GetGlobalTimeStamp() > self.rankTimeDict[index]:
				isNeedPacket=True
			else:
				isNeedPacket=False
		else:
			isNeedPacket=True
		return isNeedPacket

	def LoadInfo(self, index):
		if self.rankIndex == index:
			return

		if self.lastItem:
			self.lastItem.Hide()
			self.lastItem.Destroy()
			self.lastItem=None

		self.rankListBoxEx.RemoveAllItems()
		self.rankIndex = index

		data = self.rankButtonListBoxEx.itemList
		for listboxItem in data:
			if index == listboxItem.index:
				listboxItem.SetUpVisual(IMG_DIR+"btn_1.tga")
				listboxItem.SetOverVisual(IMG_DIR+"btn_1.tga")
				listboxItem.SetDownVisual(IMG_DIR+"btn_1.tga")
			else:
				listboxItem.SetUpVisual(IMG_DIR+"btn_0.tga")
				listboxItem.SetOverVisual(IMG_DIR+"btn_0.tga")
				listboxItem.SetDownVisual(IMG_DIR+"btn_1.tga")

		self.loadingImage.Show()
		if self.checkRankUpdateAlgorithm(index) == True:
			net.SendChatPacket("/rank_update %d"%index)
		else:
			self.LoadRankData(index, False)

	def CreateLastItem(self, rankTopIndex):
		if self.lastItem:
			self.lastItem.Hide()
			self.lastItem.Destroy()
			self.lastItem=None

		(playerName, playerEmpire, playerValue) = player.GetRankInfo(self.rankIndex,11)
		if playerName == player.GetName():
			self.lastItem = self.rankListBoxItem(self.rankListBoxEx, rankTopIndex, playerName, playerEmpire, playerValue, True)
			self.lastItem.SetPosition(0,26*10)
			self.lastItem.Show()

	def UpdateLastItem(self, index):
		if index != self.rankIndex:
			return
		for j in xrange(10):
			(playerName, playerEmpire, playerValue) = player.GetRankInfo(self.rankIndex,j+1)
			if playerName == "" or playerEmpire == 0 or playerValue == 0:
				continue
			if playerName == player.GetName():
				self.CreateLastItem(j+1)
				return
		self.CreateLastItem(11)

	def LoadRankData(self, index, isFirstUpdate):
		self.loadingImage.Hide()
		if isFirstUpdate:
			self.rankTimeDict[index] = app.GetGlobalTimeStamp()+(30*60)

		myIndexInTop = 11
		for j in xrange(10):
			(playerName, playerEmpire, playerValue) = player.GetRankInfo(index,j+1)
			if playerName == "" or playerEmpire == 0 or playerValue == 0:
				continue

			if playerName == player.GetName():
				myIndexInTop = j+1

			resultItem = self.rankListBoxItem(self, j+1, playerName, playerEmpire, playerValue, False)
			self.rankListBoxEx.AppendItem(resultItem)

		self.CreateLastItem(myIndexInTop)

	def OnUpdate(self):
		if self.loadingImage:
			if self.loadingImage.IsShow():
				self.loadingImage.SetRotation(self.loadingImageRotation)
				self.loadingImageRotation+=10

class ScrollBarNew(ui.Window):
	SCROLLBAR_WIDTH = 13
	SCROLLBAR_MIDDLE_HEIGHT = 1
	SCROLLBAR_BUTTON_WIDTH = 17
	SCROLLBAR_BUTTON_HEIGHT = 17
	SCROLL_BTN_XDIST = 2
	SCROLL_BTN_YDIST = 2
	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.SetWindowName("scrollbar_middlebar")
		def MakeImage(self):
			top = ui.ExpandedImageBox()
			top.SetParent(self)
			top.LoadImage(IMG_DIR+"scrollbar/scrollbar_middle_top.tga")
			top.AddFlag("not_pick")
			top.Show()
			topScale = ui.ExpandedImageBox()
			topScale.SetParent(self)
			topScale.SetPosition(0, top.GetHeight())
			topScale.LoadImage(IMG_DIR+"scrollbar/scrollbar_middle_topscale.tga")
			topScale.AddFlag("not_pick")
			topScale.Show()
			bottom = ui.ExpandedImageBox()
			bottom.SetParent(self)
			bottom.LoadImage(IMG_DIR+"scrollbar/scrollbar_middle_bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			bottomScale = ui.ExpandedImageBox()
			bottomScale.SetParent(self)
			bottomScale.LoadImage(IMG_DIR+"scrollbar/scrollbar_middle_bottomscale.tga")
			bottomScale.AddFlag("not_pick")
			bottomScale.Show()
			middle = ui.ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage(IMG_DIR+"scrollbar/scrollbar_middle_middle.tga")
			middle.AddFlag("not_pick")
			middle.Show()
			self.top = top
			self.topScale = topScale
			self.bottom = bottom
			self.bottomScale = bottomScale
			self.middle = middle
		def SetSize(self, height):
			minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
			height = max(minHeight, height)
			ui.DragButton.SetSize(self, 10, height)
			scale = (height - minHeight) / 2 
			extraScale = 0
			if (height - minHeight) % 2 == 1:
				extraScale = 1
			self.topScale.SetRenderingRect(0, 0, 0, scale - 1)
			self.middle.SetPosition(0, self.top.GetHeight() + scale)
			self.bottomScale.SetPosition(0, self.middle.GetBottom())
			self.bottomScale.SetRenderingRect(0, 0, 0, scale - 1 + extraScale)
			self.bottom.SetPosition(0, height - self.bottom.GetHeight())
	def __init__(self):
		ui.Window.__init__(self)
		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False
		self.CreateScrollBar()
		self.SetScrollBarSize(0)
		self.scrollStep = 0.03
		self.SetWindowName("NONAME_ScrollBar")
	def __del__(self):
		ui.Window.__del__(self)
	def CreateScrollBar(self):
		topImage = ui.ExpandedImageBox()
		topImage.SetParent(self)
		topImage.AddFlag("not_pick")
		topImage.LoadImage(IMG_DIR+"scrollbar/scrollbar_top.tga")
		topImage.Show()
		bottomImage = ui.ExpandedImageBox()
		bottomImage.SetParent(self)
		bottomImage.AddFlag("not_pick")
		bottomImage.LoadImage(IMG_DIR+"scrollbar/scrollbar_bottom.tga")
		bottomImage.Show()
		middleImage = ui.ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, topImage.GetHeight())
		middleImage.LoadImage(IMG_DIR+"scrollbar/scrollbar_middle.tga")
		middleImage.Show()
		self.topImage = topImage
		self.bottomImage = bottomImage
		self.middleImage = middleImage
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(0) # set min height
		self.middleBar = middleBar
	def Destroy(self):
		self.eventScroll = None
		self.eventArgs = None
	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args
	def SetMiddleBarSize(self, pageScale):
		self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - self.SCROLL_BTN_YDIST*2)))
		realHeight = self.GetHeight() - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		self.pageSize = realHeight
	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.pageSize = height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
		self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)
	def SetScrollStep(self, step):
		self.scrollStep = step
	def GetScrollStep(self):
		return self.scrollStep
	def GetPos(self):
		return self.curPos
	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)
	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)
	def SetPos(self, pos, moveEvent = True):
		pos = max(0.0, pos)
		pos = min(1.0, pos)
		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
		if moveEvent == True:
			self.OnMove()
	def OnMove(self):
		if self.lockFlag:
			return
		if 0 == self.pageSize:
			return
		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLL_BTN_YDIST) / float(self.pageSize)
		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)
	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		newPos = float(yMouseLocalPosition) / float(self.GetHeight())
		self.SetPos(newPos)
	def LockScroll(self):
		self.lockFlag = True
	def UnlockScroll(self):
		self.lockFlag = False


class MultiTextLine(ui.Window):
	def __del__(self):
		ui.Window.__del__(self)
	def Destroy(self):
		self.children = []
		self.rangeText = 0
	def __init__(self):
		ui.Window.__init__(self)
		self.children = []
		self.rangeText = 15
		self.textType = ""
	def SetTextType(self, textType):
		self.textType = textType
		for text in self.children:
			self.AddTextType(self.textType.split("#"),text)
	def SetTextRange(self, range):
		self.rangeText = range
		yPosition = 0
		for text in self.children:
			text.SetPosition(0,yPosition)
			yPosition+=self.rangeText
	def AddTextType(self, typeArg, text):
		if len(typeArg) > 1:
			if typeArg[0] == "vertical":
				if typeArg[1] == "top":
					text.SetVerticalAlignTop()
				elif typeArg[1] == "bottom":
					text.SetVerticalAlignBottom()
				elif typeArg[1] == "center":
					text.SetVerticalAlignCenter()
			elif typeArg[0] == "horizontal":
				if typeArg[1] == "left":
					text.SetHorizontalAlignLeft()
				elif typeArg[1] == "right":
					text.SetHorizontalAlignRight()
				elif typeArg[1] == "center":
					text.SetHorizontalAlignCenter()
	def SetText(self, cmd):
		if len(self.children) > 1:
			self.children=[]
		multi_arg = cmd.split("\n")
		yPosition = 0
		for text in multi_arg:
			childText = ui.TextLine()
			childText.SetParent(self)
			childText.SetPosition(0,yPosition)
			if self.textType != "":
				self.AddTextType(self.textType.split("#"),childText)
			childText.SetText(str(text))
			childText.Show()
			self.children.append(childText)
			yPosition+=self.rangeText

def NumberToMoneyStringNEW(n) :
	if n <= 0 :
		return "0"
	return "%s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))