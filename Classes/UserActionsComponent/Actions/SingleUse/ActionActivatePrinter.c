class ActionActivatePrinter: ActionInteractBase
{
	void ActionActivatePrinter()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor;
	}
	
	override string GetText()
	{
		return "#nam_phoenix_printer";
	}
	
	override typename GetInputType()
	{
		return InteractActionInput;
	}
	
	override void OnEndServer( ActionData action_data )
	{		
		land_sea_oilrig_printer lehs_printer;
		if ( !Class.CastTo( lehs_printer, action_data.m_Target.GetObject() ) )
		{
			return;
		}

		ItemBase access_card = ItemBase.Cast( lehs_printer.FindAttachmentBySlotName( "FLAX_ACard" ) );
		ItemBase blueprint = ItemBase.Cast( lehs_printer.FindAttachmentBySlotName( "Printer_Blueprint" ) );
		ItemBase filament1 = ItemBase.Cast( lehs_printer.FindAttachmentBySlotName( "Printer_Filament_ABS" ) );
		ItemBase filament2 = ItemBase.Cast( lehs_printer.FindAttachmentBySlotName( "Printer_Filament_TPC" ) );
		ItemBase filament3 = ItemBase.Cast( lehs_printer.FindAttachmentBySlotName( "Printer_Filament_Nylon" ) );

		if ( access_card && blueprint && filament1 && filament2 && filament3 )
		{
			if ( !access_card.IsRuined() && !blueprint.IsRuined() && !filament1.IsRuined() && !filament2.IsRuined() && !filament3.IsRuined() )
			{
				// start printing
				lehs_printer.ActivatePrinter();

				// dmg items
				access_card.AddHealth( "", "Health", -17.0 );
				blueprint.SetHealth( "", "Health", 0.0 );
				filament1.SetHealth( "", "Health", 0.0 );
				filament2.SetHealth( "", "Health", 0.0 );
				filament3.SetHealth( "", "Health", 0.0 );

				// lock in place (use UnlockFromParent to unlock)
				access_card.LockToParent();
				blueprint.LockToParent();
				filament1.LockToParent();
				filament2.LockToParent();
				filament3.LockToParent();

				// finalize printing
				GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( lehs_printer.CompletePrinting, ( lehs_printer.PRINTING_LENGTH * 1000 ) );
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
	override void OnEndClient( ActionData action_data )
	{
		land_sea_oilrig_printer lehs_printer;
		if ( !Class.CastTo( lehs_printer, action_data.m_Target.GetObject() ) )
		{
			return;
		}

		ItemBase access_card = ItemBase.Cast( lehs_printer.FindAttachmentBySlotName( "FLAX_ACard" ) );
		ItemBase blueprint = ItemBase.Cast( lehs_printer.FindAttachmentBySlotName( "Printer_Blueprint" ) );
		ItemBase filament1 = ItemBase.Cast( lehs_printer.FindAttachmentBySlotName( "Printer_Filament_ABS" ) );
		ItemBase filament2 = ItemBase.Cast( lehs_printer.FindAttachmentBySlotName( "Printer_Filament_TPC" ) );
		ItemBase filament3 = ItemBase.Cast( lehs_printer.FindAttachmentBySlotName( "Printer_Filament_Nylon" ) );

		if ( access_card && blueprint && filament1 && filament2 && filament3 && !access_card.IsDamageDestroyed() && !blueprint.IsDamageDestroyed() && !filament1.IsDamageDestroyed() && !filament2.IsDamageDestroyed() && !filament3.IsDamageDestroyed() )
		{
			// start printing
			EffectSound effect_confirm;
            effect_confirm = SEffectManager.PlaySound( "printer_confirm_soundset", action_data.m_Target.GetObject().GetPosition(), 0, 0, false );
            effect_confirm.SetParent( action_data.m_Target.GetObject() );
            effect_confirm.GetSoundObject().SetVolume( 1.0 );
            effect_confirm.SetSoundAutodestroy( true );
		}
		else
		{
			// error
			EffectSound effect_error;
            effect_error = SEffectManager.PlaySound( "printer_error_soundset", action_data.m_Target.GetObject().GetPosition(), 0, 0, false );
            effect_error.SetParent( action_data.m_Target.GetObject() );
            effect_error.GetSoundObject().SetVolume( 1.0 );
            effect_error.SetSoundAutodestroy( true );
		}
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{		
		land_sea_oilrig_printer lehs_printer;
		if ( !Class.CastTo( lehs_printer, target.GetObject() ) )
		{
			return false;
		}
					
		bool is_in_range = vector.Distance( lehs_printer.WorldToModel( player.GetPosition() ), lehs_printer.GetMemoryPointPos( "printer_control" ) ) < 1.5;
		return ( is_in_range && !lehs_printer.GetActivation() );		
	}
}