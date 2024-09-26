/**************************
* * Author: Larry Watterson *
* ? Date: 12.01.22 ?
* ! Version: v1 !
***************************/
#include "stdafx.h"

#ifdef ENABLE_NEW_BIOLOG
#include "NewBiolog.h"
#include "buffer_manager.h"
#include "char.h"
#include "desc.h" 
#include "item.h" 
#include "utils.h"
#include "config.h"
#include "db.h" 

//#define LOGS
EVENTINFO(biolog_ntf_info)
{
	DynamicCharacterPtr player;

	biolog_ntf_info()
	: player()
	{
	}
};

EVENTFUNC(biolog_ntf_event)
{
	const auto pInfo = dynamic_cast<biolog_ntf_info*>(event->info);
	if (pInfo == NULL)
	{
		sys_err("biolog_ntf_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = pInfo->player;
 
	if (!ch || !ch->GetDesc()) { return 0; }

	if (ch->GetBioNtf() == 1 && ch->GetBioTime() <= get_global_time())
	{
		char buf[52];
		int len = snprintf(buf, sizeof(buf), LC_TEXT("Biyo geldi kanka versene"));

		TPacketGCWhisper pack;
		pack.bHeader = HEADER_GC_WHISPER;
		pack.bType = WHISPER_TYPE_SYSTEM;
		pack.wSize = sizeof(TPacketGCWhisper) + len;
		strlcpy(pack.szNameFrom, "[Biyolog Bildirim]", sizeof(pack.szNameFrom));
		ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
		ch->GetDesc()->Packet(buf, len);
		ch->ChatPacket(1, "Biyolog teslim suresi bildirimi aktif.");
		// ch->CloseBiologNtf();
		return 0;
	}

	return PASSES_PER_SEC(1);
}

void CHARACTER::OpenBiologNtf()
{
	CloseBiologNtf();

	auto* pInfo = AllocEventInfo<biolog_ntf_info>();
	pInfo->player = this;

	m_pkBiologNtfEvent = event_create(biolog_ntf_event, pInfo, 1);
}

void CHARACTER::CloseBiologNtf()
{
	if (m_pkBiologNtfEvent)
	{
		event_cancel(&m_pkBiologNtfEvent);
		m_pkBiologNtfEvent = nullptr;
	}
}

auto CNewBiologManager::Initialize() -> bool {
    const std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT * FROM player.biolog_info%s", get_table_postfix()));
    if (pMsg->Get()->uiNumRows==0) {
        sys_err("Biolog tablosu bos!");
        return false;
    }
    MYSQL_ROW	row;
    BYTE mission = 0;
    while (nullptr != (row = mysql_fetch_row(pMsg->Get()->pSQLResult))) {
        DWORD col = 0;
        tBioInfo   info = {};
        str_to_number(info.dwMobVnum, row[col++]); //idx olarak güncelledi.
        str_to_number(info.bLevel, row[col++]);
        str_to_number(info.aff_type[0], row[col++]); // constant.cpp'den point numbera gore eklenecek tabloya
        str_to_number(info.aff_value[0], row[col++]);
        str_to_number(info.aff_type[1], row[col++]); // constant.cpp'den point numbera gore eklenecek tabloya
        str_to_number(info.aff_value[1], row[col++]);
        str_to_number(info.aff_type[2], row[col++]); // constant.cpp'den point numbera gore eklenecek tabloya
        str_to_number(info.aff_value[2], row[col++]);
        str_to_number(info.aff_type[3], row[col++]); // constant.cpp'den point numbera gore eklenecek tabloya
        str_to_number(info.aff_value[3], row[col++]);
        str_to_number(info.dwItemVnum, row[col++]);
        str_to_number(info.dwSoulVnum, row[col++]);
        str_to_number(info.bReqCount, row[col++]);
        str_to_number(info.bMin, row[col++]);
		str_to_number(info.bChance, row[col++]);

        m_BioInfoMap.emplace(mission, info);
        ++mission;
    }

#ifdef LOGS
    for (const auto& [k,v] : m_BioInfoMap) {
        sys_err("bio init: %d - %d - %d - %d -%d -%d -%d -%d",
        k, v.dwMobVnum, v.aff_type[0], v.aff_value[0], v.dwItemVnum, v.bReqCount, v.bMin, v.bChance);
    }
#endif
	return true;
}

auto CNewBiologManager::SendToClient(const LPCHARACTER ch) const -> void {
    if (!ch || !ch->GetDesc() || m_BioInfoMap.empty()) { return; }

    const auto& it = m_BioInfoMap.find(ch->GetBioLevel());
    if (it == m_BioInfoMap.cend()) { return; }
	tBiologGCInformation pInfo = {};
    pInfo.bHeader = HEADER_GC_BIOLOG_UPDATE;
    pInfo.dwItemVnum = it->second.dwItemVnum;
    pInfo.dwSoulVnum = it->second.dwSoulVnum;
    pInfo.bGivenCount = ch->GetBioGivenCount();
    pInfo.bState = ch->GetBioState();
    pInfo.bRequiredCount = it->second.bReqCount;
    pInfo.bAffect[0] = it->second.aff_type[0];
    pInfo.bAffValue[0] = it->second.aff_value[0];
    pInfo.bAffect[1] = it->second.aff_type[1];
    pInfo.bAffValue[1] = it->second.aff_value[1];
    pInfo.bAffect[2] = it->second.aff_type[2];
    pInfo.bAffValue[2] = it->second.aff_value[2];
    pInfo.bAffect[3] = it->second.aff_type[3];
    pInfo.bAffValue[3] = it->second.aff_value[3];
    pInfo.bChance = it->second.bChance;
    pInfo.iTime = ch->GetBioTime() - get_global_time();

    // };
    TEMP_BUFFER buf;
    buf.write(&pInfo, sizeof(pInfo));

    if (buf.size() <= 0) { return; }
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

auto CNewBiologManager::EndBiolog(LPCHARACTER ch, const BYTE idx, const bool isItem) -> bool {
	if (!ch || !ch->GetDesc()) { return false; }
	// if (idx ==  99) {
	// -> idx 1 = 92 sonrasi idx 2 = 92 oncesi
	if (isItem) {
		if (idx == 1 && ch->GetBioLevel() >= 7) {
			ch->ChatPacket(1, "92 ve sonrasi gorevler icin bu itemi kullanamazsin");
			return false;
		}

		if (idx == 0 && ch->GetBioLevel() < 7) {
			ch->ChatPacket(1, "92 uzerindeki gorevler icin bu itemi kullanamazsin.");
			return false;
		}
	}
	const auto& it = m_BioInfoMap.find(idx);
	if (it != m_BioInfoMap.cend()) {
		for (BYTE i = 0; i < sizeof(it->second.aff_type) / sizeof(*it->second.aff_type); ++i)
		{
			if (it->second.aff_type[i] == 0) { break;  }
			ch->AddAffect(AFFECT_BIOLOG_START_IDX + ch->GetBioLevel(), aApplyInfo[it->second.aff_type[i]].bPointType, it->second.aff_value[i], 0, INFINITE_AFFECT_DURATION, 0, false);
		}
		ch->SetBioTime(0);
		ch->SetBioGivenCount(0);
		ch->SetBioLevel(ch->GetBioLevel() + 1); // yeni gorev
		SendToClient(ch);
		ch->ChatPacket(1, "Gorev tamamlandi, ozellikler karaktere islendi.");
	}
	return true;
}

// 92 ve 94 indexler 8 - 9
auto CNewBiologManager::GiveBiolog(LPCHARACTER ch, const bool isTimeItem, const bool isExtractItem) const -> void {
    if (!ch || !ch->GetDesc() || m_BioInfoMap.empty()) { return; }
    const auto& it = m_BioInfoMap.find(ch->GetBioLevel());
	if (it == m_BioInfoMap.cend()) { return; }
    BYTE prob = number(1,100);
	const auto state = ch->GetBioState();


	if (state == GIVE_STATE)
	{
	// kontroller //
		if (ch->GetLevel() < it->second.bLevel) {
			ch->ChatPacket(1, "Levelin yetersiz.");
			return;
		}

		if (ch->CountSpecifyItem(it->second.dwItemVnum) < 1) {
			ch->ChatPacket(1, "Yeterli iteme sahip degilsin!");
			return;
		}

		if (isTimeItem) {
			if (ch->CountSpecifyItem(TIME_ITEM_VNUM) < 1) {
				ch->ChatPacket(1, "Yeterli süre sıfırlama nesnesine sahip degilsin!");
				return;
			}
			ch->SetBioTime(0);
			ch->RemoveSpecifyItem(TIME_ITEM_VNUM, 1);
		}

		if (ch->GetBioTime() > get_global_time()) {
			//ch->ChatPacket(1, "%d - %d - fark %d", ch->GetBioTime(), get_global_time(), ch->GetBioTime() - get_global_time());
			ch->ChatPacket(1, "Henuz gorev suresi dolmamis!");
			return;
		}

		if (isExtractItem) {// arastirma ozutu %100 gecme
			if (ch->CountSpecifyItem(EXTRACT_ITEM_VNUM) < 1) {
				ch->ChatPacket(1, "Yeterli biyolog özütüne sahip degilsin!");
				return;
			}
			 prob = 0;
			 ch->RemoveSpecifyItem(EXTRACT_ITEM_VNUM, 1);
		}

	// kontroller //

		if (prob <= it->second.bChance) {
			ch->SetBioGivenCount(ch->GetBioGivenCount() + 1);
			ch->ChatPacket(1, "Biyolog Basarili!");
		}
		else {
			ch->ChatPacket(1, "Biyolog Basarisiz!"); 
		}
		ch->RemoveSpecifyItem(it->second.dwItemVnum);

		if (it->second.bMin > 0) {
			ch->SetBioTime(get_global_time() + it->second.bMin*60);
		}

		if (ch->GetBioNtf() == 1) {
			ch->OpenBiologNtf();
		}

		if (ch->GetBioGivenCount() >= it->second.bReqCount) {
			if (ch->GetBioLevel() == 8)
			{
				ch->SetBioState(SELECT_STATE);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenBioSelect %d", ch->GetBioLevel()); 
				return;
			}


			if (it->second.dwSoulVnum != 0) {
				ch->SetBioState(SOUL_STATE); // ruh tasi
			}
			else {

				for (BYTE i (0); i < sizeof(it->second.aff_type) / sizeof(*it->second.aff_type); ++i) {
					if (it->second.aff_type[i] == 0) { break; }
					ch->AddAffect(AFFECT_BIOLOG_START_IDX+ch->GetBioLevel(), aApplyInfo[it->second.aff_type[i]].bPointType, it->second.aff_value[i], 0, INFINITE_AFFECT_DURATION, 0, false);
				}

				ch->SetBioState(GIVE_STATE);
				ch->SetBioLevel(ch->GetBioLevel() + 1); // yeni gorev

				ch->ChatPacket(1, "Gorev tamamlandi, ozellikler karaktere islendi.");
			}
			ch->SetBioTime(0);
			ch->SetBioGivenCount(0);
		}
	}
	else if(state == SOUL_STATE) // soul state
	{
		if (ch->CountSpecifyItem(it->second.dwSoulVnum) < 1) {
			ch->ChatPacket(1, "Yeterli iteme sahip degilsin!");
			return;
		}
		ch->RemoveSpecifyItem(it->second.dwSoulVnum);
		if (ch->GetBioLevel() == 9)
		{
			ch->SetBioState(SELECT_STATE);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenBioSelect %d", ch->GetBioLevel()); 
			return;
		}

		for (BYTE i (0); i < sizeof(it->second.aff_type) / sizeof(*it->second.aff_type); ++i) {
			if (it->second.aff_type[i] == 0) { break; }
			ch->AddAffect(AFFECT_BIOLOG_START_IDX+ch->GetBioLevel(), aApplyInfo[it->second.aff_type[i]].bPointType, it->second.aff_value[i], 0, INFINITE_AFFECT_DURATION, 0, false);
		}

		

		ch->SetBioState(GIVE_STATE);
		ch->SetBioLevel(ch->GetBioLevel() + 1); // yeni gorev

		ch->ChatPacket(1, "Gorev tamamlandi, ozellikler karaktere islendi.");

		if (ch->GetBioNtf() == 1) {
			ch->OpenBiologNtf();
		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenBioSelect %d", ch->GetBioLevel());
	}

    SendToClient(ch);
}

auto CNewBiologManager::SelectSpecial(LPCHARACTER ch, const BYTE bSelect) const -> void
{
	if (!ch || !ch->GetDesc()) { return; }
	if (ch->GetBioState() != SELECT_STATE) { return;  }
	const auto& it = m_BioInfoMap.find(ch->GetBioLevel());
	if (it == m_BioInfoMap.cend()) { return; }

	ch->SetBioTime(0);
	ch->SetBioGivenCount(0);
	if (ch->GetBioLevel() == 8) {
		ch->SetBioState(GIVE_STATE);
	}else {
		ch->SetBioState(SOUL_STATE);
	}
	ch->SetBioLevel(ch->GetBioLevel() + 1);
	if (ch->GetBioLevel() >= m_BioInfoMap.size()) {
		// CloseBiologPanel(ch);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseBiologWindow");
	}

	ch->ChatPacket(1, "Gorev tamamlandi, ozellikler karaktere islendi.");
	ch->AddAffect(AFFECT_BIOLOG_START_IDX + ch->GetBioLevel(), aApplyInfo[it->second.aff_type[bSelect]].bPointType, it->second.aff_value[bSelect], 0, INFINITE_AFFECT_DURATION, 0, false);
	SendToClient(ch);
}

auto CNewBiologManager::OpenBiologPanel(LPCHARACTER ch) -> void {
	if (ch->GetBioLevel() < m_BioInfoMap.size()) {
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenBioPanel");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ReminderStatus %u", ch->GetBioNtf());
	}
	else
		ch->ChatPacket(1, "Tum biyolog gorevleri tamamlanmis!");
}

auto CNewBiologManager::CloseBiologPanel(LPCHARACTER ch) -> void {
	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseBiologWindow");
}

#endif