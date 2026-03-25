// ───────────────────────────────────
//  TemperatureZones MissionGamePlay
// ───────────────────────────────────
modded class MissionGameplay
{
	void MissionGameplay() {}

	override void OnMissionStart()
	{
		super.OnMissionStart();

		GetRPCManager().AddRPC("TZ_Settings", "RPCSyncTZSettingsClient", this, SingeplayerExecutionType.Both);
		GetRPCManager().SendRPC("TZ_Settings", "RPCSyncTZSettingsServer", new Param1<TZ_Settings>(NULL), true, NULL);
	}

	void RPCSyncTZSettingsClient(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target)
	{
		Param1<TZ_Settings> data;
		if (!ctx.Read(data)) return;

		SetTZ_Settings(data.param1);
	}
}
