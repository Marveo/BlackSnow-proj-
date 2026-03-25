#ifdef A6_SecureContainers
modded class A6_SecureContainer_Base
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