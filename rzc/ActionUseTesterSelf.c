class ActionUseTesterSelf : ActionInjectMorphineSelf
{
	string inrFon;
	
	void ActionUseTesterSelf()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INJECTION;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase batteryItem = item.FindAttachmentBySlotName("BatteryD");
		
		return item.HasEnergyManager() && item.GetCompEM().CanSwitchOn() && item.GetCompEM().CanWork() && batteryItem.GetQuantity() > 0;
	}

    override bool ActionConditionContinue( ActionData action_data )
	{
		return false;
	}
		
	override string GetText()
	{
		return "#STR_action_measureRAD";
	}

	override void OnExecuteServer( ActionData action_data )
	{	
	}
	
	override void OnEndServer( ActionData action_data )
    {
    }

	private void DisplayDosimeterValue(ItemBase item, string value)
    {
        for (int j = 0; j < 4; j++)
        {
            ChangeDigit(item, j, "off");
        }
        int len = value.Length();
        for (int k = 0; k < len && k < 4; k++)
        {
            ChangeDigit(item, (4 - len + k), value.Get(k));
        }
    }

	private void ChangeDigit(ItemBase item, int digitIndex, string number)
    {
        int hiddenSelectionIndex = digitIndex + 1;
        item.SetObjectTexture(hiddenSelectionIndex, "RadiationClient\\Geiger\\data\\digits\\" + number + ".paa");
    }
	
	override void OnEndClient( ActionData action_data )
	{
		
	}

}