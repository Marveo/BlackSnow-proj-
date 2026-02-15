class KTPluginQuestStore : PluginBase
{
	private ref map<int, ref KTQuestData> quests;
	private ref array<ref KTQuestData> rawQuests;

	private ref KTQuestConfig config;
 
	private ref map<int, ref array<ref array<int>>> npcQuestsData;

	void KTPluginQuestStore()
	{
		CheckDirectories();
		LoadQuests();
		LoadConfig();
	}

	void CheckDirectories()
	{
		if (!FileExist(KTQConst.ROOT_DIR))
			MakeDirectory(KTQConst.ROOT_DIR);
		if (!FileExist(KTQConst.QUESTS_DIR))
			MakeDirectory(KTQConst.QUESTS_DIR);
		if (!FileExist(KTQConst.DB_DIR))
			MakeDirectory(KTQConst.DB_DIR);
		if (!FileExist(KTQConst.ACTIVEQ_DIR))
			MakeDirectory(KTQConst.ACTIVEQ_DIR);
	}

	void LoadQuests()
	{
		string path;
		quests = new map<int, ref KTQuestData>();
		array<string> questFiles = {};
		rawQuests = {};

		KTQConst.ScanFolderForQuests(KTQConst.QUESTS_DIR, questFiles);

		if (!questFiles.Count())
		{
			Error("[QUEST]Quests data not found!");
			return;
		}

		for (int i = 0; i < questFiles.Count(); i++)
		{
			path = KTQConst.QUESTS_DIR + questFiles[i];
			// Print("LoadQuests quest data "+path);

			JsonFileLoader<array<ref KTQuestData>>.JsonLoadFile(path, rawQuests);
			if (!rawQuests.Count())
			{
				Error("[QUEST]Quests data corrupted| path:"+path);
				continue;
			}
			ConvertRawQuestToMap(rawQuests);
		}

		// Print("Alldata "+rawQuests.Count());
		// DebugShowMap();
	}

	void LoadConfig()
	{
		config = new KTQuestConfig;

		if (!FileExist(KTQConst.CONFIG))
			DefaultConfig(config);
		else
			JsonFileLoader<KTQuestConfig>.JsonLoadFile(KTQConst.CONFIG, config);
	}

	void DefaultConfig(out KTQuestConfig conf)
	{
		KTQuestNPCData npcData = new KTQuestNPCData;

		npcData.ID = 1;
		npcData.Type = "SurvivorM_Mirek";
		npcData.Position = "2150 0 2280";
		npcData.Clothes = {"TShirt_Black", "Jeans_Blue"};

		conf.QuestSlots = 3;
		conf.Npcs.Insert(npcData);

		JsonFileLoader<KTQuestConfig>.JsonSaveFile(KTQConst.CONFIG, conf);
	}

	void ConvertRawQuestToMap(array<ref KTQuestData> raw)
	{
		foreach (KTQuestData qst : raw)
		{
			quests.Insert(qst.ID, qst);
		}
	}

	void SpawnNpc()
	{
		if (!config)
			return;

		npcQuestsData = new map<int, ref array<ref array<int>>>();
		array<ref KTQuestNPCData> npcs = config.Npcs;
		Object npc;

		foreach (KTQuestNPCData data : npcs)
		{
			npc = GetGame().CreateObject(data.Type, ValidatePos(data.Position), false);

			if (!npc)
				continue;

			npcQuestsData.Insert(data.ID, CreateQuestsCondition(data));
			
			npc.SetPosition(ValidatePos(data.Position));
			npc.SetOrientation(data.Orientation.ToVector());
			
			SpawnClothes(EntityAI.Cast(npc), data.Clothes);
			PlayerBase.Cast(npc).InitQuestNpc(data.ID);
		}
	}

	array<ref array<int>> CreateQuestsCondition(KTQuestNPCData data)
	{
		array<ref array<int>> outer = new array<ref array<int>>();
		outer.Insert(data.StartQuests);
		outer.Insert(data.FinishQuests);
		return outer;
	}

	map<int, ref array<ref array<int>>> GetNpcQuests()
	{
		return npcQuestsData;
	}

	void SpawnClothes(EntityAI ent, array<string> clothes)
	{
		foreach (string cloth : clothes)
		{
			ent.GetInventory().CreateAttachment(cloth);
		}
	}

	vector ValidatePos(string pos)
	{
		vector temp = pos.ToVector();
		float h;
		h = GetGame().SurfaceY(temp[0], temp[2]);

		if (h > temp[1])
			temp[1] = h;
		return temp;
	}

	void DebugShowMap()
	{
		Print("\n\n\t DebugShowMap");

		foreach (int key, KTQuestData value : quests)
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

	map<int, ref KTQuestData> GetQuests()
		return quests;

	array<ref KTQuestData> GetTemplates()
		return rawQuests;

	KTQuestConfig GetConfig()
		return config;
}

KTPluginQuestStore GetKTPluginQuestStore()
{
	return KTPluginQuestStore.Cast(GetPlugin(KTPluginQuestStore));
}