modded class InjuryAnimationHandler
{
	override eInjuryHandlerLevels GetInjuryLevel(float health)
	{
		eInjuryHandlerLevels result = super.GetInjuryLevel(health);
		
		if (m_Player && m_Player.GetTerjeStats())
		{
			eInjuryHandlerLevels radiationSickness = CalculateRadiationState();
			if (TerjeInjuryLevelToValue(result) < TerjeInjuryLevelToValue(radiationSickness))
			{
				result = radiationSickness;
			}
		}
		
		return result;
	}
	
	private eInjuryHandlerLevels CalculateRadiationState()
	{
		int radLevel = m_Player.GetTerjeStats().GetRadiationLevel();
		if (radLevel == 1)
		{
			return eInjuryHandlerLevels.WORN;
		}
		
		if (radLevel == 2)
		{
			return eInjuryHandlerLevels.BADLY_DAMAGED;
		}
		
		if (radLevel == 3)
		{
			return eInjuryHandlerLevels.RUINED;
		}
		
		return eInjuryHandlerLevels.PRISTINE;
	}
}