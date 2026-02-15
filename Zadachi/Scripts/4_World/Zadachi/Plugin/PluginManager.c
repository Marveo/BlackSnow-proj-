modded class PluginManager
{
	override void Init()
	{
		super.Init();

		RegisterPlugin("KTPluginQuestMenu", true, false);
	}
}