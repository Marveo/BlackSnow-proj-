class ActionPlayVinyl extends ActionSingleUseBase
{
	void ActionPlayVinyl()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_DROPITEM_HANDS;
		m_FullBody = false;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_PRONE;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override string GetText()
	{
		return "#use";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_object = target.GetObject();
		if ( player && target_object && target_object.IsStaticTransmitter() )
		{
			EntityAI target_entity = EntityAI.Cast( target_object );
			string selection = target_object.GetActionComponentName( target.GetComponentIndex() );
			
			if ( target_entity.HasEnergyManager() && target_entity.GetCompEM().IsSwitchedOn() && selection == "control_panel" )
			{
				return true;
			}
		}
		
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{	
		action_data.m_MainItem.SetHealth( "","",0 );

		// send in RPC
		array<Man> players = {};
		GetGame().GetPlayers( players );
		foreach ( Man player: players )
		{
			GetGame().RPCSingleParam( player, ERPCsNamalsk.NAM_RPC_EG_OST, NULL, true, player.GetIdentity() );
		}
	}
};