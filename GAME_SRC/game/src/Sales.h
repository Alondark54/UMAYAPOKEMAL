class CSales : public singleton<CSales>
{

	public:
		CSales();
		virtual ~CSales();
		void SalesCache();
		void List(LPCHARACTER ch);
		bool CheckSales();
		void BuySales(LPCHARACTER ch, DWORD id);
		
		typedef struct SSalesInfo
		{
			DWORD	sales_id;
			DWORD	sales_price;
			DWORD	normal_price;
			DWORD	type;
			DWORD	hour;
			DWORD	minute;
			DWORD	day;
			DWORD	month;
			std::map<DWORD, DWORD> items;

			SSalesInfo()
			{
				sales_id = sales_price = normal_price = type = hour = minute = day = month = 0;
			}
		} TSalesInfo;
		
		

	// private:
		// void BuySales(LPCHARACTER ch, DWORD id);
		
		
	protected:
		typedef std::vector<TSalesInfo*>	TSalesInfoMap;
		TSalesInfoMap	m_salesvector;
};