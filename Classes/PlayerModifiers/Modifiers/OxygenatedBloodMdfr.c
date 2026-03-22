class OxygenatedBloodMdfr: ModifierBase
{
	const int LIFETIME = 30;
	protected float m_lifetimeModifier = 1.0;

	override void Init()
	{
		m_TrackActivatedTime = true;
		m_IsPersistent = true;
		m_ID 					= eModifiersNamalsk.NAM_MDF_OXYGENATEDBLOOD;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 1;
		DisableActivateCheck();
	}

	override bool ActivateCondition(PlayerBase player)
	{
		return false;
	}
	
	override void OnReconnect(PlayerBase player)
	{
		OnActivate( player );
	}
	
	override string GetDebugText()
	{
		return ( ( LIFETIME * m_lifetimeModifier ) - GetAttachedTime() ).ToString();
	}
	
	override void OnActivate(PlayerBase player)
	{
		player.IncreaseHealingsCount();

		player.RequestSoundEventEx( EPlayerSoundEventID.HOLD_BREATH, false, EPlayerSoundEventParam.HIGHEST_PRIORITY );
	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.DecreaseHealingsCount();

		player.RequestSoundEventEx( EPlayerSoundEventID.RELEASE_BREATH, false, EPlayerSoundEventParam.HIGHEST_PRIORITY );
	}
	
	override bool DeactivateCondition(PlayerBase player)
	{
		float attached_time = GetAttachedTime();
		if( attached_time >= ( LIFETIME * m_lifetimeModifier ) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		if ( player.IsAlive() )
		{
			player.AddHealth( "GlobalHealth", "Health" , -0.4 );
			player.AddHealth( "", "Shock" , -3.5 );

			if ( Math.RandomFloat01() < 0.2 )
			{
				player.RequestSoundEventEx( EPlayerSoundEventID.EXHAUSTED_BREATH, false, EPlayerSoundEventParam.HIGHEST_PRIORITY );
			}
		}
	}
};