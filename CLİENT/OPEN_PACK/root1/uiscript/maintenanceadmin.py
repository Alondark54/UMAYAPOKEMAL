import uiScriptLocale
ROOT_PATH = "d:/ymir work/ui/public/"
window = {
	"name" : "MaintenanceDialog",
	"x" : 0,
	"y" : 0,
	"style" : ("movable", "float",),
	"width" : 250,
	"height" : 160,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"x" : 0,
			"y" : 0,
			"width" : 250,
			"height" : 160,
			"children" :
			(
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 8,
					"y" : 8,
					"width" : 238,
					"color" : "gray",
					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",
							"x" : 238/2,
							"y" : 3,
							"text" : "Bakým Paneli",
							"text_horizontal_align":"center"
						},
					),
				},
				{
					"name" : "durationtitle",
					"type" : "text",

					"x" : 0,
					"y" : 41,

					"text" : "Bakýmýn kaç dakika süreceði:",

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "timetitle",
					"type" : "text",

					"x" : 0,
					"y" : 83,

					"text" : "Oyunun kaç saniye sonra kapanacaðý:",

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "InputSlot",
					"type" : "slotbar",

					"x" : 0,
					"y" : 97,
					"width" : 90,
					"height" : 18,
					"horizontal_align" : "center",

					#"type" : "image",
					#"image" : "d:/ymir work/ui/public/Parameter_Slot_03.sub",

					"children" :
					(
						{
							"name" : "InputValue",
							"type" : "editline",

							"x" : 3,
							"y" : 3,

							"width" : 90,
							"height" : 18,

							"input_limit" : 9,
						},
					),
				},
				{
					"name" : "InputSlot2",
					"type" : "slotbar",

					"x" : 0,
					"y" : 55,
					"width" : 90,
					"height" : 18,
					"horizontal_align" : "center",

					"children" :
					(
						{
							"name" : "InputValue2",
							"type" : "editline",

							"x" : 3,
							"y" : 3,

							"width" : 90,
							"height" : 18,

							"input_limit" : 3,
						},
					),
				},
				{
					"name" : "bakimyap",
					"type" : "button",

					"x" : 22,
					"y" : 125,

					"text" : "Bakým yap",

					"default_image" : ROOT_PATH + "large_Button_01.sub",
					"over_image" : ROOT_PATH + "large_Button_02.sub",
					"down_image" : ROOT_PATH + "large_Button_03.sub",
				},
				{
					"name" : "bakimiptal",
					"type" : "button",

					"x" : 140,
					"y" : 125,

					"text" : "Bakým iptal et",

					"default_image" : ROOT_PATH + "large_Button_01.sub",
					"over_image" : ROOT_PATH + "large_Button_02.sub",
					"down_image" : ROOT_PATH + "large_Button_03.sub",
				},
				## Cancel
				{
					"name" : "cancel_button",
					"type" : "button",
					"x" : 0,
					"y" : 185,
					"text" : uiScriptLocale.CANCEL,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},
			),
		},
	),
}
