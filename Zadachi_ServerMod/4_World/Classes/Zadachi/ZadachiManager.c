modded class KTQuestManager
{
	private ref array<ref KTQuestProgress> saves;
	private float timeSinceLastTick;

	void KTQuestManager(PlayerBase _player)
	{
		activeQuests = new map<int, ref KTQuestBase>();

		InitLoad();
	}

	void ~KTQuestManager()
	{
		
	}

	string GetSteamID()
	{
		return player.GetIdentity().GetPlainId();
	}

	PlayerBase GetOwner()
	{
		return player;
	}

	void InitLoad()
	{
		int idx;
		bool needSave = false;
		saves = KTQuestFileManager.LoadActiveQuests(GetSteamID());

		foreach (KTQuestProgress questData : saves)
		{
			if (TimesUp(questData) && questData.Completed != 1)
			{
				idx = saves.Find(questData);
				saves.Remove(idx);
				needSave = true;

				NotifQuestFailed(questData.ID);
			}
		}

		//TEST();

		if (needSave)
			KTQuestFileManager.SaveActiveQuests(saves, GetSteamID());
		
		InitQuestsServer();
		InitQuestsClient();
	}

	bool TimesUp(KTQuestProgress quest)
	{
		int limit = GetKTPluginQuestStore().GetQuests().Get(quest.ID).TimeLimit;

		if (limit == -1)
			return false;

		return (quest.StartTime + limit - KTStatic.UnixNow()) <= 0;
	}

	void InitQuestsClient()
	{
		SendActiveQuests();
	}

	void SendActiveQuests()
	{
		Response(KTQRPC.SENC_INIT_ACTIVE_QUESTS, new Param1<array<ref KTQuestProgress>>(saves));
	}

	void InitQuestsServer()
	{
		foreach (KTQuestProgress questData : saves)
		{
			activeQuests.Insert(questData.ID, new KTQuestBase(questData, this));
		}
	}

	bool HasActiveQuest(int id)
	{
		return activeQuests.Contains(id);
	}

	void OnUpdate(float dt)
	{
		timeSinceLastTick += dt;

		if (timeSinceLastTick > 5.0)
		{
			questDB.Update();

			for (int i = 0; i < activeQuests.Count(); i++)
			{
				activeQuests.GetElement(i).Update();
			}

			timeSinceLastTick = 0;
		}
	}

	void ActivateQuest(int id)
	{
		KTQuestData template = GetKTPluginQuestStore().GetQuests().Get(id);

		if (!template || HasActiveQuest(id))
		{
			NotificationSystem.SendNotificationToPlayerExtended(player, 5, "#ktnotif_error_title", "#ktnotif_error_take_quest");
			return;
		}

		KTQuestProgress newQuest = new KTQuestProgress();
		newQuest.ID = id;
		newQuest.StartTime = KTStatic.UnixNow();

		for (int i = 0; i < template.Targets.Count(); i++)
		{
			newQuest.Progress.Insert(0);
		}

		saves.Insert(newQuest);
		KTQuestFileManager.SaveActiveQuests(saves, GetSteamID());
		activeQuests.Insert(id, new KTQuestBase(newQuest, this));

		OnQuestStarted(id);

		SendActiveQuests();
	}

	void Response(int packetId, Param params)
	{
		if (!player)
			return;

		player.RPCSingleParam(packetId, params, true, player.GetIdentity());
	}

	void OnQuestFailed(KTQuestBase inst)
	{
		RemoveActiveQuest(inst);
		SendActiveQuests();

		NotifQuestFailed(inst.GetID());//NotificationSystem.SendNotificationToPlayerExtended(player, 5, "#ktnotif_failed_quest_title", string.Format(Widget.TranslateString("#ktnotif_failed_quest_desc"),GetKTPluginQuestStore().GetQuests().Get(inst.GetID()).Name));
	}

	void NotifQuestFailed(int id)
	{
		NotificationSystem.SendNotificationToPlayerExtended(player, 5, "#ktnotif_failed_quest_title", GetKTPluginQuestStore().GetQuests().Get(id).Name, "set:dayz_gui image:exit");
	}

	void RemoveActiveQuest(KTQuestBase inst)
	{
		if (!activeQuests.Contains(inst.GetID()))
			return;
	
		KTQuestProgress temp = inst.GetData();
		int idx = saves.Find(temp);

		activeQuests.Remove(inst.GetID());

		if (idx >= 0)
		{
			saves.Remove(idx);
			KTQuestFileManager.SaveActiveQuests(saves, GetSteamID());
		}
	}

	void UpdateQuests(KTQuestType type, string target, float count, string extra)
	{
		bool needUpdate = false;
		KTQuestBase tempQuest;
		KTQuestData tempData;
		for (int i = 0; i < activeQuests.Count(); i++)
		{
			tempQuest = activeQuests.GetElement(i);
			tempData = tempQuest.GetQuestT();
			for (int j = 0; j < tempData.Targets.Count(); j++)
			{
				if (tempData.Targets[j].questType == type)
					needUpdate = tempQuest.UpdateProgress(target, count, extra, j) || needUpdate;
			}
		}

		if (needUpdate)
		{
			KTQuestFileManager.SaveActiveQuests(saves, GetSteamID());
			SendActiveQuests();
		}
	}

	void ForceSaveProgress()
	{
		KTQuestFileManager.SaveActiveQuests(saves, GetSteamID());
		SendActiveQuests();
	}

	void OnCompleteQuest(KTQuestBase quest)
	{
		NotificationSystem.SendNotificationToPlayerExtended(player, 5, "#ktnotif_complited_quest_title", GetKTPluginQuestStore().GetQuests().Get(quest.GetID()).Name, "set:dayz_gui image:tutorials");
	}

	override void OnRPC(int rpc_type, ParamsReadContext ctx)
	{
		switch (rpc_type)
		{
			case KTQRPC.SEND_ACCEPT_QUEST:
				HandleAcceptQuest(ctx);
			break;
			case KTQRPC.SEND_FINISH_QUEST:
				HandleFinishQuest(ctx);
			break;
			case KTQRPC.SEND_CANCEL_QUEST:
				HandleCancelQuest(ctx);
			break;
		}
	}

	void OnQuestStarted(int id)
	{
		KTQuestData template = GetKTPluginQuestStore().GetQuests().Get(id);
		NotificationSystem.SendNotificationToPlayerExtended(player, 5, "#ktnotif_apply_quest_title", template.Name, "set:dayz_gui image:tutorials");
	}

	void HandleAcceptQuest(ParamsReadContext ctx)
	{
		Param1<int> data;
		if (!ctx.Read(data))
			return;
		
		ActivateQuest(data.param1);
	}

	void HandleCancelQuest(ParamsReadContext ctx)
	{
		Param1<int> data;
		if (!ctx.Read(data))
			return;
		
		CancelQuest(data.param1);
	}

	void HandleFinishQuest(ParamsReadContext ctx)
	{
		Param1<int> data;
		if (!ctx.Read(data))
			return;

		FinishQuest(data.param1);
	}

	void CancelQuest(int questID)
	{
		KTQuestBase inst = activeQuests.Get(questID);
		if (!inst)
			return;

		OnQuestFailed(inst);
	}

	void FinishQuest(int questID)
	{
		KTQuestData template = GetKTPluginQuestStore().GetQuests().Get(questID);
		KTQuestBase mission = activeQuests.Get(questID);

		array<ref KTQuestDefaultData> tempTargets = {};

		if (!template || !mission)
		{
			NotificationSystem.SendNotificationToPlayerExtended(player, 5, "#ktnotif_error_title", "#ktnotif_error_finish_quest", "set:dayz_gui image:exit");
			return;
		}

		for (int i = 0; i < template.Targets.Count(); i++)
		{
			if (template.Targets[i].questType == 2)
			{
				tempTargets.Insert(template.Targets[i]);
			}
		}

		if (HasRequiredItemsForDelivery(questID))
			DeleteItems(tempTargets);
		else
		{
			NotificationSystem.SendNotificationToPlayerExtended(player, 5, "#ktnotif_error_title", "#ktnotif_error_finish_quest", "set:dayz_gui image:exit");
			return;
		}

		GiveReward(template.Rewards);

		RemoveActiveQuest(mission);
		questDB.InsertRecord(questID);
		SendActiveQuests();
	}

	void DebugDeletItems(map<string, float> deletedItems)
	{
		Print("deletedItems");
		foreach(string key, float value : deletedItems)
		{
			Print("key: "+key+ " value: "+value);
		}
	}

	void DeleteItems(array<ref KTQuestDefaultData> toDelete)
	{
		array<EntityAI> itemsArray = new array<EntityAI>;
		map<string, float> deletedItems = new map<string, float>();
		ItemBase item;
		string itemType;
		int quantity;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
		
		CreateCondition(deletedItems, toDelete);

		//DebugDeletItems(deletedItems);

		for (int i = 0; i < itemsArray.Count(); i++)
		{
			Class.CastTo(item, itemsArray.Get(i));
			if (item && !item.IsInherited(SurvivorBase))
			{
				itemType = item.GetType();
				if (deletedItems.Contains(itemType))
				{
					if (!HasCondition(item, toDelete))
						continue;

					int val = deletedItems.Get(itemType);

					if (HasQuantity(item))
					{
						quantity = QuantityConversions.GetItemQuantity(item);

						if (val == quantity)
						{
							val = 0;
							item.DeleteSave();
						}
						else if (val < quantity)
						{
							quantity -= val;
							val = 0;

							if (item.IsMagazine())
							{
								Magazine mag = Magazine.Cast(item);
								mag.ServerSetAmmoCount(quantity);
							}
							else
							{
								item.SetQuantity(quantity);
							}	
						}
						else
						{
							val -= quantity;
							item.DeleteSave();
						}
					}
					else
					{
						item.DeleteSave();
						val -= 1;
					}

					if (val == 0)
						deletedItems.Remove(itemType);
					else
						deletedItems.Set(itemType, val);
				}
			}
		}
	}

	bool HasCondition(ItemBase item, array<ref KTQuestDefaultData> targets)
	{
		string type = item.GetType();

		bool health = true;
		bool perc = true;
		for(int i = 0; i < targets.Count(); i++)
		{
			if (targets[i].type == type)
			{
				if (targets[i].conditions[0] != -1)
					health = item.GetHealthLevel() <= targets[i].conditions[0]
				if (targets[i].conditions[1] != -1)
					perc = HasQuantityPercent(targets[i].conditions[1], item);
			}
		}

		return health && perc;
	}

	bool HasQuantityPercent(int percent, ItemBase item)
	{
		float q_cur, q_min, q_max;
		int q_type = QuantityConversions.HasItemQuantity(item);
		if(q_type > 0)
		{
			QuantityConversions.GetItemQuantity(item, q_cur, q_min, q_max);

			q_cur = (q_cur / q_max);
			q_cur = (int)(q_cur * 100);

			return q_cur >= percent;
		}
		
		return true;
	}
	
	bool HasQuantity(ItemBase item)
	{
		return QuantityConversions.HasItemQuantity(item) == 1;
	}

	void CreateCondition(out map<string, float> deletedItems, array<ref KTQuestDefaultData> toDelete)
	{
		foreach (KTQuestDefaultData data : toDelete)
		{
			deletedItems.Insert(data.type, data.count);
		}
	}

	void GiveReward(array<ref KTQuestDefaultData> rewards)
	{
		string extra;
		bool notEnoughSpace;
		EntityAI reward;
		ItemBase ibReward;
		foreach (KTQuestDefaultData data : rewards)
		{
			for (int i = 0; i < data.count; i++)
			{
				reward = player.GetInventory().CreateInInventory(data.type);

				if (!reward)
				{
					notEnoughSpace = true;
					reward = player.SpawnEntityOnGroundPos(data.type, player.GetPosition());
				}

				extra = data.extra;

				if (reward)
				{
					if (extra && CastTo(ibReward, reward))
						SetQuantity(extra, ibReward);
					if (data.conditions[0] != -1)
						reward.SetHealth01("", "", GetHealthKoef(data.conditions[0]));
				}
			}
		}

		if (notEnoughSpace)
			NotificationSystem.SendNotificationToPlayerExtended(player, 5, "#ktreward_not_enough_space_title", "#ktreward_not_enough_space_desc", "set:dayz_gui image:icon_feedback");
	}

	float GetHealthKoef(int level)
	{
		switch(level)
		{
			case 0:
				return GameConstants.DAMAGE_PRISTINE_VALUE;
			break;
			case 1:
				return GameConstants.DAMAGE_WORN_VALUE;
			break;
			case 2:
				return GameConstants.DAMAGE_DAMAGED_VALUE;
			break;
			case 3:
				return GameConstants.DAMAGE_BADLY_DAMAGED_VALUE;
			break;
			case 4:
				return GameConstants.DAMAGE_RUINED_VALUE;
			break;
		}

		return 0;
	}

	void SetQuantity(string countRaw, ItemBase ibReward)
	{
		bool percent;
		float quantity;
		countRaw.ToLower();

		percent = countRaw.Contains("%");
		countRaw.Replace("%", "");
		countRaw.Replace(" ", "");

		if (ibReward.IsMagazine())
		{
			Magazine mag = Magazine.Cast(ibReward);
			if (countRaw == "max")
				mag.ServerSetAmmoMax();
			else
			{
				quantity = countRaw.ToFloat();

				if (quantity)
				{
					if (percent)
					{
						quantity = Math.Clamp(quantity / 100, 0, 1);
						quantity = Math.Round(Math.Lerp(0, mag.GetAmmoMax(), quantity));
					}
	
					mag.ServerSetAmmoCount(quantity);
				}
			}
		}
		else
		{
			if (countRaw == "max")
				ibReward.SetQuantityMax();
			else
			{
				quantity = countRaw.ToFloat();

				if (quantity)
				{
					if (percent)
					{
						quantity = Math.Clamp(quantity / 100, 0, 1);
						ibReward.SetQuantityNormalized(quantity);
						return;
					}

					ibReward.SetQuantity(quantity);
				}
			}
		}
	}

	bool HasRequiredItemsForDelivery(int questID)
	{
		KTQuestData template = GetKTPluginQuestStore().GetQuests().Get(questID);

		if (!template)
			return false;

		array<EntityAI> itemsArray = new array<EntityAI>;
		map<string, float> requiredItems = new map<string, float>();
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

				requiredItems.Set(itemType, requiredItems.Get(itemType) + 1);
			}
		}

		float has;
		for (int k = 0; k < targets.Count(); k++)
		{
			if (targets[k].questType != 2 || targets[k].questType != 3)
				continue;
			has = requiredItems.Get(targets[k].type);
			if (has < targets[k].count)
				return false;
		}

		return true;
	}
}