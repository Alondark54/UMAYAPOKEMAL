import ui
import net
import app
import localeInfo
import chrmgr
import wndMgr
import os
import chat

STREAMER_IMAGE_DIC = {
	1	: "streamer/logo1.png",
	2	: "streamer/logo1.png",
	3	: "streamer/logo1.png",
}

class LinkWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.streamer_image = None
		self.link = None
		self.web = 0

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/streamerlink.py")
		except:
			import exception
			exception.Abort("streamerdialog.LoadDialog.LoadScript")

		try:
			GetObject=self.GetChild
			self.titleBar = GetObject("titlebar")
			self.streamer_image = self.GetChild("streamer_image")

		except:
			import exception
			exception.Abort("streamerdialog.LoadDialog.BindObject")

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		self.imagebutton = ui.ImageBox()
		self.imagebutton.SetParent(self)
		self.imagebutton.LoadImage("streamer/logo1.png")
		# self.imagebutton.SAFE_SetStringEvent("MOUSE_LEFT_BUTTON_UP",self.__OnClickLinkButton)
		self.imagebutton.SetEvent(ui.__mem_func__(self.__OnClickLinkButton), "mouse_click")
		self.imagebutton.SetPosition(15, 34)
		self.imagebutton.Show()

	def Open(self, arg1, arg2, arg3):
		image = STREAMER_IMAGE_DIC[int(arg1)]
		self.web = int(arg2)
		self.link = str(arg3)
		self.Show()
		#self.streamer_image.LoadImage(image)
		self.imagebutton.LoadImage(image)

	def __OnClickLinkButton(self):

		if int(self.web) == 1:
			createlink = "https://www.youtube.com/" + self.link
		elif int(self.web) == 2:
			createlink = "https://www.tiktok.com/" + self.link
		elif int(self.web) == 3:
			createlink = "https://www.twitch.tv/" + self.link
		elif int(self.web) == 4:
			createlink = "https://www.kick.com/" + self.link

		os.startfile(createlink)

	def Destroy(self):
		self.ClearDictionary()
		self.streamer_image = None
		self.link = None
		self.web = 0

	def Close(self):
		self.Hide()