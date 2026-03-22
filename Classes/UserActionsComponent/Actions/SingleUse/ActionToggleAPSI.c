class ActionToggleAPSI: ActionBase
{
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		dzn_apsi apsi = dzn_apsi.Cast(target.GetObject());		
		return (apsi != null);
	}

	override void Start(ActionData action_data)
	{
		//Print("Starting APSI");
		super.Start(action_data);
		dzn_apsi apsi = dzn_apsi.Cast(action_data.m_Target.GetObject());
		if (!apsi) return;
		
		if (apsi.IsSwitchedOn()) {
			apsi.SwitchOff();
			return;
		}

		apsi.SwitchOn();
	}
	
	override typename GetInputType()
	{
		return ToggleAPSIActionInput;
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override bool CanBeUsedInVehicle()
	{
		return true;
	}

	override bool CanBeUsedSwimming()
	{
		return true;
	}
	
	override bool CanBeUsedOnLadder()
	{
		return true;
	}
}