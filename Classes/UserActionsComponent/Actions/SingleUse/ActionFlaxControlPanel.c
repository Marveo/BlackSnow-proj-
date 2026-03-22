class ActionFlaxControlPanel: ActionInteractBase
{
	void ActionFlaxControlPanel()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_HUDCursorIcon = CursorIcons.CloseDoors;
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor;
	}
	
	override string GetText()
	{
		return "#nam_action_flaxpanel";
	}
	
	override typename GetInputType()
	{
		return InteractActionInput;
	}
	
	override void OnEndServer( ActionData action_data )
	{		
		land_a3_flaxbt_panel flaxbt_panel;
		if ( !Class.CastTo( flaxbt_panel, action_data.m_Target.GetObject() ) )
		{
			return;
		}
		
		int panelActivation = PanelStartupLogicServer( flaxbt_panel );
		if ( panelActivation == 2 )
			NamEventManager.GetInstance().CancelLantiaEvents();
		if ( panelActivation == 1 )
			NamEventManager.GetInstance().CancelEVREvents();
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{		
		land_a3_flaxbt_panel flaxbt_panel;
		if ( !Class.CastTo( flaxbt_panel, target.GetObject() ) )
		{
			return false;
		}
					
		bool is_in_range = vector.Distance( flaxbt_panel.WorldToModel( player.GetPosition() ), "0.0 0.7 0.4" ) < 2;
		return ( is_in_range && !IsFacilityActive() && !flaxbt_panel.GetActivation() );		
	}
	
	bool IsFacilityActive()
	{		
		NamEventBase event_evr = NamEventManager.GetInstance().GetEvent( EVRStorm );
		NamEventBase event_evr_ext = NamEventManager.GetInstance().GetEvent( EVRStormDeadly );
		NamEventBase event_lnt = NamEventManager.GetInstance().GetEvent( LantiaLink );
		return ( event_evr || event_evr_ext || event_lnt );
	}

	int PanelStartupLogicServer( land_a3_flaxbt_panel flaxbt_panel )
	{
		flaxbt_panel.SetActivation( true );
		
		land_a3_chamber a3facility; 
		EnScript.GetClassVar( GetGame().GetMission(), "m_LandA3Chamber", 0, a3facility );
		if ( !a3facility )
			return -1;
		
		ItemBase access_card = ItemBase.Cast( flaxbt_panel.FindAttachmentBySlotName( "FLAX_ACard" ) );
		ItemBase module_lantia = ItemBase.Cast( flaxbt_panel.FindAttachmentBySlotName( "FLAX_LOChipBase" ) );
		ItemBase module_surge = ItemBase.Cast( flaxbt_panel.FindAttachmentBySlotName( "FLAX_SurgeModule" ) );
		ItemBase module_ext = ItemBase.Cast( flaxbt_panel.FindAttachmentBySlotName( "FLAX_FlowControl" ) );
			
		if ( access_card )
		{
			if ( module_lantia )
			{
				if ( module_ext )
				{
					if ( module_ext.IsKindOf( "dzn_module_ext2" ) )
					{
						// attempt to create LantiaLink with a MAX duration
						if ( !access_card.IsDamageDestroyed() && !module_lantia.IsDamageDestroyed() && !module_ext.IsDamageDestroyed() )
						{
							access_card.AddHealth( "", "Health", -17.0 );
							module_lantia.AddHealth( "", "Health", -34.0 );
							module_ext.SetHealth( "", "Health", 0.0 );
							
							// kick off extended event
							NamEventManager.GetInstance().StartEvent( LantiaLink, true );
							LantiaEC_Server.OpenLantiaLink( ELantiaMode.PHASED );
							
							// enable panel (for next use)
							GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( flaxbt_panel.DeactivatePanel, 10000 );
							return 2;
						}
						else 
						{
							// a3 facility state: error
							a3facility.FacilityError();
							return -1;
						}
					}
					else
					{
						// attempt to create LantiaLink with a longer duration
						if ( !access_card.IsDamageDestroyed() && !module_lantia.IsDamageDestroyed() && !module_ext.IsDamageDestroyed() )
						{
							access_card.AddHealth( "", "Health", -17.0 );
							module_lantia.AddHealth( "", "Health", -34.0 );
							module_ext.SetHealth( "", "Health", 0.0 );
							
							// kick off extended event
							NamEventManager.GetInstance().StartEvent( LantiaLink, true );
							LantiaEC_Server.OpenLantiaLink( ELantiaMode.EXT );
							
							// enable panel (for next use)
							GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( flaxbt_panel.DeactivatePanel, 10000 );
							return 2;
						}
						else 
						{
							// a3 facility state: error
							a3facility.FacilityError();
							return -1;
						}
					}
				}
				else
				{
					// attempt to create LantiaLink
					if ( !access_card.IsDamageDestroyed() && !module_lantia.IsDamageDestroyed() )
					{
						access_card.AddHealth( "", "Health", -17.0 );
						module_lantia.AddHealth( "", "Health", -34.0 );
						
						// kick off the event
						NamEventManager.GetInstance().StartEvent( LantiaLink, true );
						LantiaEC_Server.OpenLantiaLink( ELantiaMode.NOEXT );
						
						// enable panel (for next use)
						GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( flaxbt_panel.DeactivatePanel, 10000 );
						return 2;
					}
					else 
					{
						// a3 facility state: error
						a3facility.FacilityError();
						return -1;
					}
				}
			}
			else 
			{
				if ( module_surge )
				{
					if ( module_ext )
					{
						// attempt to launch deadly EVR sequence
						if ( !access_card.IsDamageDestroyed() && !module_surge.IsDamageDestroyed() && !module_ext.IsDamageDestroyed() )
						{
							access_card.AddHealth( "", "Health", -17.0 );
							module_surge.SetHealth( "", "Health", 0.0 );
							module_ext.SetHealth( "", "Health", 0.0 );
							
							// kick off the extended event
							NamEventManager.GetInstance().StartEvent( EVRStormDeadly, true );
							
							// enable panel (for next use)
							GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( flaxbt_panel.DeactivatePanel, 10000 );
							return 1;
						}
						else 
						{
							// a3 facility state: error
							a3facility.FacilityError();
							return -1;
						}
					}
					else 
					{
						// attempt to launch standard EVR sequence
						if ( !access_card.IsDamageDestroyed() && !module_surge.IsDamageDestroyed() )
						{
							access_card.AddHealth( "", "Health", -17.0 );
							module_surge.SetHealth( "", "Health", 0.0 );
							
							// kick off the event
							NamEventManager.GetInstance().StartEvent( EVRStorm, true );
							
							// enable panel (for next use)
							GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( flaxbt_panel.DeactivatePanel, 10000 );
							return 1;
						}
						else 
						{
							// a3 facility state: error
							a3facility.FacilityError();
							return -1;
						}
					}
				}
				else 
				{
					// a3 facility state: error
					a3facility.FacilityError();
					return -1;
				}
			}
		}
		else 
		{
			// a3 facility state: error
			a3facility.FacilityError();
			return -1;
		}
	}
}