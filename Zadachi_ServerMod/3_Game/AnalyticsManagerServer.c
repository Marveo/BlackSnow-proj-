ref ScriptInvoker KTQuestOnEntityKilled = new ScriptInvoker;
ref ScriptInvoker KTQuestOnEntityCrafted = new ScriptInvoker;
ref ScriptInvoker KTQuestOnHarvest = new ScriptInvoker;
ref ScriptInvoker KTQuestOnFishing = new ScriptInvoker;
ref ScriptInvoker KTQuestOnActionCompleted = new ScriptInvoker;

modded class AnalyticsManagerServer
{
	override void OnEntityKilled( Object killer, EntityAI target )
	{
		super.OnEntityKilled(killer, target);

		KTQuestOnEntityKilled.Invoke(killer, target);
	}
}