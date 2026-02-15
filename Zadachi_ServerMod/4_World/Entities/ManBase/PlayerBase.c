modded class PlayerBase
{
	override void OnConnect()
	{
		super.OnConnect();

		questManager = new KTQuestManager(this);

		auto params = new Param3< ref map<int, ref KTQuestData>, KTQuestConfig, ref map<int, ref array<ref array<int>>>>(null, null, null);

		params.param1 = GetKTPluginQuestStore().GetQuests();
		params.param2 = GetKTPluginQuestStore().GetConfig();
		params.param3 = GetKTPluginQuestStore().GetNpcQuests();

		RPCSingleParam(KTQRPC.SEND_INIT_C, params, true, GetIdentity());
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (questManager)
			questManager.OnRPC(rpc_type, ctx);
	}

	override void OnScheduledTick(float deltaTime)
	{
		super.OnScheduledTick(deltaTime);

		if (questManager)
			questManager.OnUpdate(deltaTime);
	}

	void InitQuestNpc(int id)
	{
		ktquestNpc = id;
		SetAllowDamage(false);
		SetSynchDirty();
	}
}