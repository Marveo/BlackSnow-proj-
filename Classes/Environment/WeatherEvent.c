class NamWeatherEvent: NamEventBase
{
	override void InitPhaseServer()
	{
		// I mean... it works :)
		for (int i = NamWeatherChangeType.FOG; i <= NamWeatherChangeType.SNOWFALL; i++) {		
			if (GetWeatherTarget(i) != -1) {
				RequestWeatherChange(i, GetWeatherTarget(i), m_InitPhaseLength, m_MidPhaseLength);
			}
		}
	}

	override void EndPhaseServer()
	{
		// Reset all weather
		// This is resposible for the weather not looking totally choppy when events end
		
		// i.e. Snowfall brings fog to 0.5, HeavyFog then brings it to 1.0, after HeavyFog finishes,
		// the fog level will be brought back down to 0.5, until Snowfall returns it back to 0
		for (int i = NamWeatherChangeType.FOG; i <= NamWeatherChangeType.SNOWFALL; i++) {
			float highest_value = GetHighestRemainingWeatherValue(i);
			if (highest_value == 0 && i == NamWeatherChangeType.OVERCAST) {
				highest_value = Math.RandomFloat(0, 0.15);
			}
			
			RequestWeatherChange(i, highest_value, m_EndPhaseLength, m_EndPhaseLength);
		}
	}
	
	override void OnEventEndServer()
	{
		for (int i = NamWeatherChangeType.FOG; i <= NamWeatherChangeType.SNOWFALL; i++) {
			WeatherPhenomenon weather_phenom = GetWeatherPhenomenon(i);
			float highest_value = GetHighestRemainingWeatherValue(i);
			if (highest_value == 0 && i == NamWeatherChangeType.OVERCAST) {
				highest_value = 0.15; // Keeps things spicy
			}
			
			weather_phenom.SetLimits(0, highest_value);
		}
	}
	
	// Override all of these magical values and everything else is handled above
	float GetFogTarget()
	{
		return -1;
	}
	
	float GetRainTarget()
	{
		return -1;
	}
	
	float GetSnowfallTarget()
	{
		return -1;
	}

	float GetOvercastTarget()
	{
		return -1;
	}
	
	float GetWeatherTarget(NamWeatherChangeType change_type)
	{
		switch (change_type) {
			case NamWeatherChangeType.FOG: return GetFogTarget();
			case NamWeatherChangeType.RAIN: return GetRainTarget();
			case NamWeatherChangeType.OVERCAST: return GetOvercastTarget();
			case NamWeatherChangeType.SNOWFALL: return GetSnowfallTarget();
		}
		
		return -1;
	}
	
	WeatherPhenomenon GetWeatherPhenomenon(NamWeatherChangeType change_type)
	{
		switch (change_type) {
			case NamWeatherChangeType.FOG: return GetGame().GetWeather().GetFog();
			case NamWeatherChangeType.RAIN: return GetGame().GetWeather().GetRain();
			case NamWeatherChangeType.OVERCAST: return GetGame().GetWeather().GetOvercast();
			case NamWeatherChangeType.SNOWFALL: return GetGame().GetWeather().GetSnowfall();
		}
		
		return null;
	}
	
	// Safe way of requesting a change in weather events
	protected void RequestWeatherChange(NamWeatherChangeType weather_type, float change, float time, float duration)
	{
		EventDebug("Requested Weather Change %1, value %2", typename.EnumToString(NamWeatherChangeType, weather_type), change.ToString());		
		array<NamEventBase> active_events = NamEventManager.GetInstance().GetActiveEvents();
		foreach (NamEventBase fog_event: active_events) {
			NamWeatherEvent weather_event;
			if (Class.CastTo(weather_event, fog_event)) {
				if (weather_event != this && weather_event.GetWeatherTarget(weather_type) != -1) {
					if (change < weather_event.GetWeatherTarget(weather_type) && weather_event.GetActivePhaseID() <= 1) {
						EventDebug("Weather Change Request Denied");
						return;
					}
				}
			}
		}
				
		EventDebug("Weather Change Approved");
		WeatherPhenomenon weather_phenomenon = GetWeatherPhenomenon(weather_type);
		if (!weather_phenomenon) {
			Error("Invalid Weather Phenomenon");
			return;
		}
				
		// adjust limits
		float low, high;
		weather_phenomenon.GetLimits(low, high);
		
		// we only want this value to go higher if the requested change is higher than something already active
		// we let the lower value changes occur in OnEventEndServer
		if (change > high) {
			EventDebug("Changing limits to [0, %2]", low.ToString(), change.ToString());			
			weather_phenomenon.SetLimits(0, change);
		}
		
		// Set weather to value
		weather_phenomenon.Set(change, time, duration);
	}
	
	float GetHighestRemainingWeatherValue(NamWeatherChangeType change_type)
	{
		// This does a few important things	//
		// 1. Grabs ceiling weather value
		// 2. If this is the last running event, value will be zero (back to sunny day)
		array<NamEventBase> active_events = NamEventManager.GetInstance().GetActiveEvents();
		float value = 0;
		foreach (NamEventBase active_event: active_events) {
			if (!active_event) {
				continue;
			}
			
			if (active_event == this || active_event.GetActivePhaseID() > 1) {
				continue;
			}
			
			NamWeatherEvent weather_event;
			if (Class.CastTo(weather_event, active_event)) {
				if (value <= weather_event.GetWeatherTarget(change_type)) {
					value = weather_event.GetWeatherTarget(change_type);
				}
			}	
		}
		
		return value;
	}
}