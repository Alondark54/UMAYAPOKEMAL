import uiScriptLocale

XROOT = "d:/ymir work/ui/game/windows/"
BKDEPO = "d:/ymir work/ui/exinventory/"

window = {"name" : "InventoryWindow", "x" : (SCREEN_WIDTH/2)-(176/2) , "y" : 200,"style" : ("movable", "float",), "width" : 176, "height" : 410,
	"children" :(
		## Inventory, Equipment Slots
		{"name" : "board", "type" : "board", "style" : ("attach",), "x" : 12-12, "y" : 0, "width" : 176, "height" : 410,
			"children" :(
				{"name" : "TitleBar", "type" : "titlebar", "style" : ("attach",), "x" : 37, "y" : 7, "width" : 130, "color" : "yellow","children" :( { "name":"TitleName", "type":"text", "x":59, "y":3, "text":"BK Depo", "text_horizontal_align":"center" }, ),},
				{"name" : "Inventory_Tab_1", "type" : "radio_button", "x" : 14+(30*0), "y" : 347, "default_image" : BKDEPO+"bk_env_3.tga", "over_image" : BKDEPO+"bk_env_1.tga", "down_image" : BKDEPO+"bk_env_2.tga" },
				{"name" : "Inventory_Tab_2", "type" : "radio_button", "x" : 14+(30*1), "y" : 347, "default_image" : BKDEPO+"tas_env_3.tga", "over_image" : BKDEPO+"tas_env_1.tga", "down_image" : BKDEPO+"tas_env_2.tga" },
				{"name" : "Inventory_Tab_3", "type" : "radio_button", "x" : 14+(30*2), "y" : 347, "default_image" : BKDEPO+"yuk_env_3.tga", "over_image" : BKDEPO+"yuk_env_1.tga", "down_image" : BKDEPO+"yuk_env_2.tga" },
				{"name" : "Inventory_Tab_4", "type" : "radio_button", "x" : 14+(30*3), "y" : 347, "default_image" : BKDEPO+"sandik_env_3.tga", "over_image" : BKDEPO+"sandik_env_1.tga", "down_image" : BKDEPO+"sandik_env_2.tga" },
				{"name" : "Inventory_Tab_5", "type" : "radio_button", "x" : 14+(30*4), "y" : 347, "default_image" : BKDEPO+"cicek_env_3.tga", "over_image" : BKDEPO+"cicek_env_1.tga", "down_image" : BKDEPO+"cicek_env_2.tga" },

				{"name" : "ItemSlot", "type" : "grid_table", "x" : 8, "y" : 30, "start_index" : 0, "x_count" : 5, "y_count" : 9, "x_step" : 32, "y_step" : 32, "image" : "d:/ymir work/ui/public/Slot_Base.sub"},

				{"name" : "Inventory_Tab_01", "type" : "radio_button", "x" : 10+(39*0), "y" : 320, "default_image" : XROOT+"tab_button_large_half_01.sub", "over_image" : XROOT+"tab_button_large_half_02.sub", "down_image" : XROOT+"tab_button_large_half_03.sub", "tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_1,"children" :({"name" : "Inventory_Tab_01_Print", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "I", }, ),},
				{"name" : "Inventory_Tab_02", "type" : "radio_button", "x" : 10+(39*1), "y" : 320, "default_image" : XROOT+"tab_button_large_half_01.sub", "over_image" : XROOT+"tab_button_large_half_02.sub", "down_image" : XROOT+"tab_button_large_half_03.sub", "tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_2,"children" :({"name" : "Inventory_Tab_02_Print", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "II",}, ),},
				{"name" : "Inventory_Tab_03", "type" : "radio_button", "x" : 10+(39*2), "y" : 320, "default_image" : XROOT+"tab_button_large_half_01.sub", "over_image" : XROOT+"tab_button_large_half_02.sub", "down_image" : XROOT+"tab_button_large_half_03.sub", "tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_3,"children" :({"name" : "Inventory_Tab_03_Print", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "III", }, ),},
				{"name" : "Inventory_Tab_04", "type" : "radio_button", "x" : 10+(39*3), "y" : 320, "default_image" : XROOT+"tab_button_large_half_01.sub", "over_image" : XROOT+"tab_button_large_half_02.sub", "down_image" : XROOT+"tab_button_large_half_03.sub", "tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_4,"children" :({"name" : "Inventory_Tab_04_Print", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "IV", }, ),},

				{"name" : "btnDerleTopla","type" : "button","x" : 5,"y" : 2,"tooltip_text" : "Düzenle", "default_image" : BKDEPO+"derle_1.tga","over_image" : BKDEPO+"derle_2.tga","down_image" : BKDEPO+"derle_3.tga",},
				{"name" : "BlackBoard", "type" : "thinboard_circle", "x" : 7, "y" : 384, "width" : 162, "height" : 19.5, },
				## Item Slot
			),
		},
	),
}
