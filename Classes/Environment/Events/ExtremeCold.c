class ExtremeCold extends NamEventBase
{
	protected float m_TempDrop;
	protected int m_PPE_ExtremeCold;

	override void EventInit()
	{
		super.EventInit();

		if (GetGame().IsServer())
		{
			m_InitPhaseLength = 100.0;
			m_MidPhaseLength = Math.RandomFloat(800, 1200);
			m_EndPhaseLength = m_InitPhaseLength;
			m_TempDrop = -1.0 * Math.RandomFloat(4.0, 7.0);
			m_NeededOvercast = 0.15;
		}

		if (GetGame().IsClient())
		{		
			m_PPE_ExtremeCold = PPERequesterBank.GetRequester( PPERequester_NamExtremeCold ).GetRequesterIDX();
		}
		
/*#ifdef NAMALSK_SURVIVAL_DEBUG
		m_InitPhaseLength *= 0.05;
		m_MidPhaseLength *= 0.05;
		m_EndPhaseLength *= 0.05;
#endif*/
	}
	
	override void InitPhaseServer()
	{
		EventInfo("init: " + m_InitPhaseLength + " (sec), mid: " + m_MidPhaseLength + " (sec), end: " + m_EndPhaseLength + " (sec), TempDrop: " + m_TempDrop + " (C)");

		// add effect to WorldData
		GetGame().GetMission().GetWorldData().SetTemperatureEvent(m_TempDrop);
	}

	override void EndPhaseClient(float phase_time)
	{
		if ( m_PPE_ExtremeCold != -1 )
			PPERequesterBank.GetRequester( m_PPE_ExtremeCold ).Stop( new Param1<bool>( true ) );
	}

	override void UpdateClient()
	{
		super.UpdateClient();
		
		// Add additional feedback for the player to see if ExtremeCold is active
		PlayerBase player;
		if (!Class.CastTo(player, GetGame().GetPlayer())) {
			return;
		}
		Particle eff = ParticleManager.GetInstance().PlayInWorld(ParticleList.BREATH_VAPOUR_HEAVY, "-0.03 0.15 0");
		if (!eff) {
			return;
		}
		player.AddChild(eff, player.GetBoneIndexByName("Head"));	
		
		// check if the ppe colorization is active 
		if ( m_PPE_ExtremeCold != -1 )
		{
			PPERequesterBase ppeRequester;
			Class.CastTo( ppeRequester, PPERequesterBank.GetRequester( m_PPE_ExtremeCold ) );
			if ( !ppeRequester.IsRequesterRunning() )
			{
				ppeRequester.Start();
			}
		}
	}
		
	override void OnEventEndServer()
	{	
		// remove effect from WorldData
		GetGame().GetMission().GetWorldData().SetTemperatureEvent(0.0);
	}

	override TTypenameArray GetDisallowedEvents()
	{
		return { LantiaLink };
	}

	override float GetClientTick()
	{
		return 1.5;
	}
}