import localeInfo

MAINBOARD_WIDTH = 610
MAINBOARD_HEIGHT = 405

LEFTBOARD_WIDTH = 291
LEFTBOARD_HEIGHT = 370
LEFTBOARD_X = 13
LEFTBOARD_Y = 36

window = {
	"name" : "EventInfoWindow",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : MAINBOARD_WIDTH,
	"height" : MAINBOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "EventBoard",
			"type" : "board",
			"style" : ("attach", "ltr"),
			"x" : 0,
			"y" : 0,
			"width" : MAINBOARD_WIDTH,
			"height" : MAINBOARD_HEIGHT,
			"children" :
			(
				{
					"name" : "EventBoardTitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 6,
					"y" : 7,
					"width" : MAINBOARD_WIDTH - 13,
					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",

							"x" : 0,
							"y" : -2,

							"text": "Umay2Games - OYUNCU SIRALAMASI",
							"all_align":"center"
						},
					),
				},

				{
					"name" : "EventButtonThinBoard",
					"type" : "thinboard",
					"style" : ("not_pick",),
					"x" : LEFTBOARD_X - 4,
					"y" : LEFTBOARD_Y - 4,
					"width" : MAINBOARD_WIDTH - 20,
					"height" : MAINBOARD_HEIGHT - 42,
					"children" :
					(
						{
							"name" : "LeftThinboard",
							"type" : "image",
							"style" : ("not_pick",),
							"x" : 10,
							"y" : 10,
							"image" : "d:/ymir work/ui/game/rank/left_thin.tga",
							"children" :
							(
								{
									"name" : "LxistBox",
									"type" : "listboxex",
									"x" : 10,
									"y" : 10,
									"width" : 400,
									"height" : 42*9,
									#"viewcount" : 4,
								},
							
							),
						},
						

						{
							"name" : "RightThinboard",
							"type" : "image",
							"style" : ("not_pick",),
							"x" : 180 + 10,
							"y" : 10,
							"image" : "d:/ymir work/ui/game/rank/right_thin.tga",
							"children" :
							(
								{
									"name" : "RightTitle",
									"type" : "image",
									"style" : ("not_pick",),
									"x" : 12,
									"y" : 10,
									"image" : "d:/ymir work/ui/game/rank/right_title.tga",
								},
								
								{
									"name" : "RightMenu",
									"type" : "image",
									"style" : ("not_pick",),
									"x" : 12,
									"y" : 10+23+3,
									"image" : "d:/ymir work/ui/game/rank/right_list.tga",
									"children":
									(
										{
											"name" : "ListBoxNEW",
											"type" : "listboxex",
											"style" : ("not_pick",),
											"x" : 0,
											"y" : 0,
											"width" : 400,
											"height" : 38*9,
											#"viewcount" : 4,
										},
									),
								},
							),
						},
					),
				},

				
			),
		},
	),
}
