class KTPluginQuestEventHandler : PluginBase
{
	void KTPluginQuestEventHandler()
	{
		KTQuestOnEntityKilled.Insert(OnEntityKilled);
		KTQuestOnEntityCrafted.Insert(OnItemCrafted);
		KTQuestOnHarvest.Insert(OnHarvest);
		KTQuestOnFishing.Insert(OnFishing);
		KTQuestOnActionCompleted.Insert(OnActionCompleted);
	}

	void ~KTPluginQuestEventHandler()
	{
		KTQuestOnEntityKilled.Remove(OnEntityKilled);
		KTQuestOnEntityCrafted.Remove(OnItemCrafted);
		KTQuestOnHarvest.Remove(OnHarvest);
		KTQuestOnFishing.Remove(OnFishing);
		KTQuestOnActionCompleted.Remove(OnActionCompleted);
	}

	void OnEntityKilled(Object killer, EntityAI target)
	{
		PlayerBase manKiller;
		EntityAI killer_entity = EntityAI.Cast( killer );
	
		if ( killer_entity && CastTo(manKiller, killer_entity.GetHierarchyRootPlayer()) && killer_entity != target)
		{
			if (manKiller.GetQuestManager())
			{
				manKiller.GetQuestManager().UpdateQuests(KTQuestType.Kill, target.GetType(), 1, killer.GetType());
			}
		}
	}

	void OnItemCrafted(PlayerBase player, array<ItemBase> results)
	{
		KTQuestManager qmanager = player.GetQuestManager();

		if (!qmanager)
			return;

		foreach (ItemBase item : results)
		{
			qmanager.UpdateQuests(KTQuestType.Craft, item.GetType(), 1, string.Empty);
		}
	}

	void OnHarvest(string plantType, PlayerBase player)
	{
		KTQuestManager qmanager = player.GetQuestManager();

		if (!qmanager)
			return;
		
		qmanager.UpdateQuests(KTQuestType.Harvest, plantType, 1, string.Empty);
	}

	void OnFishing(string fishType, PlayerBase player)
	{
		KTQuestManager qmanager = player.GetQuestManager();

		if (!qmanager)
			return;
		
		qmanager.UpdateQuests(KTQuestType.Fishing, fishType, 1, string.Empty);
	}

	void OnActionCompleted(string actionType, PlayerBase player, EntityAI target)
	{
		KTQuestManager qmanager = player.GetQuestManager();

		if (!qmanager)
			return;

		string targetType;
		if (target)
			targetType = target.GetType();

		qmanager.UpdateQuests(KTQuestType.Action, actionType, 1, targetType);
	}
}