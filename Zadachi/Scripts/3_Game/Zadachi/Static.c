enum KTQRPC
{
	SEND_INIT_C = -99314111,
	SENC_INIT_ACTIVE_QUESTS,
	SENC_ACTIVE_QUESTS,
	ERROR_QUESTS,
	SEND_ACCEPT_QUEST,
	SEND_FINISH_QUEST,
	SEND_CANCEL_QUEST,
	SYNC_DB,
}


enum KTQuestType
{
	Kill = 1,
	Delivery,
	Search,
	Craft,
	Fishing,
	Harvest,
	Exploration,
	Action
}

class KTStatic
{
	static int UnixNow()
	{
		int year, mon, day, hour, min, sec;
		GetYearMonthDayUTC(year, mon, day);
		GetHourMinuteSecondUTC(hour, min, sec);

		year -= 2000;
		
		return ((year * 365 + mon * 30 + day) * 86400) + sec + min * 60 + hour * 3600;
	}

	static string UnixToString(int stamp)
	{
		string result;
		int day, hour, min, sec;
		day = stamp / 86400;
		hour = (stamp - day * 86400) / 3600;
		min = (stamp - (day * 86400 + hour * 3600)) / 60;
		sec = stamp % 60;

		if (day)
			result = day.ToString() + "d ";
		if (hour)
			result += hour.ToString() + "h ";
		if (min)
			result += min.ToString() + "m ";
		if (sec)
			result += sec.ToString() + "s";
		return result;
	}

	static void Vanish(Widget w)
	{
		while (w.GetChildren())
		{
			w.RemoveChild(w.GetChildren());
		}
	}
}