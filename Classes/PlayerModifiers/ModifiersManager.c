enum eModifiersNamalsk: eModifiers 
{
	NAM_MDF_FROSTBITE = 200,
	NAM_MDF_AREAEXPOSURE = 202,
	NAM_MDF_ENZYME = 203,
	NAM_MDF_AREAEXPOSURE_PHOENIX = 204,
	NAM_MDF_OXYGENATEDBLOOD = 205,
	NAM_MDF_LEHSATMOSPHERE = 206,
	NAM_MDF_LANTIAATMOSPHERE = 207,
}

modded class ModifiersManager
{
	override void Init()
	{
		super.Init();

		AddModifier( new Frostbite);
		AddModifier( new AreaExposureNamMdfr );
		AddModifier( new EnzymeMdfr );
		AddModifier( new AreaExposureNamPhoenixMdfr );
		AddModifier( new OxygenatedBloodMdfr );
		AddModifier( new LEHSAtmosphereMdfr );
		AddModifier( new LantiaAtmosphereMdfr );
	}
};