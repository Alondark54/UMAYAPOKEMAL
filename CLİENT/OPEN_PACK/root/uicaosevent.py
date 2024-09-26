import ui, wndMgr

BOARD_WIDTH = 320
BOARD_HEIGHT = 150
X_START = 8
Y_START = 15
Y_PASS = 20

class CaosEventWindow(ui.ThinBoardCircle):
	def __init__(self,):
		super(CaosEventWindow, self).__init__()
		self.infos = []
		self.children = []
		self.loginImage = CaosEventImage()
		self.loginImage.Hide()
		self.SetPosition(wndMgr.GetScreenWidth() - BOARD_WIDTH - 30, 250)
		self.SetSize(BOARD_WIDTH, BOARD_HEIGHT)

	def __del__(self):
		super(CaosEventWindow, self).__del__()

	def Destroy(self):
		self.Clear()
		del self.loginImage

	def Clear(self):
		del self.children[:]
		self.children = []

	def ClearInfos(self):
		del self.infos[:]
		self.infos = []

	def Open(self):
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()
		self.Clear()
		self.ClearInfos()

	def OpenLoginImage(self):
		self.loginImage.Open()

	def AppendPlayer(self, name, race, level, kill, death, score):
		upData = { 'name' : str(name), 'race' : int(race), 'level' : int(level), 'kill' : int(kill), 'death' : int(death), 'score' : int(score) }
		self.infos.append(upData)

	def Refresh(self):
		self.Clear()
		yPos = 0
		for (idx,info) in enumerate(self.infos):
			yPos = (Y_PASS * idx) + Y_START
			raceImgPath = "caos_event/races/{}.tga".format(info['race'])
			infoStr = "{} [Lv.{}]\t\t({} / {} / {})".format(info['name'], info['level'], info['kill'], info['death'], info['score'])
			
			raceImg = ui.MakeImageBox(self, raceImgPath, X_START - 3, yPos - 6)
			nameText = ui.TextLine()
			nameText.SetParent(self)
			nameText.SetText("{}".format(infoStr))
			nameText.SetPosition(X_START + 20, yPos - 5)
			nameText.Show()
			self.children.append(nameText)
			self.children.append(raceImg)

		if not self.IsShow(): self.Open()

class CaosEventImage(ui.ExpandedImageBox):
	def __init__(self,):
		super(CaosEventImage, self).__init__()

	def __del__(self):
		super(CaosEventImage, self).__del__()

	def Open(self):
		self.LoadImage("caos_event/deneme.tga")
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True