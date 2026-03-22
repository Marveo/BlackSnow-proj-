modded class AreaExposureMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		#ifdef DEVELOPER
		if(!player.GetCanBeDestroyed())
			return;
		#endif
		
		float transmitted = TransmitAgents(player, AGENTS_PER_SEC * deltaT);
		
		m_Time2 += deltaT;
		
		if (transmitted)
		{
			m_Time1 += deltaT;
			if (m_Time1 >= m_NextEvent1 )
			{
				player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
				
				if(Math.RandomFloat01() < 0.25)//creates a cough cooldown once in a while
				{
					m_NextEvent1 = Math.RandomFloatInclusive( EVENT_1_INTERVAL_MIN * 2, EVENT_1_INTERVAL_MAX * 2 );
				}
				else
				{
					m_NextEvent1 = Math.RandomFloatInclusive( EVENT_1_INTERVAL_MIN, EVENT_1_INTERVAL_MAX );
				}
			
				m_Time1 = 0;
			}
		}
		
		if ( m_Time2 >= m_NextEvent2 )
		{
			if ( !player.IsWearingLEHS() )
				BleedingSourceCreateCheck(player);
			m_Time2 = 0;
			m_NextEvent2 = Math.RandomFloatInclusive( EVENT_2_INTERVAL_MIN, EVENT_2_INTERVAL_MAX );
		}
		
		ApplyAgentsToBleedingSources(player, deltaT);
		
	}
};