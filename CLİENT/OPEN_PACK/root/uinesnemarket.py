#-*- coding: iso-8859-1 -*-
import app
import ui ,net
import wndMgr ,localeInfo, mouseModule, uiToolTip
import player
import interfaceModule
import game
import uiChestDropInfo
import chr ,chat ,uiScriptLocale ,time, item, grp, uiCommon
import __builtin__ as selfs

LIST_ITEM_Y_INTERVAL = 22
MAX_ITEM_COUNT = 9
MAX_BUYABLE_ITEM_COUNT = 200
CASH_REFRESH_DELAY = 5
REFRESH_DELAY = 0	#sn
MIN_SEARCH_TEXT_LEN = 3

def NumberToMoney(n) :
    if n <= 0 : return "0"
    return "%s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))


class NesneMarketWindow(ui.ScriptWindow):
	isLoaded = False
	MAX_CATEGORY = 0
	class ListBox(ui.Window):
		def __init__(self, parent, mainWnd, mainVar):
			ui.Window.__init__(self)

			self.__categoryList = {}
			self.__parentWnd = parent
			self.__mainWnd = mainWnd
			self.__mainVar = mainVar
			self.__startIndex = 0

		def __del__(self):
			del self.__categoryList
			del self.__startIndex
			del self.__parentWnd
			del self.__mainWnd
			del self.__mainVar
			ui.Window.__del__(self)

		def Open(self):
			count = self.GetCount()
			# if count > self.__mainWnd.MAX_CATEGORY: count = self.__mainWnd.MAX_CATEGORY

			for i in xrange(count):
				self.__categoryList[i].Show()

		def Close(self):
			for i in xrange(len(self.__categoryList)):
				self.__categoryList[i].Hide()
		def UnselectAllList(self):
			for i in xrange(len(self.__categoryList)):
				self.__categoryList[i].checked.Hide()
		def AppendItem(self, questName, categoryIndex, categoryID):
			for i in xrange(len(self.__categoryList)):
				if self.__categoryList[i].text.GetText() == questName and self.__categoryList[i].categoryIndex == categoryIndex:
					return

			bar = ui.Bar()
			bar.SetParent(self.__parentWnd)
			bar.SetSize(273, 19)
			bar.SetColor(grp.GenerateColor(1.0, 1.0, 1.0, 0.0))
			bar.OnMouseLeftButtonDown = lambda x=len(self.__categoryList), arg=categoryIndex: self.__SelectCategory(x, arg)
			bar.Hide()

			bar.text = ui.TextLine()
			bar.text.SetParent(bar)
			bar.text.SetText(questName)
			bar.text.SetPosition(25, -5)
			bar.text.SetWindowHorizontalAlignLeft()
			bar.text.SetHorizontalAlignLeft()
			bar.text.Show()

			bar.btn = ui.Button()
			bar.btn.SetParent(bar.text)
			bar.btn.SetUpVisual("d:/ymir work/ui/itemshop/category/minikat_1.tga")
			bar.btn.SetOverVisual("d:/ymir work/ui/itemshop/category/minikat_2.tga")
			bar.btn.SetDownVisual("d:/ymir work/ui/itemshop/category/minikat_1.tga")
			bar.btn.SetPosition(-1, -2)
			bar.btn.Show()

			bar.checked = ui.ImageBox()
			bar.checked.SetParent(bar.btn)
			bar.checked.LoadImage("d:/ymir work/ui/itemshop/category/minikat_2.tga")
			bar.checked.SetPosition(0,0)
			bar.checked.Hide()

			bar.text1 = ui.TextLine()
			bar.text1.SetParent(bar.text)
			bar.text1.SetText(questName)
			bar.text1.SetPosition(15, 1)
			bar.text1.SetWindowHorizontalAlignLeft()
			bar.text1.SetHorizontalAlignLeft()
			bar.text1.Show()

			bar.categoryIndex = categoryIndex
			bar.mainIndex = categoryID

			self.__categoryList[len(self.__categoryList)] = bar

		def SetPosition(self, x, y):
			for i in xrange(len(self.__categoryList)):
				self.__categoryList[i].SetPosition(x, y + i * LIST_ITEM_Y_INTERVAL)

		def GetCount(self):
			return len(self.__categoryList)

		def GetHeight(self):
			return self.GetCount() * LIST_ITEM_Y_INTERVAL

		def SetCounterText(self, questIdx, text):
			self.__categoryList[questIdx].text.counter.SetText(text)

		def categoryIndex(self, qIdx, questIndex):
			self.__categoryList[qIdx].checked.Show()
			# chat.AppendChat(1,"<dbg> qIdx: %d , questIndex: %d" % (qIdx , questIndex))
		
		def __SelectCategory(self, qIdx, questIndex):
			if not self.__mainVar.CanSearchNow(): return
			isChecked = self.__categoryList[qIdx].checked.IsShow()
			self.__mainVar.UnselectCategories(True)
			if not isChecked:
				self.__categoryList[qIdx].checked.Show()
				self.__mainWnd.selectedCatID.append(self.__categoryList[qIdx].mainIndex)
			self.__mainVar.SearchWithName()
			# chat.AppendChat(1,"__SelectCategory > qIdx: %d, questIndex: %d | [%s]" % (qIdx , questIndex, self.__mainWnd.selectedCatID))

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = False
		self.Initialize()
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.Initialize(True)
	def Initialize(self, isClose = False):
		# self.__categorySlot = []
		self.__categorySlot = {}
		self.__categoryList = {}
		self.categoryMain = 0
		self.searchText = 0
		self.pageText = 0
		self.currPage = 0
		self.myCash = 0
#		self.myVoteCoins = 0
		self.cooldownTime = 0
		self.titleBar = 0
		self.itemIconList = []
		self.itemNameList = []
		self.itemPriceList = []
		self.itemCountList = []
		self.shopItemIDList = []
		self.shopItemVnumList = []
		self.shopItemMetinList = []
		self.shopItemAttrList = []
		self.shopItemPriceList = []
		self.shopItemCountList = []
		self.shopItemPriceTypeList = []
		self.shopBuyItemCountList = []
		self.shopItemCatID = []
		self.selectedCatID = []
		self.tooltipItem = 0
		self.interface = None
		self.wndChestDropInfo = None
		self.refreshCashDelay = 0
		self.itemBuyQuestionDialog = None
		self.delayTime = 0.0
		if not isClose:
			self.BuildWindow()
			self.SetCenterPosition()
	def Clear(self, deleteCategories = False):
		self.shopItemIDList = []
		self.shopItemVnumList = []
		self.shopItemMetinList = []
		self.shopItemAttrList = []
		self.shopItemPriceList = []
		self.shopItemCountList = []
		self.shopItemPriceTypeList = []
		self.shopBuyItemCountList = []
		self.shopItemCatID = []
		self.isLoaded = False
		# self.selectedCatID = []
		if deleteCategories:
			# self.__categorySlot = []
			self.__categorySlot = {}
			self.__categoryList = {}
		self.RefreshSlots()
		self.RePositionCategories()
	def BuildWindow(self):
		if self.isLoaded: return
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/nesne_market.py")
		except:
			import exception
			exception.Abort("NesneMarketWindow.BuildWindow.LoadObject")
		try:
			GetObject = self.GetChild
			self.categoryMain = GetObject("CategoryMain")
			self.searchText = GetObject("QueryString")
			self.pageText = GetObject("PageText")
			self.myCash = GetObject("MyCash")
			#self.myVoteCoins = GetObject("MyVC")
			self.cooldownTime = GetObject("Cooldown")
			self.titleBar = GetObject("TitleBar")
			for i in xrange(MAX_ITEM_COUNT):
				self.itemIconList.append(GetObject("ItemSlot_"+str(i)))
				self.itemNameList.append(GetObject("ItemName_"+str(i)))
				self.itemPriceList.append(GetObject("ItemPrice_"+str(i)))
				self.itemCountList.append(GetObject("ItemCount_"+str(i)))
				GetObject("ItemBuy_"+str(i)).SetEvent(ui.__mem_func__(self.ClickBuyItem), i)
				GetObject("ItemCntUp_"+str(i)).SetEvent(ui.__mem_func__(self.ClickCountUp), i)
				GetObject("ItemCntDw_"+str(i)).SetEvent(ui.__mem_func__(self.ClickCountDown), i)
			GetObject("NextButton").SetEvent(ui.__mem_func__(self.GoNextPage))
			GetObject("PrevButton").SetEvent(ui.__mem_func__(self.GoPrevPage))
			GetObject("SearchButton").SetEvent(ui.__mem_func__(self.SearchWithName))
			GetObject("BuyDRButton").SetEvent(ui.__mem_func__(self.BuyDR))
			GetObject("Itemci").SetEvent(ui.__mem_func__(self.Itemci))
			GetObject("ItemPol").SetEvent(ui.__mem_func__(self.ItemPol))
			GetObject("Discord").SetEvent(ui.__mem_func__(self.Discord))
			#GetObject("GetVOTEButton").SetEvent(ui.__mem_func__(self.GetVOTE))
			GetObject("RefreshBtn").SetEvent(ui.__mem_func__(self.RefreshMyCash))
		except:
			import exception
			exception.Abort("NesneMarketWindow.BuildWindow.BindObject")

		for i in range (0, len(self.itemIconList)):
			self.itemIconList[i].OnMouseOverIn			= lambda selfArg = self, index = i: selfArg.OverInItem(index)
			self.itemIconList[i].OnMouseOverOut 		= lambda selfArg = self, index = i: selfArg.OverOutItem()
			self.itemIconList[i].OnMouseLeftButtonDown	= lambda selfArg = self, index = i: selfArg.OnClickItem(index)
			self.itemIconList[i].OnMouseRightButtonDown	= lambda selfArg = self, index = i: selfArg.OnRightClickItem(index)

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.searchText.SetEscapeEvent(ui.__mem_func__(self.Close))
		# self.searchText.SetFocus()
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.HideToolTip()
		self.wndChestDropInfo = uiChestDropInfo.ChestDropInfoWindow()
		self.RefreshSlots()
		self.isLoaded = True
	def OnHideItemTooltip(self):
		self.tooltipItem.HideToolTip()
		self.Close()
	def Open(self):
		# chat.AppendChat(1,"isGG Value : %d | isLoaded: %d" % (ggValue, int(self.isLoaded)))
		if self.IsShow():
			self.searchText.KillFocus()
			self.__CloseCategories()
			self.Hide()
			return
		else:
			if self.delayTime > app.GetTime():
				chat.AppendChat(1, "Ýþlemler arasý %d saniye beklemelisiniz." % (REFRESH_DELAY))
				return
		self.Clear(True)
		net.SendItemShopPacket(net.PROCESS_ITEMSHOP_REFRESH, 0, long(0), "_GirisYenile_" , False)
		self.searchText.SetFocus()
		self.selectedCatID = []
		self.Show()
		self.delayTime = app.GetTime() + REFRESH_DELAY
	def RefreshSlots(self):
		xCounter = self.GetIndexIncrease()
		isClean = False
		for j in xrange(MAX_ITEM_COUNT):
			self.itemIconList[j].LoadImage("d:/ymir work/ui/null.tga")
			self.itemNameList[j].SetText("")
			self.itemPriceList[j].SetText("")
			self.itemCountList[j].SetText("")
		for i in xrange(MAX_ITEM_COUNT):
			if xCounter >= len(self.shopItemVnumList): break
			itemVnum = self.shopItemVnumList[xCounter]
			item.SelectItem(itemVnum)
			self.itemIconList[i].LoadImage(item.GetIconImageFileName())
			self.itemNameList[i].SetText(item.GetItemName())
			priceStr = " EP"
			if self.shopItemPriceTypeList[xCounter] == 1:
				priceStr = " VC"
			self.itemPriceList[i].SetTextColor(0xFF00FF00)  # Metin rengini ayarla
			self.itemPriceList[i].SetText(str(self.shopItemPriceList[xCounter] * self.shopBuyItemCountList[xCounter])+ priceStr)
			self.itemCountList[i].SetText(str(self.shopItemCountList[xCounter] * self.shopBuyItemCountList[xCounter])+" adet")
			xCounter+=1
		self.pageText.SetText(str(self.currPage))
	def AddCategory(self, categoryName, categoryIdx):
		try:
			self.__categorySlot[categoryIdx] = ui.Button()
			self.__categorySlot[categoryIdx].SetParent(self.categoryMain)
			self.__categorySlot[categoryIdx].SetSize(253, 19)
#			self.__categorySlot[categoryIdx].SetSize(253, 19)
			self.__categorySlot[categoryIdx].SetUpVisual("d:/ymir work/ui/itemshop/category/quest_category.tga")
			self.__categorySlot[categoryIdx].SetOverVisual("d:/ymir work/ui/itemshop/category/quest_category_2.tga")
			self.__categorySlot[categoryIdx].SetDownVisual("d:/ymir work/ui/itemshop/category/quest_category_3.tga")
			self.__categorySlot[categoryIdx].SetTextAlignLeft(categoryName)
			self.__categorySlot[categoryIdx].SetTextColor(0xFFFFE3AD)
			self.__categorySlot[categoryIdx].SAFE_SetEvent(self.__OnClickQuestCategory, categoryIdx)
			self.__categorySlot[categoryIdx].Show()
			self.__categorySlot[categoryIdx].openImage = ui.Button()
			self.__categorySlot[categoryIdx].openImage.AddFlag("not_pick")
			# self.__categorySlot[categoryIdx].openImage.SAFE_SetEvent(self.__OnClickQuestCategory, categoryIdx)
			self.__categorySlot[categoryIdx].openImage.Show()

			self.__categoryList[categoryIdx] = self.ListBox(self.categoryMain, self, self)
			self.__categoryList[categoryIdx].Close()
			self.MAX_CATEGORY += 1
		except:
			# chat.AppendChat(1,"Kategori eklenemedi, ID: %d" % categoryIdx)
			pass
		self.RePositionCategories()
	def __IsCategoryOpen(self, categoryIndex):
		return self.__categorySlot[categoryIndex].openImage.IsDown()
	def __GetOpenedCategories(self):
		indexes = []
		for key, val in self.__categorySlot.items():
			if self.__IsCategoryOpen(key): indexes.append(key)
		return indexes
	def __CloseCategories(self):
		try:
			iCounter = 0
			for key, val in self.__categorySlot.items():
				self.__categoryList[key].Close()
				val.openImage.SetUp()
				val.SetPosition(7, 5 + iCounter*20)
				iCounter += 1
		except:
			pass
	def UnselectCategories(self, isUncheckAllCategories = False):
		self.selectedCatID = []
		if isUncheckAllCategories:
			try:
				for key, val in self.__categorySlot.items():
					self.__categoryList[key].UnselectAllList()
					# val.UnselectAllList()
			except:
				pass
	def GetSearchQuery(self):
		categoryStr = ""
		xCounter = 0
		for i in xrange(len(self.selectedCatID)):
			if xCounter == 0: categoryStr += str(self.selectedCatID[i])
			else: categoryStr += "|" + str(self.selectedCatID[i])
			xCounter += 1
		return categoryStr
	def RePositionCategories(self):
		xPos = 0
		iCounter = 0
		for key, val in self.__categorySlot.items():
			val.SetPosition(10, (5 + iCounter * 20) + xPos)
			self.__categoryList[key].SetPosition(3, (35 + iCounter * 20) + xPos)
			if self.__IsCategoryOpen(key): 
				xPos += self.__categoryList[key].GetHeight()
			iCounter+=1
	def __OnClickQuestCategory(self, categoryIndex):
		# chat.AppendChat(1,"<dbg> __OnClickQuestCategory > categoryIndex: %d" % int(categoryIndex))
		if self.__IsCategoryOpen(categoryIndex):
			self.__categoryList[categoryIndex].Close()
			self.__categorySlot[categoryIndex].openImage.SetUp()
			# chat.AppendChat(1,"__IsCategoryOpen %d" % int(categoryIndex))
			self.RePositionCategories()
			return

		self.__categorySlot[categoryIndex].openImage.Down()
		self.__categoryList[categoryIndex].Open()

		self.RePositionCategories()
		btnCount = self.__categoryList[categoryIndex].GetCount()
		if btnCount <= 0: return
	def FillCategory(self, fCatIndex, catItemName, categoryID):
		# chat.AppendChat(1,"categoryName: %d , %s, %d" % (fCatIndex, catItemName, categoryID))
		try:
			self.__categoryList[fCatIndex].AppendItem(catItemName, fCatIndex, categoryID)
			# self.__categoryList[fCatIndex].AppendItem(self.GetCategoryNameByLang(categoryID, catItemName) , fCatIndex, categoryID)
			# chat.AppendChat(1,"categoryName: %d , %s, %d" % (fCatIndex, catItemName, categoryID))
		except:
			pass
	def AddItem(self, itemID, itemVnum, itemCount, itemPrice, categoryID, itemSockets, itemAttrs, priceType):
		iSocketList = itemSockets.split("|")
		metinSlot = []
		for i in range(0,int(len(iSocketList))):
			metinSlot.append(int(iSocketList[i]))
		iAttrList = itemAttrs.split("`")
		attrSlot = []
		for j in range(0, int(len(iAttrList))):
			attrData = iAttrList[j].split("|")
			attrSlot.append((int(attrData[0]), int(attrData[1])))
		self.shopItemMetinList.append(metinSlot)
		self.shopItemAttrList.append(attrSlot)
		self.shopItemVnumList.append(itemVnum)
		self.shopItemIDList.append(itemID)
		self.shopItemPriceList.append(itemPrice)
		self.shopItemPriceTypeList.append(priceType)
		self.shopItemCountList.append(itemCount)
		self.shopBuyItemCountList.append(1)
		self.shopItemCatID.append(categoryID)
		self.RefreshSlots()
	def GoNextPage(self):
		self.currPage += 1
		self.RefreshSlots()
	def GoPrevPage(self):
		if self.currPage > 0: self.currPage -= 1
		self.RefreshSlots()
	def CanSearchNow(self):
		if self.delayTime > app.GetTime():
			chat.AppendChat(1, "You must wait for %s" % (self.GetDelayTime()))
			return False
		return True
	def SearchWithName(self):
		if self.delayTime > app.GetTime():
			chat.AppendChat(1, "%d saniye bekle." % (REFRESH_DELAY))
			return False
		if len(self.searchText.GetText()) > 0:
			if len(self.searchText.GetText()) <= MIN_SEARCH_TEXT_LEN:
				chat.AppendChat(1, "Arama yapabilmek icin en az %d karakter uzunlugunda olmalidir." % (MIN_SEARCH_TEXT_LEN))
				return False
			#chat.AppendChat(1, "Aranacak isim girildiði için girilen [%s] ile arama yapýlýyor." % (self.searchText.GetText()))
			net.SendItemShopPacket(net.PROCESS_ITEMSHOP_REFRESH, 0, long(0), self.searchText.GetText(), False)
		else:
			#chat.AppendChat(1, "Aranacak isim girilmediði için seçilen kategorilerle [%s] arama yapýlýyor." % (self.GetSearchQuery()))
			net.SendItemShopPacket(net.PROCESS_ITEMSHOP_REFRESH, 0, long(0), self.GetSearchQuery(), True)
		self.delayTime = app.GetTime() + REFRESH_DELAY
		self.currPage = 0
		self.RefreshSlots()
		return True
	def BuyDR(self):
		import os
		o="https://umay2.com/Market/Index/"
		os.startfile(o)
	def Itemci(self):
		import os
		o="https://itemci.com/"
		os.startfile(o)
	def ItemPol(self):
		import os
		o="https://itempol.com/"
		os.startfile(o)
	def Discord(self):
		import os
		o="https://discord.gg/M2End/"
		os.startfile(o)
		# net.SendChatPacket("/in_game_mall")
	#def GetVOTE(self):
		# net.SendChatPacket("/in_game_mall")
	#	chat.AppendChat(1,"GetVOTE Button event..")
	def RefreshMyCash(self):
		if self.refreshCashDelay > app.GetTime():
			chat.AppendChat(1, "You can only refresh in every %d seconds." % (CASH_REFRESH_DELAY))
			return
		net.SendItemShopPacket(net.PROCESS_ITEMSHOP_REFRESH, 0, long(0), "_CashYenile_" , False)
		self.refreshCashDelay = app.GetTime() + CASH_REFRESH_DELAY
	def GetIndexIncrease(self):
		return self.currPage * MAX_ITEM_COUNT
	def GetTooltipVnum(self, slotIndex):
		return self.shopItemVnumList[slotIndex + self.GetIndexIncrease()]
	def GetTooltipMetinSlot(self, slotIndex):
		metinSlot = []
		for i in xrange(len(self.shopItemMetinList[slotIndex + self.GetIndexIncrease()])):
			metinSlot.append(self.shopItemMetinList[slotIndex + self.GetIndexIncrease()][i])
		
		bDiff = player.METIN_SOCKET_MAX_NUM - len(metinSlot)
		if bDiff > 0:
			for i in xrange(bDiff):
				metinSlot.append(0)
		
		item.SelectItem(self.GetTooltipVnum(slotIndex))
		for i in xrange(item.LIMIT_MAX_NUM):
			(limitType, limitValue) = item.GetLimit(i)
			if item.LIMIT_REAL_TIME == limitType:
				if metinSlot[0] != 0:
					metinSlot[0] += app.GetGlobalTimeStamp()
		return metinSlot
	def GetTooltipAttrSlot(self, slotIndex):
		return self.shopItemAttrList[slotIndex + self.GetIndexIncrease()]
	def ClickBuyItem(self, slotIdx):
		try:
			xIdx = slotIdx + self.GetIndexIncrease()
			# chat.AppendChat(1,"ClickBuyItem > %d | itemID: %d, itemVnum: %d, itemCount: %d" % (xIdx, self.shopItemIDList[xIdx], self.shopItemVnumList[xIdx], self.shopItemCountList[xIdx]))
			itemBuyQuestionDialog = uiCommon.QuestionDialog()
			item.SelectItem(self.shopItemVnumList[xIdx])
			# itemBuyQuestionDialog.SetText("%s %d adet alýnacak ?" % (item.GetItemName(), self.shopItemCountList[xIdx]))
			itemBuyQuestionDialog.SetText("%s %d adet alýnacak ?" % (item.GetItemName(), self.shopBuyItemCountList[xIdx]))
			itemBuyQuestionDialog.SetAcceptEvent(lambda arg=True: self.BuyItem(arg))
			itemBuyQuestionDialog.SetCancelEvent(lambda arg=False: self.BuyItem(arg))
			itemBuyQuestionDialog.Open()
			itemBuyQuestionDialog.slotIdx = xIdx
			self.itemBuyQuestionDialog = itemBuyQuestionDialog
		except: pass
	def ClickCountUp(self, slotIdx):
		# chat.AppendChat(1,"ClickCountUp > %d" % (slotIdx))
		try:
			if self.shopBuyItemCountList[slotIdx + self.GetIndexIncrease()] <= MAX_BUYABLE_ITEM_COUNT: 
				self.shopBuyItemCountList[slotIdx + self.GetIndexIncrease()] += 1
		except: pass
		self.RefreshSlots()
	def ClickCountDown(self, slotIdx):
		# chat.AppendChat(1,"ClickCountDown > %d" % (slotIdx))
		try:
			if self.shopBuyItemCountList[slotIdx + self.GetIndexIncrease()] > 1: 
				self.shopBuyItemCountList[slotIdx + self.GetIndexIncrease()] -= 1
		except: pass
		self.RefreshSlots()
	def RefreshCash(self, cmyCash, cmyVC):
		self.myCash.SetText("Hesaptaki EP Bakiyesi: %s EP" % (NumberToMoney(cmyCash)))
		#self.myVoteCoins.SetText("Vote Coins: %s VC" % (NumberToMoney(cmyVC)))
	def BuyItem(self, isBuy):
		if self.itemBuyQuestionDialog == None: return
		if isBuy:
			slotIdx = self.itemBuyQuestionDialog.slotIdx
			if self.shopBuyItemCountList[slotIdx] == 0 or self.shopItemIDList[slotIdx] == 0: 
				self.itemBuyQuestionDialog.Close()
				self.itemBuyQuestionDialog = None
				return
			net.SendItemShopPacket(net.PROCESS_ITEMSHOP_BUY, self.shopBuyItemCountList[slotIdx], long(self.shopItemIDList[slotIdx]), "" , False)
		self.itemBuyQuestionDialog.Close()
		self.itemBuyQuestionDialog = None
	def Close(self):
		# self.searchText.KillFocus()
		# self.Clear()
		self.Open()
	def OverInItem(self, slotIndex):
		if mouseModule.mouseController.isAttached():return
		if 0 != self.tooltipItem:
			self.tooltipItem.SetItemTooltipIS(self.GetTooltipVnum(slotIndex), self.GetTooltipMetinSlot(slotIndex), self.GetTooltipAttrSlot(slotIndex))
			# self.tooltipItem.AppendEmojiForItemShop(self.GetTooltipVnum(slotIndex))
	def OnClickItem(self, slotIndex):
		pass
	def OnRightClickItem(self, slotIndex):
		app.__BL_CHEST_DROP_INFO__ = True
		if mouseModule.mouseController.isAttached():return
		itemVnum = self.GetTooltipVnum(slotIndex)
		if app.IsPressed(app.DIK_LCONTROL) and app.IsPressed(app.DIK_LSHIFT):
			selfs.wndWiki.OpenSpecialPage(0, itemVnum, False, True)
			if not selfs.wndWiki.IsShow():
				selfs.wndWiki.Show()
				selfs.wndWiki.SetTop()
			return
		elif app.IsPressed(app.DIK_LALT) and app.IsPressed(app.DIK_LSHIFT):
			item.SelectItem(itemVnum)
			selfs.wndShopSearch.InstantSearch(item.GetItemName().strip())
			return
		elif app.IsPressed(app.DIK_LCONTROL):
			item.SelectItem(itemVnum)
			isMain = not app.IsPressed(app.DIK_LSHIFT)
			self.wndChestDropInfo.Open(itemVnum, isMain)
			return
		elif app.IsPressed(app.DIK_LSHIFT):
			wndModelPreview.ModelPreview(itemVnum)
	def OverOutItem(self):
		if 0 != self.tooltipItem:self.tooltipItem.HideToolTip()
	def GetDelayTime(self):
		return float(self.delayTime - app.GetTime())
	def OnUpdate(self):
		if self.delayTime > app.GetTime():
			if not self.cooldownTime.IsShow():
				self.cooldownTime.Show()
			self.cooldownTime.SetText("Wait for: " + str(self.GetDelayTime()))
		else:
			if self.cooldownTime.IsShow():
				self.cooldownTime.Hide()

	def BindInterfaceClass(self, interface):
		self.interface = interface