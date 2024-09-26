#pragma once

#include "stdafx.h"
#include "../../common/stl.h"
#include <boost/unordered_map.hpp>
#include "packet_info.h"
#include "../../common/tables.h"
#include <memory>

class LuckyDraw : public singleton<LuckyDraw>
{
public:
	LuckyDraw();
	virtual ~LuckyDraw();

	void GetCurrentLuckyDrawFromDB();
	void RESULT_ADD_PARTICIPANT(TPacketDGLuckyDrawAddParticipant* pData);
	void RESULT_LUCKYDRAW_TABLE(TLuckyDrawTable* pData);
	TLuckyDrawTable* GetCurrentLuckyDraw() const { return t_CurrentLuckyDraw.get(); }
	DWORD GetId() const;

protected:
	std::unique_ptr<TLuckyDrawTable> t_CurrentLuckyDraw = nullptr;
};

