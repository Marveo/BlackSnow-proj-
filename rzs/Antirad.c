modded class Radiation_BottlePills
{
	override void OnConsume(float amount, PlayerBase consumer)
	{
		if (consumer.GetModifiersManager().IsModifierActive(RadModifiers.MDF_SERVER_ANTIRAD_TABLETS))
			consumer.GetModifiersManager().DeactivateModifier(RadModifiers.MDF_SERVER_ANTIRAD_TABLETS, false);

		consumer.GetModifiersManager().ActivateModifier(RadModifiers.MDF_SERVER_ANTIRAD_TABLETS);
	}
}