#pragma once
class CNewMissions final : public singleton<CNewMissions>
{
	typedef struct SPlayerMissionData
	{
        BYTE        index;   
		BYTE		bMissionType;
		DWORD		value;
		bool		is_completed;
	} TPlayerMissionData;

    typedef struct SMissionPlayerInfo
    {
        SMissionPlayerInfo() { missionValues.clear(); }
        std::vector<TPlayerMissionData> missionValues; // -> gorev value
    } TMissionPlayerInfo;
    using PLAYER_INFO_MAP = std::map<DWORD, TMissionPlayerInfo>;

    typedef struct SMissionInfo
    {
        BYTE        index; // -> gorev idx
        std::string missionDesc;
        WORD        needCount; // -> gorevden kac tane yapilacagi
        DWORD       targetValue; // -> gorevin hedef vnumu
        BYTE        missionType; // -> gorev tipi
#ifdef EXTENDED_COUNT
        std::map<DWORD, DWORD> giftMap; // -> odul vnum ve adetler
#else
        std::map<DWORD, BYTE> giftMap; // -> odul vnum ve adetler
#endif
    } TMissionsInfo;
    using MISSION_VEC = std::vector<TMissionsInfo>;

    typedef struct SGlobalMissionInfo
    {
        BYTE            bMissionType;
        std::string     missionDesc;
        std::string     winnerName;
        DWORD           targetValue; // -> level, item vnum etc..
        DWORD           dwGiftVnum;
        DWORD           dwGiftCount;
        bool            isCompleted;
    } TGlobalMissionInfo;
    using GLOBAL_MISSION_VEC = std::vector<TGlobalMissionInfo>;
public:

    enum eMissionTypes : BYTE {
        MISSION_KILL, // -> boss oldurme  - mission_2 / data.json
        MISSION_CRAFT, // -> uretim gorevi - mission_1 / data.json
        MISSION_CHEST, // -> sandik acma - mission_3 / data.json
        MISSION_MAX_NUM,
    };

    enum class eGlobalMissionType
    {
        RESERVED_MISSION,
        FIRST_75_LEVEL,
        FIRST_99_LEVEL,
        FIRST_POISON_SWORD,
        FIRST_CROW_STEEL_BOW,
    };


    CNewMissions() = default;
    ~CNewMissions() = default;
    auto    Initialize() -> void;
    auto    LoadMissionInfo() -> void;
    auto    LoadGlobalMissionInfo() -> void;
    auto    LoadPlayerInfo() -> void;
    auto    GetGlobalMissionValue(const eGlobalMissionType type) const -> int;
    // auto    CheckGlobalMission(eGlobalMissionType type) const -> bool;
    auto    AddPlayer(LPCHARACTER ch, BYTE missionType) -> bool;
    auto    AddPlayer(DWORD dwID, BYTE missionIndex, BYTE missionType) -> bool;
    auto    UpdateGlobalMissions(LPCHARACTER ch, DWORD value, eGlobalMissionType type) -> void;
    auto    UpdateGlobalMissions(BYTE type, std::string name) -> void;
    auto    UpdateMissionData(LPCHARACTER ch, BYTE bMissionType, DWORD targetVnum = 0, DWORD val = 1) -> void;
    auto    UpdateMissionData(DWORD id, BYTE missionIdx, BYTE missionType, DWORD target_vnum, DWORD player_value) -> void;
    auto    SendToClient(LPCHARACTER ch) -> void;
private:
    PLAYER_INFO_MAP     ms_Map;
    MISSION_VEC         mission_vec;
    GLOBAL_MISSION_VEC  globalMission_vec;
};