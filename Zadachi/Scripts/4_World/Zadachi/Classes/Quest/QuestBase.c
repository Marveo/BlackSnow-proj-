class KTQuestBase
{
	private KTQuestData template;
	private ref KTQuestProgress data;
	private ref KTQuestManager parent;

	void KTQuestBase(KTQuestProgress initData, KTQuestManager _parent)
	{
		data = initData;
		parent = _parent;
	}

	void Load(KTQuestProgress _data)
	{
		data = _data;
	}

	KTQuestProgress GetData()
	{
		return data;
	}

	int GetID()
	{
		return data.ID;
	}

	int LeftTime()
	{
		if (GetQuestT().TimeLimit == -1)
			return -1;
		return Math.Clamp(data.StartTime + GetQuestT().TimeLimit - KTStatic.UnixNow(), 0, int.MAX);
	}

	string LeftTimeText()
	{
		return KTStatic.UnixToString(LeftTime());
	}

	KTQuestData GetQuestT()
	{
		if (!template)
			template = PlayerBase.Cast(GetGame().GetPlayer()).GetQuestManager().GetQuestTemplate(GetID()); // долбоебизм
		return template;
	}

	bool CanFinishQuest()
	{
		KTQuestData templData = GetQuestT();
		array<float> progress = data.Progress;
		bool canBeFinished = false;
		bool checkedInventory = false;

		if (!templData)
			return false;

		for (int i = 0; i < templData.Targets.Count(); i++)
		{
			if (templData.Targets[i].questType == 2 || templData.Targets[i].questType == 3)
			{
				if (!checkedInventory)
				{
					canBeFinished = HasRequiredItems();
					checkedInventory = true;
				}
				else
					canBeFinished = true;
			}
			else
				canBeFinished = progress[i] >= templData.Targets[i].count;

			if (!canBeFinished)
				return false;
		}

	
		return true;
	}

	// bool HasRequiredItems()
	// {
	// 	PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
	// 	map<string, float> requiredItems = player.GetQuestManager().GetRequiredItems(GetQuestT().ID);
	// 	array<ref KTQuestDefaultData> targets = GetQuestT().Targets;

	// 	float has;
	// 	for (int k = 0; k < targets.Count(); k++)
	// 	{
	// 		if (targets[k].questType != 2 && targets[k].questType != 3)
	// 			continue;
			
	// 		has = requiredItems.Get(targets[k].type);
	// 		if (has < targets[k].count)
	// 			return false;
	// 	}

	// 	return true;
	// }

	bool HasRequiredItems()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		map<string, ref array<ref array<float>>> requiredItems = player.GetQuestManager().GetRequiredItems(GetQuestT().ID);
		array<ref KTQuestDefaultData> targets = GetQuestT().Targets;

		array<ref array<float>> has;
		array<float> itemData;
		int quantity, minHealth, minPercent;

		bool health, quant;
		for (int k = 0; k < targets.Count(); k++)
		{
			if (targets[k].questType != 2 && targets[k].questType != 3)
				continue;
			
			has = requiredItems.Get(targets[k].type);
			if (!has || GetQuestItemQuantity(targets[k], has) < targets[k].count)
				return false;
		}

		return true;
	}

	static int GetQuestItemQuantity(KTQuestDefaultData temp, array<ref array<float>> itemsData)
	{
		int quantity, minHealth, minPercent;
		bool health, quant;
		array<float> itemData;

		quantity = 0;
		for (int i = 0; i < itemsData.Count(); i++)
		{
			health = true;
			quant = true;

			itemData = itemsData[i];
			minHealth = temp.conditions[0];
			minPercent = temp.conditions[1];

			if (minHealth != -1)
				health = itemData[1] <= minHealth;
			if (minPercent != -1)
				quant = itemData[3] >= minPercent;

			if (health && quant)
				quantity += itemData[2];
		}

		return quantity;
	}
}