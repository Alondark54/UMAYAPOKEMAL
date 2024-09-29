import uiScriptLocale

LOCALE_PATH = uiScriptLocale.LOGIN_PATH

ID_LIMIT_COUNT = 19
PW_LIMIT_COUNT = 16

INPUT_NAME_X = 20
INPUT_NAME_Y = 40

INPUT_PASSWORD_X = 20
INPUT_PASSWORD_Y = 90

## Channels
CHANNEL_1_X = 20
CHANNEL_1_Y = 130

CHANNEL_2_X = 105
CHANNEL_2_Y = 130

CHANNEL_3_X = 190
CHANNEL_3_Y = 130

CHANNEL_4_X = 275
CHANNEL_4_Y = 130

## Accounts
ACCOUNT_1_X = 25
ACCOUNT_1_Y = 220

ACCOUNT_2_X = 195
ACCOUNT_2_Y = 220

ACCOUNT_3_X = 25
ACCOUNT_3_Y = 280

ACCOUNT_4_X = 195
ACCOUNT_4_Y = 280

window = {
	"name" : "LoginWindow",
	"sytle" : ("movable",),

	"x" : 0,
	"y" : 0,

	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,

	"children" :
	(
		## Board
		{
			"name" : "background", "type" : "expanded_image", "x" : 0, "y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1920.0, "y_scale" : float(SCREEN_HEIGHT) / 1080.0,
			"image" : "locale/general/ui/login/background.jpg",
		},
		{
			"name":"LogoSerwera",
			"type":"image",
			"x":0,
			"y":-150,
			"horizontal_align":"center",
			"vertical_align":"center",
			"image":"logo.tga",
		},
		## ConnectBoard
		{
			"name" : "ConnectBoard",
			"type" : "border_a",

			"x" : (SCREEN_WIDTH - 208) / 2,
			"y" : (SCREEN_HEIGHT - 520),
			
			"width" : 208,
			"height" : 30,

			"children" :
			(
				{
					"name" : "ConnectName",
					"type" : "text",

					"x" : -10,
					"y" : 10,
					
					"horizontal_align" : "center",
					
					"text_vertical_align" : "center",
					"text_horizontal_align" : "center",

					"text" : uiScriptLocale.LOGIN_DEFAULT_SERVERADDR,
				},
				{
					"name" : "SelectConnectButton",
					"type" : "button",

					"x" : 189,
					"y" : 3,

					"default_image" : "d:/ymir work/ui/game/monster_card/button/down_camera/down_camera_button_default.sub",
					"over_image" : "d:/ymir work/ui/game/monster_card/button/down_camera/down_camera_button_over.sub",
					"down_image" : "d:/ymir work/ui/game/monster_card/button/down_camera/down_camera_button_down.sub",
				},
			),
		},
		

		## LoginBoard
		{
			"name" : "LoginBoard",
			"type" : "border_a",

			"x" : (SCREEN_WIDTH - 350) / 2,
			"y" : (SCREEN_HEIGHT - 500),

			"width" : 350,
			"height" : 345,

			"children" :
			(
				{
					"name" : "ID_Name",
					"type" : "border_a",
				
					"x" : INPUT_NAME_X + 10,
					"y" : INPUT_NAME_Y - 20,
				
					"width" : 120,
					"height" : 20,
					
					"children" :
					(
						{
							"name" : "ID_Name_Text",
							"type" : "text",
				
							"x" : 7,
							"y" : 3,
		
							"text" : uiScriptLocale.LOGIN_INTERFACE_USER_NAME,
						},
					),
				},	
				{
					"name" : "ID_BackLine",
					"type" : "input",
					"x" : INPUT_NAME_X,
					"y" : INPUT_NAME_Y,
		
					"width" : 160,
					"height" : 21,
		
					"children":
					(
						{
							"name" : "ID_EditLine",
							"type" : "editline",
				
							"x" : 5,
							"y" : 4,
				
							"width" : 160,
							"height" : 21,
				
							"input_limit" : ID_LIMIT_COUNT,
							"enable_codepage" : 0,
				
							"r" : 1.0,
							"g" : 1.0,
							"b" : 1.0,
							"a" : 1.0,
						},
					),
				},
				
				{
					"name" : "Password_Name",
					"type" : "border_a",
				
					"x" : INPUT_PASSWORD_X + 10,
					"y" : INPUT_PASSWORD_Y - 20,
				
					"width" : 120,
					"height" : 20,
					
					"children" :
					(
						{
							"name" : "Password_Name_Text",
							"type" : "text",
				
							"x" : 7,
							"y" : 3,
		
							"text" : uiScriptLocale.LOGIN_INTERFACE_PASSWORD,
						},
					),
				},
				{
					"name" : "Password_BackLine",
					"type" : "input",
					"x" : INPUT_PASSWORD_X,
					"y" : INPUT_PASSWORD_Y,
		
					"width" : 160,
					"height" : 21,
		
					"children" :
					(
						{
							"name" : "Password_EditLine",
							"type" : "editline",
				
							"x" : 5,
							"y" : 4,
				
							"width" : 160,
							"height" : 21,
				
							"input_limit" : PW_LIMIT_COUNT,
							"secret_flag" : 1,
							"enable_codepage" : 0,
				
							"r" : 1.0,
							"g" : 1.0,
							"b" : 1.0,
							"a" : 1.0,
						},
					),
				},
				{
					"name" : "LoginButton",
					"type" : "button",
		
					"x" : 210,
					"y" : 38,
		
					"default_image" : "d:/ymir work/ui/minigame/yutnori/yut_throw_button_default.sub",
					"over_image" : "d:/ymir work/ui/minigame/yutnori/yut_throw_button_over.sub",
					"down_image" : "d:/ymir work/ui/minigame/yutnori/yut_throw_button_down.sub",
					"text" : uiScriptLocale.LOGIN_INTERFACE_ATTEMPT_CONNECT,
					#"text" : "Conecteazã-te",
				},
				{
					"name" : "LoginExitButton",
					"type" : "button",
		
					"x" : 210,
					"y" : 89,
		
					"default_image" : "d:/ymir work/ui/minigame/yutnori/yut_throw_button_default.sub",
					"over_image" : "d:/ymir work/ui/minigame/yutnori/yut_throw_button_over.sub",
					"down_image" : "d:/ymir work/ui/minigame/yutnori/yut_throw_button_down.sub",
		            
					"text" : uiScriptLocale.LOGIN_EXIT,
				},
				
				{
					"name" : "Channel1_Selected_Image",
					"type" : "border_a",
				
					"x" : CHANNEL_1_X - 5,
					"y" : CHANNEL_1_Y - 5,
					
					"width" : 64,
					"height" : 81,
				},
				{
					"name" : "Channel1_Status_Back1",
					"type" : "image",
				
					"x" : CHANNEL_1_X,
					"y" : CHANNEL_1_Y,
					
					"image" : LOCALE_PATH + "channel/channel_status_bg.tga",
				
					"children" :
					(
						{
							"name" : "Channel1_Status",
							"type" : "image",
				
							"x" : 1,
							"y" : 1,
							
							"image" : LOCALE_PATH + "channel/channel_normal.tga",
						},
					),
				},
				{
					"name" : "Channel1_Button",
					"type" : "button",
		
					"x" : CHANNEL_1_X,
					"y" : CHANNEL_1_Y + 17,
		
					"default_image" : LOCALE_PATH + "channel/channel_button_01.tga",
					"over_image" : LOCALE_PATH + "channel/channel_button_02.tga",
					"down_image" : LOCALE_PATH + "channel/channel_button_03.tga",
		
					"text" : "CH1",
				},
				{
					"name" : "Channel1_Status_Back2",
					"type" : "input",
					
					"x" : CHANNEL_1_X + 1,
					"y" : CHANNEL_1_Y + 50,
		
					"width" : 50,
					"height" : 21,
		
					"children" :
					(
						{
							"name" : "Channel1_Status_Text",
							"type" : "text",
							
							"vertical_align" : "center",
							"horizontal_align" : "center",
							
							"text_vertical_align" : "center",
							"text_horizontal_align" : "center",
				
							"x" : 0,
							"y" : 0,
		
							"text" : uiScriptLocale.LOGIN_INTERFACE_ATTEMPT_NORMAL,
						},
					),
				},	
				{
					"name" : "Channel2_Selected_Image",
					"type" : "border_a",
				
					"x" : CHANNEL_2_X - 5,
					"y" : CHANNEL_2_Y - 5,
					
					"width" : 64,
					"height" : 81,
				},
				{
					"name" : "Channel2_Status_Back1",
					"type" : "image",
				
					"x" : CHANNEL_2_X,
					"y" : CHANNEL_2_Y,
					
					"image" : LOCALE_PATH + "channel/channel_status_bg.tga",
				
					"children" :
					(
						{
							"name" : "Channel2_Status",
							"type" : "image",
				
							"x" : 1,
							"y" : 1,
							
							"image" : LOCALE_PATH + "channel/channel_normal.tga",
						},
					),
				},
				{
					"name" : "Channel2_Button",
					"type" : "button",
		
					"x" : CHANNEL_2_X,
					"y" : CHANNEL_2_Y + 17,
		
					"default_image" : LOCALE_PATH + "channel/channel_button_01.tga",
					"over_image" : LOCALE_PATH + "channel/channel_button_02.tga",
					"down_image" : LOCALE_PATH + "channel/channel_button_03.tga",
		
					"text" : "CH2",
				},
				{
					"name" : "Channel2_Status_Back2",
					"type" : "input",
					
					"x" : CHANNEL_2_X + 1,
					"y" : CHANNEL_2_Y + 50,
		
					"width" : 50,
					"height" : 21,
		
					"children" :
					(
						{
							"name" : "Channel2_Status_Text",
							"type" : "text",
							
							"vertical_align" : "center",
							"horizontal_align" : "center",
							
							"text_vertical_align" : "center",
							"text_horizontal_align" : "center",
				
							"x" : 0,
							"y" : 0,
		
							"text" : uiScriptLocale.LOGIN_INTERFACE_ATTEMPT_NORMAL,
						},
					),
				},
				{
					"name" : "Channel3_Selected_Image",
					"type" : "border_a",
				
					"x" : CHANNEL_3_X - 5,
					"y" : CHANNEL_3_Y - 5,
					
					"width" : 64,
					"height" : 81,
				},
				{
					"name" : "Channel3_Status_Back1",
					"type" : "image",
				
					"x" : CHANNEL_3_X,
					"y" : CHANNEL_3_Y,
					
					"image" : LOCALE_PATH + "channel/channel_status_bg.tga",
				
					"children" :
					(
						{
							"name" : "Channel3_Status",
							"type" : "image",
				
							"x" : 1,
							"y" : 1,
							
							"image" : LOCALE_PATH + "channel/channel_normal.tga",
						},
					),
				},
				{
					"name" : "Channel3_Button",
					"type" : "button",
		
					"x" : CHANNEL_3_X,
					"y" : CHANNEL_3_Y + 17,
		
					"default_image" : LOCALE_PATH + "channel/channel_button_01.tga",
					"over_image" : LOCALE_PATH + "channel/channel_button_02.tga",
					"down_image" : LOCALE_PATH + "channel/channel_button_03.tga",
		
					"text" : "CH3",
				},
				{
					"name" : "Channel3_Status_Back2",
					"type" : "input",
					
					"x" : CHANNEL_3_X + 1,
					"y" : CHANNEL_3_Y + 50,
		
					"width" : 50,
					"height" : 21,
		
					"children" :
					(
						{
							"name" : "Channel3_Status_Text",
							"type" : "text",
							
							"vertical_align" : "center",
							"horizontal_align" : "center",
							
							"text_vertical_align" : "center",
							"text_horizontal_align" : "center",
				
							"x" : 0,
							"y" : 0,
		
							"text" : uiScriptLocale.LOGIN_INTERFACE_ATTEMPT_NORMAL,
						},
					),
				},
				{
					"name" : "Channel4_Selected_Image",
					"type" : "border_a",
				
					"x" : CHANNEL_4_X - 5,
					"y" : CHANNEL_4_Y - 5,
					
					"width" : 64,
					"height" : 81,
				},
				{
					"name" : "Channel4_Status_Back1",
					"type" : "image",
				
					"x" : CHANNEL_4_X,
					"y" : CHANNEL_4_Y,
					
					"image" : LOCALE_PATH + "channel/channel_status_bg.tga",
				
					"children" :
					(
						{
							"name" : "Channel4_Status",
							"type" : "image",
				
							"x" : 1,
							"y" : 1,
							
							"image" : LOCALE_PATH + "channel/channel_normal.tga",
						},
					),
				},
				{
					"name" : "Channel4_Button",
					"type" : "button",
		
					"x" : CHANNEL_4_X,
					"y" : CHANNEL_4_Y + 17,
		
					"default_image" : LOCALE_PATH + "channel/channel_button_01.tga",
					"over_image" : LOCALE_PATH + "channel/channel_button_02.tga",
					"down_image" : LOCALE_PATH + "channel/channel_button_03.tga",
		
					"text" : "CH4",
				},
				{
					"name" : "Channel4_Status_Back2",
					"type" : "input",
					
					"x" : CHANNEL_4_X + 1,
					"y" : CHANNEL_4_Y + 50,
		
					"width" : 50,
					"height" : 21,
		
					"children" :
					(
						{
							"name" : "Channel4_Status_Text",
							"type" : "text",
							
							"vertical_align" : "center",
							"horizontal_align" : "center",
							
							"text_vertical_align" : "center",
							"text_horizontal_align" : "center",
				
							"x" : 0,
							"y" : 0,
		
							"text" : uiScriptLocale.LOGIN_INTERFACE_ATTEMPT_NORMAL,
						},
					),
				},
				{
					"name" : "Account1_Name_Back",
					"type" : "input",
					
					"x" : ACCOUNT_1_X,
					"y" : ACCOUNT_1_Y,
		
					"width" : 128,
					"height" : 21,
		
					"children" :
					(
						{
							"name" : "Account1_Name_Text",
							"type" : "text",
							
							"vertical_align" : "center",
							"horizontal_align" : "center",
							
							"text_vertical_align" : "center",
							"text_horizontal_align" : "center",
				
							"x" : 0,
							"y" : 0,
		
							"text" : uiScriptLocale.LOGIN_INTERFACE_CLEAR,
						},
					),
				},
				{
					"name" : "Account1_Add_Button",
					"type" : "button",
		
					"x" : ACCOUNT_1_X,
					"y" : ACCOUNT_1_Y + 27,
		
					"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
				},
				{
					"name" : "Account1_Remove_Button",
					"type" : "button",
		
					"x" : ACCOUNT_1_X + 70,
					"y" : ACCOUNT_1_Y + 27,
		
					"default_image" : "d:/ymir work/ui/public/canclebutton00.sub",
					"over_image" : "d:/ymir work/ui/public/canclebutton01.sub",
					"down_image" : "d:/ymir work/ui/public/canclebutton02.sub",
				},
				
				{
					"name" : "Account2_Name_Back",
					"type" : "input",
					
					"x" : ACCOUNT_2_X,
					"y" : ACCOUNT_2_Y,
		
					"width" : 128,
					"height" : 21,
		
					"children" :
					(
						{
							"name" : "Account2_Name_Text",
							"type" : "text",
							
							"vertical_align" : "center",
							"horizontal_align" : "center",
							
							"text_vertical_align" : "center",
							"text_horizontal_align" : "center",
				
							"x" : 0,
							"y" : 0,
		
							"text" : uiScriptLocale.LOGIN_INTERFACE_CLEAR,
						},
					),
				},
				{
					"name" : "Account2_Add_Button",
					"type" : "button",
		
					"x" : ACCOUNT_2_X,
					"y" : ACCOUNT_2_Y + 27,
		
					"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
				},
				{
					"name" : "Account2_Remove_Button",
					"type" : "button",
		
					"x" : ACCOUNT_2_X + 70,
					"y" : ACCOUNT_2_Y + 27,
		
					"default_image" : "d:/ymir work/ui/public/canclebutton00.sub",
					"over_image" : "d:/ymir work/ui/public/canclebutton01.sub",
					"down_image" : "d:/ymir work/ui/public/canclebutton02.sub",
				},
				
				{
					"name" : "Account3_Name_Back",
					"type" : "input",
					
					"x" : ACCOUNT_3_X,
					"y" : ACCOUNT_3_Y,
		
					"width" : 128,
					"height" : 21,
		
					"children" :
					(
						{
							"name" : "Account3_Name_Text",
							"type" : "text",
							
							"vertical_align" : "center",
							"horizontal_align" : "center",
							
							"text_vertical_align" : "center",
							"text_horizontal_align" : "center",
				
							"x" : 0,
							"y" : 0,
		
							"text" : uiScriptLocale.LOGIN_INTERFACE_CLEAR,
						},
					),
				},
				{
					"name" : "Account3_Add_Button",
					"type" : "button",
		
					"x" : ACCOUNT_3_X,
					"y" : ACCOUNT_3_Y + 27,
		
					"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
				},
				{
					"name" : "Account3_Remove_Button",
					"type" : "button",
		
					"x" : ACCOUNT_3_X + 70,
					"y" : ACCOUNT_3_Y + 27,
		
					"default_image" : "d:/ymir work/ui/public/canclebutton00.sub",
					"over_image" : "d:/ymir work/ui/public/canclebutton01.sub",
					"down_image" : "d:/ymir work/ui/public/canclebutton02.sub",
				},
				
				{
					"name" : "Account4_Name_Back",
					"type" : "input",
					
					"x" : ACCOUNT_4_X,
					"y" : ACCOUNT_4_Y,
		
					"width" : 128,
					"height" : 21,
		
					"children" :
					(
						{
							"name" : "Account4_Name_Text",
							"type" : "text",
							
							"vertical_align" : "center",
							"horizontal_align" : "center",
							
							"text_vertical_align" : "center",
							"text_horizontal_align" : "center",
				
							"x" : 0,
							"y" : 0,
		
							"text" : uiScriptLocale.LOGIN_INTERFACE_CLEAR,
						},
					),
				},
				{
					"name" : "Account4_Add_Button",
					"type" : "button",
		
					"x" : ACCOUNT_4_X,
					"y" : ACCOUNT_4_Y + 27,
		
					"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
				},
				{
					"name" : "Account4_Remove_Button",
					"type" : "button",
		
					"x" : ACCOUNT_4_X + 70,
					"y" : ACCOUNT_4_Y + 27,
		
					"default_image" : "d:/ymir work/ui/public/canclebutton00.sub",
					"over_image" : "d:/ymir work/ui/public/canclebutton01.sub",
					"down_image" : "d:/ymir work/ui/public/canclebutton02.sub",
				},
			),
		},
	),
}
