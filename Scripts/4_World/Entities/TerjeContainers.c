class TerjeCaseRadMilitary: WoodenCrate
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

class TerjeCaseRadSpecial: Container_Base
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