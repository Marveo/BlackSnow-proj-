class KTQuestData
{
	int ID, Cooldown, TimeLimit;
	string Name, Description;
	ref array<int> RequiredQuests;
	ref array<ref KTQuestDefaultData> Targets;
	ref array<ref KTQuestDefaultData> Rewards;

	void KTQuestData()
	{
		RequiredQuests = {};
		Targets = {};
		Rewards = {};
	}
}