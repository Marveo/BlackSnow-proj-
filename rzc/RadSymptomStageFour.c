class RadSymptomStageFour extends SymptomBase
{
	Material m_MatGauss;

	float m_BlurDuration;
	float m_BlurStrength;
	float m_EffectTime;
	float m_EffectStartTime;
	float m_Time;
	
	const float BLUR_STRENGTH_MIN = 0.35;
	const float BLUR_STRENGTH_MAX = 0.95;
	
	const int BLUR_DURATION_TIME_MIN = 10;
	const int BLUR_DURATION_TIME_MAX = 20;
	
	const int MIN_TIME_BETWEEN_EFFECTS = 2.0;
	const int MAX_TIME_BETWEEN_EFFECTS = 10.0;

	override void OnInit()
	{
		m_SymptomType = SymptomTypes.SECONDARY;
		m_Priority = 0;
		m_ID = RadDebuffSymptomIDs.SYMPTOM_RAD4;
		m_DestroyOnAnimFinish = true;
		m_IsPersistent = false;
		m_SyncToClient = true;
	}
	
	override void OnUpdateServer(PlayerBase player, float deltatime)
	{
	}

	
	override void OnUpdateClient(PlayerBase player, float deltatime)
	{
		m_Time += deltatime;
		if( m_EffectStartTime <= 0 )
		{
			m_EffectStartTime = Math.RandomFloatInclusive(MIN_TIME_BETWEEN_EFFECTS, MAX_TIME_BETWEEN_EFFECTS);
			m_BlurDuration = Math.RandomFloatInclusive(BLUR_DURATION_TIME_MIN, BLUR_DURATION_TIME_MAX);
			m_BlurStrength = Math.RandomFloat(BLUR_STRENGTH_MIN, BLUR_STRENGTH_MAX);
		}
		
		if( m_EffectStartTime > 0 && m_Time > m_EffectStartTime )
		{
			m_EffectTime += deltatime / m_BlurDuration;
			float cos_value = Math.Sin(m_EffectTime  * Math.PI);
			float val = cos_value * m_BlurStrength;
			PPEffects.SetBlurDrunk(val);
			
			if( m_EffectTime >= 1 )
			{
				m_EffectTime = 0;
				m_EffectStartTime = -1;
				m_Time = 0;
			}
		}
	}
	
	override void OnGetActivatedServer(PlayerBase player)
	{
		
	}

	override void OnGetActivatedClient(PlayerBase player)
	{
		
	}

	override void OnGetDeactivatedServer(PlayerBase player)
	{
		
	}
	
	override void OnGetDeactivatedClient(PlayerBase player)
	{
		PPEffects.SetBlurDrunk(0);
	}
}
