class EVRConstants
{
	// Should the orb teleport you?
	static const bool ALLOW_TELEPORTING = true;
	
	// Length of beginning phase for the storm
	static const float STARTUP_LENGTH = 1080.0;
	
	// Damage Value, increase to make the storm more dangerous
	static const float DAMAGE_WHEN_UNSAFE = 50.0;
	
	// Shock Value, decrease to make the storm more dangerous (this value is SET to 10, whereas health is DECREASED by 50)
	static const float SHOCK_WHEN_UNSAFE  = 10.0; // PlayerConstants.UNCONSCIOUS_THRESHOLD - 15.0
	
	static const float ANOMALY_KILL_RADIUS = 200;
	
	static const int WATER_DRAIN_FROM_TELEPORTATION_SICKNESS = 450;
	static const int ENERGY_DRAIN_FROM_TELEPORTATION_SICKNESS = 310;

	static const float APSI_DAMAGE = 25;
	static const float APSI_DAMAGE_HEAVY = 50;
}