import uiScriptLocale

window = {
	"name" : "FastInventoryWindow",
	"x" : 0,
	"y" : 0,
	"width" : 45,
	"height" : 225,
	"children" :({
			"name" : "FastInventoryLayer",
			"type" : "invisibleboard",
			"style" : ("attach",),
			"x" : 0,
			"y" : 0,
			"width" : 45,
			"height" : 225,
			"children" :(
				{"name" : "Menu_1","type" : "button","x" : 1,"y" : 10,"default_image" : "locale/tr/ui/buttons/biyolog1.tga","over_image" : "locale/tr/ui/buttons/biyolog2.tga","down_image" : "locale/tr/ui/buttons/biyolog1.tga",},
				{"name" : "Menu_2","type" : "button","x" : 1,"y" : 10 + 25,"default_image" : "locale/tr/ui/buttons/hizlisat1.tga","over_image" : "locale/tr/ui/buttons/hizlisat2.tga","down_image" : "locale/tr/ui/buttons/hizlisat1.tga",},
			),
		},
	),
}
