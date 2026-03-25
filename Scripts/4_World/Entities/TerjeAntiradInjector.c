class TerjeAntiradInjector: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionInjectTarget);
		AddAction(ActionInjectSelf);
	}
	
	override void OnApply(PlayerBase player)
	{
		if (!player)
			return;
		
		ApplyTerjeConsumableEffects(player, 1);
	}
	
	override bool DescriptionOverride(out string output)
	{
		if (super.DescriptionOverride(output))
		{
			return true;
		}
		
		if (ConfigGetString("descriptionShort") == "...")
		{
			// Hack for compatibility with TerjeMedicine
			output = "#STR_TERJERAD_RADIOPROTECTOR_DESC";
			return true;
		}
		
		return false;
	}
}