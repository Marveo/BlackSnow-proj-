
class HeavyFog: NamWeatherEvent
{
	override void EventInit()
	{
		super.EventInit();

		if (GetGame().IsServer())
		{
			m_InitPhaseLength = 600.0;
			m_MidPhaseLength = Math.RandomFloat(500, 800);
			m_EndPhaseLength = m_InitPhaseLength;
		}
		
#ifdef NAMALSK_SURVIVAL_DEBUG
		m_InitPhaseLength *= 0.05;
		m_MidPhaseLength *= 0.05;
		m_EndPhaseLength *= 0.05;
#endif
	}
		
	override TTypenameArray GetDisallowedEvents()
	{
		return { Aurora, LantiaLink };
	}
	
	override float GetFogTarget()
	{
		return 0.9;
	}
	
	override float GetOvercastTarget()
	{
		return 0.6;
	}
}