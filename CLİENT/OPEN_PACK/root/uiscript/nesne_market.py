import uiScriptLocale
import localeInfo

HEIGHT = 612
WIDTH = 775

ROOT_PATH = "d:/ymir work/ui/game/windows/"
BG_PHOTO_WIDTH = 187
BG_PHOTO_HEIGHT = 131
BG_START_X = 172
SPACE_XY = 2

window = {
	"name" : "NMWindow", "x" : 140, "y" : 20, "style" : ("movable", "float",),
	"width" : WIDTH, "height" : HEIGHT,
	"children" :(
		{"name" : "board", "type" : "board", "x" : 0, "y" : 0, "width" : WIDTH, "height" : HEIGHT-100,
		"children" :(
				{
					"name" : "bg","type" : "image","x" : 8,"y" : 18,"width" : 532,
					"image":"d:/ymir work/ui/itemshop/bg.tga", "height" : 392,
				},
				{"name" : "TitleBar", "type" : "titlebar", "style" : ("attach",), "x" : 8, "y" : 7, "width" : WIDTH - 15, "color" : "yellow",
					"children" :( { "name":"TitleName", "type":"text", "x":97, "y":3, "text":uiScriptLocale.MALL_TITLE, "text_horizontal_align":"center" }, ),},
				{"name" : "CategoryMain", "type" : "window", "x" : 8, "y" : 40, "width" : BG_START_X , "height" : HEIGHT + 105,},
				{"name" : "SearchButton", "type" : "button", "x" : BG_START_X + 170, "y" : HEIGHT - 170, "default_image" : "d:/ymir work/ui/itemshop/btn_search_normal.dds", "over_image" : "d:/ymir work/ui/itemshop/btn_search_hover.dds", "down_image" : "d:/ymir work/ui/itemshop/btn_search_pressed.dds",},
				{"name" : "NextButton", "type" : "button", "x" : WIDTH - 42, "y" : HEIGHT - 170, "default_image" : "d:/ymir work/ui/itemshop/btn_next_normal.dds", "over_image" : "d:/ymir work/ui/itemshop/btn_next_over.dds", "down_image" : "d:/ymir work/ui/itemshop/btn_next_down.dds",},
				{"name" : "PrevButton", "type" : "button", "x" : WIDTH - 92, "y" : HEIGHT - 170, "default_image" : "d:/ymir work/ui/itemshop/btn_back_normal.dds", "over_image" : "d:/ymir work/ui/itemshop/btn_back_over.dds", "down_image" : "d:/ymir work/ui/itemshop/btn_back_down.dds",},
				{"name" : "InputSlot_Name","type" : "slotbar","x" :  BG_START_X + 28,"y" : HEIGHT - 170,"width" : 135,"height" : 18,"children" :( { "name" : "QueryString","type" : "editline","x" : 2,"y" : 4, "width" : 135,"height" : 18,"input_limit" : 24, }, ),},
				{"name" : "RefreshBtn", "type" : "button", "x" : 5, "y" : 2, "tooltip_text" : "", "default_image" : "d:/ymir work/ui/itemshop/derle_1.tga", "over_image" : "d:/ymir work/ui/itemshop/derle_2.tga", "down_image" : "d:/ymir work/ui/itemshop/derle_3.tga",},
				#{"name" : "GetVOTEButton", "type" : "button", "x" : 570, "y" : 457, "text" : "", "default_image" : "d:/ymir work/ui/itemshop/vote_1.tga", "over_image" : "d:/ymir work/ui/itemshop/vote_2.tga", "down_image" : "d:/ymir work/ui/itemshop/vote_1.tga",},
				#{"name" : "VOTETitle", "type":"text", "x":625, "y":465, "text":uiScriptLocale.VOTE_TITLE, "text_horizontal_align":"center" },
				{"name" : "MyCash", "type" : "text", "x" : BG_START_X + 28, "y" : HEIGHT - 144, "text" :"Credit: 0 DR", }, 
				#{"name" : "MyVC", "type" : "text", "x" : 574, "y" : HEIGHT - 24, "text" :"Vote Coins: 0 VC", }, 
				{"name" : "Cooldown", "type" : "text", "x" : BG_START_X + 28, "y" : HEIGHT - 124, "text" :"", }, 
				#{
				#	"name" : "Kampanya","type" : "image","x" : 15,"y" : 108,"width" : 750,
				#	"image":"d:/ymir work/ui/itemshop/banner.tga", "height" : 131,
				#	"children" :(
				#		{
				#			"name" : "PageText","type" : "text","x" : 5,"y" : 2,
				#			"width" : 50,"height" : 18,"input_limit" : 24,
				#		},
				#	),
				#},
				{
					"name" : "InputSlot","type" : "image","x" :  WIDTH - 67,"y" : HEIGHT -170,"width" : 50,
					"image":"d:/ymir work/ui/itemshop/page_bg.tga", "height" : 18,
					"children" :(
						{
							"name" : "PageText","type" : "text","x" : 5,"y" : 2,
							"width" : 50,"height" : 18,"input_limit" : 24,
						},
					),
				},
				{
					"name" : "RightMenuPanel", "type" : "window", "x" : BG_START_X + 25, "y" : 30, "width" : WIDTH - BG_START_X - 35 , "height" : (BG_PHOTO_HEIGHT * 2) + (SPACE_XY * 2) + BG_PHOTO_HEIGHT,
					"children":(
						{"name": "ItemBG_0","type": "image","x": (BG_PHOTO_WIDTH * 0) + (SPACE_XY * 0),"y": (BG_PHOTO_HEIGHT * 0) + (SPACE_XY * 0),"image":"d:/ymir work/ui/itemshop/slot_icon_new.tga",
							"children":(
								{"name": "ItemSlot_0","type": "image","x": 11,"y": 20,"image":"icon/item/00010.tga",},
								{"name": "ItemName_0","type": "text","x":55,"y":24,"text":"Nesne ismi",},
								{"name": "ItemCount_0","type": "text","x":136,"y":46,"text":"1 adet",},
								{"name": "ItemPrice_0","type": "text","x":85,"y":46,"text":"8 EP",},
								{"name":"ItemCntUp_0", "type" : "button", "x":165, "y":65, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
								{"name":"ItemCntDw_0", "type" : "button", "x":165, "y":80, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{"name" : "ItemBuy_0", "type" : "button", "x" : 72, "y" : 68, "text" : uiScriptLocale.SHOP_BUY, "default_image" : "d:/ymir work/ui/itemshop/buy_button_0.tga", "over_image" : "d:/ymir work/ui/itemshop/buy_button_1.tga","down_image" : "d:/ymir work/ui/itemshop/buy_button_2.tga",},
						),},
						
						{"name": "ItemBG_1","type": "image","x": (BG_PHOTO_WIDTH * 1) + (SPACE_XY * 1),"y": (BG_PHOTO_HEIGHT * 0) + (SPACE_XY * 0),"image":"d:/ymir work/ui/itemshop/slot_icon_new.tga",
							"children":(
								{"name": "ItemSlot_1","type": "image","x": 11,"y": 20,"image":"icon/item/00010.tga",},
								{"name": "ItemName_1","type": "text","x":55,"y":24,"text":"Nesne ismi",},
								{"name": "ItemCount_1","type": "text","x":136,"y":46,"text":"1 adet",},
								{"name": "ItemPrice_1","type": "text","x":85,"y":46,"text":"8 EP",},
								{"name":"ItemCntUp_1", "type" : "button", "x":165, "y":65, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
								{"name":"ItemCntDw_1", "type" : "button", "x":165, "y":80, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{"name" : "ItemBuy_1", "type" : "button", "x" : 72, "y" : 68, "text" : uiScriptLocale.SHOP_BUY, "default_image" : "d:/ymir work/ui/itemshop/buy_button_0.tga", "over_image" : "d:/ymir work/ui/itemshop/buy_button_1.tga","down_image" : "d:/ymir work/ui/itemshop/buy_button_2.tga",},
						),},
						{"name": "ItemBG_2","type": "image","x": (BG_PHOTO_WIDTH * 2) + (SPACE_XY * 2),"y": (BG_PHOTO_HEIGHT * 0) + (SPACE_XY * 0),"image":"d:/ymir work/ui/itemshop/slot_icon_new.tga",
							"children":(
								{"name": "ItemSlot_2","type": "image","x": 11,"y": 20,"image":"icon/item/00010.tga",},
								{"name": "ItemName_2","type": "text","x":55,"y":24,"text":"Nesne ismi",},
								{"name": "ItemCount_2","type": "text","x":136,"y":46,"text":"1 adet",},
								{"name": "ItemPrice_2","type": "text","x":85,"y":46,"text":"8 EP",},
								{"name":"ItemCntUp_2", "type" : "button", "x":165, "y":65, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
								{"name":"ItemCntDw_2", "type" : "button", "x":165, "y":80, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{"name" : "ItemBuy_2", "type" : "button", "x" : 72, "y" : 68, "text" : uiScriptLocale.SHOP_BUY, "default_image" : "d:/ymir work/ui/itemshop/buy_button_0.tga", "over_image" : "d:/ymir work/ui/itemshop/buy_button_1.tga","down_image" : "d:/ymir work/ui/itemshop/buy_button_2.tga",},
						),},
						
						
						{"name": "ItemBG_3","type": "image","x": (BG_PHOTO_WIDTH * 0) + (SPACE_XY * 0),"y": (BG_PHOTO_HEIGHT * 1) + (SPACE_XY * 1),"image":"d:/ymir work/ui/itemshop/slot_icon_new.tga",
							"children":(
								{"name": "ItemSlot_3","type": "image","x": 11,"y": 20,"image":"icon/item/00010.tga",},
								{"name": "ItemName_3","type": "text","x":55,"y":24,"text":"Nesne ismi",},
								{"name": "ItemCount_3","type": "text","x":136,"y":46,"text":"1 adet",},
								{"name": "ItemPrice_3","type": "text","x":85,"y":46,"text":"8 EP",},
								{"name":"ItemCntUp_3", "type" : "button", "x":165, "y":65, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
								{"name":"ItemCntDw_3", "type" : "button", "x":165, "y":80, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{"name" : "ItemBuy_3", "type" : "button", "x" : 72, "y" : 68, "text" : uiScriptLocale.SHOP_BUY, "default_image" : "d:/ymir work/ui/itemshop/buy_button_0.tga", "over_image" : "d:/ymir work/ui/itemshop/buy_button_1.tga","down_image" : "d:/ymir work/ui/itemshop/buy_button_2.tga",},
						),},
						
						
						{"name": "ItemBG_4","type": "image","x": (BG_PHOTO_WIDTH * 1) + (SPACE_XY * 1),"y": (BG_PHOTO_HEIGHT * 1) + (SPACE_XY * 1),"image":"d:/ymir work/ui/itemshop/slot_icon_new.tga",
							"children":(
								{"name": "ItemSlot_4","type": "image","x": 11,"y": 20,"image":"icon/item/00010.tga",},
								{"name": "ItemName_4","type": "text","x":55,"y":24,"text":"Nesne ismi",},
								{"name": "ItemCount_4","type": "text","x":136,"y":46,"text":"1 adet",},
								{"name": "ItemPrice_4","type": "text","x":85,"y":46,"text":"8 EP",},
								{"name":"ItemCntUp_4", "type" : "button", "x":165, "y":65, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
								{"name":"ItemCntDw_4", "type" : "button", "x":165, "y":80, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{"name" : "ItemBuy_4", "type" : "button", "x" : 72, "y" : 68, "text" : uiScriptLocale.SHOP_BUY, "default_image" : "d:/ymir work/ui/itemshop/buy_button_0.tga", "over_image" : "d:/ymir work/ui/itemshop/buy_button_1.tga","down_image" : "d:/ymir work/ui/itemshop/buy_button_2.tga",},
						),},
						{"name": "ItemBG_5","type": "image","x": (BG_PHOTO_WIDTH * 2) + (SPACE_XY * 2),"y": (BG_PHOTO_HEIGHT * 1) + (SPACE_XY * 1),"image":"d:/ymir work/ui/itemshop/slot_icon_new.tga",
							"children":(
								{"name": "ItemSlot_5","type": "image","x": 11,"y": 20,"image":"icon/item/00010.tga",},
								{"name": "ItemName_5","type": "text","x":55,"y":24,"text":"Nesne ismi",},
								{"name": "ItemCount_5","type": "text","x":136,"y":46,"text":"1 adet",},
								{"name": "ItemPrice_5","type": "text","x":85,"y":46,"text":"8 EP",},
								{"name":"ItemCntUp_5", "type" : "button", "x":165, "y":65, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
								{"name":"ItemCntDw_5", "type" : "button", "x":165, "y":80, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{"name" : "ItemBuy_5", "type" : "button", "x" : 72, "y" : 68, "text" : uiScriptLocale.SHOP_BUY, "default_image" : "d:/ymir work/ui/itemshop/buy_button_0.tga", "over_image" : "d:/ymir work/ui/itemshop/buy_button_1.tga","down_image" : "d:/ymir work/ui/itemshop/buy_button_2.tga",},
						),},
						{"name": "ItemBG_6","type": "image","x": (BG_PHOTO_WIDTH * 0) + (SPACE_XY * 0),"y": (BG_PHOTO_HEIGHT * 2) + (SPACE_XY * 2),"image":"d:/ymir work/ui/itemshop/slot_icon_new.tga",
							"children":(
								{"name": "ItemSlot_6","type": "image","x": 11,"y": 20,"image":"icon/item/00010.tga",},
								{"name": "ItemName_6","type": "text","x":55,"y":24,"text":"Nesne ismi",},
								{"name": "ItemCount_6","type": "text","x":136,"y":46,"text":"1 adet",},
								{"name": "ItemPrice_6","type": "text","x":85,"y":46,"text":"8 EP",},
								{"name":"ItemCntUp_6", "type" : "button", "x":165, "y":65, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
								{"name":"ItemCntDw_6", "type" : "button", "x":165, "y":80, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{"name" : "ItemBuy_6", "type" : "button", "x" : 72, "y" : 68, "text" : uiScriptLocale.SHOP_BUY, "default_image" : "d:/ymir work/ui/itemshop/buy_button_0.tga", "over_image" : "d:/ymir work/ui/itemshop/buy_button_1.tga","down_image" : "d:/ymir work/ui/itemshop/buy_button_2.tga",},
						),},
						
						{"name": "ItemBG_7","type": "image","x": (BG_PHOTO_WIDTH * 1) + (SPACE_XY * 1),"y": (BG_PHOTO_HEIGHT * 2) + (SPACE_XY * 2),"image":"d:/ymir work/ui/itemshop/slot_icon_new.tga",
							"children":(
								{"name": "ItemSlot_7","type": "image","x": 11,"y": 20,"image":"icon/item/00010.tga",},
								{"name": "ItemName_7","type": "text","x":55,"y":24,"text":"Nesne ismi",},
								{"name": "ItemCount_7","type": "text","x":136,"y":46,"text":"1 adet",},
								{"name": "ItemPrice_7","type": "text","x":85,"y":46,"text":"8 EP",},
								{"name":"ItemCntUp_7", "type" : "button", "x":165, "y":65, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
								{"name":"ItemCntDw_7", "type" : "button", "x":165, "y":80, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{"name" : "ItemBuy_7", "type" : "button", "x" : 72, "y" : 68, "text" : uiScriptLocale.SHOP_BUY, "default_image" : "d:/ymir work/ui/itemshop/buy_button_0.tga", "over_image" : "d:/ymir work/ui/itemshop/buy_button_1.tga","down_image" : "d:/ymir work/ui/itemshop/buy_button_2.tga",},
						),},
						{"name": "ItemBG_8","type": "image","x": (BG_PHOTO_WIDTH * 2) + (SPACE_XY * 2),"y": (BG_PHOTO_HEIGHT * 2) + (SPACE_XY * 2),"image":"d:/ymir work/ui/itemshop/slot_icon_new.tga",
							"children":(
								{"name": "ItemSlot_8","type": "image","x": 11,"y": 20,"image":"icon/item/00010.tga",},
								{"name": "ItemName_8","type": "text","x":55,"y":24,"text":"Nesne ismi",},
								{"name": "ItemCount_8","type": "text","x":136,"y":46,"text":"1 adet",},
								{"name": "ItemPrice_8","type": "text","x":85,"y":46,"text":"8 EP",},
								{"name":"ItemCntUp_8", "type" : "button", "x":165, "y":65, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
								{"name":"ItemCntDw_8", "type" : "button", "x":165, "y":80, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{"name" : "ItemBuy_8", "type" : "button", "x" : 72, "y" : 68, "text" : uiScriptLocale.SHOP_BUY, "default_image" : "d:/ymir work/ui/itemshop/buy_button_0.tga", "over_image" : "d:/ymir work/ui/itemshop/buy_button_1.tga","down_image" : "d:/ymir work/ui/itemshop/buy_button_2.tga",},
						),},
					),
				},
			),
		},
	),
}
#Add end of file
import app
if app.BL_REMOTE_SHOP:
	window["children"] += (
		{
			"name" : "FastInventoryLayer",
			"type" : "board",
			"x" : 0,
			"y" : 522,
			"width" : 180,
			"height" : 100,
			"children" :(
				{"name" : "BuyDRButton","type" : "button","x" : 15,"y" : 10,"default_image" : "locale/tr/ui/buttons/paywant.tga","over_image" : "locale/tr/ui/buttons/paywant1.tga","down_image" : "locale/tr/ui/buttons/paywant.tga",},
			),
		},
	)

import app
if app.BL_REMOTE_SHOP:
	window["children"] += (
		{
			"name" : "FastInventoryLayer",
			"type" : "board",
			"x" : 196,
			"y" : 522,
			"width" : 180,
			"height" : 100,
			"children" :(
				{"name" : "Itemci","type" : "button","x" : 15,"y" : 10,"default_image" : "locale/tr/ui/buttons/itemci.tga","over_image" : "locale/tr/ui/buttons/itemci1.tga","down_image" : "locale/tr/ui/buttons/itemci.tga",},
			),
		},
	)

import app
if app.BL_REMOTE_SHOP:
	window["children"] += (
		{
			"name" : "FastInventoryLayer",
			"type" : "board",
			"x" : 389,
			"y" : 522,
			"width" : 180,
			"height" : 100,
			"children" :(
				{"name" : "ItemPol","type" : "button","x" : 15,"y" : 10,"default_image" : "locale/tr/ui/buttons/itempol.tga","over_image" : "locale/tr/ui/buttons/itempol1.tga","down_image" : "locale/tr/ui/buttons/itempol.tga",},
			),
		},
	)

import app
if app.BL_REMOTE_SHOP:
	window["children"] += (
		{
			"name" : "FastInventoryLayer",
			"type" : "board",
			"x" : 582,
			"y" : 522,
			"width" : 190,
			"height" : 100,
			"children" :(
				{"name" : "Discord","type" : "button","x" : 15,"y" : 10,"default_image" : "locale/tr/ui/buttons/discord1.tga","over_image" : "locale/tr/ui/buttons/discord.tga","down_image" : "locale/tr/ui/buttons/discord1.tga",},
			),
		},
	)


import app
if app.BL_REMOTE_SHOP:
	window["children"] += (
		{
			"name" : "FastInventoryLayer",
			"type" : "board",
			"x" : 0,
			"y" : -80,
			"width" : 775,
			"height" : 72,
			"children" :(
				{"name" : "Discord","type" : "button","x" : 0,"y" : 0,"default_image" : "locale/tr/ui/buttons/banner.tga","over_image" : "locale/tr/ui/buttons/banner.tga","down_image" : "locale/tr/ui/buttons/banner.tga",},
			),
		},
	)