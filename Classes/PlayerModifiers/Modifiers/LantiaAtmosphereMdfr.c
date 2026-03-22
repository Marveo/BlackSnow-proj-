class LantiaAtmosphereMdfr: ModifierBase
{
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiersNamalsk.NAM_MDF_LANTIAATMOSPHERE;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 1;
		DisableDeactivateCheck();
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		if ( player.IsAlive() )
		{
			player.RequestSoundEventEx( EPlayerSoundEventID.STAMINA_LOW_FILTER_LOWER, false, EPlayerSoundEventParam.HIGHEST_PRIORITY );
			player.AddHealth( "", "Shock", -12.0 );
			player.AddHealth( "GlobalHealth", "Health", -6.25 );
			player.SendLantiaHitEffect();
		}
	}
};