#include "stdafx.h"
#ifdef ENABLE_AUTO_EVENTS
#include "config.h"
#include "auto_event_manager.h"
#ifdef ENABLE_CAOS_EVENT
	#include "NewCaosEvent.h"
#endif
#include "questmanager.h"
static LPEVENT running_event = NULL;

EVENTINFO(EventsManagerInfoData)
{
	CAutoEventSystem* pEvents;

	EventsManagerInfoData()
		: pEvents(0)
	{
	}
};

EVENTFUNC(automatic_event_timer)
{
	if (event == NULL)
		return 0;

	if (event->info == NULL)
		return 0;

	EventsManagerInfoData * info = dynamic_cast<EventsManagerInfoData*>(event->info);

	if (info == NULL)
		return 0;

	auto * pInstance = info->pEvents;

	if (pInstance == NULL) { return 0; }

	CAutoEventSystem::instance().PrepareChecker();
	return PASSES_PER_SEC(60);
}

void CAutoEventSystem::PrepareChecker()
{
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);

	int day = vKey.tm_wday;
	int hour = vKey.tm_hour;
	int minute = vKey.tm_min;
	int second = vKey.tm_sec;
#ifdef ENABLE_CAOS_EVENT
	if (quest::CQuestManager::instance().GetEventFlag("caos_event") == 1) {
        CNewCaosEventManager::instance().CheckEvent(day, hour, minute);
   	}
#endif
}

void CAutoEventSystem::Check(int day, int hour, int minute, int second)
{
	return;
}

bool CAutoEventSystem::Initialize()
{
	if (running_event != NULL)
	{
		event_cancel(&running_event);
		running_event = NULL;
	}

	auto* info = AllocEventInfo<EventsManagerInfoData>();
	info->pEvents = this;

	running_event = event_create(automatic_event_timer, std::move(info), PASSES_PER_SEC(30));
	return true;
}



void CAutoEventSystem::Destroy()
{
	if (running_event != NULL)
	{
		event_cancel(&running_event);
		running_event = NULL;
	}
}
#endif