// ─────────────────────────────────
//  TemperatureZones MissionServer
// ─────────────────────────────────
modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();

        Print("[TemperatureZones] TemperatureZones mod loaded successfully.");
        GetTZ_Settings();

        GetRPCManager().AddRPC( "TZ_Settings", "RPCSyncTZSettingsServer", this, SingeplayerExecutionType.Both );
    }

    // ───────────────────────────────────────────────────────────────
    void RPCSyncTZSettingsServer( CallType type, ref ParamsReadContext ctx,
                                  ref PlayerIdentity sender, ref Object target )
    {
        if ( sender )
        {
            GetRPCManager().SendRPC( "TZ_Settings", "RPCSyncTZSettingsClient",
                                     new Param1< TZ_Settings >( GetTZ_Settings() ),
                                     true, sender );
        }
    }
}
