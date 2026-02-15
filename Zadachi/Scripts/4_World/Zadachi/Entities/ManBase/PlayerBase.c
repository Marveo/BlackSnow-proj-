modded class PlayerBase
{
	private ref KTQuestManager questManager;

	private int ktquestNpc;

	void PlayerBase()
	{
		RegisterNetSyncVariableInt("ktquestNpc");
	}

	int GetQuestNpcID()
	{
		return ktquestNpc;
	}

	bool IsQuestNpc()
	{
		return ktquestNpc > 0;
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (GetGame().IsServer())
			return;

		if (!questManager)
			questManager = new KTQuestManager(this);
		questManager.OnRPC(rpc_type, ctx);

	}

	KTQuestManager GetQuestManager()
	{
		return questManager;
	}

	override void SetActionsRemoteTarget( out TInputActionMap InputActionMap)
	{
		AddAction(ActionOpenQuestMenu, InputActionMap);
		super.SetActionsRemoteTarget(InputActionMap);
	}
}