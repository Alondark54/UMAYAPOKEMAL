import uiScriptLocale

BOARD_X = 300
BOARD_Y = 350
THINBOARD_X_OFFSET = 20
THINBOARD_Y_OFFSET = 8
THINBOARD_HEIGHT = 60

window = {
    "name": "LuckyDrawWindow",
    "style": ("movable", "float",),

    "x": SCREEN_WIDTH - 400,
    "y": 70 * 800 / SCREEN_HEIGHT,

    "width": BOARD_X,
    "height": BOARD_Y,

    "children": [
        {
            "name": "Board",
            "type": "board",
            "style": ("attach",),

            "x": 0,
            "y": 0,

            "width": BOARD_X,
            "height": BOARD_Y,

            "children": [
                {
                    "name": "TitleBar",
                    "type": "titlebar",
                    "style": ("attach",),

                    "x": 8,
                    "y": 8,

                    "width": BOARD_X - 16,
                    "color": "red",

                    "children": [
                        {
                            "name": "TitleName",
                            "type": "text",
                            "text": "Þansý Kovala",
                            "horizontal_align": "center",
                            "text_horizontal_align": "center",
                            "x": 0,
                            "y": 3,
                        },
                    ],
                },
                {
                    "type": "window",
                    "x": 15,
                    "y": 35,
                    "width": 32,
                    "height": 96,
                    "children":
                    (
                        {
                        "name": "ItemSlot",
                        "type": "grid_table",
                        "x": 0,
                        "y": 0,
                        "width": 32,
                        "height": 96,
                        "start_index": 0,
                        "x_count": 1,
                        "y_count": 3,
                        "x_step": 32,
                        "y_step": 32,
                        "image" : "d:/ymir work/ui/public/Slot_Base.sub",
                        },
                    )
                },
                {
                    "name": "",
                    "type": "image",
                    "x": 55,
                    "y": 45,
                    "image": "d:/ymir work/ui/public/Parameter_Slot_03.sub"
                },
                {
                    "name": "",
                    "type": "image",
                    "x": 56,
                    "y": 46,
                    "image": "d:/ymir work/ui/game/windows/money_icon.sub"
                },
                {
                    "name": "ParticipationFee",
                    "type": "text",
                    "x": 75,
                    "y": 48,
                    "text": "500.000",
                },
                {
                    "name": "JoinButton",
                    "type": "button",
                    "x": 55,
                    "y": 100,
                    "default_image": "d:/ymir work/ui/public/large_button_01.sub",
                    "over_image": "d:/ymir work/ui/public/large_button_02.sub",
                    "down_image": "d:/ymir work/ui/public/large_button_03.sub",
                    "text": "Katýl",
                },
                {
                    "name": "ClaimButton",
                    "type": "button",
                    "x": 55,
                    "y": 100,
                    "default_image": "d:/ymir work/ui/public/large_button_01.sub",
                    "over_image": "d:/ymir work/ui/public/large_button_02.sub",
                    "down_image": "d:/ymir work/ui/public/large_button_03.sub",
                    "text": "Ödülü Al",
                },
                {
                    "name": "",
                    "type": "image",
                    "x": 155,
                    "y": 36,
                    "image": "d:/ymir work/ui/public/Parameter_Slot_05.sub"
                },
                {
                    "name": "TimerIcon",
                    "type": "image",
                    "x": 158,
                    "y": 37,
                    "image": "d:/ymir work/ui/lucky_draw/stopwatch.tga",
                },
                {
                    "name": "TimerIcon",
                    "type": "image",
                    "x": 158,
                    "y": 37,
                    "image": "d:/ymir work/ui/lucky_draw/stopwatch.tga",
                },
                {
                    "name": "TimerText",
                    "type": "text",
                    "x": 179,
                    "y": 40,
                    "text": "17sa. 46dk. 30sn.",
                },
                {
                    "name": "",
                    "type": "image",
                    "x": 155,
                    "y": 67,
                    "image": "d:/ymir work/ui/public/Parameter_Slot_05.sub"
                },
                {
                    "name": "ParticipantsIcon",
                    "type": "image",
                    "x": 158,
                    "y": 68,
                    "image": "d:/ymir work/ui/lucky_draw/group.tga",
                },
                {
                    "name": "ParticipantsIcon",
                    "type": "image",
                    "x": 158,
                    "y": 68,
                    "image": "d:/ymir work/ui/lucky_draw/group.tga",
                },
                {
                    "name": "ParticipantsCount",
                    "type": "text",
                    "x": 179,
                    "y": 70,
                    "text": "Katilimcilar: 1000/1000",
                },
                {
                    "name": "",
                    "type": "image",
                    "x": 155,
                    "y": 97,
                    "image": "d:/ymir work/ui/public/Parameter_Slot_05.sub"
                },
                {
                    "name": "MyParticipationIcon",
                    "type": "image",
                    "x": 158,
                    "y": 98,
                    "image": "d:/ymir work/ui/lucky_draw/ticket.tga",
                },
                {
                    "name": "MyParticipationIcon",
                    "type": "image",
                    "x": 158,
                    "y": 98,
                    "image": "d:/ymir work/ui/lucky_draw/ticket.tga",
                },
                {
                    "name": "MyParticipationCount",
                    "type": "text",
                    "x": 179,
                    "y": 100,
                    "text": "Katilimlarim: 0/10"
                },
                {
                    "name": "WinnersBoard1",
                    "type": "thinboard",
                    "x": 0,
                    "y": 140,
                    "height": THINBOARD_HEIGHT,
                    "width": BOARD_X - THINBOARD_X_OFFSET,
                    "horizontal_align": "center",
                    "children":
                    (
                        {
                        "name": "WinnersTitle1",
                        "type": "text",
                        "x": 20,
                        "y": 3,
                        "text": "1. Kazanan:",
                        },
                        {
                        "name": "WinnersSlot1",
                        "type": "window",
                        "x": 10,
                        "y": 20,
                        "width": BOARD_X,
                        "height": 32,
                        "children": (
                            {
                            "name": "WinnersGrid1",
                            "type": "grid_table",
                            "x": 0,
                            "y": 0,
                            "width": BOARD_X,
                            "height": 32,
                            "start_index": 0,
                            "x_count": 8,
                            "y_count": 1,
                            "x_step": 32,
                            "y_step": 32,
                            "image" : "d:/ymir work/ui/public/Slot_Base.sub",
                            },
                        ),
                        },
                    ),
                },
                {
                    "name": "WinnersBoard2",
                    "type": "thinboard",
                    "x": 0,
                    "y": 140 + (THINBOARD_HEIGHT * 1) + (THINBOARD_Y_OFFSET * 1),
                    "height": THINBOARD_HEIGHT,
                    "width": BOARD_X - THINBOARD_X_OFFSET,
                    "horizontal_align": "center",
                    "children":
                    (
                        {
                        "name": "WinnersTitle2",
                        "type": "text",
                        "x": 20,
                        "y": 3,
                        "text": "2. Kazanan:",
                        },
                        {
                        "name": "WinnersSlot2",
                        "type": "window",
                        "x": 10,
                        "y": 20,
                        "width": BOARD_X,
                        "height": 32,
                        "children": (
                            {
                            "name": "WinnersGrid2",
                            "type": "grid_table",
                            "x": 0,
                            "y": 0,
                            "width": BOARD_X,
                            "height": 32,
                            "start_index": 0,
                            "x_count": 8,
                            "y_count": 1,
                            "x_step": 32,
                            "y_step": 32,
                            "image" : "d:/ymir work/ui/public/Slot_Base.sub",
                            },
                        ),
                        },
                    ),
                },
                {
                    "name": "WinnersBoard3",
                    "type": "thinboard",
                    "x": 0,
                    "y": 140 + (THINBOARD_HEIGHT * 2) + (THINBOARD_Y_OFFSET * 2),
                    "height": THINBOARD_HEIGHT,
                    "width": BOARD_X - THINBOARD_X_OFFSET,
                    "horizontal_align": "center",
                    "children":
                    (
                        {
                        "name": "WinnersTitle3",
                        "type": "text",
                        "x": 20,
                        "y": 3,
                        "text": "3. Kazanan:",
                        },
                        {
                        "name": "WinnersSlot3",
                        "type": "window",
                        "x": 10,
                        "y": 20,
                        "width": BOARD_X,
                        "height": 32,
                        "children": (
                            {
                            "name": "WinnersGrid3",
                            "type": "grid_table",
                            "x": 0,
                            "y": 0,
                            "width": BOARD_X,
                            "height": 32,
                            "start_index": 0,
                            "x_count": 8,
                            "y_count": 1,
                            "x_step": 32,
                            "y_step": 32,
                            "image" : "d:/ymir work/ui/public/Slot_Base.sub",
                            },
                        ),
                        },
                    ),
                },
                {
                    "name": "RefreshTimeText",
                    "type": "text",
                    "x": 10,
                    "y": 310,
                    "text": "",
                },
            ],
        },
    ],
}
