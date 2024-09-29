import uiScriptLocale

ROOT = "d:/ymir work/ui/public/"
ROOT_PATH = "d:/ymir work/ui/public/"

TEMPORARY_X = +13
TEXT_TEMPORARY_X = -10
BUTTON_TEMPORARY_X = 5
PVP_X = -10

window = {
	"name" : "SystemOptionDialog",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : 230 + 100 + 80,
	"height" : 258,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 230 + 100 + 80,
			"height" : 258,

			"children" :
			(
				## Title
				{ "name" : "titlebar", "type" : "titlebar", "style" : ("attach",), "x" : 8, "y" : 8, "width" : 215 + 100 + 75 + 5, "color" : "gray",
					"children" :
					(
						{  "name":"titlename", "type":"text", "x":0, "y":3,  "horizontal_align":"center", "text_horizontal_align":"center","text" : "Efsun Botu Penceresi", },
					),
				},
				
				{ "name": "Item_Green", "type": "thinboard", "x": 12, "y": 31-24+34-8 + 30, "width": 55, "height": 64,
					"children" :
					(
						{ 
							"name":"yesil_title_bar", "type":"horizontalbar", "x":0, "y": - 25, "width":363 + 13 + 5, 
							"children" :
							( {"name" : "yesil_title_text", "type" : "text", "text" : "Yeþil Efsun Botu", "all_align" : "center", "x" : 0, "y" : 0, }, ),
							}, { "name" : "yesil_title", "type" : "bar", "width" : 55, "height" : 18, "x" : 0, "y" : 0,
							"children":({
									"name" : "gorev_basligi_text", "type" : "text", "all_align":"center", "x" : 0, "y" : 0, "text" : "Malzeme",
								},
							),
						},
						## Item Slot
						{
							"name" : "ItemSlot",
							"type" : "image",
							"x" : 4+7,
							"y" : 23,
							"image" : "d:/ymir work/ui/public/Slot_Base.sub",
							"children" :
							(
								{
									"name" : "image",
									"type" : "image",
									"x" : 0,
									"y" : 0,
									"image" : "icon/item/71151.tga",
								},
							),
						},
					),
				},
				{
					"name" : "yesil_button_thin",
					"type" : "thinboard",
					"x" : 296, #225 + 66 + 5,
					"y": 63,# 31-24+34-8 + 30,
					"width" : 100,
					"height" : 64,
				},
				{
					"name" : "thingreen_info",
					"type" : "thinboard",
					"x" : 75,#55 + 12 + 8,
					"y" : 63, #31-24+34-8 + 30,
					"width" : 215, #150 + 60 + 5,
					"height" : 64,
					"children" :
					( 
						{ "name" : "Green_Switch_Title", "type" : "text", "text" : "|cFF90EE90(Yesil) Efsun Nesnesi", "x" : 0, "y" : -15, "all_align" : "center", },
						{ "name" : "Green_Switch_Info_01", "type" : "text", "text" : "|cFF90EE90(Yesil) Efsun Nesnesi2", "x" : 0, "y" : 0, "all_align" : "center", },
						{ "name" : "Green_Switch_Info_02", "type" : "text", "text" : "|cFF90EE90(Yesil) Efsun Nesnesi3", "x" : 0, "y" : 15, "all_align" : "center", },
					),
				},
				{
					"name" : "bgm_button2",
					"type" : "button",

					"x" : 225 + 77,
					"y": 83,

					"text" : "Yesil Bot",

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
#				{
#					"name" : "bgm_button4",
#					"type" : "button",

#					"x" : 225 + 77,
#					"y": 31-24+34-8 + 30 + 10 + 25,

#					"text" : "Y. Bot (2)",

#					"default_image" : ROOT + "Large_Button_01.sub",
#					"over_image" : ROOT + "Large_Button_02.sub",
#					"down_image" : ROOT + "Large_Button_03.sub",
#				},
				
				{
					"name": "Item_Blue",
					"type": "thinboard",
					"x": 12,
					"y": 31-24+34-8 + 30 + 100,
					"width": 55,
					"height": 64,
					"children" :
					(
						{ 
							"name":"mavi_title_bar", "type":"horizontalbar", "x":0, "y": - 25, "width":363 + 13 + 5, 
							"children" :
							(
								{
									"name" : "mavi_title_text",
									"type" : "text",
									"text" : "Normal Efsun Botu",
									"all_align" : "center",
									"x" : 0,
									"y" : 0,
								},
							),
						},
						{
							"name" : "mavi_title",
							"type" : "bar",
							"width" : 55,
							"height" : 18,
							"x" : 0,
							"y" : 0,
							"children":
							(
								{									
									"name" : "gorev_basligi_text", "type" : "text", "all_align":"center",
									"x" : 0, "y" : 0,
									"text" : "Malzeme",
								},
							),
						},
						## Item Slot
						{
							"name" : "ItemSlot",
							"type" : "image",

							"x" : 4+7,
							"y" : 23,

							"image" : "d:/ymir work/ui/public/Slot_Base.sub",
							"children" :
							(
								{
									"name" : "image",
									"type" : "image",
									"x" : 0,
									"y" : 0,
									"image" : "icon/item/71084.tga",
								},
							),
						},
					),
				},
				{
					"name" : "mavi_button_thin",
					"type" : "thinboard",
					"x" : 225 + 66 + 5,
					"y": 31-24+34-8 + 30 + 100,
					"width" : 100,
					"height" : 64,
				},
				{
					"name" : "thinBlue_info",
					"type" : "thinboard",
					"x" : 55 + 12 + 8,
					"y" : 31-24+34-8 + 30 + 100,
					"width" : 150 + 60 + 5,
					"height" : 64,
					"children" :
					(
						{
							"name" : "Blue_Switch_Title",
							"type" : "text",
							"text" : "|cff00ccff(Normal) Efsun Nesnesi",
							"x" : 0,
							"y" : -15,
							"all_align" : "center",
						},
						{
							"name" : "Blue_Switch_Info_01",
							"type" : "text",
							"text" : "|cff00ccff(Normal) Efsun Nesnesi2",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
						},
						{
							"name" : "Blue_Switch_Info_02",
							"type" : "text",
							"text" : "|cff00ccff(Normal) Efsun Nesnesi3",
							"x" : 0,
							"y" : 15,
							"all_align" : "center",
						},	
					),
				},
				
#				{
#					"name" : "bgm_button3",
#					"type" : "button",

#					"x" : 225 + 77,
#					"y": 31-24+34-8 + 30 + 10 + 125,

#					"text" : "N. Bot (2)",

#					"default_image" : ROOT + "Large_Button_01.sub",
#					"over_image" : ROOT + "Large_Button_02.sub",
#					"down_image" : ROOT + "Large_Button_03.sub",
#				},
				{
					"name" : "bgm_button",
					"type" : "button",

					"x" : 225 + 77,
					"y": 183,

					"text" : "Normal Bot",

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
			
			),
		},
	),
}
