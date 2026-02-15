class KTQuestFileManager
{
	private string personalPath;

	static string ActiveQuestPath(string part)
	{
		return KTQConst.ACTIVEQ_DIR + part + ".json";
	}

	static string DBPath(string part)
	{
		return KTQConst.DB_DIR + part + ".json";
	}

	static array<ref KTQuestProgress> LoadActiveQuests(string part)
	{
		string path = ActiveQuestPath(part);
		array<ref KTQuestProgress> rawData = {};

		JsonFileLoader<array<ref KTQuestProgress>>.JsonLoadFile(path, rawData);

		return rawData;
	}

	static void SaveActiveQuests(array<ref KTQuestProgress> quests, string part)
	{
		string path = ActiveQuestPath(part);
		JsonFileLoader<array<ref KTQuestProgress>>.JsonSaveFile(path, quests);
	}

	static array<ref KTQuestCompleted> LoadDB(string part)
	{
		string path = DBPath(part);
		array<ref KTQuestCompleted> rawData = {};

		JsonFileLoader<array<ref KTQuestCompleted>>.JsonLoadFile(path, rawData);

		return rawData;
	}

	static void SaveDB(array<ref KTQuestCompleted> quests, string part)
	{
		string path = DBPath(part);
		JsonFileLoader<array<ref KTQuestCompleted>>.JsonSaveFile(path, quests);
	}
}