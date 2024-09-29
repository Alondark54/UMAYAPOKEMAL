import uiScriptLocale

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

	"width" : 230+100+100-60-20-10,
	"height" : 120/2+25,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 230+200+10-5-65-30,
			"height" : 120-60+25,

			"children" :
			(
				## Title
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,

					"width" : 210+200-60+7-20-10-2,
					"color" : "gray",

					"children" :
					(
						{ 
						"name":"titlename", "type":"text", "x":0, "y":3, 
						"horizontal_align":"center", "text_horizontal_align":"center",
						"text" : "Çoklu Efsun Botu Sistemi",
						 },
					),
				},
				{
					"name" : "bgm_button",
					"type" : "button",

					"x" : 20+20,
					"y" : 45,

					"text" : "Normal Efsun Botu",
					"text_color" : 0xffF8BF24,

					"default_image" : "d:/ymir work/ui/public/button_bank_1.tga",
					"over_image" : "d:/ymir work/ui/public/button_bank_2.tga",
					"down_image" : "d:/ymir work/ui/public/button_bank_3.tga",
				},
				{
					"name" : "bgm_button2",
					"type" : "button",

					"x" : 120+50,
					"y" : 45,

					"text" : uiScriptLocale.yesilefsunisim,
					"text_color" : 0xffF8BF24,

					"default_image" : "d:/ymir work/ui/public/button_bank_1.tga",
					"over_image" : "d:/ymir work/ui/public/button_bank_2.tga",
					"down_image" : "d:/ymir work/ui/public/button_bank_3.tga",
				},
			),
		},
	),
}
