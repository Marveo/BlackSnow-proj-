class Blizzard: NamWeatherEvent
{
	protected float m_TempBonus;

	override void EventInit()
	{
		super.EventInit();

		if (GetGame().IsServer())
		{			
			m_InitPhaseLength = 600.0;
			m_MidPhaseLength = Math.RandomFloat(750, 1000);
			m_EndPhaseLength = m_InitPhaseLength;
			m_TempBonus = -1.0 * Math.RandomFloat(3.0, 6.0);
			m_NeededOvercast = 0.25;
			m_windMinTarget = 0.0;
			m_windMaxTarget = 20.0;
		}
	}

	override void InitPhaseServer()
	{
		EventInfo("init: " + m_InitPhaseLength + " (sec), mid: " + m_MidPhaseLength + " (sec), end: " + m_EndPhaseLength + " (sec), TempDrop: " + m_TempBonus + " (C)");

		m_wObject.GetWindMagnitude().Set(m_windMinTarget, m_InitPhaseLength / 3, m_MidPhaseLength + (m_InitPhaseLength - (m_InitPhaseLength / 3)));
		
		super.InitPhaseServer();
	}

	override void MidPhaseServer()
	{
		m_wObject.GetWindMagnitude().Set(m_windMaxTarget, 60, m_MidPhaseLength);

		// add effect to WorldData
		GetGame().GetMission().GetWorldData().SetTemperatureEvent(m_TempBonus);
		
		super.MidPhaseServer();
	}

	override void EndPhaseServer()
	{
		m_wObject.GetWindMagnitude().Set(m_windMinTarget, 60, m_EndPhaseLength);

		// remove effects from WorldData
		GetGame().GetMission().GetWorldData().SetTemperatureEvent(0.0);

		super.EndPhaseServer();
	}

	override void OnEventEndServer()
	{	
		super.OnEventEndServer();
	}
		
	override TTypenameArray GetDisallowedEvents()
	{
		return { Snowfall, Aurora, LantiaLink };
	}
	
	override float GetFogTarget()
	{
		return 0.9;
	}

	override float GetSnowfallTarget()
	{
		return 1.0;
	}

	override float GetOvercastTarget()
	{
		return 0.85;
	}
};