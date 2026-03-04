modded class ModifiersManager
{
	override void Init()
	{
		super.Init();

		AddModifier(new Client_AntiRadModifier_mdfr);
	}
}