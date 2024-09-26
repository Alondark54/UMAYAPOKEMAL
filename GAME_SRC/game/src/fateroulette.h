#ifndef __FATEROULETTE__
#define __FATEROULETTE__

class CFateRoulette : public singleton<CFateRoulette>
{
	public:

		typedef std::map<DWORD, short> TFateRoulette;
		
		CFateRoulette();
		virtual ~CFateRoulette();
		
		void	Initialize();
		DWORD	ItemSize();
		void	SetReward(LPCHARACTER ch);
		
	
	protected:
		TFateRoulette	m_map_pkFateRoulette;

};

#endif
