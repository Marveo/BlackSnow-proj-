class KTQuestDB
{
	private ref array<ref KTQuestCompleted> completedQuests;
	private PlayerBase player;

	void KTQuestDB(PlayerBase _player)
	{
		player = _player;
		completedQuests = {};
	}

	void SetDB(array<ref KTQuestCompleted> _db)
	{
		completedQuests = _db;
	}

	KTQuestCompleted GetQuest(int id)
	{
		foreach (KTQuestCompleted quest : completedQuests)
		{
			if (quest.ID == id)
				return quest;
		}
		return null;
	}

	int GetCooldown(int id)
	{
		return PlayerBase.Cast(GetGame().GetPlayer()).GetQuestManager().GetQuestTemplate(id).Cooldown;
	}

	bool HasQuest(int id)
	{
		return GetQuest(id) || 0;
	}

	int CooldownLeftTime(int id)
	{
		if (!GetQuest(id))
			return -1;
			
		int finishingAt = GetQuest(id).FinishingAt;
		int cooldownTime = GetCooldown(id);

		if (cooldownTime == -1)
			return -1;

		return Math.Clamp(finishingAt + cooldownTime - KTStatic.UnixNow(), 0, int.MAX);
	}

	string CooldownLeftTimeText(int id)
	{
		return KTStatic.UnixToString(CooldownLeftTime(id));
	}
}