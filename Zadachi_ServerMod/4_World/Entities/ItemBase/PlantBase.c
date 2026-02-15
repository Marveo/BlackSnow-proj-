modded class PlantBase
{
	override void Harvest( PlayerBase player )
	{
		super.Harvest(player);

		KTQuestOnHarvest.Invoke(GetType(), player);
	}
}