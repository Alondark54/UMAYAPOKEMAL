MAIN_WIDTH = 425
MAIN_HEIGHT = 180

window = {
	"name" : "BotControlWindow",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : MAIN_WIDTH,
	"height" : MAIN_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : MAIN_WIDTH,
			"height" : MAIN_HEIGHT,

			"children" :
			(
				# Title
				# {
					# "name" : "titlebar",
					# "type" : "titlebar",
					# "style" : ("attach",),

					# "x" : 8,
					# "y" : 8,

					# "width" : MAIN_WIDTH-15,
					# "color" : "gray",

					# "children" :
					# (
						# { "name":"titlename", "type":"text", "x":0, "y":3, 
						# "text" : "Bot Kontrol", 
						# "horizontal_align":"center", "text_horizontal_align":"center" },
					# ),
				# },

				{
					"name" : "itemName",
					"type" : "text",

					"x" : 0,
					"y" : 10,
					"horizontal_align":"center", "text_horizontal_align":"center",
					"fontsize" : "LARGE",
					"color": 0xFFFEE3AE,
					"text" : "itemname",
				},

				{
					"name" : "ItemSlot",
					"type" : "grid_table",
					"x" : 18,
					"y" : 35,
					"start_index" : 0,
					"x_count" : 9,
					"y_count" : 3,
					"x_step" : 40,
					"y_step" : 32,
					# "x_blank" : 5,
					# "y_blank" : 0,
					"image" : "d:/ymir work/ui/public/Slot_Base.sub",
				},

				{
					"name" : "rofText",
					"type" : "text",

					"x" : 0,
					"y" : 145,
					"horizontal_align":"center", "text_horizontal_align":"center",
					"fontsize" : "LARGE",
					"color": 0xFFFEE3AE,
					"text" : "KALAN: ",
				},


			),
		},
	),
}
