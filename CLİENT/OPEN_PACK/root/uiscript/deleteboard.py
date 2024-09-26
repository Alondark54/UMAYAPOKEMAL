import localeInfo
BOARD_WIDTH = 416 - 64
BOARD_HEIGHT = 321+50 #159

window = {
	"name" : "deleteBoard",

	"x" : 45,
	"y" : SCREEN_HEIGHT / 2 - 50,

	"style" : ("movable", "float",),

	"width"  : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),
			
			"x" : 0,
			"y" : 0,
			
			"width"  : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,
			"title" : "Otomatik Sat",
			# "title" : localeInfo.copkutusu,

			"children" :
			(


				{
					"name" : "top_bggg",
					"type" : "thinboard_circle",
					"x" : 13, 
					"y" : 30,
					"width" : BOARD_WIDTH - 28, 
					"height" : 25,
					"children": 
					(
						{
							"name" : "info_text",
							"type" : "text",
							# "align": "center",
							"y" : 5,
							"x" : 40,  
							"text" : "Bu pencereye eklenen esyalar otomatik olarak SATILIR",
							
						},
					),
				},

				{
					"name" : "TrashSlot",
					"type" : "grid_table",
					"image" : "d:/ymir work/ui/public/Slot_Base.sub",
					"x" : 15,
					"y" : 33 + 30,
					"start_index" : 0,
					"x_count" : 10,
					"y_count" : 8,
					"x_step" : 32,
					"y_step" : 32,
				},



				{
					"name" : "bggg",
					"type" : "thinboard_circle",
					"x" : 13, 
					"y" : BOARD_HEIGHT-32-20,
					"width" : BOARD_WIDTH - 28, 
					"height" : 40,
					"children": 
					(

						
						
						{
							"name" : "silcheck",
							"type" : "radio_button",
							"x" : 85,  
							"y" : 15,
									
							"text" : "Ac",
									
							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
							
						},
						
						# {
							# "name" : "siltext",
							# "type" : "text",
							# "x" : 60,  
							# "y" : 17,
									
							# "text" : "Sil",
							
						# },
						
						
						{
							"name" : "satcheck",
							"type" : "radio_button",
							"x" : 45+150,  
							"y" : 15,
									
							"text" : "Kapat",
									
							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
							
						},
					),
				},
			),
		},
	),
}
