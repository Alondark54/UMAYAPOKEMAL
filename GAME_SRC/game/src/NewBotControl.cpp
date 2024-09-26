/**************************
* * Author: Larry Watterson *
* ? Date: 29.05.22 ?
* ! Version: v1 !
***************************/
#include "stdafx.h"
#ifdef ENABLE_BOT_CONTROL
#include <array>
#include "char.h"
#include "utils.h"
#include "desc.h"
#include "NewBotControl.h"

auto CNewBotControl::SetControlItems(const DWORD dwVnum, const char* name) -> void {
    TBotControlItems info = {};
    info.dwVnum = dwVnum;
    strlcpy(info.szName, name, sizeof(info.szName));
    itemVec.emplace_back(info);
	sys_err("added name : %s - %s", name, info.szName);
}

auto CNewBotControl::GetRandomItems(LPCHARACTER ch, std::map<DWORD, SPlayerBotControlInfo>::iterator it) -> void {
    if (!ch || !ch->GetDesc()) { return; }
    std::array<DWORD, SEND_ITEM_COUNT> arrItem;
    char szCommand[256];
    int length = snprintf(szCommand, sizeof(szCommand), "botcontrol ");
    for (BYTE i(0); i < SEND_ITEM_COUNT; ++i) {
        const auto random = number(0,itemVec.size()-1);
        arrItem[i] = itemVec[random].dwVnum;
        length += snprintf(szCommand + length, sizeof(szCommand) - length, "%d|", arrItem[i]);
    }
    const BYTE realSlot = number(0,arrItem.size()-1);
    const DWORD realVnum = arrItem[realSlot];
    it->second.dwRealVnum = realVnum;
	// ch->ChatPacket(1, "%d - %d", get_global_time(), it->second.iNextTime);
    const auto& itemIT = std::find_if(itemVec.begin(), itemVec.end(),
    [&realVnum](const auto& p) { return realVnum == p.dwVnum; }
    );
    if (itemIT != itemVec.end()) {
        std::string name = itemIT->szName;
        std::replace(name.begin(), name.end(), ' ', '_');
        ch->ChatPacket(CHAT_TYPE_COMMAND, "botcontrolname %s", name.c_str());
    }
    ch->ChatPacket(CHAT_TYPE_COMMAND, "botcontrolrof %u", it->second.bLeftROC);
    ch->ChatPacket(CHAT_TYPE_COMMAND, szCommand);
}

auto CNewBotControl::SelectBotControlItem(LPCHARACTER ch, const DWORD dwSelectedVnum) -> void {
    if (!ch || !ch->GetDesc() || dwSelectedVnum < 0) { return; }
    auto it = ch_map.find(ch->GetPlayerID());
    if (it != ch_map.end())
    {
		if(it->second.bLeftROC == 0) { return; }
        if (dwSelectedVnum != it->second.dwRealVnum) {
            it->second.bLeftROC--;
            ch->ChatPacket(CHAT_TYPE_COMMAND, "botcontrolrof %u", it->second.bLeftROC);
            if (it->second.bLeftROC == 0) {
                ch->GetDesc()->DelayedDisconnect(3);
                ch->ChatPacket(1, "Bot kontrol hakkiniz bitti, oyundan atiliyorsunuz...");
                ch->SetBotControlGUI(false);
				// ch->SetBotControl(false);
            }
        } else {
            // const BYTE rndMin = 60; // sonraki kontrol icin dakika
            ch->ChatPacket(1, "Dogrulama basarili..");
            ch->ChatPacket(CHAT_TYPE_COMMAND, "closebotcontrol");
            ch->SetBotControlGUI(false);
			it->second.iNextTime = get_global_time() + (min * 60);
			// ch->SetBotControl(true);
        }
        // ch->SetBotControl(true);
    }
}

auto CNewBotControl::CheckBotControl(LPCHARACTER ch) -> void {
    if (!ch || !ch->GetDesc()) { return; }
	// if (IsCheckTime(ch) == false) { return; }
    const auto chID = ch->GetPlayerID();
    auto it = ch_map.find(chID);
    if (it == ch_map.cend()) { // -> first bot control
        TPlayerBotControlInfo info = {};
        info.iNextTime = 0;
        info.bLeftROC = BOTCONTROL_ROF;
        info.dwRealVnum = 0;
        ch_map.emplace(chID, info);
        it = ch_map.find(chID);
    }

    it->second.bLeftROC = BOTCONTROL_ROF;

    GetRandomItems(ch, it);
    ch->SetBotControlGUI(true);
}

auto CNewBotControl::IsCheckTime(LPCHARACTER ch) const -> bool {
    if (!ch || !ch->GetDesc()) { return false; }
    const auto& it = ch_map.find(ch->GetPlayerID());
	if (it == ch_map.end()) { return true; }
    // if (it != ch_map.end()) {
        if (get_global_time() > it->second.iNextTime)
        {
			// ch->SetBotControl(false);
            return true;
        }
    // }
    return false;
}
#endif