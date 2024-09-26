/***********************************************************************
* * Author:         Larry Watterson
* ? Date:           08.10.22
* ! Version:        v1
* @ Created for:    Johnny Silverhand
***********************************************************************/
#include "stdafx.h"
#ifdef ENABLE_CAOS_EVENT
#include "char.h"
#include "NewCaosEvent.h"
#include "questmanager.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "desc_manager.h"
#include "config.h"
#include "desc.h"
#include "p2p.h"
#include "db.h"

#define GM_LOG

#ifdef GM_LOG
static void SendGmLog(const char* format, ...)
{
    auto ch = CHARACTER_MANAGER::instance().FindByPID(1);
    if (ch)
    {
        va_list args;
        va_start(args, format);
        char buf[1024];
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);
        ch->ChatPacket(CHAT_TYPE_INFO, "[CAOS_EVENT_DEBUG] %s", buf);
    }
}
#endif

EVENTINFO(exit_all_info)
{
    CNewCaosEventManager* pManager;
    bool isExit;
    exit_all_info() : pManager(nullptr), isExit(false) {}
};

EVENTFUNC(exit_all_event)
{
    if (event == nullptr) { return 0; }
    if (event->info == nullptr) { return 0; }

    const auto* info = dynamic_cast<exit_all_info*>(event->info);

    if (info == nullptr) { return 0; }

    auto* pInstance = info->pManager;

    if (pInstance == nullptr) { return 0; }
    if (info->isExit) {
        pInstance->ExitAllPlayers();
    }
    else {
        pInstance->StartWar();
    }

    return 0;
}

CNewCaosEventManager::TPlayerInfo* CNewCaosEventManager::GetPlayerInfo(const uint32_t dwPlayerID)
{
    const auto it = m_PlayerInfo.find(dwPlayerID);
    // if (it == m_PlayerInfo.end())
    // {
    //     m_PlayerInfo[dwPlayerID] = std::make_unique<TPlayerInfo>();
    //     return m_PlayerInfo[dwPlayerID].get();
    // }
    return it != m_PlayerInfo.end() ? it->second.get() : nullptr;
}

auto CNewCaosEventManager::CheckEvent(const int day, const int hour, const int min) -> void
{
    if (day == EVENT_START_DAY && hour == EVENT_START_HOUR)
    {
        if (min == EVENT_START_MINUTE)
        {
            StartEvent();
        }
        else if (min == EVENT_END_MIN)
        {
            EndEvent();
        }
    }
}

auto CNewCaosEventManager::StartWar() -> void
{
    quest::CQuestManager::instance().RequestSetEventFlag("caos_event", 1);
    BroadcastNotice("Kaos eventi basladi!");
}

auto CNewCaosEventManager::StartEvent() -> void
{
    quest::CQuestManager::instance().RequestSetEventFlag("caos_event", 2);
    CreateTimer(false, EVENT_PREPARE_SEC);
    BroadcastNotice("Kaos eventi 1 dakika icinde baslayacak.");
    SendP2PRequest();
    // send all cores
	const BYTE bHeader = HEADER_GG_CAOS_EVENT;
	P2P_MANAGER::instance().Send(&bHeader, sizeof(BYTE));
}

auto CNewCaosEventManager::EndEvent() -> void
{
    quest::CQuestManager::instance().RequestSetEventFlag("caos_event", 0);
    CreateTimer(true, EVENT_EXIT_SEC);
    GiveReward();
    BroadcastNotice("Kaos eventi bitti, az sonra koye isinlanacaksin.");
}

auto CNewCaosEventManager::SendP2PRequest() -> void {
    const auto& SendRequestQuery = [](LPDESC d) -> void {
        auto ch = d->GetCharacter();
        if (!ch) { return; }
        if (!ch->IsPC()) { return; }
        d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "CaosEventRequest");
	};
	const auto& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), SendRequestQuery);
}

auto CNewCaosEventManager::RegisterPlayer(LPCHARACTER ch) -> void
{
    if (!ch || !ch->GetDesc()) { return; }

    ch->SetPKMode(PK_MODE_FREE);
    ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenCaosEventIMG");
	ch->RemoveAffect(AFFECT_MOUNT);
	ch->RemoveAffect(AFFECT_MOUNT_BONUS);
    const auto binek = ch->GetWear(WEAR_COSTUME_MOUNT);
    if (binek) { ch->UnequipItem(binek); }
    if (ch->IsHorseRiding()) { ch->StopRiding(); ch->HorseSummon(false); }
    SendToClient(); // -> update list

    const auto dwPlayerID = ch->GetPlayerID();
    const auto it = GetPlayerInfo(dwPlayerID);
    if (it != nullptr) { return; } // -> already registered
    m_PlayerInfo[dwPlayerID] = std::make_unique<TPlayerInfo>(ch->GetName(),ch->GetLevel(),ch->GetRaceNum());

#ifdef GM_LOG
    SendGmLog("RegisterPlayer: %s - Total Player : %u", ch->GetName(), m_PlayerInfo.size());
#endif
}
auto CNewCaosEventManager::DeathPlayer(const uint32_t killerID, const uint32_t victimID) -> void {
    const auto killerIT = GetPlayerInfo(killerID);
    const auto victimIT = GetPlayerInfo(victimID);
    constexpr BYTE KILL_POINT = 3; // -> her kill kac puan
    if (killerIT && victimIT) {
        killerIT->wKillCount++;
        killerIT->wScore += KILL_POINT;
        victimIT->wDeathCount++;
    }
    SendToClient();
}

auto CNewCaosEventManager::ExitAllPlayers() -> void
{
	auto pMap = SECTREE_MANAGER::instance().GetMap(CAOS_EVENT_MAP);
	if (!pMap) { return; }

    const auto& ExitAllFunc = [&](LPENTITY ent) -> void {
        if (ent->IsType(ENTITY_CHARACTER))
        {
            auto ch = (LPCHARACTER)ent;
            if(!ch || !ch->IsPC() || ch->IsGM()) { return; }

            ch->GoHome();
        }
    };
	pMap->for_each(ExitAllFunc);
    Destroy();
}

auto CNewCaosEventManager::CreateTimer(const bool isExit, const uint8_t sec) -> void
{
    DestroyTimer();
    auto* info = AllocEventInfo<exit_all_info>();
    info->pManager = this;
    info->isExit = isExit;
    m_pTimer = event_create(exit_all_event, info, PASSES_PER_SEC(sec));
#ifdef GM_LOG
    SendGmLog("Event timer created");
#endif
}

auto CNewCaosEventManager::DestroyTimer() -> void
{
    if (m_pTimer != nullptr){
        event_cancel(&m_pTimer);
        m_pTimer = nullptr;
    }
#ifdef GM_LOG
    SendGmLog("Event timer destroyed");
#endif
}

auto CNewCaosEventManager::GetDamages(const uint8_t bJob) -> std::tuple<uint32_t, uint32_t>
{
    static const std::map<uint8_t, std::pair<uint32_t, uint32_t> > damageMap = {
        /* KARAKTER TIPI - SKILL HASAR - DUZ HASAR*/
        {JOB_WARRIOR, {3500, 1250} }, // savasci
        {JOB_ASSASSIN, {3500, 1000}} , // ninja
        {JOB_SURA, {3000, 1250} }, // sura
        {JOB_SHAMAN, {3500, 1250} } // saman
    };
// #ifdef GM_LOG
//     SendGmLog("GetDamages: job: %u - %u - %u", bJob, it->second.first, it->second.second);
// #endif
    const auto& it = damageMap.find(bJob);
    if (it == damageMap.cend()) { return { 0, 0}; } // if not found
#ifdef GM_LOG
    SendGmLog("GetDamages: job: %u - %u - %u", bJob, it->second.first, it->second.second);
#endif
    return { it->second.first, it->second.second };

}

auto CNewCaosEventManager::GetCostumes(const uint8_t bRace) -> std::tuple<uint32_t, uint32_t, uint32_t>
{
    static const std::map<uint8_t, std::tuple<uint32_t, uint32_t, uint32_t> > costumeMap = {
        /* KARAKTER TIPI - SILAH - BODY - SAC*/
         {MAIN_RACE_WARRIOR_M, {219, 11971, 351} }, // savasci erkek (sac kismina msm kodu girilecek, itemin value3 degerinde mevcut)
        { MAIN_RACE_ASSASSIN_M, {1169, 11972, 351}} , // ninja erkek
        { MAIN_RACE_SURA_M, {219, 11973, 351} }, // sura erkek
        { MAIN_RACE_SHAMAN_M, {7189, 11974, 351} }, // saman erkek
        { MAIN_RACE_WARRIOR_W, {219, 11971, 351} }, // savasci kadin
        { MAIN_RACE_ASSASSIN_W, {1169, 11972, 351} }, // ninja kadin
         {MAIN_RACE_SURA_W, {219, 11973, 351} }, // sura kadin
        { MAIN_RACE_SHAMAN_W, {7189, 11974, 351} } // saman kadin
    };
    
    const auto& it = costumeMap.find(bRace);
    if (it == costumeMap.cend()) { return { 0, 0, 0 }; } // if not found
    const auto& [weapon, body, hair] = it->second;
    return { weapon, body, hair };
}

auto CNewCaosEventManager::WarpPlayer(LPCHARACTER ch) -> void
{
    if (!ch || !ch->GetDesc()) { return; }
    if (quest::CQuestManager::instance().GetEventFlag("caos_event") != 2) { return; }
    ch->WarpSet(PLAYER_WARP_X * 100, PLAYER_WARP_Y * 100, CAOS_EVENT_MAP);
}

auto CNewCaosEventManager::GetSortedPlayerInfo(const uint8_t resizeCount) const -> std::vector<std::pair<uint32_t, TPlayerInfo*>>
{
    std::vector<std::pair<uint32_t, TPlayerInfo*>> vec;
    for (const auto& [key, val] : m_PlayerInfo)
    {
        vec.emplace_back(key, val.get());
    }
    std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
        return a.second->wScore > b.second->wScore;
    });
    // std::sort(vec.begin(), vec.end(), std::greater{}); // sort by score

    if (resizeCount > 0 && vec.size() > resizeCount)
    {
        vec.resize(resizeCount);
    }
#ifdef GM_LOG
    SendGmLog("Sorted vec created! Vec size: %u", vec.size());
#endif
    return vec;
}

auto CNewCaosEventManager::GiveReward() -> void {

    const auto& ReadRewards = [&]() -> bool { // -> read rewards from table
        m_RewardInfo.clear();
        const std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT * FROM caosevent_rewards%s", get_table_postfix()));
        if (pMsg->Get()->uiNumRows==0) {
            sys_err("Caos event odul tablosu bos!");
            return false;
        }
        MYSQL_ROW	row;
        m_RewardInfo.reserve(pMsg->Get()->uiNumRows);
        while (nullptr != (row = mysql_fetch_row(pMsg->Get()->pSQLResult))) {
            DWORD col = 0;
            TRewarInfo info{};
            str_to_number(info.dwVnum, row[col++]);
            str_to_number(info.dwCount, row[col++]);

            m_RewardInfo.emplace_back(info);
        }
        return true;
    };

    if (ReadRewards()) {
        const auto& vec = GetSortedPlayerInfo(PLAYER_REWARD_COUNT);
        if (vec.empty()) { return; }
        BYTE bRank = 1;
        for (const auto& [key, val] : vec) {
            if (val->wScore == 0) { continue; }
            auto ch = CHARACTER_MANAGER::instance().FindByPID(key);
            if (!ch || !ch->GetDesc()) { continue; }
            const auto& reward = m_RewardInfo[bRank++];
            if (reward.dwVnum == 0 || reward.dwCount == 0) { continue; }
            ch->AutoGiveItem(reward.dwVnum, reward.dwCount);
        }
    }
}

auto CNewCaosEventManager::SendToClient() -> void
{
    const auto& vec = GetSortedPlayerInfo(UI_MAX_PLAYER);
    if (vec.empty()) { return; }

    auto pMap = SECTREE_MANAGER::instance().GetMap(CAOS_EVENT_MAP);
	if (!pMap) { return; }

    char szQuery[128];

    const auto& sendFunc = [&](LPENTITY ent) -> void {
        if (ent->IsType(ENTITY_CHARACTER))
        {
            auto ch = (LPCHARACTER)ent;
            if(!ch || !ch->IsPC()) { return; }
            ch->ChatPacket(CHAT_TYPE_COMMAND, szQuery);
        }
    };

    snprintf(szQuery, sizeof(szQuery), "ClearCaosUI");
    pMap->for_each(sendFunc);

    for (const auto& [key, val] : vec)
    {
        snprintf(szQuery, sizeof(szQuery),
            "CaosEventInfo %s %u %u %u %u %u",
            val->szName,
            val->bRace,
            val->bLevel,
            val->wKillCount,
            val->wDeathCount,
            val->wScore
        );
        pMap->for_each(sendFunc);
    }
    snprintf(szQuery, sizeof(szQuery), "RefreshCaosUI");
    pMap->for_each(sendFunc);
}

auto CNewCaosEventManager::Destroy() -> void
{
    DestroyTimer();
    m_PlayerInfo.clear();
    m_RewardInfo.clear();
}

#endif