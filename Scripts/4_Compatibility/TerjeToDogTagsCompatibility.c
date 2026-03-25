#ifdef WRDG_DOGTAGS
modded class Dogtag_Base
{
	override bool IsTerjeRadiationAccumulated()
	{
		return false;
	}
	
	override float GetTerjeRadiationInventoryIsolation()
	{
		return 1;
	}
}
#endif