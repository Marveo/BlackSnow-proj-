modded class RecipeBase
{
	override void SpawnItems(ItemBase ingredients[], PlayerBase player, array<ItemBase> spawned_objects/*out*/)
	{
		super.SpawnItems(ingredients, player, spawned_objects);

		KTQuestOnEntityCrafted.Invoke(player, spawned_objects);
	}
}