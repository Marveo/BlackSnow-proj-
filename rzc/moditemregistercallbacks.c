modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
    {
        super.RegisterOneHanded( pType, pBehavior );
        pType.AddItemInHandsProfileIK("Radiation_Geiger_Pripyat", "dz/anims/workspaces/player/player_main/props/player_main_1h_GPSReciever.asi", pBehavior,      "dz/anims/anm/player/ik/gear/GPSReciever.anm");
		pType.AddItemInHandsProfileIK("Radiation_Dosimeter_Pripyat", "dz/anims/workspaces/player/player_main/props/player_main_1h_GPSReciever.asi", pBehavior,      "dz/anims/anm/player/ik/gear/GPSReciever.anm");
    } 
}