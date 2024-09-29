import uiScriptLocale
import item
import app
import localeInfo
import constInfo

LOCALE_PATH = "d:/ymir work/ui/privatesearch/"
GOLD_COLOR	= 0xFFFEE3AE

BOARD_WIDTH = 445
window = {
	"name" : "firsatekran",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : 350+139,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : 350+139,
				
			"title" : "Fýrsat Penceresi",
			
			"children" :
			(

				
				{
					"name" : "RefreshSymbol",
					"type" : "ani_image",

					"x" : 20, "y" : 35+139,

					"images": (
						"d:/ymir work/ui/sales/loading/loading_image0.png",
						"d:/ymir work/ui/sales/loading/loading_image1.png",
						"d:/ymir work/ui/sales/loading/loading_image2.png",
						"d:/ymir work/ui/sales/loading/loading_image3.png",
						"d:/ymir work/ui/sales/loading/loading_image4.png",
						"d:/ymir work/ui/sales/loading/loading_image5.png",
						"d:/ymir work/ui/sales/loading/loading_image6.png",
						"d:/ymir work/ui/sales/loading/loading_image7.png",
						"d:/ymir work/ui/sales/loading/loading_image8.png",
						"d:/ymir work/ui/sales/loading/loading_image9.png",
						"d:/ymir work/ui/sales/loading/loading_image10.png",
						"d:/ymir work/ui/sales/loading/loading_image11.png",
						"d:/ymir work/ui/sales/loading/loading_image12.png",
						"d:/ymir work/ui/sales/loading/loading_image13.png",
						"d:/ymir work/ui/sales/loading/loading_image14.png",
						"d:/ymir work/ui/sales/loading/loading_image15.png",
						"d:/ymir work/ui/sales/loading/loading_image16.png",
						"d:/ymir work/ui/sales/loading/loading_image17.png",
					),
				},
				
				{
					"name" : "big_board",
					"type" : "thinboard_circle",
					"x" : 20,
					"y" : 35,
					"width" : 408,
					"height" : 139,
					
					'children': 
					(	
						{
							"name" : "firstbg",
							"type" : "image",
							"x" : 2,
							"y" : 2,
							"image" : "sales/sales_bg.png",
						},
					),
					
				},
				
				
				{
					"name" : "bg1",
					"type" : "thinboard_circle",
					"x" : 20,
					"y" : 35+139,
					"width" : 200,
					"height" : 300,
					
				},	
				
				
				{
					"name" : "bg2",
					"type" : "thinboard_circle",
					"x" : 20+222,
					"y" : 35+139,
					"width" : 186,
					"height" : 300,
					
					'children': 
					(	
						{
							"name" : "baslikbg",
							"type" : "image",
							"x" : 4,
							"y" : 4,
							"image" : "d:/ymir work/ui/sales/baslik.tga",
							'children': 
							(	
								{
									'name': 'basliktext',
									'type': 'text',
									'text': 'Test',
									'horizontal_align': 'center',
									'text_horizontal_align': 'center',
									'x': 0,
									'y': 3,
								},
							),
						},

						{
							"name" : "reward",
							"type" : "grid_table",

							"start_index" : 0,

							"x" : 13,
							"y" : 35,

							"x_count" : 5,
							"y_count" : 4,
							"x_step" : 32,
							"y_step" : 32,
							"x_blank" : 0,
							"y_blank" : 0,

							"image" : "d:/ymir work/ui/public/slot_base.sub",
						},
						
						{
							"name" : "baslik2bg",
							"type" : "image",
							"x" : 4,
							"y" : 175,
							"image" : "d:/ymir work/ui/sales/baslik.tga",
							'children': 
							(	
								{
									'name': 'girisitemtext',
									'type': 'text',
									'text': "",
									'horizontal_align': 'center',
									'text_horizontal_align': 'center',
									'x': 0,
									'y': 3,
								},
							),
						},
						
						{
							"name" : "firsatbg",
							"type" : "image",
							"x" : 19,
							"y" : 205,
							"image" : "d:/ymir work/ui/pet/pet_incu_001.tga",
							'children': 
							(	
								{
									'name': 'firsattext',
									'type': 'text',
									'text': "Fýrsat",
									'horizontal_align': 'center',
									'text_horizontal_align': 'center',
									'x': 0,
									'y': 7,
								},
								{
									'name': 'fiyattext',
									'type': 'text',
									'text': "",
									'horizontal_align': 'center',
									'text_horizontal_align': 'center',
									'x': 0,
									'y': 28,
								},
							),
						},
						
						{
							"name" : "buy",
							"type" : "button",
							"x" : 19,
							"y" : 268,
							
							"default_image" : "d:/ymir work/ui/buton/1.tga",
							"over_image" : "d:/ymir work/ui/buton/2.tga",
							"down_image" : "d:/ymir work/ui/buton/3.tga",
							"text" : "Satýn Al",
						},
						
						
					),
				},	

				{
					"name" : "scroll",
					"type" : "scrollbar",

					"x" : 225,
					"y" : 40+139,
					"size" : 36 * 7 + 5 * 8,
				},		
			),
		},
	),
}
