import wndInfo as selfs
import ui
import player
import mouseModule
import net
import app
import snd
import item
import chat
import uiRefine
import uiAttachMetin
import uiPickMoney
import uiCommon
import uiPrivateShopBuilder
import uiOfflineShopBuilder
import uiOfflineShop
import localeInfo
import constInfo
import ime
import wndMgr
import exchange
import safebox
import systemSetting

import uiScriptLocale
import player
import grp

ITEM_FLAG_APPLICABLE = 1 << 14

class InventoryWindow(ui.ScriptWindow):
	USE_TYPE_TUPLE = ("USE_CLEAN_SOCKET", "USE_CHANGE_ATTRIBUTE", "USE_ADD_ATTRIBUTE", "USE_ADD_ATTRIBUTE2", "USE_ADD_ACCESSORY_SOCKET", "USE_PUT_INTO_ACCESSORY_SOCKET", "USE_PUT_INTO_BELT_SOCKET", "USE_PUT_INTO_RING_SOCKET", "USE_CHANGE_COSTUME_ATTR", "USE_RESET_COSTUME_ATTR", "USE_RESET_LOOK_VNUM",)
	questionDialog = None
	dlgPickMoney = None
	isLoaded = 0
	evnTur   = 0 #0:BK, 1:Tas, 2:Basma
	evnSayfa = 0
	sellingSlotNumber = -1
	if app.WJ_ENABLE_TRADABLE_ICON:
		bindWnds = []

	bolunenPos = -1
	bolunecekSayi = 0
	baslangicPos = -1
	islemYapiliyor = False
	sonIslemMs = 0
	tasinanSayi = 0
	tasinacakSayi = 0
	tasinacakWindow = 0
	tasinanWindow = 0
	islemBitisSuresi = 0

	isChestOpening = False
	openedChestCount = 0
	if app.ENABLE_ITEM_DELETE_SYSTEM:
		wndItemDelete = None

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()
	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	if app.WJ_ENABLE_TRADABLE_ICON:
		def BindWindow(self, wnd):
			self.bindWnds.append(wnd)

	def __LoadWindow(self):
		if self.isLoaded == 1: return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/exinventorywindow.py")
		except:
			import exception
			exception.Abort("bkdepo.LoadWindow.LoadObject")

		try:
			board = self.GetChild("board")
			self.board = board
			wndItem = self.GetChild("ItemSlot")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.btnDerleTopla = self.GetChild("btnDerleTopla")

			self.depoTab = []
			self.depoTab.append(self.GetChild("Inventory_Tab_1"))
			self.depoTab.append(self.GetChild("Inventory_Tab_2"))
			self.depoTab.append(self.GetChild("Inventory_Tab_3"))
			self.depoTab.append(self.GetChild("Inventory_Tab_4"))
			self.depoTab.append(self.GetChild("Inventory_Tab_5"))
			
			self.inventoryTab = []
			self.inventoryTab.append(self.GetChild("Inventory_Tab_01"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_02"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_03"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_04"))
		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.BindObject")

		## Item
		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.wndItem = wndItem

		## AttachMetinDialog
		self.attachMetinDialog = uiAttachMetin.AttachMetinDialog(self)
		self.attachMetinDialog.Hide()

		dlgPickMoney = uiPickMoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.Hide()
		self.dlgPickMoney = dlgPickMoney
		
		for i in xrange(4): self.inventoryTab[i].SetEvent(lambda arg=i: self.SetInventoryPage(-1, arg))
		self.inventoryTab[0].Down()

		for i in xrange(5): self.depoTab[i].SetEvent(lambda arg=i: self.SetInventoryPage(arg, 0))
		self.depoTab[0].Down()

		self.btnDerleTopla.SetEvent(ui.__mem_func__(self.btnStackFunc))

		self.checkBox = ui.CheckBox()
		self.checkBox.SetParent(self)
		self.checkBox.SetPosition(15,388)
		self.checkBox.SetEvent(ui.__mem_func__(self.autoOpenClick), "ON_CHECK", True)
		self.checkBox.SetEvent(ui.__mem_func__(self.autoOpenClick), "ON_UNCKECK", False)
		self.checkBox.SetTextInfo(localeInfo.EXINVENTORY_CHECKBOX)
		self.checkBox.Show()
		self.autoOpenCheck()

		self.listUnusableSlot = []
		## Refresh
		self.SetInventoryPage(0,0)
		self.RefreshBagSlotWindow()

	def Destroy(self):
		self.Close()
		self.ClearDictionary()
		self.attachMetinDialog.Destroy()
		self.attachMetinDialog = 0
		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0
		self.wndItem = 0
		if app.WJ_ENABLE_TRADABLE_ICON: self.bindWnds = []
		self.inventoryTab = []
	def Hide(self):
		if None != selfs.tooltipItem: selfs.tooltipItem.HideToolTip()
		if self.dlgPickMoney: self.dlgPickMoney.Close()
		wndMgr.Hide(self.hWnd)
	def Close(self):
		self.Hide()

	def btnStackFunc(self):
		net.SendChatPacket("/exinven_stack")

	def RefreshSingle(self, itemPos):
		if self.wndItem == None: return
		localPos = self.__InventoryGlobalSlotPosToLocalSlotPos(itemPos)
		self.wndItem.ForceRefresh(localPos)

	def SetInventoryPage(self, evnTur, evnSayfa=0):
		if int(evnTur)>-1: self.evnTur = evnTur
		self.evnSayfa = evnSayfa
		for i in range(0,len(self.depoTab)): self.depoTab[i].SetUp()
		for i in range(0,len(self.inventoryTab)): self.inventoryTab[i].SetUp()
		self.depoTab[self.evnTur].Down()
		self.inventoryTab[self.evnSayfa].Down()
		if self.evnTur==0: self.GetChild("TitleName").SetText(localeInfo.EXINVENTORY_TYPE_1)
		if self.evnTur==1: self.GetChild("TitleName").SetText(localeInfo.EXINVENTORY_TYPE_2)
		if self.evnTur==2: self.GetChild("TitleName").SetText(localeInfo.EXINVENTORY_TYPE_3)
		if self.evnTur==3: self.GetChild("TitleName").SetText(localeInfo.EXINVENTORY_TYPE_4)
		if self.evnTur==4: self.GetChild("TitleName").SetText(localeInfo.EXINVENTORY_TYPE_5)
		self.RefreshBagSlotWindow()

	def GetSlotKonum(self, slotIndex):
		evnPos = player.EXINVENTORY_POS_START + (self.evnTur*player.INVENTORY_PAGE_SIZE*player.INVENTORY_PAGE_COUNT) + (self.evnSayfa*player.INVENTORY_PAGE_SIZE) + int(slotIndex)
		return evnPos

	def __InventoryGlobalSlotPosToLocalSlotPos(self, glob):
		return glob - (self.evnSayfa*player.INVENTORY_PAGE_SIZE) - (self.evnTur*player.INVENTORY_PAGE_SIZE*player.INVENTORY_PAGE_COUNT) - player.EXINVENTORY_POS_START


	if app.WJ_ENABLE_TRADABLE_ICON:
		def GetAktifSayfa(self): return self.evnSayfa
		def GetAktifDepoTuru(self): return self.evnTur

		def RefreshMarkSlots(self, localIndex=None):
			if not selfs.wndInterface: return
			onTopWnd = selfs.wndInterface.GetOnTopWindow()
			if localIndex:
				slotNumber = self.GetSlotKonum(localIndex)
				if onTopWnd == player.ON_TOP_WND_NONE:
					self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				return

			for i in xrange(player.INVENTORY_PAGE_SIZE):
				slotNumber = self.GetSlotKonum(i)

				if onTopWnd == player.ON_TOP_WND_NONE:
					self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)



	def RefreshBagSlotWindow(self):
		if self.wndItem == None: return
		getItemVNum = player.GetItemIndex
		getItemCount= player.GetItemCount

		for i in xrange(player.INVENTORY_PAGE_SIZE):
			slotNumber = int(self.GetSlotKonum(i))
			itemCount = getItemCount(slotNumber)
			if 0 == itemCount: self.wndItem.ClearSlot(i); continue
			elif 1 == itemCount: itemCount = 0
			itemVnum = getItemVNum(slotNumber)
			self.wndItem.SetItemSlot(i, int(itemVnum), int(itemCount))
			if app.WJ_ENABLE_TRADABLE_ICON: self.RefreshMarkSlots(i)

		self.wndItem.RefreshSlot()
		if app.WJ_ENABLE_TRADABLE_ICON: map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)

	def RefreshSingle(self, itemPos):
		if self.wndItem == None: return
		localPos = self.__InventoryGlobalSlotPosToLocalSlotPos(itemPos)
		self.wndItem.ForceRefresh(localPos)

	def SellItem(self):
		if self.sellingSlotitemIndex == player.GetItemIndex(self.sellingSlotNumber):
			if self.sellingSlotitemCount == player.GetItemCount(self.sellingSlotNumber): net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, player.INVENTORY); snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def SelectEmptySlot(self, selectedSlotPos):
		selectedSlotPos = self.GetSlotKonum(selectedSlotPos)

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()
			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				iVnum = player.GetItemIndex(attachedSlotPos)
				if self.TasimaIzni(iVnum)==False: return
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount, attachedSlotType, player.SLOT_TYPE_INVENTORY)
				if item.IsRefineScroll(attachedItemIndex): self.wndItem.SetUseMode(False)
			elif player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
				mouseModule.mouseController.RunCallBack("INVENTORY")
			elif player.SLOT_TYPE_OFFLINE_SHOP == attachedSlotType:
				iVnum = shop.GetOfflineShopItemID(attachedSlotPos)
				if self.TasimaIzni(iVnum)==False: return
				mouseModule.mouseController.RunCallBack("INVENTORY")
			elif player.SLOT_TYPE_SHOP == attachedSlotType:
				iVnum = shop.GetItemID(attachedSlotPos)
				if self.TasimaIzni(iVnum)==False: return
				net.SendShopBuyPacket(attachedSlotPos)
			elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:
				iVnum = safebox.GetItemID(attachedSlotPos)
				if self.TasimaIzni(iVnum)==False: return
				net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)
			elif player.SLOT_TYPE_MALL == attachedSlotType:
				iVnum = safebox.GetMallItemID(attachedSlotPos)
				if self.TasimaIzni(iVnum)==False: return
				net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotPos)
			mouseModule.mouseController.DeattachObject()

	def TasimaIzni(self, iVnum):
		item.SelectItem(iVnum)
		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()
		if self.evnTur==0 and itemType!=item.ITEM_TYPE_SKILLBOOK and (iVnum<55003 or iVnum>55007) and (iVnum<55010 or iVnum>55026) and (iVnum<50301 or iVnum>50306) and (iVnum<50311 or iVnum>50320) and (iVnum<55034 or iVnum>55040):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXINVENTORY_ONLYSB)
			mouseModule.mouseController.DeattachObject()
			return False
		if self.evnTur==1 and itemType!=item.ITEM_TYPE_METIN:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXINVENTORY_ONLYST)
			mouseModule.mouseController.DeattachObject()
			return False
		if self.evnTur==2 and (itemType!=item.ITEM_TYPE_MATERIAL) and iVnum!=27987 and iVnum!=27990 and iVnum!=27992 and iVnum!=27993 and iVnum!=27994 and iVnum!=27799:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXINVENTORY_ONLYUP)
			mouseModule.mouseController.DeattachObject()
			return False
		if self.evnTur==3 and (itemType!=item.ITEM_TYPE_GIFTBOX):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXINVENTORY_ONLYGI)
			mouseModule.mouseController.DeattachObject()
			return False
		if self.evnTur==4 and ((iVnum<50721 or iVnum>50728) and (iVnum<50801 or iVnum>50804) and (iVnum<50814 or iVnum>50820)):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXINVENTORY_ONLYFL)
			mouseModule.mouseController.DeattachObject()
			return False
		return True

	def SelectItemSlot(self, itemSlotIndex):
		# chat.AppendChat(chat.CHAT_TYPE_INFO, "itemSlotIndex: %d" % (itemSlotIndex))
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		itemSlotIndex = self.GetSlotKonum(itemSlotIndex)
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemVID = mouseModule.mouseController.GetAttachedItemIndex()
			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex)
			mouseModule.mouseController.DeattachObject()
		else:
			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				self.__SellItem(itemSlotIndex)
			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)
			elif app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(itemSlotIndex)
				ime.PasteString(link)
			elif app.IsPressed(app.DIK_LSHIFT):
				itemCount = player.GetItemCount(itemSlotIndex)
				if itemCount > 1:
					self.dlgPickMoney.SetTitleName(localeInfo.PICK_ITEM_TITLE)
					self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
					self.dlgPickMoney.Open(itemCount,0)
					# self.dlgPickMoney.SetTopluAyirma(True)
					self.dlgPickMoney.itemGlobalSlotIndex = itemSlotIndex
			elif app.IsPressed(app.DIK_LCONTROL):
				itemIndex = player.GetItemIndex(itemSlotIndex)
				#/*itemtasi*/
				# player.ItemTasiClick(player.INVENTORY, itemSlotIndex)
				# chat.AppendChat(chat.CHAT_TYPE_INFO, "python tiklandi SLOT_TYPE_INVENTORY: " + str(player.SLOT_TYPE_INVENTORY) + ", itemSlotIndex:" +str(itemSlotIndex))
				return
			else:
				selectedItemVNum = player.GetItemIndex(itemSlotIndex)
				itemCount = player.GetItemCount(itemSlotIndex)
				mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
				
				if self.__IsUsableItemToItem(selectedItemVNum, itemSlotIndex):
					self.wndItem.SetUseMode(TRUE)
				else:
					self.wndItem.SetUseMode(FALSE)

				snd.PlaySound("sound/ui/pick.wav")

	def __DropSrcItemToDestItemInInventory(self, srcItemVID, srcItemSlotPos, dstItemSlotPos):
		if srcItemSlotPos == dstItemSlotPos: return

		# if app.ENABLE_SOULBIND_SYSTEM and (item.IsSealScroll(srcItemVID) or item.IsUnSealScroll(srcItemVID)):
			# self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		if srcItemVID == player.GetItemIndex(dstItemSlotPos):
			self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)
			return
		elif item.IsRefineScroll(srcItemVID):
			self.RefineItem(srcItemSlotPos, dstItemSlotPos)
			self.wndItem.SetUseMode(FALSE)
		elif item.IsMetin(srcItemVID):
			self.AttachMetinToItem(srcItemSlotPos, dstItemSlotPos)
		elif item.IsDetachScroll(srcItemVID):
			self.DetachMetinFromItem(srcItemSlotPos, dstItemSlotPos)
		elif item.IsKey(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)			
		elif (player.GetItemFlags(srcItemSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		elif item.GetUseType(srcItemVID) in self.USE_TYPE_TUPLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		else:
			self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)

	def __SellItem(self, itemSlotPos):
		self.sellingSlotNumber = itemSlotPos
		itemIndex = player.GetItemIndex(itemSlotPos)
		itemCount = player.GetItemCount(itemSlotPos)
		
		self.sellingSlotitemIndex = itemIndex
		self.sellingSlotitemCount = itemCount

		item.SelectItem(itemIndex)
		if item.IsAntiFlag(item.ANTIFLAG_SELL):
			popup = uiCommon.PopupDialog()
			popup.SetText(localeInfo.SHOP_CANNOT_SELL_ITEM)
			popup.SetAcceptEvent(self.__OnClosePopupDialog)
			popup.Open()
			self.popup = popup
			return

		itemPrice = item.GetISellItemPrice()

		if item.Is1GoldItem():
			itemPrice = itemCount / itemPrice
		else:
			itemPrice = itemPrice * itemCount

		item.GetItemName(itemIndex)
		itemName = item.GetItemName()

		self.questionDialog = uiCommon.QuestionDialog()
		self.questionDialog.SetText(localeInfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.count = itemCount
	
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return
		
		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def __OnClosePopupDialog(self):
		self.pop = None

	def RefineItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if player.REFINE_OK != player.CanRefine(scrollIndex, targetSlotPos):
			return

		###########################################################
		self.__SendUseItemToItemPacket(scrollSlotPos, targetSlotPos)
		#net.SendItemUseToItemPacket(scrollSlotPos, targetSlotPos)
		return
		###########################################################

		###########################################################
		#net.SendRequestRefineInfoPacket(targetSlotPos)
		#return
		###########################################################

		result = player.CanRefine(scrollIndex, targetSlotPos)

		if player.REFINE_ALREADY_MAX_SOCKET_COUNT == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_MORE_SOCKET)

		elif player.REFINE_NEED_MORE_GOOD_SCROLL == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NEED_BETTER_SCROLL)

		elif player.REFINE_CANT_MAKE_SOCKET_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_SOCKET_DISABLE_ITEM)

		elif player.REFINE_NOT_NEXT_GRADE_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_UPGRADE_DISABLE_ITEM)

		elif player.REFINE_CANT_REFINE_METIN_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.REFINE_OK != result:
			return

		self.refineDialog.Open(scrollSlotPos, targetSlotPos)

	def SetExchangeWindow(self, wndExchange):
			self.wndExchange = wndExchange

	def DetachMetinFromItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)
		if not player.CanDetach(scrollIndex, targetSlotPos):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			return

		self.questionDialog = uiCommon.QuestionDialog()
		self.questionDialog.SetText(localeInfo.REFINE_DO_YOU_SEPARATE_METIN)
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnDetachMetinFromItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.sourcePos = scrollSlotPos
		self.questionDialog.targetPos = targetSlotPos

	def AttachMetinToItem(self, metinSlotPos, targetSlotPos):
		metinIndex = player.GetItemIndex(metinSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		item.SelectItem(metinIndex)
		itemName = item.GetItemName()

		result = player.CanAttachMetin(metinIndex, targetSlotPos)

		if player.ATTACH_METIN_NOT_MATCHABLE_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_CAN_NOT_ATTACH(itemName))
		if player.ATTACH_METIN_NO_MATCHABLE_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_SOCKET(itemName))
		elif player.ATTACH_METIN_NOT_EXIST_GOLD_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_GOLD_SOCKET(itemName))
		elif player.ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.ATTACH_METIN_OK != result:
			return

		self.attachMetinDialog.Open(metinSlotPos, targetSlotPos)

	def OnPickItem(self, count):
		itemSlotIndex = self.dlgPickMoney.itemGlobalSlotIndex
		selectedItemVNum = player.GetItemIndex(itemSlotIndex)
		mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count)
		# mouseModule.mouseController.AttachObjectTopluAyrim(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count, self.dlgPickMoney.TopluAyirmaMi(), player.GetItemMetinSocket(player.SLOT_TYPE_INVENTORY, itemSlotIndex, 0))

	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if None != selfs.tooltipItem:
			selfs.tooltipItem.HideToolTip()

	def OverInItem(self, overSlotPos):
		overSlotPos = self.GetSlotKonum(overSlotPos)
		self.wndItem.SetUsableItem(False)

		if mouseModule.mouseController.isAttached():
			attachedItemType = mouseModule.mouseController.GetAttachedType()
			if player.SLOT_TYPE_INVENTORY == attachedItemType:
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()
				if self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPos)==True:
					self.wndItem.SetUsableItem(True)
					self.ShowToolTip(overSlotPos)
					return
		
		self.ShowToolTip(overSlotPos)
		if selfs.wndInterface.GetOnTopWindow() == player.ON_TOP_WND_SHOP:
			if not player.IsAntiFlagBySlot(overSlotPos, item.ANTIFLAG_SELL):
				selfs.tooltipItem.AppendExtraInfo("|Xchatemoji/key_ctrl|x + |Xchatemoji/key_x|x + |Xchatemoji/key_rclick|x: Hýzlý sat")

	def __IsUsableItemToItem(self, srcItemVNum, srcSlotPos):
		if srcItemVNum >= 55701 and srcItemVNum <= 55710:
			return True
		
		if srcItemVNum >= 50160 and srcItemVNum <= 50179:
			return True

		if srcItemVNum == 55001:
			return True
		
		if item.IsRefineScroll(srcItemVNum):
			return TRUE
		elif app.ENABLE_SOULBIND_SYSTEM and (item.IsSealScroll(srcItemVNum) or item.IsUnSealScroll(srcItemVNum)):
			return True
		elif item.IsMetin(srcItemVNum):
			return TRUE
		elif item.IsDetachScroll(srcItemVNum):
			return TRUE
		elif item.IsKey(srcItemVNum):
			return TRUE
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return TRUE
		else:
			if item.GetUseType(srcItemVNum) in self.USE_TYPE_TUPLE:
				return TRUE
		
		return FALSE

	def __CanUseSrcItemToDstItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
		if srcSlotPos == dstSlotPos:
			return False
		
		if srcItemVNum == player.GetItemIndex(dstSlotPos):
			if player.GetItemCount(dstSlotPos) < 200:
				return True
		elif item.IsMetin(srcItemVNum):
			if player.ATTACH_METIN_OK == player.CanAttachMetin(srcItemVNum, dstSlotPos):
				return True
		elif item.IsDetachScroll(srcItemVNum):
			if player.DETACH_METIN_OK == player.CanDetach(srcItemVNum, dstSlotPos):
				return True
		elif item.IsKey(srcItemVNum):
			if player.CanUnlock(srcItemVNum, dstSlotPos):
				return True
		elif app.ENABLE_SOULBIND_SYSTEM and item.IsSealScroll(srcItemVNum):
			if (item.IsSealScroll(srcItemVNum) and player.CanSealItem(dstSlotPos)) or (item.IsUnSealScroll(srcItemVNum) and player.CanUnSealItem(dstSlotPos)):
				return True
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return True
		else:
			useType = item.GetUseType(srcItemVNum)
			if "USE_CLEAN_SOCKET" == useType:
				if self.__CanCleanBrokenMetinStone(dstSlotPos):
					return True
			elif "USE_ADD_ACCESSORY_SOCKET" == useType:
				if self.__CanAddAccessorySocket(dstSlotPos):
					return TRUE
			elif "USE_PUT_INTO_ACCESSORY_SOCKET" == useType:
				if self.__CanPutAccessorySocket(dstSlotPos, srcItemVNum):
					return TRUE;
			elif "USE_PUT_INTO_BELT_SOCKET" == useType:
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				print "USE_PUT_INTO_BELT_SOCKET", srcItemVNum, dstItemVNum
				item.SelectItem(dstItemVNum)
				if item.ITEM_TYPE_BELT == item.GetItemType():
					return TRUE
		
		return False

	def __CanCleanBrokenMetinStone(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)
		
		if item.ITEM_TYPE_WEAPON != item.GetItemType():
			return FALSE

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemMetinSocket(dstSlotPos, i) == constInfo.ERROR_METIN_STONE:
				return TRUE

		return FALSE

	def __CanPutAccessorySocket(self, dstSlotPos, mtrlVnum):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return FALSE

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return FALSE

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		if mtrlVnum != constInfo.GET_ACCESSORY_MATERIAL_VNUM(dstItemVNum, item.GetItemSubType()):
			return FALSE
		
		if curCount>=maxCount:
			return FALSE

		return TRUE

	def __CanAddAccessorySocket(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return FALSE

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return FALSE

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)
		
		ACCESSORY_SOCKET_MAX_SIZE = 3
		if maxCount >= ACCESSORY_SOCKET_MAX_SIZE:
			return FALSE

		return TRUE


	def ShowToolTip(self, slotIndex):
		if None != selfs.tooltipItem:
			selfs.tooltipItem.SetInventoryItem(slotIndex)

	def OnTop(self):
		if None != selfs.tooltipItem:
			selfs.tooltipItem.SetTop()
		if app.WJ_ENABLE_TRADABLE_ICON:
			map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)
			self.RefreshMarkSlots()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def UseItemSlot(self, slotIndex):
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		slotIndex = self.GetSlotKonum(slotIndex)

		if selfs.wndInterface.GetOnTopWindow() == player.ON_TOP_WND_SHOP:
			if app.IsPressed(app.DIK_LCONTROL) and app.IsPressed(app.DIK_X):
				net.SendShopSellPacketNew(slotIndex, player.GetItemCount(slotIndex))
				return

		if self.wndExchange.IsShow() and slotIndex < player.EQUIPMENT_SLOT_START:
			for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
				itemVnum = exchange.GetItemVnumFromSelf(i)
				if itemVnum != 0:
					continue

				if not net.SendExchangeItemAddPacket(1, slotIndex, i):
					continue
				break
			return

		if app.ENABLE_ITEM_DELETE_SYSTEM:
			if self.isShowDeleteItemDlg():
				self.wndItemDelete.AddItemWithoutMouse(player.INVENTORY, slotIndex)
				return
		if app.ENABLE_ITEM_DELETE_SYSTEM:
			if self.isShowDeleteItemDlg():
				self.wndItemDelete.InventoryRightClick(player.INVENTORY, slotIndex)
				return

		self.__UseItem(slotIndex)
		mouseModule.mouseController.DeattachObject()
		self.OverOutItem()

	def IsTreasureBox(self, slotIndex):
		itemVnum = player.GetItemIndex(slotIndex)
		item.SelectItem(itemVnum)
		
		if item.GetItemType() == item.ITEM_TYPE_GIFTBOX:
			return True
			
			
		## you can set own treasures which not have item giftbox type, simply add vnums here
		treasures = {
			0: 50011,
			1: 50024,
			2: 50025,
			3: 50031,
			4: 50032,
		}
		
		if itemVnum in treasures.values():
			return True
			
		return False

	def SendMultipleUseItemPacket(self, slotIndex):	
		for i in xrange(1):
			self.__SendUseItemPacket(slotIndex)
			

	def __UseItem(self, slotIndex):
		# if selfs.wndInterface and selfs.wndInterface.AttachInvenItemToOtherWindowSlot(player.INVENTORY, slotIndex): return
		ItemVNum = player.GetItemIndex(slotIndex)
		item.SelectItem(ItemVNum)
 		if app.IsPressed(app.DIK_LALT) and self.IsTreasureBox(slotIndex):
			self.SendMultipleUseItemPacket(slotIndex)
			return
		if app.__BL_CHEST_DROP_INFO__:
			if app.IsPressed(app.DIK_LCONTROL):
				isMain = not app.IsPressed(app.DIK_LSHIFT)
				if item.HasDropInfo(ItemVNum, isMain) and selfs.wndInterface:
					selfs.wndInterface.OpenChestDropWindow(ItemVNum, isMain)
				return
		if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_ITEM)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
			self.questionDialog.Open()
			self.questionDialog.slotIndex = slotIndex

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
		elif player.GetItemTypeBySlot(slotIndex) == item.ITEM_TYPE_GIFTBOX:
			self.__SendUseItemPacket(slotIndex)
		else:
			self.__SendUseItemPacket(slotIndex)

	def OnUpdate(self):
		if self.islemYapiliyor:
			suanMs=app.GetTime()
			if self.sonIslemMs <= suanMs:
				if (player.GetItemCount(self.tasinanWindow, self.bolunenPos) <= self.bolunecekSayi):
					net.SendItemMovePacket(self.tasinanWindow, self.bolunenPos, self.tasinacakWindow, self.baslangicPos + self.tasinanSayi, player.GetItemCount(self.tasinanWindow, self.bolunenPos))
					self.islemYapiliyor = False
					return
				net.SendItemMovePacket(self.tasinanWindow, self.bolunenPos, self.tasinacakWindow, self.baslangicPos + self.tasinanSayi, self.bolunecekSayi)
				self.tasinanSayi+=1
				self.sonIslemMs = app.GetTime()+0.02
				if self.tasinanSayi >= self.tasinacakSayi: self.islemYapiliyor = False
				if self.islemBitisSuresi <= suanMs: self.islemYapiliyor = False
		if self.isChestOpening:
			if app.GetTime() > (self.lastChestOpenTime + constInfo.giftboxWaitTime):
				self.__SendUseItemPacket(self.chestSlotIndex)
				self.openedChestCount += 1
				self.lastChestOpenTime = app.GetTime()
				if self.openedChestCount == self.chestOpenCount:
					self.isChestOpening = False
					self.openedChestCount = 0

	def __UseItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	def __UseItemQuestionDialog_OnAccept(self):
		self.__SendUseItemPacket(self.questionDialog.slotIndex)
		self.OnCloseQuestionDialog()

	def __SendUseItemToItemPacket(self, srcSlotPos, dstSlotPos):
		# °³ÀÎ»óÁ¡ ¿­°í ÀÖ´Â µ¿¾È ¾ÆÀÌÅÛ »ç¿ë ¹æÁö
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return
			
		if (uiOfflineShopBuilder.IsBuildingOfflineShop()):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_OFFLINE_SHOP)
			return
			
		if (uiOfflineShop.IsEditingOfflineShop()):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_OFFLINE_SHOP)
			return

		net.SendItemUseToItemPacket(srcSlotPos, dstSlotPos)

	def __SendUseItemPacket(self, slotPos):
		# °³ÀÎ»óÁ¡ ¿­°í ÀÖ´Â µ¿¾È ¾ÆÀÌÅÛ »ç¿ë ¹æÁö
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return
			
		if (uiOfflineShopBuilder.IsBuildingOfflineShop()):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_OFFLINE_SHOP)
			return
			
		if (uiOfflineShop.IsEditingOfflineShop()):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_OFFLINE_SHOP)
			return

		net.SendItemUsePacket(slotPos)

	def __SendMoveItemPacket(self, srcSlotPos, dstSlotPos, srcItemCount, gelenWindowType = -1, hedefWindowType = -1, topluAyrim = False):
		# °³ÀÎ»óÁ¡ ¿­°í ÀÖ´Â µ¿¾È ¾ÆÀÌÅÛ »ç¿ë ¹æÁö
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return
			
		if (uiOfflineShopBuilder.IsBuildingOfflineShop()):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_OFFLINE_SHOP)
			return
			
		if (uiOfflineShop.IsEditingOfflineShop()):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_OFFLINE_SHOP)
			return

		if topluAyrim:
			hedefWindow=player.SlotTypeToInvenType(hedefWindowType)
			gelenWindow=player.SlotTypeToInvenType(gelenWindowType)
			sonCell = dstSlotPos + (player.GetItemCount(gelenWindow, srcSlotPos) / srcItemCount)
			maxSonCell = 360
			if (dstSlotPos >= 361 and dstSlotPos < 495): maxSonCell = 495
			elif (dstSlotPos >= 496 and dstSlotPos < 630): maxSonCell = 630
			elif (dstSlotPos >= 631 and dstSlotPos < 765): maxSonCell = 765
			if sonCell > maxSonCell: chat.AppendChat(1,"Envanter sýnýrý iþlem için yeterli deðil."); return
			for i in range(dstSlotPos,sonCell):
				try:
					if (player.GetItemCount(hedefWindow, i) > 0): chat.AppendChat(1,"Ayýrma iþlemi sýrasýndaki sýrada item var.."); return
				except: pass
			self.islemYapiliyor = True
			self.bolunecekSayi = srcItemCount
			self.bolunenPos = srcSlotPos
			self.baslangicPos = dstSlotPos
			self.tasinanSayi = 0
			self.tasinacakSayi = sonCell - dstSlotPos
			self.tasinanWindow = gelenWindow
			self.tasinacakWindow = hedefWindow
			self.islemBitisSuresi = app.GetTime() + (self.tasinacakSayi * 200)
			chat.AppendChat(1,"Biraz bekleyin, iþlem toplamda %sms sürecektir. " % str(self.tasinacakSayi*200))
		else:
			net.SendItemMovePacket(srcSlotPos, dstSlotPos, srcItemCount)

	def SetDragonSoulRefineWindow(self, wndDragonSoulRefine):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoulRefine = wndDragonSoulRefine

	def autoOpenCheck(self):
		status = constInfo.exInventory_isToggle
		if status == True: self.checkBox.SetCheckStatus(True); return True
		self.checkBox.SetCheckStatus(False); return False

	def autoOpenClick(self, checkType, autoFlag):
		status = True if autoFlag else False
		constInfo.exInventory_isToggle = status


	if app.ENABLE_ITEM_DELETE_SYSTEM:
		def SetDeleteItemDlg(self, wndItemDelete):
			self.wndItemDelete = wndItemDelete
			
		def isShowDeleteItemDlg(self):
			if self.wndItemDelete:
				if self.wndItemDelete.IsShow():
					return 1
					
			return 0