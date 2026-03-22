class EVRStormDeadly: EVRStorm
{
	void EVRStormDeadly()
	{
		m_Colour_SkyOrang = 1.0;		
	}
	
	override void EventInit()
	{
		super.EventInit();

		m_InitPhaseLength = 120.0;
	}
	
	override void OnEVRFinished( notnull SurvivorBase player )
	{
		float current_shock = player.GetHealth( "", "Shock" );
			
		// Knock players out on event
		float shock_value = EVRConstants.SHOCK_WHEN_UNSAFE;
		
		if ( APSICheck( player ) || LEHSCheck( player ) ) {
			shock_value += 15.0;
		} 
		
		if ( player.m_Environment.IsSafeFromEVR() )
		{
			shock_value += 7.0;
		}
		
		if ( player.IsPlayerInStance( DayZPlayerConstants.STANCEMASK_PRONE ) )
		{
			shock_value += 2.0;
		}
		
		// If you are in the "safe bunkers"
		if ( UndergroundCheck( player ) )
		{
			shock_value += 20.0;
		}

		//! Shock value handled
		if ( shock_value < current_shock )
		{
			player.SetHealth( "", "Shock", shock_value );
		}
		
		//! Handle Stamina Value
		player.GetStaminaHandler().SetStamina( 0 );
					
		//! Handle health value
		if ( APSICheck( player ) || LEHSCheck( player ) )
		{
			dzn_apsi apsi = GetAPSI( player );
			if ( apsi && apsi.IsSwitchedOn() )
				apsi.AddHealth( "", "Health", -EVRConstants.APSI_DAMAGE_HEAVY );
			return;
		}
		
		if ( player.m_Environment.IsSafeFromEVR() )
		{
			return;
		}
		
		// if you are not safe during the event, yeet some HP
		player.AddHealth( "", "Health", -75.0 );
		
		// Welp son. you fucked up
		if ( DistanceFromCenter( player ) < EVRConstants.ANOMALY_KILL_RADIUS )
		{
			player.SetHealth( "", "Health", 0 );
		}
	}

	override TTypenameArray GetDisallowedEvents()
	{
		return { Aurora, LantiaLink, EVRStorm };
	}
}