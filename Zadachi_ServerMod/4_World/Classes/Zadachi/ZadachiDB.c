modded class KTQuestDB
{
	void KTQuestDB(PlayerBase _player)
	{
		LoadDB();
	}

	string GetSteamID()
	{
		return player.GetIdentity().GetPlainId();
	}

	void LoadDB()
	{
		completedQuests = KTQuestFileManager.LoadDB(GetSteamID());

		CheckTimesUp();

		SyncDB();
	}

	void RemoveRecord(KTQuestCompleted data)
	{
		int idx = completedQuests.Find(data);
		if (idx + 1)
			completedQuests.Remove(idx);
	}

	override int GetCooldown(int id)
	{
		return GetKTPluginQuestStore().GetQuests().Get(id).Cooldown;
	}

	void InsertRecord(int id)
	{
		KTQuestCompleted newRecord = new KTQuestCompleted();
		newRecord.ID = id;
		newRecord.FinishingAt = KTStatic.UnixNow();
		completedQuests.Insert(newRecord);

		KTQuestFileManager.SaveDB(completedQuests, GetSteamID());
		SyncDB();
	}

	bool CheckTimesUp()
	{
		bool hasChanges = false;

		foreach (KTQuestCompleted data : completedQuests)
		{
			if (!data)
				continue;
				
			if (!CooldownLeftTime(data.ID))
			{
				RemoveRecord(data);
				hasChanges = true;
			}
		}

		if (hasChanges)
		{
			KTQuestFileManager.SaveDB(completedQuests, GetSteamID());
		}

		return hasChanges;
	}

	void Update()
	{
		if (CheckTimesUp())
			SyncDB();
	}

	void SyncDB()
	{
		player.RPCSingleParam(KTQRPC.SYNC_DB, new Param1<array<ref KTQuestCompleted>>(completedQuests), true, player.GetIdentity());
	}
}