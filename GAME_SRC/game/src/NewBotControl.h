#pragma once
constexpr BYTE SEND_ITEM_COUNT = 9; // -> max kac item gitsin
constexpr BYTE min = 60;
constexpr BYTE BOTCONTROL_ROF = 3; // -> secim hakki
class CNewBotControl : public singleton<CNewBotControl>
{
    using TBotControlItems = struct SBotControlItems
    {
        DWORD   dwVnum;
        char    szName[ITEM_NAME_MAX_LEN + 1];
    };

    using TPlayerBotControlInfo = struct SPlayerBotControlInfo
    {
        int     iNextTime; // -> sonraki kontrol zamani
        BYTE    bLeftROC; // -> kalan secim hakki
        DWORD   dwRealVnum; // -> secilmesi gereken item
    };
public:
    CNewBotControl() = default;
    ~CNewBotControl() = default;

    auto    SetControlItems(DWORD dwVnum, const char* name) -> void;
    auto    GetRandomItems(LPCHARACTER ch, std::map<DWORD, TPlayerBotControlInfo>::iterator it) -> void;
    auto    CheckBotControl(LPCHARACTER ch) -> void;
    auto    SelectBotControlItem(LPCHARACTER ch, DWORD dwSelectedVnum) -> void;
    auto    IsCheckTime(LPCHARACTER ch) const -> bool;

private:
    std::vector<TBotControlItems> itemVec;
    std::map<DWORD, TPlayerBotControlInfo> ch_map; // -> id ve sorulacak zaman(aksiyona bagli)
};