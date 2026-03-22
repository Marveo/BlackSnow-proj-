class EnzymeMdfr: ModifierBase
{
	const int LIFETIME = 2700;
	protected float m_lifetimeModifier = 1.0;

	override void Init()
	{
		m_TrackActivatedTime = true;
		m_IsPersistent = true;
		m_ID 					= eModifiersNamalsk.NAM_MDF_ENZYME;
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
		player.SetAITargetableFlag( false );
		m_lifetimeModifier = Math.Max( GetPlayer().GetHealth("GlobalHealth", "Health") / 100.0, 0.25 );
	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.DecreaseHealingsCount();
		player.SetAITargetableFlag( true );
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
};