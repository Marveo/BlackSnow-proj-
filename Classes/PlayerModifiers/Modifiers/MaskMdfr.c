modded class MaskMdfr
{
	const float IN_AREA_CONSUME_FILTER_QUANTITY_PER_SEC = 0.10;

	override void OnTick(PlayerBase player, float deltaT)
	{	
		MaskBase mask = MaskBase.Cast(player.GetInventory().FindAttachment(InventorySlots.MASK));
		
		float consume_quantity = OUT_AREA_CONSUME_FILTER_QUANTITY_PER_SEC;
		
		bool inside_area = m_Manager.IsModifierActive( eModifiers.MDF_AREAEXPOSURE ) || m_Manager.IsModifierActive( eModifiersNamalsk.NAM_MDF_AREAEXPOSURE );
		
		if(inside_area)
		{
			consume_quantity = IN_AREA_CONSUME_FILTER_QUANTITY_PER_SEC;
		}

		if ( mask && !mask.IsDamageDestroyed())
		{
			bool consuming_filter = mask.ConsumeQuantity(consume_quantity * deltaT, player);
			StaminaSoundHandlerServer.Cast(player.GetPlayerSoundManagerServer().GetHandler(eSoundHandlers.STAMINA)).SetStaminaZoneOffset(1);

			if (mask.HasValidFilter())
			{
				player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypes.MASK);
				player.GetStaminaHandler().ActivateDepletionModifier(EStaminaMultiplierTypes.MASK);
				
			}
			else
			{
				player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.MASK);
				player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.MASK);
			}
			HandleSounds(player, deltaT, mask);
		}
	}
}