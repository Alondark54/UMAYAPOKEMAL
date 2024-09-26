#include "StdAfx.h"
#include "Timer.h"

static LARGE_INTEGER gs_liTickCountPerSec;
static DWORD gs_dwBaseTime=0;
static DWORD gs_dwServerTime=0;
static DWORD gs_dwClientTime=0;
static DWORD gs_dwFrameTime=0;

#pragma comment(lib, "winmm.lib")

BOOL ELTimer_Init()
{
	/*
	gs_liTickCountPerSec.QuadPart=0;

	if (!QueryPerformanceFrequency(&gs_liTickCountPerSec))
		return 0;

	LARGE_INTEGER liTickCount;
	QueryPerformanceCounter(&liTickCount);
	gs_dwBaseTime= (liTickCount.QuadPart*1000  / gs_liTickCountPerSec.QuadPart);
	*/
	gs_dwBaseTime = timeGetTime();
	return 1;
}

DWORD ELTimer_GetMSec()
{
	//LARGE_INTEGER liTickCount;
	//QueryPerformanceCounter(&liTickCount);
	return timeGetTime() - gs_dwBaseTime; //(liTickCount.QuadPart*1000  / gs_liTickCountPerSec.QuadPart)-gs_dwBaseTime;
}

VOID	ELTimer_SetServerMSec(DWORD dwServerTime)
{
	NANOBEGIN
	if (0 != dwServerTime)
	{
		gs_dwServerTime = dwServerTime;
		gs_dwClientTime = CTimer::instance().GetCurrentMillisecond();
	}
	NANOEND
}

DWORD	ELTimer_GetServerMSec()
{
	return CTimer::instance().GetCurrentMillisecond() - gs_dwClientTime + gs_dwServerTime;
	//return ELTimer_GetMSec() - gs_dwClientTime + gs_dwServerTime;
}

DWORD	ELTimer_GetFrameMSec()
{
	return gs_dwFrameTime;
}

DWORD	ELTimer_GetServerFrameMSec()
{
	return ELTimer_GetFrameMSec() - gs_dwClientTime + gs_dwServerTime;
}

VOID	ELTimer_SetFrameMSec()
{
	gs_dwFrameTime = ELTimer_GetMSec();
}

CTimer::CTimer()
{
	ELTimer_Init();

	NANOBEGIN
	if (this)
	{
		m_dwCurrentTime = 0;
		m_bUseRealTime = true;
		m_index = 0;

		m_dwElapsedTime = 0;

		m_fCurrentTime = 0.0f;
#ifdef ENABLE_FPS
			// 60FPS DEFAULT CONFIG
			m_firstArg = 16;
			m_SecondArg = 1;
#endif

	}
	NANOEND
}

CTimer::~CTimer()
{
}

void CTimer::SetBaseTime()
{
	m_dwCurrentTime = 0;
}

void CTimer::Advance()
{
	if (!m_bUseRealTime)
	{
		++m_index;

		if (m_index == 1)
			m_index = -1;

#ifdef ENABLE_FPS
		m_dwCurrentTime += m_firstArg+ (m_index & m_SecondArg);
#else
		m_dwCurrentTime += 16 + (m_index & 1);
#endif
		m_fCurrentTime = m_dwCurrentTime / 1000.0f;
	}
	else
	{
		DWORD currentTime = ELTimer_GetMSec();

		if (m_dwCurrentTime == 0)
			m_dwCurrentTime = currentTime;

		m_dwElapsedTime = currentTime - m_dwCurrentTime;
		m_dwCurrentTime = currentTime;
	}
}

void CTimer::Adjust(int iTimeGap)
{
	m_dwCurrentTime += iTimeGap;
}

float CTimer::GetCurrentSecond()
{
	if (m_bUseRealTime)
		return ELTimer_GetMSec() / 1000.0f;

	return m_fCurrentTime;
}

DWORD CTimer::GetCurrentMillisecond()
{
	if (m_bUseRealTime)
		return ELTimer_GetMSec();

	return m_dwCurrentTime;
}

float CTimer::GetElapsedSecond()
{
	return GetElapsedMilliecond() / 1000.0f;
}

DWORD CTimer::GetElapsedMilliecond()
{
	if (!m_bUseRealTime)
#ifdef ENABLE_FPS
		return m_firstArg + (m_index & m_SecondArg);
#else
		return 16 + (m_index & 1);
#endif

	return m_dwElapsedTime;
}

void CTimer::UseCustomTime()
{
	m_bUseRealTime = false;
}

#ifdef ENABLE_FPS
void CTimer::SetUpdateTime(BYTE first, BYTE sec)
{
	m_firstArg = first;
	m_SecondArg = sec;
}
#endif
//martysama0134's 2022
