class KTQuestConfig
{
	int QuestSlots;
	bool HideUnavailableQuests;
	bool CanCancelQuest;
	int ImportanceNPC;

	ref array<ref KTQuestNPCData> Npcs;

	void KTQuestConfig()
	{
		Npcs = {};
	}
}

class KTQuestNPCData
{
	string Type, Position, Orientation;
	int ID;
	ref array<string> Clothes;
	ref array<int> StartQuests;
	ref array<int> FinishQuests;

	void KTQuestNPCData()
	{
		Clothes = {};
		StartQuests = {};
		FinishQuests = {};
	}
}