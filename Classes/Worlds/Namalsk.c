class NamalskData extends WorldData
{
	override void Init()
	{
		super.Init();

		m_Sunrise_Jan = 9.16;
		m_Sunset_Jan = 14.85;
		m_Sunrise_Jul = 2.85;
		m_Sunset_Jul = 21.15;

		// (inspiration at https://en.wikipedia.org/wiki/Shemya#Climate)
		//              1    2    3    4    5    6    7    8   9    10   11   12 
		m_MaxTemps = {-12,  -8,  -3,   0,   2,   5,   7,  11,   8,   5,   2,  -8};
		m_MinTemps = {-32, -27, -21, -14,  -7,  -4,  -2,   2,  -3,  -7, -15, -25};

		m_WorldWindCoef 						= 0.85;
		m_CloudsTemperatureEffectModifier 		= 2.5;
		m_TemperatureInsideBuildingsModifier 	= 3.0;
		m_UniversalTemperatureSourceCapModifier = -3.0;
		m_Pollution = m_Pollution | EPollutionNamalsk.CHOLERA;
	}

	override void InitYieldBank()
	{
		super.InitYieldBank();
		
		m_YieldBank.ClearAllRegisteredItems();
		
		// fishing
		m_YieldBank.RegisterYieldItem(new YieldItemWalleyePollock(24));
		m_YieldBank.RegisterYieldItem(new YieldItemSteelheadTrout(12));
		m_YieldBank.RegisterYieldItem(new YieldItemCarp(10));
				
		// traps
		m_YieldBank.RegisterYieldItem(new YieldItemShrimp(1));
		m_YieldBank.RegisterYieldItem(new YieldItemBitterlings(1));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadRabbit(1));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadFox(1));
	}

	override void SetupLiquidTemperatures()
	{
		m_LiquidSettings = new WorldDataLiquidSettings();
		
		m_LiquidSettings.m_Temperatures[LIQUID_SALTWATER] 	= -0.5;
		m_LiquidSettings.m_Temperatures[LIQUID_WATER] 		= 2.0;
		m_LiquidSettings.m_Temperatures[LIQUID_STILLWATER] 	= 2.0;
		m_LiquidSettings.m_Temperatures[LIQUID_RIVERWATER] 	= 2.0;
		m_LiquidSettings.m_Temperatures[LIQUID_FRESHWATER] 	= 2.0;
		m_LiquidSettings.m_Temperatures[LIQUID_CLEANWATER] 	= 5.0;
		m_LiquidSettings.m_Temperatures[LIQUID_SNOW] 		= -5.0;
		m_LiquidSettings.m_Temperatures[LIQUID_HOTWATER] 	= 90.0;
	}

	override float GetAgentSpawnChance(eAgents agent)
	{
		if (agent == eAgents.CHOLERA)
			return 20;
		
		return 0;
	}
}