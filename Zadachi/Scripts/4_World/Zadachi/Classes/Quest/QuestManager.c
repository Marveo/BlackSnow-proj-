class KTQuestManager
{
	private PlayerBase player;

	private int questSlots;
	private ref KTQuestDB questDB;
	private ref KTQuestConfig config;
	private ref map<int, ref KTQuestBase> activeQuests;
	private ref map<int, ref KTQuestData> questTemplates;
	private ref map<int, ref array<ref array<int>>> questNpcConditions;

	private ref array<bool> collapseSaves;

	void KTQuestManager(PlayerBase _player)
	{
		player = _player;
		questDB = new KTQuestDB(player);

		collapseSaves = {false, false, false, false};
	}

	bool CanTakeQuest(int id)
	{
		return HasFreeSlots() && !InProgress(id) && !questDB.HasQuest(id) && !HasMissedQuests(id).Count();
	}

	bool CanFinishQuest(int id)
	{
		KTQuestBase temp = activeQuests.Get(id);
		return (InProgress(id) && (temp.GetData().Completed == 1 || temp.CanFinishQuest()));
	}

	bool HasFreeSlots()
	{
		return activeQuests.Count() < GetQuestCap(); 
	}

	array<bool> GetCollapseSaves()
	{
		return collapseSaves;
	}

	void SetCollapsedState(array<bool> states)
	{
		collapseSaves = states;
	}

	KTQuestDB GetDB()
	{
		return questDB;
	}

	array<int> HasMissedQuests(int questID)
	{
		array<int> result = {};
		KTQuestData qdata = GetQuestTemplate(questID);

		if (!qdata || !qdata.RequiredQuests || !qdata.RequiredQuests.Count())
		{
			return result;
		}

		foreach (int id : qdata.RequiredQuests)
		{
			if (!questDB.HasQuest(id))
				result.Insert(id);
		}

		return result;
	}

	bool InProgress(int id)
	{
		if (!activeQuests)
			return false;

		return activeQuests.Contains(id);
	}

	string GetMissedQuestNames(int questID)
	{
		string result;
		KTQuestData tempQuestData;
		array<int> missedQuests = HasMissedQuests(questID);
		foreach (int id : missedQuests)
		{
			tempQuestData = GetQuestTemplate(id);
			result += tempQuestData.Name + ", ";
		}
		
		return result.Substring(0, result.Length() - 2);
	}

	map<int, ref KTQuestBase> GetActiveQuests()
	{
		return activeQuests;
	}

	KTQuestData GetQuestTemplate(int id)
	{
		return questTemplates.Get(id);
	}

	map<int, ref KTQuestData> GetQuestTemplates()
	{
		return questTemplates;
	}

	array<ref array<int>> GetQuestCondition(int id)
	{
		return questNpcConditions.Get(id);
	}

	bool StartQuestBelongAnyNPC(int questID)
	{
		foreach (int key, array<ref array<int>> arr : questNpcConditions)
		{
			if (arr[0].Find(questID) >= 0)
			{
				return true;
			}
		}
		return false;
	}

	bool FinishQuestBelongAnyNPC(int questID)
	{
		foreach (int key, array<ref array<int>> arr : questNpcConditions)
		{
			if (arr[1].Find(questID) >= 0)
			{
				return true;
			}
		}
		return false;
	}

	bool QuestBelongNPC(int questID, int npcID)
	{
		if (!npcID)
			return !FinishQuestBelongAnyNPC(questID);
		array<int> quests = GetQuestCondition(npcID)[1];
		if (!quests)
			return false;
		return (quests.Find(questID) >= 0);
	}

	array<float> GetProgress(int id)
	{
		if (!InProgress(id))
			return {};
		return activeQuests.Get(id).GetData().Progress;
	}

	int GetQuestCap()
		return config.QuestSlots;
	bool HideUnavailableQuests()
		return config.HideUnavailableQuests;
	bool CanCancelQuest()
		return config.CanCancelQuest;
	int ImportanceNPC()
		return config.ImportanceNPC;

	array<ref array<int>> GetSortedQuests()
	{
		array<ref array<int>> results = new array<ref array<int>>();
		array<int> quests = GetQuestTemplates().GetKeyArray();

		results.Insert(new array<int>);
		results.Insert(new array<int>);
		results.Insert(new array<int>);
		results.Insert(new array<int>);

		foreach (int id : quests)
		{
			if (CanTakeQuest(id))
			{
				results[ColContainers.AvailableQuests].Insert(id);
				continue;
			}
			if (InProgress(id))
			{
				results[ColContainers.InProgressQuests].Insert(id);
				continue;
			}

			if (questDB.HasQuest(id))
			{
				results[ColContainers.CompletedQuests].Insert(id);
				continue;
			}

			if (HasMissedQuests(id).Count() || !HasFreeSlots())
			{
				results[ColContainers.UnavailableQuests].Insert(id);
				continue;
			}
		}

		return results;
	}

	void OnRPC(int rpc_type, ParamsReadContext ctx)
	{
		switch (rpc_type)
		{
			case KTQRPC.SEND_INIT_C:
				HandleInitData(ctx);
			break;
			case KTQRPC.SENC_INIT_ACTIVE_QUESTS:
				HandleInitQuests(ctx);
			break;
			case KTQRPC.SYNC_DB:
				HandleSyncDB(ctx);
			break;
		}
	}

	void HandleSyncDB(ParamsReadContext ctx)
	{
		Param1<array<ref KTQuestCompleted>> data;
		if (!ctx.Read(data))
			return;

		questDB.SetDB(data.param1);
		OnQuestsUpdated();
	}

	void HandleInitData(ParamsReadContext ctx)
	{
		Param3< ref map<int, KTQuestData>, KTQuestConfig, ref map<int, array<ref array<int>>>> data;
		if (!ctx.Read(data))
			return;

		questTemplates = new map<int, ref KTQuestData>();
		questTemplates.Copy(data.param1);

		config = data.param2;

		questNpcConditions = new map<int, ref array<ref array<int>>>();
		questNpcConditions.Copy(data.param3);
		//DebugShowMap();
	}

	void DebugShowMap()
	{
		Print("\n\n\t DebugShowMap");

		foreach (int key, KTQuestData value : questTemplates)
		{
			Print("\n");
			Print("key: "+key);
			Print("ID: "+value.ID);
			Print("Name: "+value.Name);
			Print("RequiredQuests: "+value.RequiredQuests);
			Print("\n");
		}

		Print("\n\n\t ./DebugShowMap");
	}

	void HandleInitQuests(ParamsReadContext ctx)
	{
		Param1<array<ref KTQuestProgress>> data;
		if (!ctx.Read(data))
			return;

		array<ref KTQuestProgress> temp = data.param1;
		activeQuests = new map<int, ref KTQuestBase>();
		
		foreach (KTQuestProgress item : temp)
		{
			activeQuests.Insert(item.ID, new KTQuestBase(item, this));
		}


		OnQuestsUpdated();
	}

	void OnQuestsUpdated()
	{
		GetQuestMenuHandler().RecreateMenu();
	}

	map<string, ref array<ref array<float>>> GetRequiredItems(int questID)
	{
		KTQuestData template = GetQuestTemplate(questID);

		if (!template)
			return null;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		array<EntityAI> itemsArray = new array<EntityAI>;
		map<string, ref array<ref array<float>>> requiredItems = new map<string, ref array<ref array<float>>>();
		array<ref KTQuestDefaultData> targets = template.Targets;
		ItemBase item;
		string itemType;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

		for (int i = 0; i < itemsArray.Count(); i++)
		{
			Class.CastTo(item, itemsArray.Get(i));
			if (item && !item.IsInherited(SurvivorBase))
			{
				itemType = item.GetType();

				if (requiredItems.Contains(itemType))
				{
					array<ref array<float>> exist = requiredItems.Get(itemType);
					exist.Insert(GetItemData(item));
					requiredItems.Set(itemType, exist);
				}
				else
				{
					array<ref array<float>> arr = {};
					arr.Insert(GetItemData(item));
					requiredItems.Insert(itemType, arr);
				}
			}
		}

		return requiredItems;
	}

	array<float> GetItemData(ItemBase item)
	{
		float count;
		float q_cur, q_min, q_max;
		int healthLevel;
		int q_type = QuantityConversions.HasItemQuantity(item);
		if(q_type > 0)
		{
			QuantityConversions.GetItemQuantity(item, q_cur, q_min, q_max);

			count = q_cur;
			q_cur = (q_cur / q_max);
			q_cur = (int)(q_cur * 100);
		}
		
		if (q_type == QUANTITY_HIDDEN || q_type == QUANTITY_PROGRESS)
		{
			count = 1;
			if (q_type == QUANTITY_HIDDEN)
				q_cur = 1;
		}

		healthLevel = item.GetHealthLevel();

		return {q_type, healthLevel, count, q_cur};
	}
}