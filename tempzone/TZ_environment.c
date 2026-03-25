// ─────────────────────────────────
//  TemperatureZones Environment.c
// ─────────────────────────────────
modded class Environment
{
	override float GetEnvironmentTemperature()
	{
		float temperature = super.GetEnvironmentTemperature();

		float zoneTemp;
		if (m_Player && m_Player.TZ_GetZoneTemperature(zoneTemp))
		{
			temperature = zoneTemp;

			// Based on Scripts/4_world/classes/environment/environment.c : GetEnvironmentTemperature()
			if (Math.AbsFloat(m_UTSAverageTemperature) > 0.0 && m_UTSAverageTemperature > temperature)
				temperature = m_UTSAverageTemperature;
		}

		return temperature;
	}

	override void OnTemperatureSourcesLeft()
	{
		SetEnvironmentTemperature();
	}
}
