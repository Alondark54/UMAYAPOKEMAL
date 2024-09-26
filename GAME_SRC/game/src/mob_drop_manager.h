#if !defined(__INCLUDE_MOB_DROP_MANAGER_HEADER__) && defined(ENABLE_DROP_FROM_TABLE)
#define __INCLUDE_MOB_DROP_MANAGER_HEADER__

class MOB_DROP_MANAGER : public singleton<MOB_DROP_MANAGER>
{

/*
	TYPEDEFINES, STRUCTURES, ENUMS
*/
public:
	//common drop structures
	struct CommonDropInfo{
		DWORD		dwItemVnum;
		int			iLevelStart;
		int			iLevelEnd;
		int			iDropPct;

		CommonDropInfo(TCommonDropItemTable& rTable)
		{
			dwItemVnum		= rTable.dwItemVnum;
			iLevelStart		= rTable.iLevelStart;
			iLevelEnd		= rTable.iLevelEnd;
			iDropPct		= rTable.iDropPct;
		}

		bool IsRightLevel(int iLevel)
		{
			return iLevel >= iLevelStart && iLevel<= iLevelEnd;
		}
	};

	typedef std::vector<CommonDropInfo>		COMMON_DROP_VEC;
	typedef std::map<BYTE, COMMON_DROP_VEC> COMMON_DROP_MAP;

	
	//default drop
	struct DefaultDropInfo {
		DWORD		dwItemVnum;
		int			iCount;
		float			iProb;

		DefaultDropInfo( TDropItemTable& table)
		{
			dwItemVnum		= table.dwItemVnum;
			iCount			= table.iCount;
			iProb			= table.iProb;
		}
	};

	typedef std::vector<DefaultDropInfo>		DEFAULT_DROP_VEC;
	typedef std::map<DWORD,DEFAULT_DROP_VEC>	DEFAULT_DROP_MAP;

	

	//mob drop kill
	struct MobDropKillInfo
	{
		DWORD		dwItemVnum;
		int			iCount;
		int			iPartPct;

		MobDropKillInfo(TMobDropItemKill& table)
		{
			dwItemVnum		= table.dwItemVnum;
			iCount			= table.iCount;
			iPartPct		= table.iPartPct;
		}
	};
	typedef std::vector<MobDropKillInfo> MOB_DROP_KILL_INFO_VEC;

	struct MobDropKillGroupInfo
	{
		int								iKillCount;
		MOB_DROP_KILL_INFO_VEC			itemVector;
		int								iTotalRange;

		void CalculateTotalPct()
		{
			int tot = 0;
			for (MOB_DROP_KILL_INFO_VEC::iterator it = itemVector.begin(); it != itemVector.end(); it++)
				tot += it->iPartPct;
			iTotalRange = tot;
		}

		int GetTotalRange()
		{
			return iTotalRange;
		}


		MobDropKillInfo& GetOne()
		{
			int extract = number(1, GetTotalRange());
			int tot = 0;
			
			for (MOB_DROP_KILL_INFO_VEC::iterator it = itemVector.begin(); it != itemVector.end(); it++)
			{
				tot += it->iPartPct;
				if(tot > extract)
					return *it;
			}

			return itemVector.back();
		}

		bool IsEmpty()
		{
			return itemVector.empty();
		}
	};

	typedef std::map<DWORD,MobDropKillGroupInfo>			MOB_DROP_GROUP_KILL_MAP_ID;
	typedef std::map<DWORD, MOB_DROP_GROUP_KILL_MAP_ID>		MOB_DROP_GROUP_KILL_MAP_VNUM;
	typedef std::map<DWORD,DWORD>							MOB_DROP_GROUP_MAP_ID_TO_MAP;

	//mob drop level
	struct MobDropLevelInfo
	{
		DWORD		dwItemVnum;
		int			iCount;
		int			iDropPct;

		MobDropLevelInfo( TMobDropItemLevel& table)
		{
			dwItemVnum		= table.dwItemVnum;
			iCount			= table.iCount;
			iDropPct		= table.iDropPct;
		}
	};

	typedef std::vector<MobDropLevelInfo> MOB_DROP_LEVEL_INFO_VEC;

	struct MobDropLevelGroupInfo
	{
		int								iLevelStart;
		int								iLevelEnd;
		MOB_DROP_LEVEL_INFO_VEC			itemVector;

	};

	typedef std::map<DWORD,MobDropLevelGroupInfo>	MOB_DROP_LEVEL_ID_MAP;

	typedef std::map<DWORD, MOB_DROP_LEVEL_ID_MAP>	MOB_DROP_GROUP_LEVEL_MAP_VNUM;
	typedef std::map<DWORD,DWORD>					MOB_DROP_GROUP_LEVEL_ID_TO_VNUM;




/*
	PUBLIC METHODS
*/
public:
	MOB_DROP_MANAGER();
	~MOB_DROP_MANAGER();


	void Initialize();
	void Destroy();



	bool RegisterDropInfo(TPacketDGDropTables* pPack, const char * pData);
	void ClearDropInfo();

	void MakeDropItems(LPCHARACTER pkChar , LPCHARACTER pkKiller , std::vector<LPITEM>& items);

	#ifdef __SEND_TARGET_INFO__
	void MakeDropInfoItems(LPCHARACTER pkChar , LPCHARACTER pkKiller , std::vector<LPITEM>& items);
	#endif


/*
	PRIVATE METHODS
*/
private:
	COMMON_DROP_MAP						m_commonDropMap;
	DEFAULT_DROP_MAP					m_defaultDropMap;

	MOB_DROP_GROUP_KILL_MAP_VNUM		m_dropGroupKillMap;
	MOB_DROP_GROUP_MAP_ID_TO_MAP		m_dropGroupKillIDToVnum;


	MOB_DROP_GROUP_LEVEL_MAP_VNUM		m_dropGroupLevelMap;
	MOB_DROP_GROUP_LEVEL_ID_TO_VNUM		m_dropGroupLevelIDToVnum;
/*
	PRIVATE MEMBERS
*/
private:
	

};





#endif //__INCLUDE_MOB_DROP_MANAGER_HEADER__

