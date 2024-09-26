#include "stdafx.h"
#ifdef ENABLE_NEW_MISSIONS
#include "NewMissions.hxx"
#include "char.h"
#include "db.h"
#include "locale_service.h"
#include "p2p.h"
#include "config.h"
#include "packet.h"

#define LOGS

#define MAX_REWARD_ITEM 6

auto CNewMissions::Initialize() -> void {
    LoadMissionInfo(); // -> gorev bilgileri
    LoadPlayerInfo(); // -> oyuncu bilgileri
    LoadGlobalMissionInfo(); //-> load global missions
}

auto CNewMissions::LoadMissionInfo() -> void { 

    char szQuery[QUERY_MAX_LEN];
    int column = snprintf(szQuery, sizeof(szQuery), "SELECT `index`, target_value, mission_value, mission_type, mission_desc");
    for(BYTE i (1); i <= MAX_REWARD_ITEM; ++i){
        column += snprintf(szQuery + column, sizeof(szQuery) - column, ", msreward_vnum%d", i);
        column += snprintf(szQuery + column, sizeof(szQuery) - column, ", msreward_count%d", i);
    }
    snprintf(szQuery + column, sizeof(szQuery) - column, " FROM missions_info%s", get_table_postfix());

    const std::unique_ptr<SQLMsg> pkMsg(DBManager::instance().DirectQuery(szQuery));
    if (pkMsg->Get()->uiNumRows>0)
	{
		MYSQL_ROW row;
		mission_vec.reserve(pkMsg->Get()->uiNumRows);
		while ((row = mysql_fetch_row(pkMsg->Get()->pSQLResult)) != NULL)
		{
			TMissionsInfo info = {};
			DWORD col = 0;
			str_to_number(info.index, row[col++]);
			str_to_number(info.targetValue, row[col++]);
			str_to_number(info.needCount, row[col++]);
			str_to_number(info.missionType, row[col++]);
			info.missionDesc = row[col++];
			for (BYTE i(0); i < MAX_REWARD_ITEM; ++i) {
				DWORD vnum = 0;
				str_to_number(vnum, row[col++]);
#ifdef EXTENDED_COUNT // -> 64k stack definesi, kendinize gore ayarlayabilirsiniz
				WORD count = 0;
#else
				BYTE count = 0;
#endif
				str_to_number(count, row[col++]);
				if (vnum == 0 || count == 0) { break; }
				info.giftMap.emplace(vnum, count);
			}
			mission_vec.emplace_back(info);
		}
	}
}

auto CNewMissions::LoadGlobalMissionInfo() -> void {
    const std::unique_ptr<SQLMsg> pkMsg(DBManager::instance().DirectQuery("SELECT * FROM global_missions%s", get_table_postfix()));
    if (pkMsg->Get()->uiNumRows > 0)
    {
        globalMission_vec.reserve(pkMsg->Get()->uiNumRows);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(pkMsg->Get()->pSQLResult)) != nullptr)
        {
            DWORD col = 0;
            TGlobalMissionInfo info = {};
            str_to_number(info.bMissionType, row[col++]);
            info.missionDesc = row[col++];
            info.winnerName = row[col++];
            str_to_number(info.targetValue, row[col++]);
            str_to_number(info.dwGiftVnum, row[col++]);
            str_to_number(info.dwGiftCount, row[col++]);
            str_to_number(info.isCompleted, row[col++]);
            globalMission_vec.emplace_back(info);
        }
    }
}

auto CNewMissions::LoadPlayerInfo() -> void {
    const std::unique_ptr<SQLMsg> pkMsg(DBManager::instance().DirectQuery("SELECT DISTINCT player_id FROM missions_playerinfo%s", get_table_postfix()));
	MYSQL_ROW row;
    if (pkMsg->Get()->uiNumRows > 0)
    {
        while ((row = mysql_fetch_row(pkMsg->Get()->pSQLResult)) != nullptr)
		{
			DWORD player_id = 0;
			str_to_number(player_id, row[0]);
			TMissionPlayerInfo info{};
			ms_Map.emplace(player_id, info);
		}
	}
	for (auto& it : ms_Map)
	{
		const std::unique_ptr<SQLMsg> pkMsg2(DBManager::instance().DirectQuery("SELECT * FROM missions_playerinfo%s WHERE player_id = %u", get_table_postfix(), it.first));
		if (pkMsg2->Get()->uiNumRows > 0)
		{
			while ((row = mysql_fetch_row(pkMsg2->Get()->pSQLResult)) != nullptr)
			{
				DWORD col = 1;
				TPlayerMissionData missionData = {};
				str_to_number(missionData.index, row[col++]);
				str_to_number(missionData.bMissionType, row[col++]);
				str_to_number(missionData.value, row[col++]);
				str_to_number(missionData.is_completed, row[col++]);
				it.second.missionValues.emplace_back(missionData);
			}
		}
	}
}

auto CNewMissions::AddPlayer(LPCHARACTER ch, const BYTE missionIndex) -> bool {
    if (!ch || !ch->GetDesc()) { return false; }
    const auto id = ch->GetPlayerID();

    const auto& missionIT = std::find_if(std::begin(mission_vec), std::end(mission_vec),
    [&] (const TMissionsInfo& elm) { return elm.index == missionIndex; }
    );
    if (missionIT == std::end(mission_vec)) { return false; }

	if (!AddPlayer(id, missionIndex, missionIT->missionType))
    {
        ch->ChatPacket(1, "Gorevler zaten alinmis!");
        return false;
    }

	TPacketGGNewMissionsCharacter p2pInfo = {};
	p2pInfo.bHeader = HEADER_GG_NEWMISSIONS_NEW_PLAYER;
	p2pInfo.dwID = id;
	p2pInfo.missionType = missionIT->missionType;
	p2pInfo.index = missionIndex;
	P2P_MANAGER::instance().Send(&p2pInfo, sizeof(TPacketGGNewMissionsCharacter));
    DBManager::instance().DirectQuery("INSERT INTO missions_playerinfo%s (player_id, mission_index, mission_type) VALUES(%u, %u, %u)", get_table_postfix(), id, missionIndex, missionIT->missionType);

    return true;
}

auto CNewMissions::AddPlayer(const DWORD dwID, const BYTE missionIndex, const BYTE missionType) -> bool {
    const auto& it = ms_Map.find(dwID);
    if (it != ms_Map.end()) {
        const auto& missionIT = std::find_if(std::begin(it->second.missionValues), std::end(it->second.missionValues), 
            [&](const TPlayerMissionData& elm) { return elm.index == missionIndex; }
        );
        if (missionIT == std::end(it->second.missionValues)) {
            TPlayerMissionData info{};
            info.index = missionIndex;
            info.bMissionType = missionType;
            it->second.missionValues.emplace_back(info);
            return true;
        }
        return false;
    }
    // else {
    TMissionPlayerInfo info = {};
	TPlayerMissionData datas = {};
	datas.index = missionIndex;
	datas.bMissionType = missionType;
    info.missionValues.emplace_back(datas);
    ms_Map.emplace(dwID, info);
    // }
    return true;
}

auto CNewMissions::GetGlobalMissionValue(const eGlobalMissionType type) const -> int {
    const auto& it = std::find_if(
                                globalMission_vec.begin(),
                                globalMission_vec.end(),
                                [=](const TGlobalMissionInfo& a) { return a.bMissionType == static_cast<BYTE>(type); });

    if (it == globalMission_vec.end()) { return -1; }
    if (it->isCompleted) { return -1; }
    
    return static_cast<int>(it->targetValue);
}

auto CNewMissions::UpdateGlobalMissions(LPCHARACTER ch, const DWORD value, const eGlobalMissionType type) -> void {
    if (!ch || !ch->GetDesc()) { return; }

    auto it = std::find_if(
                        globalMission_vec.begin(),
                        globalMission_vec.end(),
                        [=](const TGlobalMissionInfo& a) { return a.bMissionType == static_cast<BYTE>(type); });

    if (it == globalMission_vec.end()) { return; }
    if (it->isCompleted) { return; }
    if (it->targetValue != value) { return; }
    it->isCompleted = true;
    it->winnerName = std::string(ch->GetName());
    DBManager::instance().DirectQuery("UPDATE global_missions%s SET winner_name = '%s', is_completed = 1 WHERE mission_idx = %u", get_table_postfix(), ch->GetName(), it->bMissionType);
    ch->AutoGiveItem(it->dwGiftVnum, it->dwGiftCount);
    ch->ChatPacket(1, "Congratulations! You are the first player have been sucssesfull this mission.");

	TPacketGGNewGlobalMissions p2pInfo = {};
	p2pInfo.bHeader = HEADER_GG_GLOBAL_MISSIONS;
	p2pInfo.type = static_cast<BYTE>(type);
	p2pInfo.name = it->winnerName;
	P2P_MANAGER::instance().Send(&p2pInfo, sizeof(TPacketGGNewGlobalMissions));

}

auto CNewMissions::UpdateGlobalMissions(const BYTE type, const std::string name) -> void {
    auto it = std::find_if(
                        globalMission_vec.begin(),
                        globalMission_vec.end(),
                        [=](const TGlobalMissionInfo& a) { return a.bMissionType == type; });
    it->isCompleted = true;
    it->winnerName = name;
}

auto CNewMissions::UpdateMissionData(LPCHARACTER ch, const BYTE bMissionType, const DWORD targetVnum, const DWORD val) -> void {
    if (!ch || !ch->GetDesc() || ms_Map.empty()) { return; }
    
    if (bMissionType >= MISSION_MAX_NUM) { 
        sys_err("bilinmeyen gorev tipi gelen: %d", bMissionType);
        return;
    }
    const auto id = ch->GetPlayerID();

	const auto& it = ms_Map.find(id);
    if (it != ms_Map.cend()) {
// controls and action
        const auto& findlmb = [&] (const TMissionsInfo& elm) {
            const bool isOk = targetVnum > 0 ? (elm.targetValue == targetVnum && elm.missionType == bMissionType) : (elm.missionType == bMissionType);
            return isOk;
        };
        const auto& missionIT = std::find_if(std::begin(mission_vec), std::end(mission_vec), findlmb);
        if (missionIT != std::end(mission_vec)) {
            const auto& playerIT = std::find_if(std::begin(it->second.missionValues), std::end(it->second.missionValues),
            [&](const TPlayerMissionData& elm) { return elm.index == missionIT->index; });
            if (playerIT != std::end(it->second.missionValues))
            {
                if (playerIT->is_completed == true) { return; }
                const auto needCount = missionIT->needCount;
                const auto myCount = playerIT->value + val;

                if (myCount >= needCount) {
                    for (const auto& giftIT : missionIT->giftMap)
                    {
                        ch->AutoGiveItem(giftIT.first, giftIT.second);
                    }
                    playerIT->is_completed = true;
                 }

                ch->ChatPacket(CHAT_TYPE_COMMAND, "UpMission %u %u", missionIT->index, val);

        // controls and action
                UpdateMissionData(id, missionIT->index, bMissionType, targetVnum, val);
                TPacketGGNewMissions p2pInfo = {};
                p2pInfo.bHeader = HEADER_GG_NEWMISSIONS;
                p2pInfo.dwID = id;
                p2pInfo.target_vnum = targetVnum;
                p2pInfo.bMissionType = bMissionType;
                p2pInfo.index =  missionIT->index;
                p2pInfo.value = val;
                P2P_MANAGER::instance().Send(&p2pInfo, sizeof(TPacketGGNewMissions));
                DBManager::instance().DirectQuery("UPDATE missions_playerinfo%s SET mission_value = %u, is_completed = %u WHERE player_id = %u and mission_index = %u", get_table_postfix(), myCount + val, playerIT->is_completed, id, missionIT->index);
            }

        }
	}
}

auto CNewMissions::UpdateMissionData(const DWORD id, const BYTE missionIdx, const BYTE missionType, const DWORD target_vnum, const DWORD player_value) -> void {
    const auto& it = ms_Map.find(id);
    if (it != ms_Map.end()) {
            const auto& missionIT = std::find_if(std::begin(mission_vec), std::end(mission_vec),
            [&] (const TMissionsInfo& elm) { 
                const bool isOk = target_vnum > 0 ? (elm.targetValue == target_vnum && elm.index == missionIdx) : true;
                return isOk; 
                }
            );
            if (missionIT != std::end(mission_vec)) {
                const auto& val = std::find_if(std::begin(it->second.missionValues), std::end(it->second.missionValues),
                [&](const TPlayerMissionData& elm) { return elm.index == missionIdx && elm.bMissionType == missionType; });
                    
            if (val != std::end(it->second.missionValues)) {
                val->value += player_value;
                if (val->value >= missionIT->needCount) {
                    val->is_completed = true;
                }
            }
        }
    }
}

auto CNewMissions::SendToClient(LPCHARACTER ch) -> void {
    if (!ch || !ch->GetDesc() || ms_Map.empty()) { return; }

    char notice[256] = {};
    BYTE idx = 0;
    for (const auto& it : globalMission_vec) {
        snprintf(notice, sizeof(notice),"MissionGlobalInfo %u %s %s %d %d",
        idx,
        it.missionDesc.c_str(), 
        it.winnerName.c_str(), 
        it.dwGiftVnum,
        it.dwGiftCount);
        ch->ChatPacket(CHAT_TYPE_COMMAND, notice);
        ++idx;
    }

    idx = 0;
    for (const auto& it : mission_vec) { // -> gorev bilgileri gonder
        snprintf(notice, sizeof(notice),"MissionInfo %u %s %u",
        idx,
        it.missionDesc.c_str(), 
        it.needCount);
        ch->ChatPacket(CHAT_TYPE_COMMAND, notice);
        // ch->ChatPacket(1, notice);

        for (const auto& giftIT : it.giftMap) {
            ch->ChatPacket(CHAT_TYPE_COMMAND, 
            "MissionItems %u %u %u", idx, giftIT.first, giftIT.second);
        }
        idx++;
    }

    const auto& playerIT = ms_Map.find(ch->GetPlayerID());
    if ( playerIT != ms_Map.cend()) {
		idx = 0;
        for (const auto& missionIT : playerIT->second.missionValues)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "MyMissions %u %u",
			idx++, missionIT.value);
		}
    }

    ch->ChatPacket(CHAT_TYPE_COMMAND, "AppendMissions");
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenMissionPanel");
}
#endif