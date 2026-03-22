class ActionEnableFlaxCore: ActionInteractBase
{
	void ActionEnableFlaxCore()
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
		return "#nam_action_enable_flax_core";
	}
	
	override typename GetInputType()
	{
		return InteractActionInput;
	}
	
	override void OnEndServer(ActionData action_data)
	{		
		land_a2_basement_3rd basement_3rd;
		if (!Class.CastTo(basement_3rd, action_data.m_Target.GetObject())) {
			Error("basement_3rd not found");
			return;
		}
		
		basement_3rd.SetBuildingState(AthenaBuildingState.ACTIVE);
	}
		
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{		
		land_a2_basement_3rd basement_3rd;
		if (!Class.CastTo(basement_3rd, target.GetObject())) {
			return false;
		}
					
		bool is_in_range = vector.Distance(basement_3rd.WorldToModel(player.GetPosition()), "4.5 -1.5 10.81") < 2;//basement_3rd.GetMemoryPointPos("flax_core_switch")
		return (is_in_range && basement_3rd.GetBuildingState() == AthenaBuildingState.STARTUP && IsEVRActive());		
	}
	
	bool IsEVRActive()
	{		
		NamEventBase event_base = NamEventManager.GetInstance().GetEvent(EVRStorm);
		if ( !event_base )
			event_base = NamEventManager.GetInstance().GetEvent(EVRStormDeadly);
		return (event_base && event_base.GetActivePhaseID() <= 1);
	}
}