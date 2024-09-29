##
## Interface
##
import wndInfo as selfs
import constInfo
import systemSetting
import wndMgr
import chat
import app
import player
import uiTaskBar
import uiCharacter
import uiInventory
import uiDragonSoul
import uiChat
import uiMessenger
import guild
import uibotcontrol
import uiBonusChanger
import uiluckydraw
if app.__AUTO_HUNT__:
	import uiAutoHunt
if app.LINK_IN_CHAT:
	import os

import ui
import uiHelp
import uiWhisper
import uiPointReset
import uiShop
import uiExchange
import uiSystem
import uiRestart
import uiToolTip
import uiMiniMap
import uiParty
import uiSafebox
import uiGuild
import uiQuest
import uiPrivateShopBuilder
import uiCommon
import uiRefine
import uiEquipmentDialog
import uiGameButton
import uiTip
import uiCube
import uiCards
import miniMap
import ui_activity_day
if app.ENABLE_EVENT_MANAGER:
	import uiEventCalendarNew

if app.ENABLE_ITEM_DELETE_SYSTEM:
	import uiDeleteItem

if app.__RANKING_SYSTEM__:
	import uiRank

if app.ENABLE_AUTOMATIC_ITEM_PROCESS:
	import uideleteitems

if app.ENABLE_REWARD_SYSTEM:
	import uiReward

if app.AUTO_SHOUT:
	import uishout
if app.ENABLE_NEW_RANKING:
	import uirank
if app.ENABLE_NEW_MISSIONS:
	import uinewmissions
if app.__BL_CHEST_DROP_INFO__:
	import uiChestDropInfo

if app.BL_REMOTE_SHOP:
	import uiRemoteShop

if app.ENABLE_SWITCHBOT:
	import uiSwitchbot

if app.ENABLE_OFFLINE_SHOP_SYSTEM:
	import uiOfflineShopBuilder
	import uiOfflineShop

import uiExInventory

if app.ENABLE_NEW_BIOLOG:
	import uinewbiolog

if app.NEW_SALES_SYSTEM:
	import uisales
	import net

if app.BL_PRIVATESHOP_SEARCH_SYSTEM:
	import uiPrivateShopSearch
	

if app.FATE_ROULETTE:
	import uifateroulette
	

if app.ENABLE_CAOS_EVENT:
	import uicaosevent

if app.ENABLE_EXTENDED_BATTLE_PASS:
	import uiBattlePassExtended

# ACCESSORY_REFINE_ADD_METIN_STONE
import uiSelectItem
# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE
import uiScriptLocale

import event
import localeInfo

if app.ENABLE_ACCE_COSTUME_SYSTEM:
	import uiacce

if app.ITEM_SHOP:
	import uinesnemarket
if app.ENABLE_STREAMER_SYSTEM:
	import uistreamerlink
IsQBHide = 0
class Interface(object):
	CHARACTER_STATUS_TAB = 1
	CHARACTER_SKILL_TAB = 2
	#LastContactTimeStamp = app.GetTime() - 0
	#WaitTime = 0
	#State = "Kapali"

	def __init__(self):
		systemSetting.SetInterfaceHandler(self)
		if app.__AUTO_HUNT__:
			self.wndAutoHunt = None

		self.windowOpenPosition = 0
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.onTopWindow = player.ON_TOP_WND_NONE
		self.dlgWhisperWithoutTarget = None
		self.inputDialog = None
		self.tipBoard = None
		self.bigBoard = None
		if app.ENABLE_REWARD_SYSTEM:
			self.wndReward = None

		# ITEM_MALL
		self.mallPageDlg = None
		# END_OF_ITEM_MALL

		self.wndWeb = None
		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			self.wndCubeRenewal = None
		self.wndTaskBar = None
		if app.BL_REMOTE_SHOP:
			self.wndRemoteShop = None
			
		self.wndCharacter = None
		self.wndExpandedTaskBar = None
		self.wndDragonSoul = None
		self.wndDragonSoulRefine = None
		self.wndChat = None
		self.ChatKapat = None
		self.wndMessenger = None
		if app.LINK_IN_CHAT:
			self.OpenLinkQuestionDialog = None
		self.wndMiniMap = None
		self.wndGuild = None
		self.wndGuildBuilding = None
		if app.__RANKING_SYSTEM__:
			self.wndRanking = None
		if app.ENABLE_NEW_MISSIONS:
			self.wndMissionPanel = None
		if app.ENABLE_SWITCHBOT:
			self.wndSwitchbot = None
			
		if app.ENABLE_ITEM_DELETE_SYSTEM:
			self.deleteitem = None
		if app.__BL_CHEST_DROP_INFO__:
			self.wndChestDropInfo = None
		if app.ENABLE_CAOS_EVENT:
			self.wndCaosEvent = None
		if app.ENABLE_EVENT_MANAGER:
			self.wndEventManager = None
			self.wndEventIcon = None

		luckyDrawButton = ui.Button()
		luckyDrawButton.SetUpVisual("d:/ymir work/ui/lucky_draw/main_join.tga")
		luckyDrawButton.SetOverVisual("d:/ymir work/ui/lucky_draw/main_join_over.tga")
		luckyDrawButton.SetDownVisual("d:/ymir work/ui/lucky_draw/main_join_over.tga")
		luckyDrawButton.SetPosition(systemSetting.GetWidth()-170, 10)#fýsatý yakala butonu ayarla
		luckyDrawButton.SetEvent(ui.__mem_func__(self.LuckDrawOpen))
		luckyDrawButton.Show()
		self.luckyDrawButton = luckyDrawButton

		if app.FATE_ROULETTE:
			self.wndFate = None
			self.wndFateButton = None
			self.wndFate = uifateroulette.FateRoulette()
			self.wndFate.Hide()
			wndFateButton = ui.Button()
			wndFateButton.SetUpVisual("d:/ymir work/ui/minigame/rumi/berkay2.sub")
			wndFateButton.SetOverVisual("d:/ymir work/ui/minigame/rumi/berkay2.sub")
			wndFateButton.SetDownVisual("d:/ymir work/ui/minigame/rumi/berkay2.sub")
			wndFateButton.SetPosition(systemSetting.GetWidth()-425, 20)
			wndFateButton.SetEvent(ui.__mem_func__(self.RouletteOpen))
			wndFateButton.Hide()
			self.wndFateButton = wndFateButton

		if app.ENABLE_EXTENDED_BATTLE_PASS:
			self.wndBattlePassExtended = None
			self.isFirstOpeningExtBattlePass = False
		if app.ENABLE_NEW_RANKING:
			self.wndRank = None
		self.listGMName = {}
		self.wndBotControl = None
		self.wndQuestWindow = {}
		self.wndQuestWindowNewKey = 0
		self.privateShopAdvertisementBoardDict = {}
		if app.ENABLE_OFFLINE_SHOP_SYSTEM: self.offlineShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict = {}
		event.SetInterfaceWindow(self)

	def __del__(self):
		systemSetting.DestroyInterfaceHandler()
		event.SetInterfaceWindow(None)

	################################
	## Make Windows & Dialogs
	def __MakeUICurtain(self):
		wndUICurtain = ui.Bar("TOP_MOST")
		wndUICurtain.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		wndUICurtain.SetColor(0x77000000)
		wndUICurtain.Hide()
		self.wndUICurtain = wndUICurtain


	if app.ENABLE_STREAMER_SYSTEM:
		def __MakeStreamerLink(self):
			self.wndStreamerLink = uistreamerlink.LinkWindow()
			self.wndStreamerLink.LoadWindow()
			self.wndStreamerLink.Hide()

	def __MakeMessengerWindow(self):
		self.wndMessenger = uiMessenger.MessengerWindow()

		from _weakref import proxy
		self.wndMessenger.SetWhisperButtonEvent(lambda n,i=proxy(self):i.OpenWhisperDialog(n))
		self.wndMessenger.SetGuildButtonEvent(ui.__mem_func__(self.ToggleGuildWindow))

	def __MakeGuildWindow(self):
		self.wndGuild = uiGuild.GuildWindow()

	def __MakeChatWindow(self):

		wndChat = uiChat.ChatWindow()

		wndChat.SetSize(wndChat.CHAT_WINDOW_WIDTH, 0)
		wndChat.SetPosition(wndMgr.GetScreenWidth()/2 - wndChat.CHAT_WINDOW_WIDTH/2, wndMgr.GetScreenHeight() - wndChat.EDIT_LINE_HEIGHT - 37)
		wndChat.SetHeight(200)
		wndChat.Refresh()
		wndChat.Show()

		self.wndChat = wndChat
		self.wndChat.BindInterface(self)
		self.wndChat.SetSendWhisperEvent(ui.__mem_func__(self.OpenWhisperDialogWithoutTarget))
		self.wndChat.SetOpenChatLogEvent(ui.__mem_func__(self.ToggleChatLogWindow))
		if self.ChatKapat:
			self.ChatKapat.Hide()
		ChatKapat = self.chatackapa(None, wndMgr.GetScreenWidth()/2 - wndChat.CHAT_WINDOW_WIDTH/2 - 45, wndMgr.GetScreenHeight() - wndChat.EDIT_LINE_HEIGHT - 35 + 9)
		self.ChatKapat = ChatKapat

	def __MakeTaskBar(self):
		wndTaskBar = uiTaskBar.TaskBar()
		wndTaskBar.LoadWindow()
		self.wndTaskBar = wndTaskBar
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_CHARACTER, ui.__mem_func__(self.ToggleCharacterWindowStatusPage))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_INVENTORY, ui.__mem_func__(self.ToggleInventoryWindow))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_MESSENGER, ui.__mem_func__(self.ToggleMessenger))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_SYSTEM, ui.__mem_func__(self.ToggleSystemDialog))


		if uiTaskBar.TaskBar.IS_EXPANDED:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_EXPAND, ui.__mem_func__(self.ToggleExpandedButton))
			self.wndExpandedTaskBar = uiTaskBar.ExpandedTaskBar()
			self.wndExpandedTaskBar.LoadWindow()
			self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_DRAGON_SOUL, ui.__mem_func__(self.ToggleDragonSoulWindow))

		else:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_CHAT, ui.__mem_func__(self.ToggleChat))

		#self.wndEnergyBar = None
		#import app
		#if app.ENABLE_ENERGY_SYSTEM:
		#	wndEnergyBar = uiTaskBar.EnergyBar()
		#	wndEnergyBar.LoadWindow()
		#	self.wndEnergyBar = wndEnergyBar



	def __MakeParty(self):
		wndParty = uiParty.PartyWindow()
		wndParty.Hide()
		self.wndParty = wndParty

	def __MakeGameButtonWindow(self):
		wndGameButton = uiGameButton.GameButtonWindow()
		wndGameButton.SetTop()
		wndGameButton.Show()
		wndGameButton.SetButtonEvent("STATUS", ui.__mem_func__(self.__OnClickStatusPlusButton))
		wndGameButton.SetButtonEvent("SKILL", ui.__mem_func__(self.__OnClickSkillPlusButton))
		wndGameButton.SetButtonEvent("QUEST", ui.__mem_func__(self.__OnClickQuestButton))
		wndGameButton.SetButtonEvent("HELP", ui.__mem_func__(self.__OnClickHelpButton))
		wndGameButton.SetButtonEvent("BUILD", ui.__mem_func__(self.__OnClickBuildButton))

		self.wndGameButton = wndGameButton

	def __IsChatOpen(self):
		return True

	def __MakeWindows(self):
		wndCharacter = uiCharacter.CharacterWindow()
		selfs.wndInventory = uiInventory.InventoryWindow()
		selfs.wndInventory.BindInterfaceClass(self)
		selfs.wndExInventory = uiExInventory.InventoryWindow()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			wndDragonSoul = uiDragonSoul.DragonSoulWindow()
			wndDragonSoulRefine = uiDragonSoul.DragonSoulRefineWindow()
		else:
			wndDragonSoul = None
			wndDragonSoulRefine = None

		wndMiniMap = uiMiniMap.MiniMap()
		wndSafebox = uiSafebox.SafeboxWindow()
		if app.WJ_ENABLE_TRADABLE_ICON:
			wndSafebox.BindInterface(self)

		# ITEM_MALL
		wndMall = uiSafebox.MallWindow()
		self.wndMall = wndMall
		# END_OF_ITEM_MALL

		if app.BL_PRIVATESHOP_SEARCH_SYSTEM:
			self.wndPrivateShopSearch = uiPrivateShopSearch.PrivateShopSeachWindow()
			

		wndChatLog = uiChat.ChatLogWindow()
		wndChatLog.BindInterface(self)

		if app.BL_REMOTE_SHOP:
			self.wndRemoteShop = uiRemoteShop.RemoteShopDialog()
			

		self.wndCharacter = wndCharacter
		self.wndDragonSoul = wndDragonSoul
		self.wndDragonSoulRefine = wndDragonSoulRefine
		self.wndMiniMap = wndMiniMap
		self.wndSafebox = wndSafebox
		self.wndChatLog = wndChatLog
		if app.ENABLE_NEW_MISSIONS:
			self.wndMissionPanel = uinewmissions.MissionWindow()
		if app.ENABLE_NEW_BIOLOG:
			self.wndBioWindow = uinewbiolog.NewBiologWindow()
		if app.__BL_CHEST_DROP_INFO__:
			self.wndChestDropInfo = uiChestDropInfo.ChestDropInfoWindow()

		if app.ENABLE_CAOS_EVENT:
			self.wndCaosEvent = uicaosevent.CaosEventWindow()

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.SetDragonSoulRefineWindow(self.wndDragonSoulRefine)
			self.wndDragonSoulRefine.SetInventoryWindows(selfs.wndInventory, self.wndDragonSoul)
			selfs.wndInventory.SetDragonSoulRefineWindow(self.wndDragonSoulRefine)
		if app.ENABLE_SWITCHBOT:
			self.wndSwitchbot = uiSwitchbot.SwitchbotWindow()
		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			selfs.wndOfflineShopManager = uiOfflineShop.OfflineShopManagerWindow()
			selfs.wndOpenOfflineShop = uiOfflineShop.OpenOfflineShop()
		if app.ITEM_SHOP:
			self.wndNesneMarket = uinesnemarket.NesneMarketWindow()
		if app.ENABLE_EXTENDED_BATTLE_PASS:
			self.wndBattlePassExtended = uiBattlePassExtended.BattlePassWindow()

		if app.ENABLE_NEW_RANKING:
			self.wndRank = uirank.RankingGUI()

		self.wndBotControl = uibotcontrol.BotControlWindow()

	def __MakeDialogs(self):
		self.dlgExchange = uiExchange.ExchangeDialog()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgExchange.BindInterface(self)
			self.dlgExchange.SetInven(selfs.wndInventory)
			selfs.wndInventory.BindWindow(self.dlgExchange)
		self.dlgExchange.LoadDialog()
		self.dlgExchange.SetCenterPosition()
		self.dlgExchange.Hide()
		self.dlgLuckyDraw = uiluckydraw.LuckyDrawWindow()
		self.dlgPointReset = uiPointReset.PointResetDialog()
		self.dlgPointReset.LoadDialog()
		self.dlgPointReset.Hide()

		self.dlgShop = uiShop.ShopDialog()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgShop.BindInterface(self)
		self.dlgShop.LoadDialog()
		self.dlgShop.Hide()

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			selfs.dlgOfflineShop = uiOfflineShop.OfflineShopDialog()
			selfs.dlgOfflineShop.LoadDialog()
			selfs.dlgOfflineShop.Hide()

######	TICARET	SAG	TIK
		selfs.wndInventory.SetExchangeWindow(self.dlgExchange)
		selfs.wndExInventory.SetExchangeWindow(self.dlgExchange)


		self.dlgRestart = uiRestart.RestartDialog()
		self.dlgRestart.LoadDialog()
		self.dlgRestart.Hide()

		self.dlgSystem = uiSystem.SystemDialog()
		self.dlgSystem.LoadDialog()
		self.dlgSystem.SetOpenHelpWindowEvent(ui.__mem_func__(self.OpenHelpWindow))

		self.dlgSystem.Hide()

		self.dlgPassword = uiSafebox.PasswordDialog()
		self.dlgPassword.Hide()

		self.hyperlinkItemTooltip = uiToolTip.HyperlinkItemToolTip()
		self.hyperlinkItemTooltip.Hide()


		selfs.tooltipItem = uiToolTip.ItemToolTip()
		selfs.tooltipItem.Hide()

		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()

		self.tooltipSkill = uiToolTip.SkillToolTip()
		self.tooltipSkill.Hide()

		self.privateShopBuilder = uiPrivateShopBuilder.PrivateShopBuilder()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.privateShopBuilder.BindInterface(self)
			self.privateShopBuilder.SetInven(selfs.wndInventory)
			selfs.wndInventory.BindWindow(self.privateShopBuilder)
		self.privateShopBuilder.Hide()

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			selfs.offlineShopBuilder = uiOfflineShopBuilder.OfflineShopBuilder()
			selfs.wndInventory.BindWindow(selfs.offlineShopBuilder)
			selfs.offlineShopBuilder.Hide()

		if app.ENABLE_ITEM_DELETE_SYSTEM:
			self.deleteitem = uiDeleteItem.DeleteItem()
			self.deleteitem.Hide()
			if selfs.wndInventory:
				selfs.wndInventory.SetDeleteItemDlg(self.deleteitem)
				
			## Call back inventory expanded fix.
			if (selfs.wndExInventory):
				selfs.wndExInventory.SetDeleteItemDlg(self.deleteitem)


		self.dlgRefineNew = uiRefine.RefineDialogNew()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgRefineNew.SetInven(selfs.wndInventory)
			selfs.wndInventory.BindWindow(self.dlgRefineNew)
		self.dlgRefineNew.Hide()

		if app.ENABLE_AUTOMATIC_ITEM_PROCESS:
			self.wndDeleteWindow = uideleteitems.TrashWindow()
			self.wndDeleteWindow.Hide()
			self.wndDeleteWindow.SetItemToolTip(selfs.tooltipItem)
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.wndDeleteWindow.BindInterface(self)
				self.wndDeleteWindow.SetInven(selfs.wndInventory)

	def __MakeHelpWindow(self):
		self.wndHelp = uiHelp.HelpWindow()
		self.wndHelp.LoadDialog()
		self.wndHelp.SetCloseEvent(ui.__mem_func__(self.CloseHelpWindow))
		self.wndHelp.Hide()

	def __MakeTipBoard(self):
		self.tipBoard = uiTip.TipBoard()
		self.tipBoard.Hide()

		self.bigBoard = uiTip.BigBoard()
		self.bigBoard.Hide()

	def __MakeWebWindow(self):
		if constInfo.IN_GAME_SHOP_ENABLE:
			import uiWeb
			self.wndWeb = uiWeb.WebWindow()
			self.wndWeb.LoadWindow()
			self.wndWeb.Hide()

	if app.AUTO_SHOUT:
		def __MakeShoutWindow(self):
			self.wndShout = uishout.ShoutManager()
			self.wndShout.LoadWindow()
			self.wndShout.Hide()

		def __MakeCubeWindow(self):
			self.wndCube = uiCube.CubeWindow()
			self.wndCube.LoadWindow()
			self.wndCube.Hide()

		def __MakeCubeResultWindow(self):
			self.wndCubeResult = uiCube.CubeResultWindow()
			self.wndCubeResult.LoadWindow()
			self.wndCubeResult.Hide()

	def __MakeChangerWindow(self):
		self.wndChangerWindow = uiBonusChanger.ChangerWindow()
		self.wndChangerWindow.LoadWindow()
		self.wndChangerWindow.Hide()

	def __MakeCardsInfoWindow(self):
		self.wndCardsInfo = uiCards.CardsInfoWindow()
		self.wndCardsInfo.LoadWindow()
		self.wndCardsInfo.Hide()
		
	def __MakeCardsWindow(self):
		self.wndCards = uiCards.CardsWindow()
		self.wndCards.LoadWindow()
		self.wndCards.Hide()
		
	def __MakeCardsIconWindow(self):
		self.wndCardsIcon = uiCards.IngameWindow()
		self.wndCardsIcon.LoadWindow()
		self.wndCardsIcon.Hide()
		

	def LuckDrawOpen(self):
		net.SendLuckyDrawCurrent()

	def OpenLuckyDrawDialog(self, joinItemVnum, joinItemCount, entryPrice, endTime, totalParticipants,
			  maxParticipants, maxParticipantsPerPlayer, playerParticipants,
				awardItems, winner1Name, winner2Name, winner3Name, isFinished, isWon):
		self.dlgLuckyDraw.Open(joinItemVnum, joinItemCount, entryPrice, endTime, totalParticipants,
			  maxParticipants, maxParticipantsPerPlayer, playerParticipants,
				awardItems, winner1Name, winner2Name, winner3Name, isFinished, isWon)
			
	

	if app.NEW_SALES_SYSTEM:
		def __MakeSalesWindow(self):
			self.wndSales = uisales.SalesClass()
			self.wndSales.Open()
			self.wndSales.Hide()
			
			wndSalesButton = ui.Button()
			wndSalesButton.SetUpVisual("d:/ymir work/ui/minigame/rumi/berkay.sub")
			wndSalesButton.SetOverVisual("d:/ymir work/ui/minigame/rumi/berkay.sub")
			wndSalesButton.SetDownVisual("d:/ymir work/ui/minigame/rumi/berkay.sub")
			wndSalesButton.SetPosition(systemSetting.GetWidth()-325, 20)
			wndSalesButton.SetEvent(ui.__mem_func__(self.OpenSalesWindow))
			wndSalesButton.Hide()
			self.wndSalesButton = wndSalesButton
			
			

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def __MakeAcceWindow(self):
			self.wndAcceCombine = uiacce.CombineWindow()
			self.wndAcceCombine.LoadWindow()
			self.wndAcceCombine.Hide()

			self.wndAcceAbsorption = uiacce.AbsorbWindow()
			self.wndAcceAbsorption.LoadWindow()
			self.wndAcceAbsorption.Hide()

			if selfs.wndInventory:
				selfs.wndInventory.SetAcceWindow(self.wndAcceCombine, self.wndAcceAbsorption)

	# ACCESSORY_REFINE_ADD_METIN_STONE
	def __MakeItemSelectWindow(self):
		self.wndItemSelect = uiSelectItem.SelectItemWindow()
		self.wndItemSelect.Hide()
	# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

	if app.ENABLE_CUBE_RENEWAL_WORLDARD:
		def __MakeCubeRenewal(self):
			import uicuberenewal
			self.wndCubeRenewal = uicuberenewal.CubeRenewalWindows()
			self.wndCubeRenewal.Hide()

	def MakeInterface(self):
		self.__MakeMessengerWindow()
		self.__MakeGuildWindow()
		self.__MakeChatWindow()
		self.__MakeParty()
		self.__MakeWindows()
		self.__MakeDialogs()
		self.__MakeEventWindow()
		self.__MakeUICurtain()
		self.__MakeTaskBar()
		self.__MakeGameButtonWindow()
		self.__MakeHelpWindow()
		self.__MakeTipBoard()
		self.__MakeWebWindow()
		self.__MakeCubeWindow()
		if app.ENABLE_STREAMER_SYSTEM:
			self.__MakeStreamerLink()
		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			self.__MakeCubeRenewal()

		#BONUS CHANGER
		self.__MakeChangerWindow()
		#END OF BONUS CHANGER
		self.__MakeCubeResultWindow()
		self.__MakeCardsInfoWindow()
		self.__MakeCardsWindow()
		self.__MakeCardsIconWindow()
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.__MakeAcceWindow()
		if app.AUTO_SHOUT:
			self.__MakeShoutWindow()
		# ACCESSORY_REFINE_ADD_METIN_STONE
		self.__MakeItemSelectWindow()
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}
		self.privateShopAdvertisementBoardDict = {}

		selfs.wndInventory.SetItemToolTip(selfs.tooltipItem)
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.SetItemToolTip(selfs.tooltipItem)
			self.wndDragonSoulRefine.SetItemToolTip(selfs.tooltipItem)
		self.wndSafebox.SetItemToolTip(selfs.tooltipItem)
		self.wndCube.SetItemToolTip(selfs.tooltipItem)
		self.wndCubeResult.SetItemToolTip(selfs.tooltipItem)

		if app.ENABLE_SWITCHBOT:
			self.wndSwitchbot.SetItemToolTip(selfs.tooltipItem)

		if app.ENABLE_ITEM_DELETE_SYSTEM:
			self.deleteitem.SetItemToolTip(selfs.tooltipItem)

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.wndAcceCombine.SetItemToolTip(selfs.tooltipItem)
			self.wndAcceAbsorption.SetItemToolTip(selfs.tooltipItem)

		if app.BL_PRIVATESHOP_SEARCH_SYSTEM:
			self.wndPrivateShopSearch.SetItemToolTip(selfs.tooltipItem)
			

		# ITEM_MALL
		self.wndMall.SetItemToolTip(selfs.tooltipItem)
		# END_OF_ITEM_MALL

		self.wndCharacter.SetSkillToolTip(self.tooltipSkill)
		self.wndTaskBar.SetItemToolTip(selfs.tooltipItem)
		self.wndTaskBar.SetSkillToolTip(self.tooltipSkill)
		self.wndGuild.SetSkillToolTip(self.tooltipSkill)

		# ACCESSORY_REFINE_ADD_METIN_STONE
		self.wndItemSelect.SetItemToolTip(selfs.tooltipItem)
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		self.dlgShop.SetItemToolTip(selfs.tooltipItem)
		if app.ENABLE_NEW_BIOLOG:
			self.wndBioWindow.SetItemToolTip(selfs.tooltipItem)
		self.dlgExchange.SetItemToolTip(selfs.tooltipItem)
		self.privateShopBuilder.SetItemToolTip(selfs.tooltipItem)
		if app.ENABLE_NEW_MISSIONS:
			self.wndMissionPanel.SetItemToolTip(selfs.tooltipItem)
		self.__InitWhisper()
		  
		if app.NEW_SALES_SYSTEM:
			self.__MakeSalesWindow()
		  
		  
		self.DRAGON_SOUL_IS_QUALIFIED = False


	if app.LINK_IN_CHAT:
		def AnswerOpenLink(self, answer):
			if not self.OpenLinkQuestionDialog:
				return

			self.OpenLinkQuestionDialog.Close()
			self.OpenLinkQuestionDialog = None

			if not answer:
				return

			link = constInfo.link
			os.system(link)		

	def MakeHyperlinkTooltip(self, hyperlink):
		tokens = hyperlink.split(":")
		if tokens and len(tokens):
			type = tokens[0]
			if "item" == type:
				self.hyperlinkItemTooltip.SetHyperlinkItem(tokens)
			elif "msg" == type and str(tokens[1]) != player.GetMainCharacterName():
				self.OpenWhisperDialog(str(tokens[1]))				
			elif app.LINK_IN_CHAT and "web" == type and tokens[1].startswith("httpXxX") or "web" == type and tokens[1].startswith("httpsXxX"):
					OpenLinkQuestionDialog = uiCommon.QuestionDialog2()
					OpenLinkQuestionDialog.SetText1(localeInfo.CHAT_OPEN_LINK_DANGER)
					OpenLinkQuestionDialog.SetText2(localeInfo.CHAT_OPEN_LINK)
					OpenLinkQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.AnswerOpenLink(arg))
					OpenLinkQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.AnswerOpenLink(arg))
					constInfo.link = "start " + tokens[1].replace("XxX", "://").replace("&","^&")
					OpenLinkQuestionDialog.Open()
					self.OpenLinkQuestionDialog = OpenLinkQuestionDialog
			elif app.LINK_IN_CHAT and "sysweb" == type:
				os.system("start " + tokens[1].replace("XxX", "://"))
			elif "Vectors" == type:  
				self.OpenWhisperDialog(str(tokens[1]))

	## Make Windows & Dialogs
	################################

	def __MakeEventWindow(self):
		self.wndCalendarEvent = ui_activity_day.EventTakvim()
		self.wndCalendarEvent.LoadWindow()
		self.wndCalendarEvent.Hide()

	if app.FATE_ROULETTE:
		def RouletteOpen(self):
			if self.wndFate.IsShow():
				self.wndFate.Hide()	
			else:
				self.wndFate.Open()
				
		def RoulettePrepare(self, info):
			self.wndFate.Prepare(str(info))
			
		def RouletteReset(self):
			self.wndFate.Reset()
			
		def RouletteRun(self, info):
			self.wndFate.Run(str(info))
			
		def RouletteShowIcon(self):
			self.wndFateButton.Show()
			
			

	def Close(self):
		if app.__AUTO_HUNT__:
			if self.wndAutoHunt:
				self.wndAutoHunt.Close()
				self.wndAutoHunt.Destroy()
				self.wndAutoHunt = None
		if app.ENABLE_REWARD_SYSTEM:
			if self.wndReward:
				self.wndReward.Close()
				self.wndReward.Destroy()
				self.wndReward = None

		if self.dlgWhisperWithoutTarget:
			self.dlgWhisperWithoutTarget.Destroy()
			del self.dlgWhisperWithoutTarget

		if uiQuest.QuestDialog.__dict__.has_key("QuestCurtain"):
			uiQuest.QuestDialog.QuestCurtain.Close()

		if self.wndQuestWindow:
			for key, eachQuestWindow in self.wndQuestWindow.items():
				eachQuestWindow.nextCurtainMode = -1
				eachQuestWindow.CloseSelf()
				eachQuestWindow = None
		self.wndQuestWindow = {}

		if self.wndCalendarEvent:
			self.wndCalendarEvent.Destroy()

		if self.wndChat:
			self.wndChat.Destroy()

		if self.wndCharacter:
			self.wndCharacter.Close()
		else:
			if self.wndCharacter:
				self.wndCharacter.Hide()

		if self.ChatKapat:
			self.ChatKapat.Destroy()

		if self.wndTaskBar:
			self.wndTaskBar.Destroy()

		if self.wndBotControl:
			self.wndBotControl.Hide()

		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Destroy()

		#if self.wndEnergyBar:
		#	self.wndEnergyBar.Destroy()

		if app.ENABLE_STREAMER_SYSTEM:
			if self.wndStreamerLink:
				self.wndStreamerLink.Destroy()


		if self.wndCharacter:
			self.wndCharacter.Destroy()

		if selfs.wndInventory: selfs.wndInventory.Destroy(); selfs.wndInventory = None
		if selfs.wndExInventory: selfs.wndExInventory.Destroy(); selfs.wndExInventory = None

		if self.wndDragonSoul:
			self.wndDragonSoul.Destroy()

		if app.BL_REMOTE_SHOP:
			if self.wndRemoteShop:
				del self.wndRemoteShop
				

		if app.__RANKING_SYSTEM__:
			if self.wndRanking:
				self.wndRanking.Hide()
				self.wndRanking.Destroy()
				del self.wndRanking

		if self.wndDragonSoulRefine:
			self.wndDragonSoulRefine.Destroy()

		if self.dlgExchange:
			self.dlgExchange.Destroy()

		if self.dlgPointReset:
			self.dlgPointReset.Destroy()

		if self.dlgShop:
			self.dlgShop.Destroy()

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if selfs.dlgOfflineShop: selfs.dlgOfflineShop.Destroy(); selfs.dlgOfflineShop = None
			if selfs.wndOfflineShopManager: selfs.wndOfflineShopManager.Destroy(); selfs.wndOfflineShopManager = None
			if selfs.wndOpenOfflineShop: selfs.wndOpenOfflineShop.Destroy(); selfs.wndOpenOfflineShop = None

		if self.dlgRestart:
			self.dlgRestart.Destroy()

		if self.dlgSystem:
			self.dlgSystem.Destroy()

		if self.dlgPassword:
			self.dlgPassword.Destroy()

		if self.wndMiniMap:
			self.wndMiniMap.Destroy()

		if self.wndSafebox:
			self.wndSafebox.Destroy()

		if app.NEW_SALES_SYSTEM:
			if self.wndSales:
				self.wndSales.Destroy()
			
		if self.wndSalesButton:
			self.wndSalesButton.Destroy()
			
			

		if app.ENABLE_ITEM_DELETE_SYSTEM:
			if self.deleteitem:
				self.deleteitem.Hide()
				self.deleteitem.Destroy()
				self.deleteitem = None
				del self.deleteitem

		if self.wndWeb:
			self.wndWeb.Destroy()
			self.wndWeb = None

		if self.wndMall:
			self.wndMall.Destroy()

		if app.ENABLE_NEW_MISSIONS:
			if self.wndMissionPanel:
				self.wndMissionPanel.Destroy()

		if self.wndParty:
			self.wndParty.Destroy()

		if self.wndBotControl:
			self.wndBotControl.Destroy()

		if app.ENABLE_CAOS_EVENT:
			if self.wndCaosEvent:
				self.wndCaosEvent.Destroy()

		if self.wndHelp:
			self.wndHelp.Destroy()

		if self.wndCardsInfo:
			self.wndCardsInfo.Destroy()

		if self.wndCards:
			self.wndCards.Destroy()

		if self.wndCardsIcon:
			self.wndCardsIcon.Destroy()

		if app.AUTO_SHOUT:
			if self.wndShout:
				self.wndShout.Destroy()

		if self.wndCube:
			self.wndCube.Destroy()

		if app.ENABLE_NEW_RANKING:
			if self.wndRank:
				self.wndRank.Destroy()

		if app.ENABLE_NEW_BIOLOG:
			if self.wndBioWindow:
				self.wndBioWindow.Destroy()

		if app.ENABLE_ACCE_COSTUME_SYSTEM and self.wndAcceCombine:
			self.wndAcceCombine.Destroy()

		if app.ENABLE_ACCE_COSTUME_SYSTEM and self.wndAcceAbsorption:
			self.wndAcceAbsorption.Destroy()

		if self.wndCubeResult:
			self.wndCubeResult.Destroy()

		if self.wndMessenger:
			self.wndMessenger.Destroy()

		if self.wndGuild:
			self.wndGuild.Destroy()

		if self.privateShopBuilder:
			self.privateShopBuilder.Destroy()

		if self.wndChangerWindow:
			self.wndChangerWindow.Destroy()

		if self.dlgRefineNew:
			self.dlgRefineNew.Destroy()

		if self.wndGuildBuilding:
			self.wndGuildBuilding.Destroy()

		if self.wndGameButton:
			self.wndGameButton.Destroy()
		if app.__BL_CHEST_DROP_INFO__:
			if self.wndChestDropInfo:
				del self.wndChestDropInfo

		# ITEM_MALL
		if self.mallPageDlg:
			self.mallPageDlg.Destroy()
		# END_OF_ITEM_MALL

		if app.ENABLE_AUTOMATIC_ITEM_PROCESS:
			if self.wndDeleteWindow:
				self.wndDeleteWindow.Destroy()
				del self.wndDeleteWindow


		if app.ENABLE_EXTENDED_BATTLE_PASS:
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.Destroy()

		# ACCESSORY_REFINE_ADD_METIN_STONE
		if self.wndItemSelect:
			self.wndItemSelect.Destroy()
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		if app.BL_PRIVATESHOP_SEARCH_SYSTEM:
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch.Destroy()
				del self.wndPrivateShopSearch
				

		if app.ENABLE_SWITCHBOT:
			if self.wndSwitchbot:
				self.wndSwitchbot.Destroy()

		if app.ITEM_SHOP:
			if self.wndNesneMarket:
				self.wndNesneMarket.Destroy()

		if app.ENABLE_EVENT_MANAGER:
			if self.wndEventManager:
				self.wndEventManager.Hide()
				self.wndEventManager.Destroy()
				self.wndEventManager = None
			if self.wndEventIcon:
				self.wndEventIcon.Hide()
				self.wndEventIcon.Destroy()
				self.wndEventIcon = None

		if selfs.tooltipItem: selfs.tooltipItem.Destroy(); selfs.tooltipItem = None

		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			if self.wndCubeRenewal:
				self.wndCubeRenewal.Destroy()
				self.wndCubeRenewal.Close()

		self.wndChatLog.Destroy()
		for btn in self.questButtonList:
			btn.SetEvent(0)
		for btn in self.whisperButtonList:
			btn.SetEvent(0)
		for dlg in self.whisperDialogDict.itervalues():
			dlg.Destroy()
		for brd in self.guildScoreBoardDict.itervalues():
			brd.Destroy()
		for dlg in self.equipmentDialogDict.itervalues():
			dlg.Destroy()

		# ITEM_MALL
		del self.mallPageDlg
		# END_OF_ITEM_MALL

		del self.wndCalendarEvent
		del self.wndGuild
		del self.wndMessenger
		del self.wndUICurtain
		del self.wndChat
		del self.ChatKapat
		del self.wndTaskBar
		if app.ENABLE_CUBE_RENEWAL_WORLDARD:
			del self.wndCubeRenewal
		if self.wndExpandedTaskBar:
			del self.wndExpandedTaskBar
		#del self.wndEnergyBar
		del self.wndCharacter
		if self.wndDragonSoul:
			del self.wndDragonSoul
		if self.wndDragonSoulRefine:
			del self.wndDragonSoulRefine
		del self.dlgExchange
		if app.FATE_ROULETTE:
			del self.wndFate
			del self.wndFateButton
		del self.dlgPointReset
		del self.dlgShop
		del self.dlgRestart
		del self.dlgSystem
		del self.dlgPassword
		del self.hyperlinkItemTooltip
		del self.tooltipItem
		del self.tooltipSkill
		del self.wndMiniMap
		del self.wndSafebox
		del self.wndMall
		del self.wndParty
		del self.wndBotControl
		del self.wndHelp
		del self.wndCardsInfo
		del self.wndCards
		del self.wndCardsIcon
		if app.ENABLE_NEW_MISSIONS:
			del self.wndMissionPanel
		if app.ENABLE_NEW_RANKING:
			del self.wndRank
		if app.AUTO_SHOUT:
			del self.wndShout
		del self.wndCube
		if app.NEW_SALES_SYSTEM:
			del self.wndSales
			del self.wndSalesButton
		if app.ENABLE_NEW_BIOLOG:
			del self.wndBioWindow
		if app.ENABLE_STREAMER_SYSTEM:
			del self.wndStreamerLink
		del self.wndCubeResult
		del self.privateShopBuilder
		del self.inputDialog
		del self.wndChatLog
		del self.dlgRefineNew
		del self.wndGuildBuilding
		del self.wndGameButton
		del self.tipBoard
		del self.bigBoard
		del self.wndItemSelect
		del self.wndChangerWindow

		if app.ENABLE_CAOS_EVENT:
			del self.wndCaosEvent

		if app.ENABLE_SWITCHBOT:
			del self.wndSwitchbot	

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			del self.wndAcceCombine
			del self.wndAcceAbsorption

		if app.ITEM_SHOP:
			del self.wndNesneMarket

		if app.ENABLE_EXTENDED_BATTLE_PASS:
			del self.wndBattlePassExtended

		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}
		self.privateShopAdvertisementBoardDict = {}
		if app.ENABLE_OFFLINE_SHOP_SYSTEM: self.offlineShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict = {}

		uiChat.DestroyChatInputSetWindow()


	def BossTracking(self):
		if self.BossTracking.IsShow():
			self.BossTracking.Hide()
		else:
			self.BossTracking.Open()

	def EventCalendarGame(self):
		if self.wndCalendarEvent.IsShow():
			self.wndCalendarEvent.Hide()
		else:
			self.wndCalendarEvent.Open()

	## Skill
	def OnUseSkill(self, slotIndex, coolTime):
		self.wndCharacter.OnUseSkill(slotIndex, coolTime)
		self.wndTaskBar.OnUseSkill(slotIndex, coolTime)
		self.wndGuild.OnUseSkill(slotIndex, coolTime)

	def OnActivateSkill(self, slotIndex):
		self.wndCharacter.OnActivateSkill(slotIndex)
		self.wndTaskBar.OnActivateSkill(slotIndex)

	def OnDeactivateSkill(self, slotIndex):
		self.wndCharacter.OnDeactivateSkill(slotIndex)
		self.wndTaskBar.OnDeactivateSkill(slotIndex)

	def OnChangeCurrentSkill(self, skillSlotNumber):
		self.wndTaskBar.OnChangeCurrentSkill(skillSlotNumber)

	def SelectMouseButtonEvent(self, dir, event):
		self.wndTaskBar.SelectMouseButtonEvent(dir, event)

	if app.BL_PRIVATESHOP_SEARCH_SYSTEM:
		def OpenPShopSearchDialogCash(self):
			self.wndPrivateShopSearch.Open(1)
		def RefreshPShopSearchDialog(self):
			self.wndPrivateShopSearch.RefreshList()

	## Refresh
	def RefreshAlignment(self):
		self.wndCharacter.RefreshAlignment()

	def RefreshStatus(self):
		self.wndTaskBar.RefreshStatus()
		self.wndCharacter.RefreshStatus()
		selfs.wndInventory.RefreshStatus()
		#if self.wndEnergyBar:
		#	self.wndEnergyBar.RefreshStatus()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.RefreshStatus()

	def RefreshStamina(self):
		self.wndTaskBar.RefreshStamina()

	if app.ENABLE_EXTEND_INVEN_SYSTEM:
		def ExInvenItemUseMsg(self, enough_count):
			selfs.wndInventory.OpenExInvenFallShortCountMsgDlg(enough_count)

	def RefreshSkill(self):
		self.wndCharacter.RefreshSkill()
		self.wndTaskBar.RefreshSkill()

	def RefreshInventory(self):
		self.wndTaskBar.RefreshQuickSlot()
		selfs.wndInventory.RefreshItemSlot()
		selfs.wndExInventory.RefreshBagSlotWindow()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.RefreshItemSlot()

		if constInfo.IS_BONUS_CHANGER:
			self.UpdateBonusChanger()

	def RefreshCharacter(self):
		self.wndCharacter.RefreshCharacter()
		self.wndTaskBar.RefreshQuickSlot()

	def RefreshQuest(self):
		self.wndCharacter.RefreshQuest()

	def RefreshSafebox(self):
		self.wndSafebox.RefreshSafebox()

	# ITEM_MALL
	def RefreshMall(self):
		self.wndMall.RefreshMall()

	def OpenItemMall(self):
		if not self.mallPageDlg:
			self.mallPageDlg = uiShop.MallPageDialog()

		self.mallPageDlg.Open()
	# END_OF_ITEM_MALL

	def RefreshMessenger(self):
		self.wndMessenger.RefreshMessenger()

	def RefreshGuildInfoPage(self):
		self.wndGuild.RefreshGuildInfoPage()

	def RefreshGuildBoardPage(self):
		self.wndGuild.RefreshGuildBoardPage()

	def RefreshGuildMemberPage(self):
		self.wndGuild.RefreshGuildMemberPage()

	def RefreshGuildMemberPageGradeComboBox(self):
		self.wndGuild.RefreshGuildMemberPageGradeComboBox()

	def RefreshGuildSkillPage(self):
		self.wndGuild.RefreshGuildSkillPage()

	def RefreshGuildGradePage(self):
		self.wndGuild.RefreshGuildGradePage()

	def DeleteGuild(self):
		self.wndMessenger.ClearGuildMember()
		self.wndGuild.DeleteGuild()

	def RefreshMobile(self):
		self.dlgSystem.RefreshMobile()

	def OnMobileAuthority(self):
		self.dlgSystem.OnMobileAuthority()

	def OnBlockMode(self, mode):
		self.dlgSystem.OnBlockMode(mode)

	## Calling Functions
	# PointReset
	def OpenPointResetDialog(self):
		self.dlgPointReset.Show()
		self.dlgPointReset.SetTop()

	def ClosePointResetDialog(self):
		self.dlgPointReset.Close()

	# Shop
	def OpenShopDialog(self, vid):
		selfs.wndInventory.Show()
		selfs.wndInventory.SetTop()
		self.dlgShop.Open(vid)
		self.dlgShop.SetTop()

	def CloseShopDialog(self):
		self.dlgShop.Close()

	def RefreshShopDialog(self):
		self.dlgShop.Refresh()

	if app.ENABLE_OFFLINE_SHOP_SYSTEM:
		def GetOpenShopDialog(self, vid):
			if not selfs.dlgOfflineShop: return False
			return selfs.dlgOfflineShop.HaveShopVidOpen(vid)

		def ReloadOfflineShopItemPrice(self, vid, pos, Price):
			if not selfs.dlgOfflineShop: return
			if not self.GetOpenShopDialog(vid): return
			selfs.dlgOfflineShop.ReloadOfflineShopItemPrice(vid, pos, Price)

		def OpenOfflineShopDialog(self, vid):
			if selfs.wndOfflineShopManager.IsShow(): selfs.wndOfflineShopManager.Close()
			selfs.wndInventory.Show()
			selfs.wndInventory.SetTop()
			selfs.dlgOfflineShop.Open(vid)
			selfs.dlgOfflineShop.SetTop()

		def CloseOfflineShopDialog(self):
			selfs.dlgOfflineShop.Close()

		def RefreshOfflineShopDialog(self):
			selfs.dlgOfflineShop.Refresh()

		def RefreshOfflineShopDialogManager(self):
			selfs.wndOfflineShopManager.Refresh()

		def CloseOfflineShopDialogManager(self):
			selfs.wndOfflineShopManager.Close()

		def BINARY_CloseOfflineShop(self):
			if selfs.wndOfflineShopManager:
				import shop; shop.ClearOfflineShopStock()
				selfs.wndOfflineShopManager.Close()

		def BINARY_ChangeOfflineShopName(self, shopName):
			if selfs.wndOfflineShopManager: selfs.wndOfflineShopManager.ChangeOfflineShopName(shopName)

	if app.ENABLE_STREAMER_SYSTEM:
		def StreamerLinkShow(self, arg1, arg2, arg3):
			self.wndStreamerLink.Open(arg1, arg2, arg3)

	## Quest
	def OpenCharacterWindowQuestPage(self):
		self.wndCharacter.Show()
		self.wndCharacter.SetState("QUEST")

	def OpenQuestWindow(self, skin, idx):

		wnds = ()

		q = uiQuest.QuestDialog(skin, idx)
		q.SetWindowName("QuestWindow" + str(idx))
		q.Show()
		if skin:
			q.Lock()
			wnds = self.__HideWindows()

			# UNKNOWN_UPDATE
			q.AddOnDoneEvent(lambda tmp_self, args=wnds: self.__ShowWindows(args))
			# END_OF_UNKNOWN_UPDATE

		if skin:
			q.AddOnCloseEvent(q.Unlock)
		q.AddOnCloseEvent(lambda key = self.wndQuestWindowNewKey:ui.__mem_func__(self.RemoveQuestDialog)(key))
		self.wndQuestWindow[self.wndQuestWindowNewKey] = q

		self.wndQuestWindowNewKey = self.wndQuestWindowNewKey + 1

		# END_OF_UNKNOWN_UPDATE

	def RemoveQuestDialog(self, key):
		del self.wndQuestWindow[key]

	## Exchange
	def StartExchange(self):
		self.dlgExchange.OpenDialog()
		self.dlgExchange.Refresh()

	def EndExchange(self):
		self.dlgExchange.CloseDialog()

	def RefreshExchange(self):
		self.dlgExchange.Refresh()

	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantTradableItemExchange(self, dstSlotIndex, srcSlotIndex):
			self.dlgExchange.CantTradableItem(dstSlotIndex, srcSlotIndex)

	## Party
	def AddPartyMember(self, pid, name):
		self.wndParty.AddPartyMember(pid, name)

		self.__ArrangeQuestButton()

	def UpdatePartyMemberInfo(self, pid):
		self.wndParty.UpdatePartyMemberInfo(pid)

	def RemovePartyMember(self, pid):
		self.wndParty.RemovePartyMember(pid)

		self.__ArrangeQuestButton()

	def LinkPartyMember(self, pid, vid):
		self.wndParty.LinkPartyMember(pid, vid)

	def UnlinkPartyMember(self, pid):
		self.wndParty.UnlinkPartyMember(pid)

	def UnlinkAllPartyMember(self):
		self.wndParty.UnlinkAllPartyMember()

	def ExitParty(self):
		self.wndParty.ExitParty()

		self.__ArrangeQuestButton()

	def PartyHealReady(self):
		self.wndParty.PartyHealReady()

	def ChangePartyParameter(self, distributionMode):
		self.wndParty.ChangePartyParameter(distributionMode)

	## Safebox
	def AskSafeboxPassword(self):
		if self.wndSafebox.IsShow():
			return

		# SAFEBOX_PASSWORD
		self.dlgPassword.SetTitle(localeInfo.PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/safebox_password ")
		# END_OF_SAFEBOX_PASSWORD

		self.dlgPassword.ShowDialog()

	def OpenSafeboxWindow(self, size):
		self.dlgPassword.CloseDialog()
		self.wndSafebox.ShowWindow(size)

	def RefreshSafeboxMoney(self):
		self.wndSafebox.RefreshSafeboxMoney()

	def CommandCloseSafebox(self):
		self.wndSafebox.CommandCloseSafebox()

	# ITEM_MALL
	def AskMallPassword(self):
		if self.wndMall.IsShow():
			return
		self.dlgPassword.SetTitle(localeInfo.MALL_PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/mall_password ")
		self.dlgPassword.ShowDialog()

	def OpenMallWindow(self, size):
		self.dlgPassword.CloseDialog()
		self.wndMall.ShowWindow(size)

	def CommandCloseMall(self):
		self.wndMall.CommandCloseMall()
	# END_OF_ITEM_MALL

	## Guild
	def OnStartGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnStartGuildWar(guildSelf, guildOpp)

		guildWarScoreBoard = uiGuild.GuildWarScoreBoard()
		guildWarScoreBoard.Open(guildSelf, guildOpp)
		guildWarScoreBoard.Show()
		self.guildScoreBoardDict[uiGuild.GetGVGKey(guildSelf, guildOpp)] = guildWarScoreBoard

	def OnEndGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnEndGuildWar(guildSelf, guildOpp)

		key = uiGuild.GetGVGKey(guildSelf, guildOpp)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].Destroy()
		del self.guildScoreBoardDict[key]

	# GUILDWAR_MEMBER_COUNT
	def UpdateMemberCount(self, gulidID1, memberCount1, guildID2, memberCount2):
		key = uiGuild.GetGVGKey(gulidID1, guildID2)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].UpdateMemberCount(gulidID1, memberCount1, guildID2, memberCount2)
	# END_OF_GUILDWAR_MEMBER_COUNT

	def OnRecvGuildWarPoint(self, gainGuildID, opponentGuildID, point):
		key = uiGuild.GetGVGKey(gainGuildID, opponentGuildID)
		if not self.guildScoreBoardDict.has_key(key):
			return

		guildBoard = self.guildScoreBoardDict[key]
		guildBoard.SetScore(gainGuildID, opponentGuildID, point)

	## PK Mode
	def OnChangePKMode(self):
		self.wndCharacter.RefreshAlignment()
		self.dlgSystem.OnChangePKMode()

	## Refine
	def OpenRefineDialog(self, targetItemPos, nextGradeItemVnum, cost, prob, type):
		self.dlgRefineNew.Open(targetItemPos, nextGradeItemVnum, cost, prob, type)

	def AppendMaterialToRefineDialog(self, vnum, count):
		self.dlgRefineNew.AppendMaterial(vnum, count)


	if app.ENABLE_REFINE_RENEWAL:
		def CheckRefineDialog(self, isFail):
			self.dlgRefineNew.CheckRefine(isFail)

	## Show & Hide
	def ShowDefaultWindows(self):
		self.wndTaskBar.Show()
		self.wndMiniMap.Show()
		self.wndMiniMap.ShowMiniMap()
		#if self.wndEnergyBar:
		#	self.wndEnergyBar.Show()

	def ShowAllWindows(self):
		self.wndTaskBar.Show()
		self.wndCharacter.Show()
		selfs.wndInventory.Show()
		selfs.wndExInventory.Show()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.Show()
			self.wndDragonSoulRefine.Show()
		self.wndChat.Show()
		self.ChatKapat.Show()
		self.wndMiniMap.Show()
		#if self.wndEnergyBar:
		#	self.wndEnergyBar.Show()
		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Show()
			self.wndExpandedTaskBar.SetTop()

	if app.ENABLE_NEW_BIOLOG:
		def OpenBiologWindow(self):
			if self.wndBioWindow.IsShow():
				self.wndBioWindow.Close()
			else:
				self.wndBioWindow.Open()

		def CloseBiologWindow(self):
			if self.wndBioWindow.IsShow():
				self.wndBioWindow.Close()

		def UpdateBiologInfo(self, needitem, soulitem, givecount, state, reqcount, aff_type, aff_value, aff_type2, aff_value2, aff_type3, aff_value3, aff_type4, aff_value4, chance, time):
			if self.wndBioWindow:
				self.wndBioWindow.UpdateInfo(needitem, soulitem, givecount, state, reqcount, aff_type, aff_value, aff_type2, aff_value2, aff_type3, aff_value3, aff_type4, aff_value4, chance, time)


	def HideAllWindows(self):
		if self.wndTaskBar:
			self.wndTaskBar.Hide()

		if app.ENABLE_AUTOMATIC_ITEM_PROCESS and self.wndDeleteWindow:
			self.wndDeleteWindow.Hide()

		if app.ENABLE_NEW_MISSIONS:
			if self.wndMissionPanel:
				self.wndMissionPanel.Hide()

		#if self.wndEnergyBar:
		#	self.wndEnergyBar.Hide()

		if app.ENABLE_DETAILS_UI:
			if self.wndCharacter:
				self.wndCharacter.Close()
		else:
			if self.wndCharacter:
				self.wndCharacter.Hide()

		if app.ENABLE_NEW_BIOLOG:
			if self.wndBioWindow:
				self.wndBioWindow.Hide()

		if app.ENABLE_NEW_RANKING:
			if self.wndRank:
				self.wndRank.Hide()

		if selfs.wndInventory:
			selfs.wndInventory.Hide()

		if selfs.wndExInventory:
			selfs.wndExInventory.Hide()

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.Hide()
			self.wndDragonSoulRefine.Hide()

		if app.ENABLE_SWITCHBOT:
			if self.wndSwitchbot:
				self.wndSwitchbot.Hide()

		if self.wndChat:
			self.wndChat.Hide()

		if self.ChatKapat:
			self.ChatKapat.Hide()

		if self.wndMiniMap:
			self.wndMiniMap.Hide()

		if self.wndMessenger:
			self.wndMessenger.Hide()

		if self.wndGuild:
			self.wndGuild.Hide()

		if app.ENABLE_EXTENDED_BATTLE_PASS:
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.Hide()

		if app.ENABLE_CAOS_EVENT:
			if self.wndCaosEvent:
				self.wndCaosEvent.Hide()

		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Hide()

		if app.__BL_CHEST_DROP_INFO__:
			if self.wndChestDropInfo:
				self.wndChestDropInfo.Hide()

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if selfs.wndOfflineShopManager:
				selfs.wndOfflineShopManager.Hide()

			if selfs.wndOpenOfflineShop:
				selfs.wndOpenOfflineShop.Hide()

		if app.ITEM_SHOP:
			if self.wndNesneMarket:
				self.wndNesneMarket.Hide()

	def ShowMouseImage(self):
		self.wndTaskBar.ShowMouseImage()

	def HideMouseImage(self):
		self.wndTaskBar.HideMouseImage()

	def ToggleChat(self):
		if True == self.wndChat.IsEditMode():
			self.wndChat.CloseChat()
		else:
			if self.wndWeb and self.wndWeb.IsShow():
				pass
			else:
				self.wndChat.OpenChat()

	def IsOpenChat(self):
		return self.wndChat.IsEditMode()

	def SetChatFocus(self):
		self.wndChat.SetChatFocus()

	def OpenRestartDialog(self):
		self.dlgRestart.OpenDialog()
		self.dlgRestart.SetTop()

	def CloseRestartDialog(self):
		self.dlgRestart.Close()

	def ToggleSystemDialog(self):
		if False == self.dlgSystem.IsShow():
			self.dlgSystem.OpenDialog()
			self.dlgSystem.SetTop()
		else:
			self.dlgSystem.Close()

	def OpenSystemDialog(self):
		self.dlgSystem.OpenDialog()
		self.dlgSystem.SetTop()

	def ToggleMessenger(self):
		if self.wndMessenger.IsShow():
			self.wndMessenger.Hide()
		else:
			self.wndMessenger.SetTop()
			self.wndMessenger.Show()

	def ToggleMiniMap(self):
		if app.IsPressed(app.DIK_LSHIFT) or app.IsPressed(app.DIK_RSHIFT):
			if False == self.wndMiniMap.isShowMiniMap():
				self.wndMiniMap.ShowMiniMap()
				self.wndMiniMap.SetTop()
			else:
				self.wndMiniMap.HideMiniMap()

		else:
			self.wndMiniMap.ToggleAtlasWindow()

	def PressMKey(self):
		if app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT):
			self.ToggleMessenger()

		else:
			self.ToggleMiniMap()

	def SetMapName(self, mapName):
		self.wndMiniMap.SetMapName(mapName)

	def MiniMapScaleUp(self):
		self.wndMiniMap.ScaleUp()

	def MiniMapScaleDown(self):
		self.wndMiniMap.ScaleDown()

	def ToggleCharacterWindow(self, state):
		if False == player.IsObserverMode():
			if False == self.wndCharacter.IsShow():
				self.OpenCharacterWindowWithState(state)
			else:
				if state == self.wndCharacter.GetState():
					self.wndCharacter.OverOutItem()
					if app.ENABLE_DETAILS_UI:
						self.wndCharacter.Close()
					else:
						self.wndCharacter.Hide()
						self.wndCharacter.Close()
				else:
					self.wndCharacter.SetState(state)

	def OpenCharacterWindowWithState(self, state):
		if False == player.IsObserverMode():
			self.wndCharacter.SetState(state)
			self.wndCharacter.Show()
			self.wndCharacter.SetTop()

	def ToggleCharacterWindowStatusPage(self):
		self.ToggleCharacterWindow("STATUS")

	def ToggleInventoryWindow(self):
		#if self.State == "Kapali":
		#	chat.AppendChat(chat.CHAT_TYPE_INFO, "Envanteri acabilmek icin " + str(int(int(self.LastContactTimeStamp) + self.WaitTime) - int(app.GetTime())) + " saniye beklemelisin.")
		#	return	
		#else:
		toggleExInventory = constInfo.exInventory_isToggle
		if FALSE == player.IsObserverMode():
			if FALSE == selfs.wndInventory.IsShow():
				selfs.wndInventory.Show()
				selfs.wndInventory.SetTop()
				if False == selfs.wndExInventory.IsShow() and toggleExInventory == True: selfs.wndExInventory.Show(); selfs.wndExInventory.SetTop()
			else:
				selfs.wndInventory.OverOutItem()
				selfs.wndInventory.Close()
				if toggleExInventory == True: selfs.wndExInventory.OverOutItem(); selfs.wndExInventory.Close()

	def ToggleExInventoryWindow(self):
	#	if self.State == "Kapali":
	#		chat.AppendChat(chat.CHAT_TYPE_INFO, "Envanteri acabilmek icin " + str(int(int(self.LastContactTimeStamp) + self.WaitTime) - int(app.GetTime())) + " saniye beklemelisin.")
	#		return	
	#	else:
		if selfs.wndExInventory:
			if False == selfs.wndExInventory.IsShow(): selfs.wndExInventory.Show(); selfs.wndExInventory.SetTop()
			else: selfs.wndExInventory.OverOutItem(); selfs.wndExInventory.Close()


	if app.ENABLE_EXTENDED_BATTLE_PASS:
		def ReciveOpenExtBattlePass(self):
			if False == self.isFirstOpeningExtBattlePass:
				self.isFirstOpeningExtBattlePass = True
				self.wndBattlePassExtended.SetPage("NORMAL")
			if False == self.wndBattlePassExtended.IsShow():
				self.ToggleBattlePassExtended()
			else:
				self.wndBattlePassExtended.SetPage(self.wndBattlePassExtended.GetPage())

		def ToggleBattlePassExtended(self):
			if False == self.isFirstOpeningExtBattlePass:
				net.SendExtBattlePassAction(1)
			if False == self.wndBattlePassExtended.IsShow():
				self.wndBattlePassExtended.Show()
				self.wndBattlePassExtended.SetTop()
			else:
				self.wndBattlePassExtended.Close()
		
		def AddExtendedBattleGeneralInfo(self, BattlePassType, BattlePassName, BattlePassID, battlePassStartTime, battlePassEndTime):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.RecvGeneralInfo(BattlePassType, BattlePassName, BattlePassID, battlePassStartTime, battlePassEndTime)
		
		def AddExtendedBattlePassMission(self, battlepassType, battlepassID, missionIndex, missionType, missionInfo1, missionInfo2, missionInfo3):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.AddMission(battlepassType, battlepassID, missionIndex, missionType, missionInfo1, missionInfo2, missionInfo3)

		def UpdateExtendedBattlePassMission(self, battlepassType, missionIndex, missionType, newProgress):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.UpdateMission(battlepassType, missionIndex, missionType, newProgress)

		def AddExtendedBattlePassMissionReward(self, battlepassType, battlepassID, missionIndex, missionType, itemVnum, itemCount):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.AddMissionReward(battlepassType, battlepassID, missionIndex, missionType, itemVnum, itemCount)

		def AddExtendedBattlePassReward(self, battlepassType, battlepassID, itemVnum, itemCount):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.AddReward(battlepassType, battlepassID, itemVnum, itemCount)

		def AddExtBattlePassRanklistEntry(self, playername, battlepassType, battlepassID, startTime, endTime):
			if self.wndBattlePassExtended:
				self.wndBattlePassExtended.AddRankingEntry(playername, battlepassType, battlepassID, startTime, endTime)


	def ToggleExpandedButton(self):
		if False == player.IsObserverMode():
			if False == self.wndExpandedTaskBar.IsShow():
				self.wndExpandedTaskBar.Show()
				self.wndExpandedTaskBar.SetTop()
			else:
				self.wndExpandedTaskBar.Close()

	def DragonSoulActivate(self, deck):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.ActivateDragonSoulByExtern(deck)

	def DragonSoulDeactivate(self):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.DeactivateDragonSoul()

	def Highligt_Item(self, inven_type, inven_pos):
		if player.DRAGON_SOUL_INVENTORY == inven_type:
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				self.wndDragonSoul.HighlightSlot(inven_pos)

		elif app.ENABLE_HIGHLIGHT_NEW_ITEM and player.SLOT_TYPE_INVENTORY == inven_type:
			selfs.wndInventory.HighlightSlot(inven_pos)

	def ToggleGuildWindow(self):
		if not self.wndGuild.IsShow():
			if self.wndGuild.CanOpen():
				self.wndGuild.Open()
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GUILD_YOU_DO_NOT_JOIN)
		else:
			self.wndGuild.OverOutItem()
			self.wndGuild.Hide()

	if app.ENABLE_NEW_MISSIONS:
		def OpenMissionPanel(self):
			if False == player.IsObserverMode():
				if False == self.wndMissionPanel.IsShow():
					self.wndMissionPanel.Open()
				else:
					self.wndMissionPanel.Close()

		def AppendMissionInfos(self, idx, desc, needcount):
			if self.wndMissionPanel:
				self.wndMissionPanel.AppendMissionInfos(idx, desc, needcount)

		def AppendGlobalMissionInfos(self, idx, desc, winner, vnum, count):
			if self.wndMissionPanel:
				self.wndMissionPanel.AppendGlobalMissionInfos(idx, desc, winner, vnum, count)

		def UpdateMissionItem(self, idx, vnum, count):
			if self.wndMissionPanel:
				self.wndMissionPanel.UpdateMissionItem(idx, vnum, count)

		def MyMissions(self, idx, val):
			if self.wndMissionPanel:
				self.wndMissionPanel.MyMissions(idx, val)

		def AppendMissions(self):
			if self.wndMissionPanel:
				self.wndMissionPanel.AppenMissions()

	def ToggleChatLogWindow(self):
		if self.wndChatLog.IsShow():
			self.wndChatLog.Hide()
		else:
			self.wndChatLog.Show()

	if app.ENABLE_OFFLINE_SHOP_SYSTEM:
		def ToggleOfflineShopAdminPanelWindow(self, isOpen, mapIndex, shopChannel, time, isPremium, displayedCount, shopName):
			if selfs.wndOpenOfflineShop.IsShow() == True: return
			if selfs.dlgOfflineShop.IsShow(): selfs.dlgOfflineShop.Close()
			if not selfs.offlineShopBuilder.IsShow():
				if selfs.wndOfflineShopManager.IsShow() == True:
					selfs.wndOfflineShopManager.Close()
				else:
					if selfs.offlineShopBuilder.IsBuilding() == False:
						selfs.wndOfflineShopManager.Open(isOpen, mapIndex, shopChannel, time, isPremium, displayedCount, shopName)

	if app.ENABLE_SWITCHBOT:
		def ToggleSwitchbotWindow(self):
			if self.wndSwitchbot.IsShow():
				self.wndSwitchbot.Close()
			else:
				self.wndSwitchbot.Open()
				
		def RefreshSwitchbotWindow(self):
			if self.wndSwitchbot and self.wndSwitchbot.IsShow():
				self.wndSwitchbot.RefreshSwitchbotWindow()

		def RefreshSwitchbotItem(self, slot):
			if self.wndSwitchbot and self.wndSwitchbot.IsShow():
				self.wndSwitchbot.RefreshSwitchbotItem(slot)
				

	if app.ENABLE_NEW_RANKING:
		def OpenRank(self):
			if self.wndRank.IsShow():
				self.wndRank.Close()
			else:
				self.wndRank.Open()


	def CheckGameButton(self):
		if self.wndGameButton:
			self.wndGameButton.CheckGameButton()

	def __OnClickStatusPlusButton(self):
		self.ToggleCharacterWindow("STATUS")

	def __OnClickSkillPlusButton(self):
		self.ToggleCharacterWindow("SKILL")

	def __OnClickQuestButton(self):
		self.ToggleCharacterWindow("QUEST")

	def __OnClickHelpButton(self):
		player.SetPlayTime(1)
		self.CheckGameButton()
		self.OpenHelpWindow()

	def __OnClickBuildButton(self):
		self.BUILD_OpenWindow()

	def OpenHelpWindow(self):
		self.wndUICurtain.Show()
		self.wndHelp.Open()

	def CloseHelpWindow(self):
		self.wndUICurtain.Hide()
		self.wndHelp.Close()

	def OpenWebWindow(self, url):
		self.wndWeb.Open(url)

		self.wndChat.CloseChat()

	# show GIFT
	def ShowGift(self):
		self.wndTaskBar.ShowGift()

	if app.ENABLE_CUBE_RENEWAL_WORLDARD:
		def BINARY_CUBE_RENEWAL_OPEN(self):
			self.wndCubeRenewal.Show()

	def CloseWbWindow(self):
		self.wndWeb.Close()

	def OpenCardsInfoWindow(self):
		self.wndCardsInfo.Open()
		
	def OpenCardsWindow(self, safemode):
		self.wndCards.Open(safemode)
		
	def UpdateCardsInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points):
		self.wndCards.UpdateCardsInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points)
		
	def UpdateCardsFieldInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
		self.wndCards.UpdateCardsFieldInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)
		
	def CardsPutReward(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
		self.wndCards.CardsPutReward(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)
		
	def CardsShowIcon(self):
		self.wndCardsIcon.Show()

	def Event1ShowIcon(self):
		self.wndEvent1Show.Show()

	def Event2ShowIcon(self):
		self.wndEvent2Show.Show()

	def Event3ShowIcon(self):
		self.wndEvent3Show.Show()

	def Event4ShowIcon(self):
		self.wndEvent4Show.Show()

	def Event5ShowIcon(self):
		self.wndEvent5Show.Show()

	def Event6ShowIcon(self):
		self.wndEvent6Show.Show()

	if app.AUTO_SHOUT:
		def OpenShoutWindow(self):
			if self.wndShout.IsShow():
				self.wndShout.Hide()
			else:
				self.wndShout.Open()

	if app.NEW_SALES_SYSTEM:
		def OpenSalesWindow(self):
			if self.wndSales.IsShow():
				self.wndSales.Close()
			else:
				# net.SendChatPacket("/sales_list")
				self.wndSales.Open()
				
		def SalesShowIcon(self):
			self.wndSalesButton.Show()

	def OpenCubeWindow(self):
		self.wndCube.Open()

		if False == selfs.wndInventory.IsShow():
			selfs.wndInventory.Show()

	def UpdateCubeInfo(self, gold, itemVnum, count):
		self.wndCube.UpdateInfo(gold, itemVnum, count)

	def CloseCubeWindow(self):
		self.wndCube.Close()

	def FailedCubeWork(self):
		self.wndCube.Refresh()

	if app.ENABLE_AUTOMATIC_ITEM_PROCESS:
		def OpenItemDeleteWindow(self):
			if self.wndDeleteWindow.IsShow():
				self.wndDeleteWindow.Close()
			else:
				self.wndDeleteWindow.Open()

		def IsItemDeleteWindowOpen(self):
			if self.wndDeleteWindow.IsShow():
				return True
			return False

	def SucceedCubeWork(self, itemVnum, count):
		self.wndCube.Clear()

		print "Å¥ºê Á¦ÀÛ ¼º°ø! [%d:%d]" % (itemVnum, count)

		if 0:
			self.wndCubeResult.SetPosition(*self.wndCube.GetGlobalPosition())
			self.wndCubeResult.SetCubeResultItem(itemVnum, count)
			self.wndCubeResult.Open()
			self.wndCubeResult.SetTop()

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def ActAcce(self, iAct, bWindow):
			board = (self.wndAcceAbsorption,self.wndAcceCombine)[int(bWindow)]
			if iAct == 1:
				self.ActAcceOpen(board)
			elif iAct == 2:
				self.ActAcceClose(board)
			elif iAct == 3 or iAct == 4:
				self.ActAcceRefresh(board, iAct)

		def ActAcceOpen(self,board):
			if not board.IsOpened():
				board.Open()
			if not selfs.wndInventory.IsShow():
				selfs.wndInventory.Show()
			selfs.wndInventory.RefreshBagSlotWindow()
			selfs.wndExInventory.RefreshBagSlotWindow()

		def ActAcceClose(self,board):
			if board.IsOpened():
				board.Close()
			selfs.wndInventory.RefreshBagSlotWindow()
			selfs.wndExInventory.RefreshBagSlotWindow()

		def ActAcceRefresh(self,board,iAct):
			if board.IsOpened():
				board.Refresh(iAct)
			selfs.wndInventory.RefreshBagSlotWindow()
			selfs.wndExInventory.RefreshBagSlotWindow()

	def __HideWindows(self):
		hideWindows = self.wndTaskBar,\
						self.wndCharacter,\
						selfs.wndInventory,\
						selfs.wndExInventory,\
						self.wndMiniMap,\
						self.wndGuild,\
						self.wndMessenger,\
						self.wndChat,\
						self.wndParty,\
						self.ChatKapat,\
						self.wndGameButton,

		#if self.wndEnergyBar:
		#	hideWindows += self.wndEnergyBar,

		if self.wndExpandedTaskBar:
			hideWindows += self.wndExpandedTaskBar,

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			hideWindows += self.wndDragonSoul,\
						self.wndDragonSoulRefine,

		if app.ENABLE_SWITCHBOT and self.wndSwitchbot:
			hideWindows += self.wndSwitchbot,

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if selfs.wndOfflineShopManager:
				hideWindows += selfs.wndOfflineShopManager,

			if selfs.wndOpenOfflineShop:
				hideWindows += selfs.wndOpenOfflineShop,

		if app.ENABLE_EXTENDED_BATTLE_PASS:
			if self.wndBattlePassExtended:
				hideWindows += self.wndBattlePassExtended,


		hideWindows = filter(lambda x:x.IsShow(), hideWindows)
		map(lambda x:x.Hide(), hideWindows)

		self.HideAllQuestButton()
		self.HideAllWhisperButton()

		if self.wndChat.IsEditMode():
			self.wndChat.CloseChat()

		return hideWindows

	def __ShowWindows(self, wnds):
		map(lambda x:x.Show(), wnds)
		global IsQBHide
		if not IsQBHide:
			self.ShowAllQuestButton()
		else:
			self.HideAllQuestButton()

		self.ShowAllWhisperButton()

	def BINARY_OpenAtlasWindow(self):
		if self.wndMiniMap:
			self.wndMiniMap.ShowAtlas()

	def BINARY_SetObserverMode(self, flag):
		self.wndGameButton.SetObserverMode(flag)

	# ACCESSORY_REFINE_ADD_METIN_STONE
	def BINARY_OpenSelectItemWindow(self):
		self.wndItemSelect.Open()
	# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

	#####################################################################################
	### Private Shop ###

	def OpenPrivateShopInputNameDialog(self):
		#if player.IsInSafeArea():
		#	chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CANNOT_OPEN_PRIVATE_SHOP_IN_SAFE_AREA)
		#	return

		inputDialog = uiCommon.InputDialog()
		inputDialog.SetTitle(localeInfo.PRIVATE_SHOP_INPUT_NAME_DIALOG_TITLE)
		inputDialog.SetMaxLength(32)
		inputDialog.SetAcceptEvent(ui.__mem_func__(self.OpenPrivateShopBuilder))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.ClosePrivateShopInputNameDialog))
		inputDialog.Open()
		self.inputDialog = inputDialog

	def ClosePrivateShopInputNameDialog(self):
		self.inputDialog = None
		return True

	def OpenPrivateShopBuilder(self):

		if not self.inputDialog:
			return True

		if not len(self.inputDialog.GetText()):
			return True

		self.privateShopBuilder.Open(self.inputDialog.GetText())
		self.ClosePrivateShopInputNameDialog()
		return True

	def AppearPrivateShop(self, vid, text):

		board = uiPrivateShopBuilder.PrivateShopAdvertisementBoard()
		board.Open(vid, text)

		self.privateShopAdvertisementBoardDict[vid] = board

	#BONUS CHANGER
	def UpdateBonusChanger(self):
		if self.wndChangerWindow:
			self.wndChangerWindow.OnUpdate()
	
	def AddToBonusChange(self, item1, item2):
		if self.wndChangerWindow:
			self.wndChangerWindow.AddItems(item1, item2)
	#END OF BONUS CHANGER
	

	def DisappearPrivateShop(self, vid):

		if not self.privateShopAdvertisementBoardDict.has_key(vid):
			return

		del self.privateShopAdvertisementBoardDict[vid]
		uiPrivateShopBuilder.DeleteADBoard(vid)

	if app.ENABLE_OFFLINE_SHOP_SYSTEM:
		#####################################################################################
		### Offline Shop ###

		def OpenOfflineShopSalesWindow(self):
			if selfs.wndOfflineShopManager.IsShow():
				selfs.wndOfflineShopManager.RefreshSalesWindow()

		def OpenOfflineShopCreateDialog(self):
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
			selfs.wndOfflineShopManager.Close()
			selfs.wndOpenOfflineShop.Open()

		def CloseOfflineShopCreateDialog(self):
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)
			selfs.wndOpenOfflineShop.Hide()

		def OpenOfflineShopBuilder(self, shopName, npcVnum, borderStyle):
			selfs.offlineShopBuilder.Open(shopName, npcVnum, borderStyle) # arg2.style
			self.CloseOfflineShopCreateDialog()
			return True

		def AppearOfflineShop(self, vid, text, borderStyle):
			real_bord_style = borderStyle
			if borderStyle < 0 or borderStyle > 5: real_bord_style = 0
			base = ["", "fire", "ice", "paper", "ribon", "wing"]
			board = uiOfflineShopBuilder.OfflineShopAdvertisementBoard(base[borderStyle])
			board.Open(vid, text)
			self.offlineShopAdvertisementBoardDict[vid] = board

		def ChangeOfflineShopSign(self, vid, text):
			if self.offlineShopAdvertisementBoardDict.has_key(vid):
				uiOfflineShopBuilder.UpdateADTextBoard(vid, text)
				return

		def DisappearOfflineShop(self, vid):
			if not self.offlineShopAdvertisementBoardDict.has_key(vid): return
			del self.offlineShopAdvertisementBoardDict[vid]
			uiOfflineShopBuilder.DeleteADBoard(vid)

	#####################################################################################
	### Equipment ###

	def OpenEquipmentDialog(self, vid):
		dlg = uiEquipmentDialog.EquipmentDialog()
		dlg.SetItemToolTip(selfs.tooltipItem)
		dlg.SetCloseEvent(ui.__mem_func__(self.CloseEquipmentDialog))
		dlg.Open(vid)

		self.equipmentDialogDict[vid] = dlg

	def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogItem(slotIndex, vnum, count)

	def SetEquipmentDialogSocket(self, vid, slotIndex, socketIndex, value):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogSocket(slotIndex, socketIndex, value)

	def SetEquipmentDialogAttr(self, vid, slotIndex, attrIndex, type, value):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogAttr(slotIndex, attrIndex, type, value)

	def CloseEquipmentDialog(self, vid):
		if not vid in self.equipmentDialogDict:
			return
		del self.equipmentDialogDict[vid]

	#####################################################################################

	#####################################################################################
	### Quest ###
	def BINARY_ClearQuest(self, index):
		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)

	def RecvQuest(self, index, name):
		# QUEST_LETTER_IMAGE
		self.BINARY_RecvQuest(index, name, "file", localeInfo.GetLetterImageName())
		# END_OF_QUEST_LETTER_IMAGE

	def BINARY_RecvQuest(self, index, name, iconType, iconName):

		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)

		btn = uiWhisper.WhisperButton()

		# QUEST_LETTER_IMAGE
		import item
		if "item"==iconType:
			item.SelectItem(int(iconName))
			buttonImageFileName=item.GetIconImageFileName()
		else:
			buttonImageFileName=iconName

		if iconName and (iconType not in ("item", "file")): # type "ex" implied
			btn.SetUpVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName.replace("open", "close")))
			btn.SetOverVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName))
			btn.SetDownVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName))
		else:
			if localeInfo.IsEUROPE():
				btn.SetUpVisual(localeInfo.GetLetterCloseImageName())
				btn.SetOverVisual(localeInfo.GetLetterOpenImageName())
				btn.SetDownVisual(localeInfo.GetLetterOpenImageName())
			else:
				btn.SetUpVisual(buttonImageFileName)
				btn.SetOverVisual(buttonImageFileName)
				btn.SetDownVisual(buttonImageFileName)
				btn.Flash()
		# END_OF_QUEST_LETTER_IMAGE

		if localeInfo.IsARABIC():
			btn.SetToolTipText(name, 0, 35)
			btn.ToolTipText.SetHorizontalAlignCenter()
		else:
			btn.SetToolTipText(name, -20, 35)
			btn.ToolTipText.SetHorizontalAlignLeft()

		listOfTypes = iconType.split(",")
		if "blink" in listOfTypes:
			btn.Flash()

		listOfColors = {
			"golden":	0xFFffa200,
			"green":	0xFF00e600,
			"blue":		0xFF0099ff,
			"purple":	0xFFcc33ff,

			"fucsia":	0xFFcc0099,
			"aqua":		0xFF00ffff,
		}
		for k,v in listOfColors.iteritems():
			if k in listOfTypes:
				btn.ToolTipText.SetPackedFontColor(v)

		btn.SetEvent(ui.__mem_func__(self.__StartQuest), btn)
		btn.Show()

		btn.index = index
		btn.name = name

		self.questButtonList.insert(0, btn)
		self.__ArrangeQuestButton()

	def __ArrangeQuestButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		if self.wndParty.IsShow():
			xPos = 100 + 30
		else:
			xPos = 20

		if localeInfo.IsARABIC():
			xPos = xPos + 15

		yPos = 170 * screenHeight / 600
		yCount = (screenHeight - 330) / 63

		count = 0
		for btn in self.questButtonList:

			btn.SetPosition(xPos + (int(count/yCount) * 100), yPos + (count%yCount * 63))
			count += 1
			global IsQBHide
			if IsQBHide:
				btn.Hide()
			else:
				btn.Show()

	def __StartQuest(self, btn):
		event.QuestButtonClick(btn.index)
		self.__DestroyQuestButton(btn)

	def __FindQuestButton(self, index):
		for btn in self.questButtonList:
			if btn.index == index:
				return btn

		return 0

	def __DestroyQuestButton(self, btn):
		btn.SetEvent(0)
		self.questButtonList.remove(btn)
		self.__ArrangeQuestButton()

	def HideAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Hide()

	def ShowAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Show()
	#####################################################################################

	#####################################################################################
	### Whisper ###

	def __InitWhisper(self):
		chat.InitWhisper(self)

	def OpenWhisperDialogWithoutTarget(self):
		if not self.dlgWhisperWithoutTarget:
			dlgWhisper = uiWhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
			dlgWhisper.BindInterface(self)
			dlgWhisper.LoadDialog()
			dlgWhisper.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)
			dlgWhisper.SetPosition(self.windowOpenPosition*30,self.windowOpenPosition*30)
			dlgWhisper.Show()
			self.dlgWhisperWithoutTarget = dlgWhisper

			self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		else:
			self.dlgWhisperWithoutTarget.SetTop()
			self.dlgWhisperWithoutTarget.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)

	def RegisterTemporaryWhisperDialog(self, name):
		if not self.dlgWhisperWithoutTarget:
			return

		btn = self.__FindWhisperButton(name)
		if 0 != btn:
			self.__DestroyWhisperButton(btn)

		elif self.whisperDialogDict.has_key(name):
			oldDialog = self.whisperDialogDict[name]
			oldDialog.Destroy()
			del self.whisperDialogDict[name]

		self.whisperDialogDict[name] = self.dlgWhisperWithoutTarget
		self.dlgWhisperWithoutTarget.OpenWithTarget(name)
		self.dlgWhisperWithoutTarget = None
		self.__CheckGameMaster(name)

	def OpenWhisperDialog(self, name):
		if not self.whisperDialogDict.has_key(name):
			dlg = self.__MakeWhisperDialog(name)
			dlg.OpenWithTarget(name)
			dlg.chatLine.SetFocus()
			dlg.Show()

			self.__CheckGameMaster(name)
			btn = self.__FindWhisperButton(name)
			if 0 != btn:
				self.__DestroyWhisperButton(btn)

	def RecvWhisper(self, name):
		if not self.whisperDialogDict.has_key(name):
			btn = self.__FindWhisperButton(name)
			if 0 == btn:
				btn = self.__MakeWhisperButton(name)
				btn.Flash()

				chat.AppendChat(chat.CHAT_TYPE_NOTICE, localeInfo.RECEIVE_MESSAGE % (name))

			else:
				btn.Flash()
		elif self.IsGameMasterName(name):
			dlg = self.whisperDialogDict[name]
			dlg.SetGameMasterLook()

	def MakeWhisperButton(self, name):
		self.__MakeWhisperButton(name)

	if app.BL_REMOTE_SHOP:
		def OpenRemoteShop(self):
			if self.wndRemoteShop:
				if self.wndRemoteShop.IsShowWindow():				
					self.wndRemoteShop.Close()
				else:
					self.wndRemoteShop.Show()

	def ShowWhisperDialog(self, btn):
		try:
			self.__MakeWhisperDialog(btn.name)
			dlgWhisper = self.whisperDialogDict[btn.name]
			dlgWhisper.OpenWithTarget(btn.name)
			dlgWhisper.Show()
			self.__CheckGameMaster(btn.name)
		except:
			import dbg
			dbg.TraceError("interface.ShowWhisperDialog - Failed to find key")

		self.__DestroyWhisperButton(btn)

	def MinimizeWhisperDialog(self, name):

		if 0 != name:
			self.__MakeWhisperButton(name)

		self.CloseWhisperDialog(name)

	def CloseWhisperDialog(self, name):

		if 0 == name:

			if self.dlgWhisperWithoutTarget:
				self.dlgWhisperWithoutTarget.Destroy()
				self.dlgWhisperWithoutTarget = None

			return

		try:
			dlgWhisper = self.whisperDialogDict[name]
			dlgWhisper.Destroy()
			del self.whisperDialogDict[name]
		except:
			import dbg
			dbg.TraceError("interface.CloseWhisperDialog - Failed to find key")

	def __ArrangeWhisperButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		xPos = screenWidth - 70
		yPos = 170 * screenHeight / 600
		yCount = (screenHeight - 330) / 63
		#yCount = (screenHeight - 285) / 63

		count = 0
		for button in self.whisperButtonList:

			button.SetPosition(xPos + (int(count/yCount) * -50), yPos + (count%yCount * 63))
			count += 1

	def __FindWhisperButton(self, name):
		for button in self.whisperButtonList:
			if button.name == name:
				return button

		return 0

	def __MakeWhisperDialog(self, name):
		dlgWhisper = uiWhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
		dlgWhisper.BindInterface(self)
		dlgWhisper.LoadDialog()
		dlgWhisper.SetPosition(self.windowOpenPosition*30,self.windowOpenPosition*30)
		self.whisperDialogDict[name] = dlgWhisper

		self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		return dlgWhisper

	def __MakeWhisperButton(self, name):
		whisperButton = uiWhisper.WhisperButton()
		whisperButton.SetUpVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		whisperButton.SetOverVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		whisperButton.SetDownVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		if self.IsGameMasterName(name):
			whisperButton.SetToolTipTextWithColor(name, 0xffffa200)
		else:
			whisperButton.SetToolTipText(name)
		whisperButton.ToolTipText.SetHorizontalAlignCenter()
		whisperButton.SetEvent(ui.__mem_func__(self.ShowWhisperDialog), whisperButton)
		whisperButton.Show()
		whisperButton.name = name

		self.whisperButtonList.insert(0, whisperButton)
		self.__ArrangeWhisperButton()

		return whisperButton

	def __DestroyWhisperButton(self, button):
		button.SetEvent(0)
		self.whisperButtonList.remove(button)
		self.__ArrangeWhisperButton()

	def HideAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Hide()

	def ShowAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Show()

	def __CheckGameMaster(self, name):
		if not self.listGMName.has_key(name):
			return
		if self.whisperDialogDict.has_key(name):
			dlg = self.whisperDialogDict[name]
			dlg.SetGameMasterLook()

	def RegisterGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return
		self.listGMName[name] = "GM"

	def IsGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return True
		else:
			return False

	#####################################################################################

	#####################################################################################
	### Guild Building ###

	def BUILD_OpenWindow(self):
		self.wndGuildBuilding = uiGuild.BuildGuildBuildingWindow()
		self.wndGuildBuilding.Open()
		self.wndGuildBuilding.wnds = self.__HideWindows()
		self.wndGuildBuilding.SetCloseEvent(ui.__mem_func__(self.BUILD_CloseWindow))

	def BUILD_CloseWindow(self):
		self.__ShowWindows(self.wndGuildBuilding.wnds)
		self.wndGuildBuilding = None

	def BUILD_OnUpdate(self):
		if not self.wndGuildBuilding:
			return

		if self.wndGuildBuilding.IsPositioningMode():
			import background
			x, y, z = background.GetPickingPoint()
			self.wndGuildBuilding.SetBuildingPosition(x, y, z)

	def BUILD_OnMouseLeftButtonDown(self):
		if not self.wndGuildBuilding:
			return

		# GUILD_BUILDING
		if self.wndGuildBuilding.IsPositioningMode():
			self.wndGuildBuilding.SettleCurrentPosition()
			return True
		elif self.wndGuildBuilding.IsPreviewMode():
			pass
		else:
			return True
		# END_OF_GUILD_BUILDING
		return False

	def BUILD_OnMouseLeftButtonUp(self):
		if not self.wndGuildBuilding:
			return

		if not self.wndGuildBuilding.IsPreviewMode():
			return True

		return False

	def BULID_EnterGuildArea(self, areaID):
		# GUILD_BUILDING
		mainCharacterName = player.GetMainCharacterName()
		masterName = guild.GetGuildMasterName()

		if mainCharacterName != masterName:
			return

		if areaID != player.GetGuildID():
			return
		# END_OF_GUILD_BUILDING

		self.wndGameButton.ShowBuildButton()

	def BULID_ExitGuildArea(self, areaID):
		self.wndGameButton.HideBuildButton()
#	if app.__BL_CHEST_DROP_INFO__:
	def OpenChestDropWindow(self, itemVnum, isMain):
		if self.wndChestDropInfo:
			self.wndChestDropInfo.Open(itemVnum, isMain)

	#####################################################################################

	def IsEditLineFocus(self):
		if self.ChatWindow.chatLine.IsFocus():
			return 1

		if self.ChatWindow.chatToLine.IsFocus():
			return 1

		return 0

	def EmptyFunction(self):
		pass

	if app.BL_MOVE_CHANNEL:
		def RefreshServerInfo(self):
			if self.wndMiniMap:
				self.wndMiniMap.RefreshServerInfo()

	def GetInventoryPageIndex(self):
		if selfs.wndInventory:
			return selfs.wndInventory.GetInventoryPageIndex()
		else:
			return -1

	if app.WJ_ENABLE_TRADABLE_ICON:
		def SetOnTopWindow(self, onTopWnd):
			self.onTopWindow = onTopWnd

		def GetOnTopWindow(self):
			return self.onTopWindow

		def RefreshMarkInventoryBag(self):
			selfs.wndInventory.RefreshMarkSlots()
			selfs.wndExInventory.RefreshMarkSlots()

	if app.ENABLE_ITEM_DELETE_SYSTEM:
		def OpenDeleteItem(self):
			# if app.ENABLE_PLAYER_SECURITY_SYSTEM and player.IsSecurityActivate():
				# return

			if False == player.IsObserverMode():
				if False == self.deleteitem.IsShow():
					self.deleteitem.Open()
				else:
					self.deleteitem.Close()

	if app.ENABLE_DROP_DIALOG_EXTENDED_SYSTEM:
		def DeleteItem(self, slotPos, invenType):
			# if app.ENABLE_PLAYER_SECURITY_SYSTEM and player.IsSecurityActivate():
				# return
			if selfs.wndInventory:
				selfs.wndInventory.DeleteItem(slotPos, invenType)
					
	if app.ITEM_SHOP:
		def AddCategory(self, categoryName, categoryID):
			self.wndNesneMarket.AddCategory(categoryName, categoryID)
		def FillCategory(self, mainID, categoryName, categoryID):
			self.wndNesneMarket.FillCategory(mainID, categoryName, categoryID)
		def AddItem(self, itemID, itemVnum, itemCount, itemPrice, categoryID, itemSockets, itemAttrs, priceType):
			self.wndNesneMarket.AddItem(itemID, itemVnum, itemCount, itemPrice, categoryID, itemSockets, itemAttrs, priceType)
		def ClearList(self):
			self.wndNesneMarket.Clear()
		def RefreshCashToItemShop(self, cash, coins):
			self.wndNesneMarket.RefreshCash(cash, coins)
		def NesneMarketAc(self):
			self.wndNesneMarket.Open()

#	if app.ENABLE_HIDE_COSTUME_SYSTEM:
#		def RefreshVisibleCostume(self):
#			selfs.wndInventory.RefreshVisibleCostume()

	if app.ENABLE_REWARD_SYSTEM:
		def RewardData(self, isGlobal, isNeedClean, commandText):
			if self.wndReward:
				self.wndReward.RewardData(int(isGlobal), int(isNeedClean), commandText)
		def OpenRewardWindow(self):
			if self.wndReward == None:
				self.wndReward = uiReward.RewardWindow()
			if self.wndReward.IsShow():
				self.wndReward.Close()
			else:
				self.wndReward.Open()


	if app.ENABLE_EVENT_MANAGER:
		def MakeEventIcon(self):
			if self.wndEventIcon == None:
				self.wndEventIcon = uiEventCalendarNew.MovableImage()
				self.wndEventIcon.Show()
		def MakeEventCalendar(self):
			if self.wndEventManager == None:
				self.wndEventManager = uiEventCalendarNew.EventCalendarWindow()
		def OpenEventCalendar(self):
			self.MakeEventCalendar()
			if self.wndEventManager.IsShow():
				self.wndEventManager.Close()
			else:
				self.wndEventManager.Open()
		def RefreshEventStatus(self, eventID, eventStatus, eventendTime, eventEndTimeText):
			if eventendTime != 0:
				eventendTime += app.GetGlobalTimeStamp()
			uiEventCalendarNew.SetEventStatus(eventID, eventStatus, eventendTime, eventEndTimeText)
			self.RefreshEventManager()
		def ClearEventManager(self):
			uiEventCalendarNew.server_event_data={}
		def RefreshEventManager(self):
			if self.wndEventManager:
				self.wndEventManager.Refresh()
			if self.wndEventIcon:
				self.wndEventIcon.Refresh()
		def AppendEvent(self, dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart):
			self.MakeEventCalendar()
			self.MakeEventIcon()
			#import dbg
			#dbg.TraceError("startTime: %d endTime: %d"%(startRealTime, endRealTime))
			if startRealTime != 0:
				startRealTime += app.GetGlobalTimeStamp()
			if endRealTime != 0:
				endRealTime += app.GetGlobalTimeStamp()
			uiEventCalendarNew.SetServerData(dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart)


	class chatackapa(ui.Window):
		def __init__(self, parent = None, x = 0, y = 0):
			ui.Window.__init__(self)
			self.parent = parent
			self.x = x
			self.y = y
			self.ColorValue = 0xff40EF37
			self.show = self.checkBox(x,y)
			self.Show()


		def checkBox(self, x,y):
			checkBox = ui.CheckBoxIbo()
			checkBox.SetParent(self.parent)
			checkBox.SetPosition(x, y)
			checkBox.SetEvent(ui.__mem_func__(self.__OnClickCheckBox), "ON_CHECK", True)
			checkBox.SetEvent(ui.__mem_func__(self.__OnClickCheckBox), "ON_UNCKECK", False)
			checkBox.SetCheckStatus(systemSetting.IsViewChat())
			checkBox.SetTextInfo("Chat")
			checkBox.Show()
			return checkBox

		def __OnClickCheckBox(self, checkType, autoFlag):
			systemSetting.SetViewChatFlag(autoFlag)
			

	def OpenBotControl(self):
		if self.wndBotControl.IsShow():
			self.wndBotControl.Close()
		else:
			self.wndBotControl.Open()

	if app.__AUTO_HUNT__:
		def AutoHuntStatus(self, status):
			if self.wndAutoHunt:
				self.wndAutoHunt.SetStatus(True if int(status) else False)
		def CheckAutoLogin(self):
			if self.wndAutoHunt == None:
				self.wndAutoHunt = uiAutoHunt.Window()
			self.wndAutoHunt.CheckAutoLogin()
		def OpenAutoHunt(self):
			if self.wndAutoHunt == None:
				self.wndAutoHunt = uiAutoHunt.Window()
			if self.wndAutoHunt.IsShow():
				self.wndAutoHunt.Close()
			else:
				self.wndAutoHunt.Open()


	if app.__RANKING_SYSTEM__:
		def __MakeRaking(self):
			self.wndRanking = uiRank.RankingWindow()
		def OpenRanking(self):
			if self.wndRanking==None:
				self.__MakeRaking()
			if self.wndRanking.IsShow():
				self.wndRanking.Close()
			else:
				self.wndRanking.Open()
		def LoadRankData(self, index, isFirst):
			if self.wndRanking!=None:
				self.wndRanking.LoadRankData(index, isFirst)
		def LoadRankUpdate(self, index):
			if self.wndRanking!=None:
				self.wndRanking.UpdateLastItem(index)

if __name__ == "__main__":

	import app
	import wndMgr
	import systemSetting
	import mouseModule
	import grp
	import ui
	import localeInfo

	app.SetMouseHandler(mouseModule.mouseController)
	app.SetHairColorEnable(True)
	wndMgr.SetMouseHandler(mouseModule.mouseController)
	wndMgr.SetScreenSize(systemSetting.GetWidth(), systemSetting.GetHeight())
	app.Create(localeInfo.APP_TITLE, systemSetting.GetWidth(), systemSetting.GetHeight(), 1)
	mouseModule.mouseController.Create()

	class TestGame(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)

			localeInfo.LoadLocaleData()
			player.SetItemData(0, 27001, 10)
			player.SetItemData(1, 27004, 10)

			self.interface = Interface()
			self.interface.MakeInterface()
			self.interface.ShowDefaultWindows()
			self.interface.RefreshInventory()
			#self.interface.OpenCubeWindow()

		def __del__(self):
			ui.Window.__del__(self)

		def OnUpdate(self):
			app.UpdateGame()

		def OnRender(self):
			app.RenderGame()
			grp.PopState()
			grp.SetInterfaceRenderState()

	game = TestGame()
	game.SetSize(systemSetting.GetWidth(), systemSetting.GetHeight())
	game.Show()

	app.Loop()

