import app
import player
import net
import chr
import chat
import item
import shop
import snd
import systemSetting
import ui
import mouseModule
import uiCommon
import localeInfo
import constInfo
import wndInfo as selfs

g_isBuildingOfflineShop = False
g_offlineShopAdvertisementBoardDict = {}

def Clear():
	global g_isBuildingOfflineShop
	g_isBuildingOfflineShop = False

def IsBuildingOfflineShop():
	global g_isBuildingOfflineShop
	if g_isBuildingOfflineShop:
		return True
	else:
		return False

if app.WJ_ENABLE_TRADABLE_ICON:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE

def UpdateADBoard():
	for key in g_offlineShopAdvertisementBoardDict.keys():
		g_offlineShopAdvertisementBoardDict[key].Show()

def DeleteADBoard(vid):
	if not g_offlineShopAdvertisementBoardDict.has_key(vid):
		return

	del g_offlineShopAdvertisementBoardDict[vid]

def UpdateADTextBoard(vid, text):
	if not g_offlineShopAdvertisementBoardDict.has_key(vid):
		return
	
	g_offlineShopAdvertisementBoardDict[vid].UpdateADSign(text)


def GetADBoardName(vid):
	if not g_offlineShopAdvertisementBoardDict.has_key(vid):
		return ""
	
	return g_offlineShopAdvertisementBoardDict[vid].GetADSign() 

class OfflineShopAdvertisementBoard(ui.ShopDecoration):
	SHOP_VISIT_COLOR=0xFFC49157
	def __init__(self, borderType):
		ui.ShopDecoration.__init__(self, "UI_BOTTOM")
		self.MakeBoard(borderType)
		self.shopAdvertismentBoardSeen = []
		self.vid = None
		self.__MakeTextLine()

	def __del__(self):
		ui.ShopDecoration.__del__(self)

	def __MakeTextLine(self):
		self.textLine = ui.TextLine()
		# if app.WJ_MULTI_TEXTLINE:
			# self.textLine.DisableEnterToken()
		self.textLine.SetParent(self)
		self.textLine.SetWindowHorizontalAlignCenter()
		self.textLine.SetWindowVerticalAlignCenter()
		self.textLine.SetHorizontalAlignCenter()
		self.textLine.SetVerticalAlignCenter()
		self.textLine.Show()

	def Open(self, vid, text):
		self.vid = vid

		self.textLine.SetText(text)
		if vid in self.shopAdvertismentBoardSeen:
			self.textLine.SetPackedFontColor(self.SHOP_VISIT_COLOR)
		self.textLine.UpdateRect()
		self.SetSize(self.textLine.GetTextSize()[0])
		self.Show()

		g_offlineShopAdvertisementBoardDict[vid] = self
	
	def UpdateADSign(self, text):
		if not self.vid:
			return
		
		self.textLine.SetText(text)
		self.textLine.UpdateRect()
		self.SetSize(self.textLine.GetTextSize()[0])
	
	def GetADSign(self):
		if not self.vid:
			return ""
		
		return self.textLine.GetText()
	
	def OnMouseLeftButtonUp(self):
		if not self.vid:
			return

		net.SendOnClickPacket(self.vid)
		if self.vid != player.GetMainCharacterIndex():
			self.textLine.SetPackedFontColor(self.SHOP_VISIT_COLOR)
			self.shopAdvertismentBoardSeen.append(self.vid)

		return True

	def OnUpdate(self):
		if not self.vid:
			return

		if (chr.GetInstanceType(self.vid) != chr.INSTANCE_TYPE_NPC or not\
			(chr.GetVirtualNumber(self.vid) == 30000 or (chr.GetVirtualNumber(self.vid) >= 30002 and chr.GetVirtualNumber(self.vid) <= 30008))):
			self.Hide()
			DeleteADBoard(self.vid)

		if systemSetting.IsShowSalesText():
			self.Show()
			x, y = chr.GetProjectPosition(self.vid, 220)
			self.SetPosition(x - self.GetWidth()/2, y - self.GetHeight()/2)
		else:
			for key in g_offlineShopAdvertisementBoardDict.keys():
				if player.GetMainCharacterIndex() == key: #상점풍선을 안보이게 감추는 경우에도, 플레이어 자신의 상점 풍선은 보이도록 함. by 김준호
					g_offlineShopAdvertisementBoardDict[key].Show()
					x, y = chr.GetProjectPosition(player.GetMainCharacterIndex(), 220)
					g_offlineShopAdvertisementBoardDict[key].SetPosition(x - self.GetWidth()/2, y - self.GetHeight()/2)
				else:
					g_offlineShopAdvertisementBoardDict[key].Hide()

class OfflineShopBuilder(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__LoadWindow()
		self.itemStock = {}
		self.priceInputBoard = None
		self.title = ""
		self.time = 0
		self.style = 0
		self.borderStyle = 0

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/OfflineShopBuilder.py")
		except:
			import exception
			exception.Abort("OfflineShopBuilderWindow.LoadWindow.LoadObject")

		try:
			GetObject = self.GetChild
			self.nameLine = GetObject("NameLine")
			# if app.WJ_MULTI_TEXTLINE:
				# self.nameLine.DisableEnterToken()
			self.itemSlot = GetObject("ItemSlot")
			self.btnOk = GetObject("OkButton")
			self.btnClose = GetObject("CloseButton")
			self.titleBar = GetObject("TitleBar")

		except:
			import exception
			exception.Abort("OfflineShopBuilderWindow.LoadWindow.BindObject")

		self.btnOk.SetEvent(ui.__mem_func__(self.OnOk))
		self.btnClose.SetEvent(ui.__mem_func__(self.OnClose))
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.OnClose))

		self.itemSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectEmptySlot))
		self.itemSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.OnSelectItemSlot))
		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OnOverInItem))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))

	def Destroy(self):
		self.ClearDictionary()

		self.nameLine = None
		self.itemSlot = None
		self.btnOk = None
		self.btnClose = None
		self.titleBar = None
		self.priceInputBoard = None

	def Open(self, title, style, borderStyle):
		self.title = title
		self.time = 0
		self.style = style
		self.borderStyle = borderStyle

		if len(title) > 25:
			title = title[:22] + "..."

		self.itemStock = {}
		shop.ClearOfflineShopStock()
		self.nameLine.SetText(title)
		self.SetCenterPosition()
		self.Refresh()
		self.SetTop()
		self.Show()

		global g_isBuildingOfflineShop
		g_isBuildingOfflineShop = True

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.ItemListIdx = []

		if app.ENABLE_CHEQUE_SYSTEM:
			self.IsCashItem = False

	def Close(self):
		global g_isBuildingOfflineShop
		g_isBuildingOfflineShop = False

		if self.priceInputBoard:
			self.priceInputBoard.Close()

		self.title = ""
		self.time = 0
		self.style = 0
		self.borderStyle = 0
		self.itemStock = {}
		shop.ClearOfflineShopStock()
		self.priceInputBoard = None;
		self.Hide()

		if app.WJ_ENABLE_TRADABLE_ICON:
			if selfs.wndInterface:
				selfs.wndInterface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				selfs.wndInterface.RefreshMarkInventoryBag()

			self.ItemListIdx = None

		if app.ENABLE_CHEQUE_SYSTEM:
			self.IsCashItem = False

	def Refresh(self):
		getitemVNum = player.GetItemIndex
		getItemCount = player.GetItemCount
		setitemVNum = self.itemSlot.SetItemSlot
		delItem = self.itemSlot.ClearSlot

		for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT):
			if not self.itemStock.has_key(i):
				delItem(i)
				continue

			pos = self.itemStock[i]

			itemCount = getItemCount(*pos)
			if itemCount <= 1:
				itemCount = 0
			setitemVNum(i, getitemVNum(*pos), itemCount)

			# if app.ENABLE_CHANGE_LOOK_SYSTEM:
				# itemTransmutedVnum = player.GetItemTransmutation(*pos)
				# if itemTransmutedVnum:
					# self.itemSlot.DisableCoverButton(i)
				# else:
					# self.itemSlot.EnableCoverButton(i)

		self.itemSlot.RefreshSlot()

	def OnSelectEmptySlot(self, selectedSlotPos):
		if self.priceInputBoard:
			return

		isAttached = mouseModule.mouseController.isAttached()
		if isAttached:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			mouseModule.mouseController.DeattachObject()

			if player.SLOT_TYPE_INVENTORY != attachedSlotType and player.SLOT_TYPE_DRAGON_SOUL_INVENTORY != attachedSlotType and\
				(player.SLOT_TYPE_SKILL_BOOK_INVENTORY != attachedSlotType and\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY != attachedSlotType and\
				player.SLOT_TYPE_STONE_INVENTORY != attachedSlotType and\
				player.SLOT_TYPE_GIFT_BOX_INVENTORY != attachedSlotType and\
				player.SLOT_TYPE_CHANGERS_INVENTORY != attachedSlotType and app.ENABLE_SPECIAL_INVENTORY_SYSTEM):

				return

			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)

			itemVNum = player.GetItemIndex(attachedInvenType, attachedSlotPos)
			itemCount = player.GetItemCount(attachedInvenType, attachedSlotPos)
			item.SelectItem(itemVNum)

			# if app.ENABLE_SOUL_BIND_SYSTEM:
				# if player.GetItemSealDate(attachedInvenType, attachedSlotPos) == -1 or player.GetItemSealDate(attachedInvenType, attachedSlotPos) > 0: #봉인아이템 걸러냄.
					# return True

			if item.IsAntiFlag(item.ANTIFLAG_GIVE) or item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PRIVATE_SHOP_CANNOT_SELL_ITEM)
				return

			priceInputBoard = uiCommon.MoneyInputDialog()
			priceInputBoard.SetTitle(localeInfo.PRIVATE_SHOP_INPUT_PRICE_DIALOG_TITLE)
			priceInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPrice))
			priceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPrice))
			priceInputBoard.Open()

			itemPrice = constInfo.GetOfflineShopItemPrice(itemVNum, itemCount)
			if itemPrice > 0:
				priceInputBoard.SetValue(itemPrice)

			self.priceInputBoard = priceInputBoard
			self.priceInputBoard.itemVNum = itemVNum
			self.priceInputBoard.itemCount = itemCount
			self.priceInputBoard.sourceWindowType = attachedInvenType
			self.priceInputBoard.sourceSlotPos = attachedSlotPos
			self.priceInputBoard.targetSlotPos = selectedSlotPos

			if app.WJ_ENABLE_TRADABLE_ICON:
				self.ItemListIdx.append(attachedSlotPos)
				self.priceInputBoard.Socket = self.GetItemDataSocketValue(attachedInvenType, attachedSlotPos)

	def OnSelectItemSlot(self, selectedSlotPos):
		isAttached = mouseModule.mouseController.isAttached()
		if isAttached:
			snd.PlaySound("sound/ui/loginfail.wav")
			mouseModule.mouseController.DeattachObject()
		else:
			if not selectedSlotPos in self.itemStock:
				return

			invenType, invenPos = self.itemStock[selectedSlotPos]
			shop.DelOfflineShopItemStock(invenType, invenPos)
			snd.PlaySound("sound/ui/drop.wav")

			del self.itemStock[selectedSlotPos]

			if app.WJ_ENABLE_TRADABLE_ICON:
				for i in xrange(len(self.ItemListIdx)):
					if self.ItemListIdx[i] == invenPos:
						del self.ItemListIdx[i]
						break

			self.Refresh()

	def AcceptInputPrice(self):
		if not self.priceInputBoard:
			return True

		price = long(self.priceInputBoard.GetText())
		
		if not price:
			return

		attachedInvenType = self.priceInputBoard.sourceWindowType
		sourceSlotPos = self.priceInputBoard.sourceSlotPos
		targetSlotPos = self.priceInputBoard.targetSlotPos

		for offlinePos, (itemWindowType, itemSlotIndex) in self.itemStock.items():
			if itemWindowType == attachedInvenType and itemSlotIndex == sourceSlotPos:
				shop.DelOfflineShopItemStock(itemWindowType, itemSlotIndex)
				del self.itemStock[offlinePos]


		constInfo.SetOfflineShopItemPrice(self.priceInputBoard.itemVNum, price, self.priceInputBoard.itemCount)
		shop.AddOfflineShopItemStock(attachedInvenType, sourceSlotPos, targetSlotPos, price)

		self.itemStock[targetSlotPos] = (attachedInvenType, sourceSlotPos)
		snd.PlaySound("sound/ui/drop.wav")

		self.Refresh()

		#####

		self.priceInputBoard = None
		return True

	def CancelInputPrice(self):
		if app.WJ_ENABLE_TRADABLE_ICON:
			pos = self.priceInputBoard.sourceSlotPos
			for i in xrange(len(self.ItemListIdx)):
				if self.ItemListIdx[i] == pos:
					del self.ItemListIdx[i]
					break

				if selfs.wndInventory:
					page = selfs.wndInventory.GetInventoryPageIndex() # range 0 ~ 1

					if (page * INVENTORY_PAGE_SIZE) <= pos < ((page + 1) * INVENTORY_PAGE_SIZE): # range 0 ~ 44, 45 ~ 89
						idx = pos - (page * INVENTORY_PAGE_SIZE)
						selfs.wndInventory.wndItem.SetCanMouseEventSlot(idx)

		self.priceInputBoard = None
		return True

	def OnOk(self):
		if not self.title:
			return

		if self.style < 0:
			return
		
		if self.borderStyle < 0:
			return

		if 0 == len(self.itemStock):
			return

		shop.BuildOfflineShop(self.title, self.style, self.borderStyle)
		self.Close()

	def OnClose(self):
		self.Close()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnOverInItem(self, slotIndex):
		if selfs.tooltipItem:
			if self.itemStock.has_key(slotIndex):
				selfs.tooltipItem.SetOfflineShopBuilderItem(*self.itemStock[slotIndex] + (slotIndex,))

	def OnOverOutItem(self):
		if selfs.tooltipItem:
			selfs.tooltipItem.HideToolTip()

	def IsBuilding(self):
		global g_isBuildingOfflineShop
		if g_isBuildingOfflineShop:
			return True
		else:
			return False

	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantTradableItem(self, slotIndex):
			itemIndex = player.GetItemIndex(slotIndex)

			if itemIndex:
				# if app.ENABLE_SOUL_BIND_SYSTEM:
					# if player.GetItemSealDate(player.INVENTORY, slotIndex) == -1 or player.GetItemSealDate(player.INVENTORY, slotIndex) > 0: #봉인아이템 걸러냄.
						# return True

				if itemIndex == 50200:
					return True

				if player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_GIVE) or player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_MYSHOP):
					return True

			return False

		def OnTop(self):
			if not selfs.wndInterface:
				return

			selfs.wndInterface.SetOnTopWindow(player.ON_TOP_WND_OFFLINE_SHOP)
			selfs.wndInterface.RefreshMarkInventoryBag()

		def OnUpdate(self):
			if not selfs.wndInventory:
				return

			page = selfs.wndInventory.GetInventoryPageIndex() # range 0 ~ 1

			for i in self.ItemListIdx:
				if (page * INVENTORY_PAGE_SIZE) <= i < ((page + 1) * INVENTORY_PAGE_SIZE): # range 0 ~ 44, 45 ~ 89
					lock_idx = i - (page * INVENTORY_PAGE_SIZE) 
					selfs.wndInventory.wndItem.SetCantMouseEventSlot(lock_idx)

		def GetItemDataSocketValue(self, winType, SlotPos):
			for i in xrange(item.LIMIT_MAX_NUM):

				(limitType, limitValue) = item.GetLimit(i)
				if limitType == item.LIMIT_REAL_TIME or limitType == item.LIMIT_REAL_TIME_START_FIRST_USE or limitType == item.LIMIT_TIMER_BASED_ON_WEAR:
					return 0

			return player.GetItemMetinSocket(winType, SlotPos, 0)

		def RefreshLockedSlot(self):
			pass

	if app.ENABLE_CHEQUE_SYSTEM:
		def SetIsCashItem(self, bCashItem):
			self.IsCashItem = bCashItem