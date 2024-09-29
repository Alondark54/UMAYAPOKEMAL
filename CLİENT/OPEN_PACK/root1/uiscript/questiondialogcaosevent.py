import uiScriptLocale

window = {
	"name" : "QuestionDialog",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH/2 - 125,
	"y" : SCREEN_HEIGHT/2 - 52,

	"width" : 370,
	"height" : 150,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 370,
			"height" : 150,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6,
					"y" : 6,

					"width" : 370-13,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":350/2-5, "y":3, "text": "Ronark Arenasý", "text_horizontal_align":"center" },
					),
				},
				{
					"name" : "message1",
					"type" : "text",

					"x" : -75+10,
					"y" : 40,

					"text" : "Kaos Güç savaþý baþlýyor !",

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "message2",
					"type" : "text",

					"x" : -51+10,
					"y" : 55,

					"text" : "Gücüne güveniyorsan savaþa katýl ve lider ol !",

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},		
				{
					"name" : "accept",
					"type" : "button",

					"x" : -50,
					"y" : 105-10,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",
					"text" : "Katýl",

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				{
					"name" : "cancel",
					"type" : "button",

					"x" : +50,
					"y" : 105-10,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",
					"text" : "Reddet",

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
			),
		},
	),
}