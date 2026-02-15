modded class PluginManager
{
	override void Init()
	{
		super.Init();

		RegisterPlugin("KTPluginQuestStore", false, true);
		RegisterPlugin("KTPluginQuestEventHandler", false, true);
	}
}