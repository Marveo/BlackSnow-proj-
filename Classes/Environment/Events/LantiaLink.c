class LantiaLink: NamWeatherEvent
{
	// If you are looking for Lantia logic, you will find it in LantiaEC_Client and LantiaEC_Server.
	// This event is here just a dummy event to make sure no other events run together with the real Lantia link.

	protected Object m_LordOfLantia;
	
	void LantiaLink()
	{
		// CLIENT 
		if ( GetGame().IsClient() || !GetGame().IsMultiplayer() )
		{

		}
		
		// SERVER
		if ( GetGame().IsServer() )
		{
			// yes, really, seems like we need to summon the lord of lantia to wake up this far away world on server so people dont fall through on first jump
			if ( GetCEApi() )
			{
				m_LordOfLantia = GetGame().CreateObjectEx( "ZmbM_priestPopSkinny", Vector( 11096.7, 853.3, 1610.7 ), ECE_PLACE_ON_SURFACE );
			}
		}
	}
	
	void ~LantiaLink()
	{
		EventInfo( "~LantiaLink" );
		
		// CLIENT
		if ( GetGame().IsClient() || !GetGame().IsMultiplayer() )
		{

		}
		
		// SERVER
		if ( GetGame().IsServer() )
		{
			if ( m_LordOfLantia )
			{
				GetGame().ObjectDelete( m_LordOfLantia );
			}
		}
	}
	
	override void EventInit()
	{
		super.EventInit();
		
		m_InitPhaseLength = 120;
		m_MidPhaseLength = 600;
		m_EndPhaseLength = 10;
	}

	// main event logic here
	override void InitPhaseServer()
	{
		super.InitPhaseServer();
		
		// calm down weather 
		m_wObject.GetOvercast().Set( 0.0, m_InitPhaseLength, m_MidPhaseLength + m_EndPhaseLength );
		m_wObject.GetFog().Set( 0.0, m_InitPhaseLength, m_MidPhaseLength + m_EndPhaseLength );
		m_wObject.GetSnowfall().Set( 0.0, m_InitPhaseLength, m_MidPhaseLength + m_EndPhaseLength );
		//m_wObject.SetWindFunctionParams( 0.1, 1.0, 42 );
	}

	override float GetFogTarget()
	{
		return 0.0;
	}
	override float GetOvercastTarget()
	{
		return 0.0;
	}
	override float GetSnowfallTarget()
	{
		return 0.0;
	}
	override int GetEventCooldown()
	{
		return 0;
	}
}

