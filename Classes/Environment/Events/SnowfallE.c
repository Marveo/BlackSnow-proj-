
class SnowfallE: NamWeatherEvent
{
	override void EventInit()
	{
		super.EventInit();

		if (GetGame().IsServer())
		{
			m_InitPhaseLength = 600.0;
			m_MidPhaseLength = Math.RandomFloat(900, 1200);
			m_EndPhaseLength = m_InitPhaseLength;
			m_NeededOvercast = 0.2;
			m_windMaxTarget = 0.0;
		}
	}

	override void InitPhaseServer()
	{
		EventInfo("init: " + m_InitPhaseLength + " (sec), mid: " + m_MidPhaseLength + " (sec), end: " + m_EndPhaseLength + " (sec)");

		m_wObject.GetWindMagnitude().Set(m_windMaxTarget, m_InitPhaseLength, m_MidPhaseLength + m_EndPhaseLength);
		
		super.InitPhaseServer();
	}

	override void MidPhaseServer()
	{
		super.MidPhaseServer();
	}

	override void EndPhaseServer()
	{
		super.EndPhaseServer();
	}

	override void OnEventEndServer()
	{
		super.OnEventEndServer();
	}
	
	override TTypenameArray GetDisallowedEvents()
	{
		return { Blizzard, Aurora, LantiaLink };
	}
	
	override float GetFogTarget()
	{
		return 0.5;
	}

	override float GetSnowfallTarget()
	{
		return 0.25;
	}

	override float GetOvercastTarget()
	{
		return 0.75;
	}
}