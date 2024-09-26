import uiScriptLocale
import app

INTERFACE_PATH = "d:/ymir work/ui/gui_interface/"

window = {
	"name" : "LoadingWindow",
	"sytle" : ("movable","ltr",),

	"x" : 0,
	"y" : 0,

	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,

	"children" :
	(
		{
			"name":"ErrorMessage", 
			"type":"text", "x":10, "y":10, 
			"text": uiScriptLocale.LOAD_ERROR, 
		},
		
		{
			"name" : "GageBoard",
			"type" : "window",
			"x" : float(SCREEN_WIDTH) / 2  - 180,
			"y" : float(SCREEN_HEIGHT) / 2 - 180,
			"width" : 380, 
			"height": 380,

			"children" :
			(
				{
					"name" : "BackGage",
					"type" : "ani_image",


					"x" : 0,
					"y" : 0,

					"delay" : 3,

					"images" :
					(
						"locale/tr/ui/loading/load/1.png",
						"locale/tr/ui/loading/load/2.png",
						"locale/tr/ui/loading/load/3.png",
						"locale/tr/ui/loading/load/4.png",
						"locale/tr/ui/loading/load/5.png",
						"locale/tr/ui/loading/load/6.png",
						"locale/tr/ui/loading/load/7.png",
						"locale/tr/ui/loading/load/8.png",
						"locale/tr/ui/loading/load/9.png",
						"locale/tr/ui/loading/load/10.png",
						"locale/tr/ui/loading/load/11.png",
						"locale/tr/ui/loading/load/12.png",
						"locale/tr/ui/loading/load/14.png",
						"locale/tr/ui/loading/load/15.png",
						"locale/tr/ui/loading/load/16.png",
						"locale/tr/ui/loading/load/17.png",
						"locale/tr/ui/loading/load/18.png",
						"locale/tr/ui/loading/load/19.png",
						"locale/tr/ui/loading/load/20.png",
						"locale/tr/ui/loading/load/21.png",
						"locale/tr/ui/loading/load/22.png",
						"locale/tr/ui/loading/load/24.png",
						"locale/tr/ui/loading/load/25.png",
						"locale/tr/ui/loading/load/26.png",
						"locale/tr/ui/loading/load/27.png",
						"locale/tr/ui/loading/load/28.png",
						"locale/tr/ui/loading/load/29.png",
						"locale/tr/ui/loading/load/30.png",
						"locale/tr/ui/loading/load/31.png",
						"locale/tr/ui/loading/load/32.png",
						"locale/tr/ui/loading/load/34.png",
						"locale/tr/ui/loading/load/35.png",
						"locale/tr/ui/loading/load/36.png",
						"locale/tr/ui/loading/load/37.png",
						"locale/tr/ui/loading/load/38.png",
						"locale/tr/ui/loading/load/39.png",
						"locale/tr/ui/loading/load/40.png",
						"locale/tr/ui/loading/load/41.png",
						"locale/tr/ui/loading/load/42.png",
						"locale/tr/ui/loading/load/44.png",
						"locale/tr/ui/loading/load/45.png",
						"locale/tr/ui/loading/load/46.png",
						"locale/tr/ui/loading/load/47.png",
						"locale/tr/ui/loading/load/48.png",
						"locale/tr/ui/loading/load/49.png",
						"locale/tr/ui/loading/load/51.png",
						"locale/tr/ui/loading/load/52.png",
						"locale/tr/ui/loading/load/53.png",
						"locale/tr/ui/loading/load/54.png",
						"locale/tr/ui/loading/load/55.png",
						"locale/tr/ui/loading/load/56.png",
						"locale/tr/ui/loading/load/57.png",
						"locale/tr/ui/loading/load/58.png",
						"locale/tr/ui/loading/load/59.png",
						
					)
				},
			),
		},
	),
}
