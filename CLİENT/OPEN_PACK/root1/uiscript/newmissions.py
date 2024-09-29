MAIN_WIDTH = 500
MAIN_HEIGTH = 385

PATH = "missions/"

window = {
	"name" : "MissionWindow",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : MAIN_WIDTH,
	"height" : MAIN_HEIGTH + 25,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : MAIN_WIDTH,
			"height" : MAIN_HEIGTH,

			"children" :
			(
				## Title
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,

					"width" : MAIN_WIDTH - 15,
					"color" : "gray",

					"children" :
					(
						{ "name":"titlename", "type":"text", "x":0, "y":3, 
						"text" : "Gorev Penceresi", 
						"horizontal_align":"center", "text_horizontal_align":"center" },
					),
				},

				{
					"name" : "bg",
					"type" : "image",
					"x" : 15,
					"y" : 35,
					"image" : PATH + "kenarbg.png",
				},

				{
					"name" : "ScrollBar",
					"type" : "scrollbar",
					"x" : 30,
					"y" : 40,
					"size" : 85*4 - 20,
					"horizontal_align" : "right",
				},

				{
					"name" : "ListBox",
					"type" : "listboxex",
					"x" : 20,
					"y" : 40,
					"width" : 444,
					"height" : 85*4,
				},
			),
		},

		{
			"name" : "global_button",
			"type" : "button",

			"x" : 10,
			"y" : MAIN_HEIGTH - 10,
			# "horizontal_align" : "center",

			# "text" : uiScriptLocale.CANCEL,

			"default_image" : "missions/global.png",
			"over_image" : "missions/global.png",
			"down_image" : "missions/global.png",
		},


		{
			"name" : "missions_button",
			"type" : "button",

			"x" : 10,
			"y" : MAIN_HEIGTH - 10,
			# "horizontal_align" : "center",

			# "text" : uiScriptLocale.CANCEL,

			"default_image" : "missions/player.png",
			"over_image" : "missions/player.png",
			"down_image" : "missions/player.png",
		},


	),
}