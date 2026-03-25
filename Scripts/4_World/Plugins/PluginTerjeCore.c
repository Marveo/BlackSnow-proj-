modded class PluginTerjeCore
{
	protected override void RegisterSubmodules(array<string> submodules)
	{
		super.RegisterSubmodules(submodules);
		submodules.Insert("TerjeRadiation");
	}
}