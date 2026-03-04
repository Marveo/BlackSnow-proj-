modded class ModifiersManager
{
	override void Init()
	{
		super.Init();

		AddModifier(new Radiation_DeBuff_Mdfr);
		AddModifier(new Server_AntiRadTablets_mdfr);
	}
}