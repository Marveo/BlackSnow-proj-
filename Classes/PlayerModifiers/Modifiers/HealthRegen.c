modded class HealthRegenMdfr
{
	float m_LastFrostbiteCap = 100.0;

	override void OnTick(PlayerBase player, float deltaT)
	{
		if ( player.IsAlive() )
		{
			float frostbite_cap = 100.0 - player.GetStatFrostbiteEffect().Get();
			float blood =  player.GetHealth("GlobalHealth", "Blood");
	
			float blood_scale_normalized = Math.InverseLerp(PlayerConstants.BLOOD_THRESHOLD_FATAL, player.GetMaxHealth("", "Blood"), blood);
			blood_scale_normalized = Math.Clamp(blood_scale_normalized,0,1);
			float regen_speed = Math.Lerp(PlayerConstants.HEALTH_REGEN_MIN, PlayerConstants.HEALTH_REGEN_MAX, blood_scale_normalized);
			regen_speed = regen_speed * deltaT;
			if ( player.GetHealth( "GlobalHealth", "Health") < frostbite_cap )
				player.AddHealth( "GlobalHealth", "Health",  regen_speed );

			if ( ( m_LastFrostbiteCap != frostbite_cap ) && ( player.GetHealth( "GlobalHealth", "Health") > frostbite_cap ) )
				player.SetHealth( "GlobalHealth", "Health", frostbite_cap);
			m_LastFrostbiteCap = frostbite_cap;
			
			player.AddHealth("RightArm","Health",regen_speed * PlayerConstants.DAMAGE_ZONE_BLOOD_REGEN_MODIFIER );
			player.AddHealth("RightHand","Health",regen_speed * PlayerConstants.DAMAGE_ZONE_BLOOD_REGEN_MODIFIER);
			player.AddHealth("LeftArm","Health",regen_speed * PlayerConstants.DAMAGE_ZONE_BLOOD_REGEN_MODIFIER);
			player.AddHealth("LeftHand","Health",regen_speed * PlayerConstants.DAMAGE_ZONE_BLOOD_REGEN_MODIFIER);
			
			//Leg regen when legs are NOT BROKEN
			if ( player.m_BrokenLegState == eBrokenLegs.NO_BROKEN_LEGS )
			{
				player.AddHealth("RightLeg","Health", PlayerConstants.LEG_HEALTH_REGEN);
				player.AddHealth("RightFoot","Health", PlayerConstants.LEG_HEALTH_REGEN);
				player.AddHealth("LeftLeg","Health", PlayerConstants.LEG_HEALTH_REGEN);
				player.AddHealth("LeftFoot","Health", PlayerConstants.LEG_HEALTH_REGEN);
			}
			else
			{
				//Leg regen when legs are BROKEN or SPLINTED
				player.AddHealth("RightLeg","Health", PlayerConstants.LEG_HEALTH_REGEN_BROKEN);
				player.AddHealth("RightFoot","Health",PlayerConstants.LEG_HEALTH_REGEN_BROKEN);
				player.AddHealth("LeftLeg","Health", PlayerConstants.LEG_HEALTH_REGEN_BROKEN);
				player.AddHealth("LeftFoot","Health", PlayerConstants.LEG_HEALTH_REGEN_BROKEN);
			}
			
			player.AddHealth("Torso","Health",regen_speed * PlayerConstants.DAMAGE_ZONE_BLOOD_REGEN_MODIFIER);
			player.AddHealth("Head","Health",regen_speed * PlayerConstants.DAMAGE_ZONE_BLOOD_REGEN_MODIFIER);
		}
	}
};