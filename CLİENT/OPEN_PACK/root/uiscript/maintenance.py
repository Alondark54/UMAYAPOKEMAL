import uiScriptLocale

window = {
	"name" : "MaintenanceWindow",
	"style" : ("movable", "float",),

	"x" : 980,
	"y" : 60,

	"width" : 490,
	"height" : 125,

	"children" :
	(
		{
			"name" : "Thinboard",
			"type" : "thinboard",

			"x" : 0,
			"y" : 0,
			"width" : 490,
			"height" : 125,

			"children" :
			(
				{
					"name" : "maintitle",
					"type" : "text",

					"x" : 0,
					"y" : 17,

					"color" : 0xfff8d090,

					"text" : "Oyun Henüz Açýlmadý!",

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "maintitleimage",
					"type" : "image",

					"x" : 0,
					"y" : 10,

					"image" : "d:/ymir work/center.tga",

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "message1",
					"type" : "text",

					"x" : 0,
					"y" : 50,

					"text" : uiScriptLocale.MESSAGE,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "message2",
					"type" : "text",

					"x" : 0,
					"y" : 65,

					"color" : 0xfff8d090,

					"text" : uiScriptLocale.MESSAGE,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "message3",
					"type" : "text",

					"x" : 0,
					"y" : 80,

					"text" : uiScriptLocale.MESSAGE,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "message4",
					"type" : "text",

					"x" : 0,
					"y" : 95,

					"text" : uiScriptLocale.MESSAGE,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
			),
		},
	),
}