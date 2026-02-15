class KTPluginQuestMenu : PluginBase
{
	private ref KTQuestMenu menu;

	void TryOpenUI(bool selfBook = false, int id = 0)
	{
		if (GetGame().GetUIManager().GetMenu() || menu)
        	return;

		menu = new KTQuestMenu(selfBook, id);
		GetGame().GetUIManager().ShowScriptedMenu( menu, NULL );
	}
	
	void RecreateMenu()
	{
		if (menu)
			menu.Recreate();
	}

	bool CanTakeQuest(int id)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		KTQuestManager qManager = player.GetQuestManager();

		if (!qManager)
			return false;
		
		return qManager.CanTakeQuest(id);
	}

	bool CanFinishQuest(int id)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		KTQuestManager qManager = player.GetQuestManager();

		bool canFinish = false;

		if (!qManager)
			return false;
		
		switch(qManager.ImportanceNPC())
		{
			case 2:
				canFinish = !menu.IsSelfBook();
			break;
			case 1:
				canFinish = qManager.QuestBelongNPC(id, menu.CurrentNpc());
			break;
			case 0:
				canFinish = true;
			break;
		}
		return qManager.CanFinishQuest(id) && canFinish;
	}

	array<float> GetProgress(int id)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		KTQuestManager qManager = player.GetQuestManager();

		if (!qManager)
			return {};

		return qManager.GetProgress(id);
	}

	int GetQuestState(int id)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		KTQuestManager qManager = player.GetQuestManager();

		if (!qManager)
			return -1;

		array<ref array<int>> temp = qManager.GetSortedQuests();

		for (int i = 0; i < temp.Count(); i++)
		{
			if (temp[i].Find(id) + 1)
				return i;
		}

		return -1;
	}

	int GetColdownTimeLeft(int id)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		KTQuestManager qManager = player.GetQuestManager();

		if (!qManager)
			return 0;

		return qManager.GetDB().CooldownLeftTime(id);
	}

	int GetLimitLeftTime(int id)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		KTQuestManager qManager = player.GetQuestManager();

		if (!qManager)
			return 0;
		
		if (!qManager.GetActiveQuests().Get(id))
			return -1;

		return qManager.GetActiveQuests().Get(id).LeftTime();
	}

	string GetRequireds(int id)
	{
		string result;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		KTQuestManager qManager = player.GetQuestManager();

		string div = "| ";

		if (!qManager)
			return "Error";
		
		KTQuestData template = qManager.GetQuestTemplates().Get(id);
		if (!template)
			return "Error";
		
		array<int> requiredsRaw = template.RequiredQuests;
		if (!requiredsRaw.Count())
			return string.Empty;
		
		for (int i = 0; i < requiredsRaw.Count(); i++)
		{
			template = qManager.GetQuestTemplates().Get(requiredsRaw[i]);
			if (!template)
				result += "Error";
			else
				result += template.Name;

			result += div;
		}

		return result;
	}

	KTQuestBase GetActiveQuest(int questID)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		KTQuestManager qManager = player.GetQuestManager();

		if (!qManager)
			return null;
		
		if (!qManager.GetActiveQuests().Get(questID))
			return null;

		return qManager.GetActiveQuests().Get(questID);
	}

	bool CanCancelQuest(int questID)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		KTQuestManager qManager = player.GetQuestManager();

		if (!qManager)
			return false;
		
		return (qManager.CanCancelQuest() && qManager.InProgress(questID));
	}
}

KTPluginQuestMenu GetQuestMenuHandler()
{
	return KTPluginQuestMenu.Cast(GetPlugin(KTPluginQuestMenu));
}