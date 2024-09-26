import ui
import player,skill,chat,net,app,uiTooltip
import wndMgr,mouseModule
import dbg, grp, grid, item, constInfo

import player, exchange

if app.WJ_ENABLE_TRADABLE_ICON:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE
	# if app.ENABLE_SPECIAL_STORAGE:
		# SPECIAL_INVENTORY_PAGE_SIZE = player.SPECIAL_PAGE_SIZE

SLOT_COUNT = 8 * 10

class TrashWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.mod = 0
		self.LoadWindow()
		self.information = {}

		self.tooltipItem = None
		# self.Debugenable = True

		self.interface = 0
		self.questionDialog = 0
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.inven = None
			# if app.ENABLE_SPECIAL_STORAGE:
				# self.invenSpecial = None
		self.m_pGrid = grid.PythonGrid(10, 8)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	# def BindInterfaceClass(self, interface):
		# self.interface = interface

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/deleteboard.py")
		except:
			import exception
			exception.Abort("TrashWindow.LoadWindow.LoadScript")

		try:
			GetObject=self.GetChild
			self.board = GetObject("board")
			self.trashSlot = GetObject("TrashSlot")
			# self.delBtn = GetObject("deleteBtn")
			self.silcheck = GetObject("silcheck")
			self.satcheck = GetObject("satcheck")
		except:
			import exception
			exception.Abort("TrashWindow.LoadWindow.BindObject")

		self.board.SetCloseEvent(self.Close)
		# self.delBtn.SetEvent(ui.__mem_func__(self.SendDeletePacket))
		self.silcheck.SetEvent(ui.__mem_func__(self.SilMod))
		self.satcheck.SetEvent(ui.__mem_func__(self.SatMod))

		self.trashSlot.SetUnselectItemSlotEvent(self.OnUseItemSlot)
		self.trashSlot.SetUseSlotEvent(self.OnUseItemSlot)
		self.trashSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.trashSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.trashSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.__OnSelectEmptySlot))

		self.Yenile()

	def SetMod(self, status):
		self.mod = int(status)
		self.Yenile()

	def Yenile(self):
		if self.mod == 1:
			self.silcheck.Down()
			self.satcheck.SetUp()
		else:
			self.silcheck.SetUp()
			self.satcheck.Down()

	def SilMod(self):
		self.mod = 1
		# chat.AppendChat(1,"Islem sonunda itemler silinecek.")
		net.SendChatPacket("/automatic_item_process status 1")
		self.Yenile()
		
	def SatMod(self):
		self.mod = 0
		# chat.AppendChat(1,"Islem sonunda itemler satilacak.")
		net.SendChatPacket("/automatic_item_process status 0")
		self.Yenile()

	def LoadDialog(self):
		pass

	# def BindInterface(self, interface):
		# self.interface = interface

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	# def SetInven(self, invType, wndInventory):
		# from _weakref import proxy
		# self.wndInventory[invType] = proxy(wndInventory)

	def Destroy(self):
		self.Close()
		self.information = None
		self.tooltipItem = None

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.inven = None
			# if app.ENABLE_SPECIAL_STORAGE:
				# self.invenSpecial = None
		self.m_pGrid		= None

	def Open(self):

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.ItemListIdx = { 
			# player.UPGRADE_INVENTORY : [],
			# player.BOOK_INVENTORY : [],
			# player.STONE_INVENTORY : [],
			# player.CHEST_INVENTORY : [],
			# player.ATTR_INVENTORY : [],
			player.DRAGON_SOUL_INVENTORY : [],
			player.INVENTORY : [],
			}
		# self.RefreshPanel()
		self.SetTop()
		self.SetCenterPosition()
		self.Show()
		net.SendChatPacket("/automatic_item_process list 1")

	def RefreshPanel(self):
		if app.WJ_ENABLE_TRADABLE_ICON:
			try:
				for k,v in self.ItemListIdx.items():
					for i in range(len(v)):

						if self.inven and self.invenSpecial:
							if k == player.INVENTORY:
								self.inven.wndItem.SetCanMouseEventSlot(v[i][5])

						# if app.ENABLE_SPECIAL_STORAGE:
							# if self.inven and self.invenSpecial:
								# if k == player.INVENTORY:
									# self.inven.wndItem.SetCanMouseEventSlot(v[i][5])
								# else:
										# self.invenSpecial.wndItem.SetCanMouseEventSlot(v[i][5])
			except: pass

		self.information = {}
		self.m_pGrid.Clear()
		for i in range(SLOT_COUNT):
			self.trashSlot.ClearSlot(i)
		self.trashSlot.RefreshSlot()
		

	def Close(self):
		self.RefreshPanel()
		self.Hide()
		#if app.WJ_ENABLE_TRADABLE_ICON:
			#if self.interface:
				#self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
#				self.interface.RefreshMarkInventoryBag()


	def OnPressEscapeKey(self):
		self.Close()
		return True

	# def SendDeletePacket(self):
		# for i in range(SLOT_COUNT):
			# self.trashSlot.ClearSlot(i)

		# self.trashSlot.RefreshSlot()
		
		# for key, info in self.information.iteritems():
			# if self.mod == 0:
				# net.SendItemDestroyPacket(info[4], info[3])
				# continue
			# else:
				# net.SendItemSellPacket(info[4], info[3])
				# continue
		
		# self.m_pGrid.Clear()
		# self.information = {}

	def AddItem(self, vnum):
		if int(vnum) == 0: return
		item.SelectItem(vnum)
		(itemWidth, itemHeight) = item.GetItemSize()
		iPos = self.FindBlank(itemWidth, itemHeight)
		self.SetBuildItem(iPos, int(vnum), 1)
		self.m_pGrid.Put(iPos, itemWidth, itemHeight)
		details = [ iPos, int(vnum), 1, -1, -1, -1, -1 ]
		self.information[iPos] = details


	def __OnSelectEmptySlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)

			itemIndex = player.GetItemIndex(attachedInvenType, attachedSlotPos)
			itemCount = player.GetItemCount(attachedInvenType, attachedSlotPos)
			
			invenPage = self.inven.GetInventoryPageIndex()
			realPos = self.inven.InventoryLocalSlotPosToGlobalSlotPos(attachedSlotPos)
			if attachedInvenType != player.INVENTORY:
				invenPage = self.invenSpecial.GetInventoryPageIndex()
				realPos = self.invenSpecial.InventoryLocalSlotPosToGlobalSlotPos(attachedSlotPos)
			
			
			mouseModule.mouseController.DeattachObject()

			

			if player.IsEquipmentSlot(attachedInvenType, realPos):
				return

			if self.IsInList((attachedInvenType, attachedSlotPos, invenPage)):
				return
			details = [ selectedSlotPos, itemIndex, itemCount, attachedInvenType, attachedSlotPos, realPos, invenPage ]
			self.information[selectedSlotPos] = details
			item.SelectItem(itemIndex)
			(iWidth, iHeight) = item.GetItemSize()
			self.m_pGrid.Put(selectedSlotPos, iWidth, iHeight)

			if app.WJ_ENABLE_TRADABLE_ICON:
				self.ItemListIdx[attachedInvenType].append(details)

			self.SetBuildItem(selectedSlotPos, itemIndex, itemCount)
			net.SendChatPacket("/automatic_item_process add %d" % (itemIndex))

	def SetBuildItem(self, selectedSlotPos, itemIndex, itemCount):
		self.trashSlot.SetItemSlot(selectedSlotPos, itemIndex, itemCount)
		self.trashSlot.RefreshSlot()

	def IsInList(self, pos):
		(inv_type, inv_pos, invenPage) = pos
		for key, info in self.information.iteritems():
			if (info and info[3] == inv_type and info[4] == inv_pos and info[6] == invenPage):
				return True
		return False

	def GetGrid(self):
		return self.m_pGrid

	def FindBlank(self, itemWidth, itemHeight):
		return self.m_pGrid.FindBlank(itemWidth, itemHeight)

	def SetItemFromInventory(self, slotIndex, invPos):
		(window, pos) = invPos
		mouseModule.mouseController.DeattachObject()

		if player.INVENTORY == window:
			attachSlotType = player.SLOT_TYPE_INVENTORY
		# elif player.BOOK_INVENTORY == window:
			# attachSlotType = player.SLOT_TYPE_BOOK_INVENTORY
		# elif player.UPGRADE_INVENTORY == window:
			# attachSlotType = player.SLOT_TYPE_UPGRADE_INVENTORY
		# elif player.CHEST_INVENTORY == window:
			# attachSlotType = player.SLOT_TYPE_CHEST_INVENTORY
		# elif player.STONE_INVENTORY == window:
			# attachSlotType = player.SLOT_TYPE_STONE_INVENTORY
		# elif player.ATTR_INVENTORY == window:
			# attachSlotType = player.SLOT_TYPE_ATTR_INVENTORY
		# elif player.DRAGON_SOUL_INVENTORY == window:
			# attachSlotType = player.SLOT_TYPE_DRAGON_SOUL_INVENTORY
		else:
			return

		selectedItemVNum = player.GetItemIndex(window, pos)
		count			 = player.GetItemCount(window, pos)

		for key, info in self.information.iteritems():
			if info and info[1] == selectedItemVNum:
				chat.AppendChat(1, "Bu item zaten eklenilmis.")
				return

		mouseModule.mouseController.AttachObject(self, attachSlotType, pos, selectedItemVNum, count)
		self.__OnSelectEmptySlot(slotIndex)

	def OnUseItemSlot(self, slotIndex):
		if self.information.has_key(slotIndex):

			if app.WJ_ENABLE_TRADABLE_ICON:
				for k,v in self.ItemListIdx.items():
					for i in range(len(v)):
						invenType = self.information[slotIndex][3]
						attSlot = self.information[slotIndex][4]
						if v[i][3] == invenType and v[i][4] == attSlot:

							if self.inven:
								if k == player.INVENTORY:
									self.inven.wndItem.SetCanMouseEventSlot(v[i][4])
									

							# if app.ENABLE_SPECIAL_STORAGE:
								# if self.inven or self.invenSpecial:
									# if k == player.INVENTORY:
										# self.inven.wndItem.SetCanMouseEventSlot(v[i][4])
									# else:
										# if self.invenSpecial.GetInventoryWindow() == v[i][3]:
											# self.invenSpecial.wndItem.SetCanMouseEventSlot(v[i][4])
							del v[i]
							break

						# if app.ENABLE_SPECIAL_INVENTORY_SYSTEM:
							# if self.inven or self.invenSpecial:

			self.SetBuildItem(self.information[slotIndex][0], 0, 0)
			net.SendChatPacket("/automatic_item_process remove {}".format(self.information[slotIndex][1]))
			del self.information[slotIndex]
			self.m_pGrid.Clear()
			for key, info in self.information.iteritems():
				selectPos = info[0]
				itemIndex = info[1]
				item.SelectItem(itemIndex)
				(w, h) = item.GetItemSize()
				self.m_pGrid.Put(selectPos, w, h)

	def OverInItem(self, slot):
		if not self.tooltipItem:
			return

		self.tooltipItem.ClearToolTip()
		if self.information.has_key(slot):
			details = self.information[slot]
			self.tooltipItem.SetDeleteItemEx(details[1])
			# self.tooltipItem.AppendTextLine("Bu nesne silinmeye hazirdir.", 0xff26EB7F)
			# if self.Debugenable == True:
				# self.tooltipItem.AppendTextLine("Bulundugu Slot: "+str(details[4]),0xff26EB1f)
			# self.tooltipItem.AppendTextLine("|Eemoji/key_rclick|e - Itemi Kaldir")

	def OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	if app.WJ_ENABLE_TRADABLE_ICON:
		def OnUpdate(self):
			# if app.ENABLE_SPECIAL_STORAGE:
				# if not self.inven or not self.invenSpecial:
					# return
			# else:
			if not self.inven:
				return

			page = self.inven.GetInventoryPageIndex() # range 0 ~ 1
			# if app.ENABLE_SPECIAL_STORAGE:
				# special_page = self.invenSpecial.GetInventoryPageIndex()

			for k,v in self.ItemListIdx.items():
				for i in range(len(v)):
					if k == player.INVENTORY:
						if page == v[i][6]:
							realPos = v[i][5]
							self.inven.wndItem.SetCantMouseEventSlot(realPos - (90 * page))
					else:
						if self.invenSpecial.GetInventoryWindow() == v[i][3] and special_page == v[i][6]:
							realPos = v[i][5]
							self.invenSpecial.wndItem.SetCantMouseEventSlot(realPos - (90 * special_page))

	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantTradableItem(self, slotIndex):
			itemIndex = player.GetItemIndex(slotIndex)
			if itemIndex:
				return player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_GIVE)

			return False

		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def OnTop(self):
			self.tooltipItem.SetTop()
			if not self.interface:
				return

			self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
			self.interface.RefreshMarkInventoryBag()

		def SetInven(self, inven):
			self.inven = inven

		# if app.ENABLE_SPECIAL_STORAGE:
			# def SetSpecialInven(self, invenSpecial):
				# self.invenSpecial = invenSpecial