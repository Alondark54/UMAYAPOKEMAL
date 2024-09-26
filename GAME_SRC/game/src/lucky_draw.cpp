#include "stdafx.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "../../common/CommonDefines.h"

#include "lucky_draw.h"

LuckyDraw::LuckyDraw() : t_CurrentLuckyDraw(nullptr)
{
}
LuckyDraw::~LuckyDraw()
{
	sys_log(0, "Lucky Draw DESTROY");
}
void LuckyDraw::GetCurrentLuckyDrawFromDB()
{
	TPacketGDLuckyDrawGetCurrent packet;
	memset(&packet, 0, sizeof(TPacketGDLuckyDrawGetCurrent));
	if (t_CurrentLuckyDraw)
		packet.dCurrentId = t_CurrentLuckyDraw->dId;
	else
		packet.dCurrentId = 0;
	db_clientdesc->DBPacket(HEADER_GD_LUCKY_DRAW_CURRENT_LD, 0, &packet, sizeof(TPacketGDLuckyDrawGetCurrent));
}

void LuckyDraw::RESULT_ADD_PARTICIPANT(TPacketDGLuckyDrawAddParticipant* pData)
{
	sys_log(0, "RESULT_ADD_PARTICIPANT - %d", pData->resType);
}

void LuckyDraw::RESULT_LUCKYDRAW_TABLE(TLuckyDrawTable* pData)
{
	if (!t_CurrentLuckyDraw)
	{
		sys_log(0, "RESULT_LUCKYDRAW_TABLE - t_CurrentLuckyDraw is nullptr, setting new data.");
		t_CurrentLuckyDraw = std::make_unique<TLuckyDrawTable>(*pData);
		return;
	}

	sys_log(0, "LuckyDraw Client:%d - %d", t_CurrentLuckyDraw->dId, pData->dId);
	
	if (t_CurrentLuckyDraw->dId == pData->dId && t_CurrentLuckyDraw->bIsFinished == pData->bIsFinished)
	{
		t_CurrentLuckyDraw->dTotalParticipant = pData->dTotalParticipant;
		sys_log(0, "LuckyDraw PT:%d - %d", t_CurrentLuckyDraw->dId, pData->dId);
		return;
	}

	sys_log(0, "LuckyDraw RENEW:%d - %d", t_CurrentLuckyDraw->dId, pData->dId);

	t_CurrentLuckyDraw = std::make_unique<TLuckyDrawTable>(*pData);
}

DWORD LuckyDraw::GetId() const
{
	if (!t_CurrentLuckyDraw)
	{
		sys_log(0, "LuckyDraw::GetId() called but t_CurrentLuckyDraw is nullptr");
		return 0;
	}

	sys_log(0, "LuckyDraw::GetId() - Current LuckyDraw ID: %u", t_CurrentLuckyDraw->dId);
	return t_CurrentLuckyDraw->dId;
}