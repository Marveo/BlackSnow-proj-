modded class PluginRecipesManagerBase
{
	override void RegisterRecipies()
	{
		super.RegisterRecipies();
		RegisterRecipe(new TerjeCraftSoapWater);
		RegisterRecipe(new TerjeCraftRadTent);
	}
}