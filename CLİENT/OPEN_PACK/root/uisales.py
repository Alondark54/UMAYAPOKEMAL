import app
import ui
import uiScriptLocale
import uiToolTip
import constInfo
import localeInfo
import uiCommon
import net
import player
import item
import chr
import dbg
import background
from _weakref import proxy
from itertools import islice
import chat
import grid		 
  
class SalesClass(ui.ScriptWindow):
	class SearchResultItem(ui.Window):

		def __init__(self, parent, index, parent2):
			ui.Window.__init__(self)
			
			self.parent = parent
			self.parent2 = parent2
			self.isLoad = True
			self.isSelected = False
			self.index = index
			self.id = 0
			
			self.SetParent(parent2)
			self.InitItem()

			
		def InitItem(self):
			startX = 18
			yPos = 3
			
			self.tabimage = ui.MakeImageBox(self, "sales/normal.tga", 3, yPos+2)
			self.tabimage.SAFE_SetStringEvent("MOUSE_LEFT_UP",self.__OnSelect)
			self.tabimage.SetTop()
			self.tabimage.Show()
			
			self.title = ui.TextLine()
			self.title.SetParent(self)
			self.title.SetPosition(startX+67-75, yPos+6)
			self.title.SetFontName("Arial:12")
			self.title.Show()
			
			
			self.timer = ui.TextLine()
			self.timer.SetParent(self)
			self.timer.SetPosition(startX+67-75, yPos+18)
			self.timer.SetFontName("Arial:12")
			self.timer.Show()
			

						
			self.SetSize(self.tabimage.GetWidth(), self.tabimage.GetHeight())
			

			
		def SetQuestName(self, name):
			self.title.SetText(name)
			
		def SetID(self, id):
			self.id = int(id)

		def SetTimerText(self, time):
			self.timer.SetText(time)
			
				

		
		
		def __OnSelect(self):
			self.parent.OnSearchResultItemSelect(self.index)

		def Select(self):

			self.isSelected = True
			self.isLoad = True

		def UnSelect(self):
			self.isSelected = False
			self.isLoad = True
			


		def OnUpdate(self):
			sure = int(constInfo.SalesList[int(self.index)]["time"])
			tip = int(constInfo.SalesList[int(self.index)]["type"])

			sure = sure-app.GetGlobalTimeStamp()
			
			
			
			if sure > 0:
				if tip > 0:
					self.title.SetText("Teklif "+str(self.id)+" "+ constInfo.sarirenk+" Teklif henüz baþlamadý! ")
					self.SetTimerText(constInfo.sarirenk + localeInfo.CalcTimeWithLetter(sure))
				else:
					self.title.SetText("Teklif "+str(self.id)+" "+ constInfo.yesilrenk+" Teklif devam ediyor! ")
					self.SetTimerText(constInfo.yesilrenk +localeInfo.CalcTimeWithLetter(sure))
			else:
				if tip > 0:
					sure = sure+(tip*60*60)
					if sure > 0:
						self.title.SetText("Teklif "+str(self.id)+" "+ constInfo.yesilrenk+" Teklif devam ediyor! ")
						self.SetTimerText(constInfo.yesilrenk +localeInfo.CalcTimeWithLetter(sure))
					else:
						self.title.SetText("Teklif "+str(self.id))
						self.SetTimerText(constInfo.kirmizirenk + "Teklifin süresi doldu!")
				else:
					self.title.SetText("Teklif "+str(self.id))
					self.SetTimerText(constInfo.kirmizirenk + "Teklifin süresi doldu!")
			


		def OnRender(self):
			if self.isLoad:
				if self.isSelected:
					self.tabimage.LoadImage("sales/active.tga")
				else:
					self.tabimage.LoadImage("sales/normal.tga")
					
				self.isLoad = False


	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
		self.selectedItemIndex = -1
		self.board = None
		self.RefreshSymbol = None
		self.QuestionDialog = None
		self.secilen = -1
		self.id = -1
		self.m_pGrid = grid.PythonGrid(5, 4)							  

		self.searchResultItems = []

		self.itemDataList = []
		self.slotvnum = []
		
		self.timerdone = 0
		self.timer = 0
		self.isload = 0
		
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		
		self.LoadWindow()
		



	def __del__(self):
		self.Destroy()
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/sales.py")
		except:
			import exception
			exception.Abort("SalesClass.LoadDialog.LoadScript")

		try:
			GetObject=self.GetChild
			self.board = GetObject("board")
			self.basliktext = GetObject("basliktext")
			self.reward = GetObject("reward")
			self.scroll = self.GetChild("scroll")
			self.buy = self.GetChild("buy")
			self.fiyattext = self.GetChild("fiyattext")
			self.RefreshSymbol = GetObject("RefreshSymbol")
			self.bg2 = GetObject("bg2")
			self.bg1 = GetObject("bg1")
			self.scrollstartindex = 0
			self.buy.SetEvent(ui.__mem_func__(self.BuyEvent))
			self.scroll.SetScrollEvent(ui.__mem_func__(self.OnQuestScroll))
			
			self.board.SetCloseEvent(ui.__mem_func__(self.__OnCloseButtonClick))
			# self.SetOnRunMouseWheelEvent(self.OnQuestScroll2)

		except:
			import exception
			exception.Abort("SalesClass.LoadDialog.BindObject")
			
			
		self.reward.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.reward.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		
		
	def OverOutItem(self):
		self.tooltipItem.ClearToolTip()
		self.tooltipItem.HideToolTip()
		
		
		
	def OverInItem(self, slotIndex):
		self.tooltipItem.ClearToolTip()
		if int(self.slotvnum[slotIndex]) != 0:
			metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			self.tooltipItem.AddItemData(self.slotvnum[slotIndex],metinSlot)
			self.tooltipItem.Show()
		else:
			self.tooltipItem.HideToolTip()
			
			



	def Destroy(self):
		self.ClearDictionary()
		self.searchResultItems[:] = [] 
		self.itemDataList[:] = [] 
		self.slotvnum[:] = [] 
		self.titleBar = None
		self.scroll = None
		self.RefreshSymbol = None
		self.board = None
		self.scrollstartindex = None
		self.QuestionDialog = None
		self.id = -1
		self.secilen = -1
		self.selectedItemIndex = -1
		self.timerdone = 0
		self.timer = 0
		self.isload = 0
		self.m_pGrid = None			  
		

		
		
	def OnQuestScroll2(self, len):
		if str(len) == "True":
			self.scroll.OnDown()
		else:
			self.scroll.OnUp()
		
	def OnQuestScroll(self):
		salecount = len(constInfo.SalesList)
		scrollLineCount = max(0, salecount - 8)
		startIndex = int(scrollLineCount * self.scroll.GetPos())

		if startIndex != self.scrollstartindex:
			self.scrollstartindex = startIndex
			self.RefreshInfo()

		
	def RefreshInfo(self):
		salecount = len(constInfo.SalesList)
		self.searchResultItems[:] = []
		
		if salecount > 8:
			self.scroll.Show()
		else:
			self.scroll.Hide()

		count = 8
		
		if len(constInfo.SalesList) < count:
			count = len(constInfo.SalesList)
		
		basePos = 5
		for i in range(0+self.scrollstartindex, count+self.scrollstartindex):
			resultItem = SalesClass.SearchResultItem(self, i, self.bg1)
			resultItem.SetPosition(5, basePos+((i-self.scrollstartindex)*36))
			resultItem.SetID(int(constInfo.SalesList[i]["id"]))
			resultItem.Show()
			
			self.searchResultItems.append(resultItem)
		
		self.Children.append(self.searchResultItems)




	def Open(self):	
		self.selectedItemIndex = -1
		if self.isload == 0:
			self.isload = 1
		else:
			net.SendChatPacket("/sales_list")
		self.timer = app.GetTime()+1
		self.timerdone = 0
		self.scroll.Hide()
		self.bg1.Hide()
		self.bg2.Hide()
		self.RefreshSymbol.Show()
		
		self.Show()
		self.SetCenterPosition()

	
	def BuyEvent(self):
		self.QuestionDialog = None
		
		QuestionDialog = uiCommon.QuestionDialog()
		QuestionDialog.SetText("Satýn almak istiyor musun?")
		QuestionDialog.SetAcceptEvent(lambda arg=True: self.Answer(arg))
		QuestionDialog.SetCancelEvent(lambda arg=False: self.Answer(arg))
		QuestionDialog.Open()
		self.QuestionDialog = QuestionDialog
		
	def Answer(self, answer):

		if not self.QuestionDialog:
			return
			
		if answer == False:
			self.QuestionDialog.Close()
			self.QuestionDialog = None
			return

		net.SendChatPacket("/sales_buy "+str(self.secilen+1))

		self.QuestionDialog.Close()
		self.QuestionDialog = None
		
		
	def OnUpdate(self):
		if self.timer <= app.GetTime() and self.timerdone == 0:
			self.RefreshInfo()
			self.RefreshSymbol.Hide()
			self.bg1.Show()
			self.bg2.Show()
			self.OnSearchResultItemSelect(0+self.scrollstartindex)
			self.timerdone = 1
		else:
			if self.secilen == -1:
				return
				
			if len(constInfo.SalesList) == 0:
				return
				
			if self.secilen > len(constInfo.SalesList):
				return
			
			sure = int(constInfo.SalesList[int(self.secilen)]["time"])
			tip = int(constInfo.SalesList[int(self.secilen)]["type"])
			
			sure = sure - app.GetGlobalTimeStamp()
				
			if (sure > 0):
				if (tip > 0):
					self.basliktext.SetText("Baþlamasýna Kalan Süre : "+localeInfo.CalcTime(sure))
				else:
					self.basliktext.SetText("Kalan Süre : "+localeInfo.CalcTime(sure))
			else:
				if tip > 0:
					sure = sure+(tip*60*60)
					if sure > 0:
						self.basliktext.SetText("Kalan Süre : "+localeInfo.CalcTime(sure))
					else:
						self.basliktext.SetText("Teklifin süresi doldu!")
				else:
					self.basliktext.SetText("Teklifin süresi doldu!")
	
	
	def FindBlank(self, itemWidth, itemHeight):
		return self.m_pGrid.FindBlank(itemWidth, itemHeight)	
		
	def OnSearchResultItemSelect(self, index):
		if self.scrollstartindex > 0:
			self.selectedItemIndex = index - self.scrollstartindex
		else:
			self.selectedItemIndex = index
		self.secilen = index
		
		map(SalesClass.SearchResultItem.UnSelect,  self.searchResultItems)
		
		if len(constInfo.SalesList) == 0:
			return
		
		listcount = len(constInfo.SalesList[index]["items"][0])
		if listcount == 0:
			return
		
		self.searchResultItems[self.selectedItemIndex].Select()
		self.fiyattext.SetText(str(constInfo.SalesList[index]["normal_price"])+ " EP yerine " +str(constInfo.SalesList[index]["sales_price"])+ " EP")
				
		for i in range(self.reward.GetSlotCount()):
			self.reward.ClearSlot(i)
			
		self.reward.RefreshSlot()
			
		self.slotvnum = [0 for i in xrange(24)]
		
		self.m_pGrid.Clear()
		
		for i in range(self.reward.GetSlotCount()):
			if i < listcount:
				count = int(constInfo.SalesList[index]["items"][0][i]["count"])
				vnum = int(constInfo.SalesList[index]["items"][0][i]["vnum"])
				item.SelectItem(vnum)
				(iWidth, iHeight) = item.GetItemSize()
				iPos = self.FindBlank(iWidth, iHeight)
				if iPos != -1:
					self.m_pGrid.Put(iPos, iWidth, iHeight)
					if count == 1:
						count = 0
					self.reward.SetItemSlot(iPos, vnum, count)
					self.slotvnum[iPos] = vnum 
			
				
		self.reward.RefreshSlot()
		
		sure = int(constInfo.SalesList[int(self.secilen)]["time"])
		tip = int(constInfo.SalesList[int(self.secilen)]["type"])
		
		suree = sure - app.GetGlobalTimeStamp()


	def Close(self):
		map(SalesClass.SearchResultItem.Hide, self.searchResultItems)
		self.Hide()


	def __OnCloseButtonClick(self):
		map(SalesClass.SearchResultItem.Hide, self.searchResultItems)
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()

