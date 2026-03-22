class AreaExposureNamMdfr: ModifierBase
{
	const int EVENT_1_INTERVAL_MIN = 3;
	const int EVENT_1_INTERVAL_MAX = 5;
	
	const float AGENTS_PER_SEC = 5;
	protected float m_NextEvent1;
	protected float m_Time1;
	
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiersNamalsk.NAM_MDF_AREAEXPOSURE;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE_LONG;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE_SHORT;
		m_SyncID				= eModifierSyncIDs.MODIFIER_SYNC_ZONE_EXPOSURE;
		DisableActivateCheck();
		DisableDeactivateCheck();
		
	}

	override bool ActivateCondition(PlayerBase player)
	{
		return false;
	}
	
	override void OnActivate(PlayerBase player)
	{
	
		JsonDataContaminatedAreas data = EffectAreaLoader.GetData();
		if (data)
		{
			MiscGameplayFunctions.TeleportCheck(player, data.SafePositions);
		}
		
		//make the player cough immediately
		float transmitted = TransmitAgents(player, 1);
		if(transmitted) 
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
		
		m_NextEvent1 = Math.RandomFloatInclusive( EVENT_1_INTERVAL_MIN, EVENT_1_INTERVAL_MAX );
	}
	
	override void OnDeactivate(PlayerBase player)
	{
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		#ifdef DEVELOPER
		if(!player.GetCanBeDestroyed())
			return;
		#endif
		
		float transmitted = TransmitAgents(player, AGENTS_PER_SEC * deltaT);
	
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
	}

	float TransmitAgents(PlayerBase player, float count)
	{
		PluginTransmissionAgents plugin = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		return plugin.TransmitAgentsEx(null, player, AGT_AIRBOURNE_CHEMICAL, count, eAgents.CHEMICAL_POISON);
	}
};