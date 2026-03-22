enum EPlayerStatsNamalsk: EPlayerStats_current
{
	COLDRESISTANCE,
	FROSTBITEEFF,
	FROSTBITEZONES,
}

modded class PlayerStatsPCO_current
{
	override void Init()
	{
		super.Init();

		RegisterStat(EPlayerStatsNamalsk.COLDRESISTANCE,  	new PlayerStat<int>		(0,						100000,					0,						"ColdResistance",	EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStatsNamalsk.FROSTBITEEFF,  	new PlayerStat<float>	(0,						100,					0,						"FrostbiteEffect",	EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStatsNamalsk.FROSTBITEZONES,  	new PlayerStat<int>		(0,						63,						0,						"FrostbiteZones",	EPSstatsFlags.EMPTY) );
	}
}