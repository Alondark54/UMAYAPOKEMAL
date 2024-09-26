#pragma once
#include <memory>
#include "../../common/length.h"
constexpr WORD CAOS_EVENT_MAP = 241; // -> event map index
constexpr BYTE MIN_LEVEL_CAOS_EVENT = 30; // -> min level
class CNewCaosEventManager final : public singleton<CNewCaosEventManager>
{
    enum : BYTE
    {
        EVENT_START_DAY = 5, // cuma gunu 
        EVENT_START_HOUR = 21, // -> event baslama saati
        EVENT_START_MINUTE = 0, // -> event baslama dakikasi (21.00)
        EVENT_END_MIN    = 30, // -> event bitis dakikasi (21.30)

        EVENT_PREPARE_SEC = 60, // -> event baslamadan onceki sure (hazirlanma sureci)
        EVENT_EXIT_SEC    = 60, // -> event bitisinden sonra kalan sure (oyuncularin cikis sureci)

        UI_MAX_PLAYER = 7, // -> ui'de gosterilecek max oyuncu sayisi
        PLAYER_REWARD_COUNT = 3, // -> kazanacak oyuncu sayisi (odul alacak)
    };

    enum : DWORD // 
    {
        PLAYER_WARP_X = 50130, // -> event basladiktan sonra oyuncularin gidecegi x koordinati
        PLAYER_WARP_Y = 50140, // -> event basladiktan sonra oyuncularin gidecegi y koordinati
    };

    using TPlayerInfo = struct SPlayerInfo
    {
        SPlayerInfo(const char* name, const uint8_t bLevel, const uint8_t bRace) :  
        bRace(bRace), bLevel(bLevel), wDeathCount(0), wKillCount(0), wScore(0)
        {
            strlcpy(szName, name, sizeof(szName));
        }
        char        szName[CHARACTER_NAME_MAX_LEN + 1];
        uint8_t     bRace;
        uint8_t     bLevel;
        uint16_t    wDeathCount;
        uint16_t    wKillCount;
        uint16_t    wScore;

        bool operator<(const auto& a) const { return wScore > a.wScore; }
    };
    using PLAYER_INFO_MAP = std::map<uint32_t, std::unique_ptr<TPlayerInfo>>;

    using TRewarInfo = struct SRewardInfo
    {
        SRewardInfo() : dwVnum(0), dwCount(0) {}
        uint32_t    dwVnum;
        uint32_t    dwCount;
    };
    using REWARD_INFO_VEC = std::vector<TRewarInfo>;
public:
    CNewCaosEventManager() = default;
    ~CNewCaosEventManager() = default;

    TPlayerInfo*    GetPlayerInfo(uint32_t dwPlayerID);
    auto        CheckEvent(int day, int hour, int min) -> void;
    auto        StartWar() -> void;
    auto        StartEvent() -> void;
    auto        EndEvent() -> void;
    static auto SendP2PRequest() -> void;
    auto        RegisterPlayer(LPCHARACTER ch) -> void;
    auto        DeathPlayer(uint32_t killerID, uint32_t victimID) -> void;
    auto        ExitAllPlayers() -> void;
    auto        CreateTimer(bool isExit = false, uint8_t sec = 60) -> void;
    auto        DestroyTimer() -> void;
    static auto GetDamages(uint8_t bJob) -> std::tuple<uint32_t, uint32_t>;
    static auto GetCostumes(uint8_t bRace) -> std::tuple<uint32_t, uint32_t, uint32_t>;
    static auto WarpPlayer(LPCHARACTER ch) -> void;
    auto        GetSortedPlayerInfo(uint8_t resizeCount) const -> std::vector<std::pair<uint32_t, TPlayerInfo*>>;
    auto        GiveReward() -> void;
    auto        SendToClient() -> void;
    auto        Destroy() -> void;
private:
    PLAYER_INFO_MAP     m_PlayerInfo;
    REWARD_INFO_VEC     m_RewardInfo;
    LPEVENT             m_pTimer;;
};