import uiScriptLocale

BOARD_WIDTH = 200
BOARD_HEIGHT = 150

window = {
	"name" : "streamerdialog",

	"x" : SCREEN_WIDTH - 136 - 300,
	"y" : 15,

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "streamerdialog",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"children" :
			(
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,

					"width" : BOARD_WIDTH-12,
					"color" : "gray",

					"children" :
					(
						{ "name":"titlename", "type":"text", "x":0, "y":3,
						"text" : "Canlý Yayýn",
						"horizontal_align":"center", "text_horizontal_align":"center" },
					),
				},
				{
					"name" : "streamer_image",
					"type" : "image", "x" : 10, "y" : 30,
					"image" : "streamer/logo1.png"
				},
			),
		},
	),
}
