class ActionToggleLEHSNVG: ActionBase
{
	void ActionToggleLEHSNVG()
	{
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override typename GetInputType()
	{
		return ToggleLEHSNVGActionInput;
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		dzn_lehs_helmet helmet = dzn_lehs_helmet.Cast( target.GetObject() );
		if ( !helmet )
		{
			return false;
		}
		else
		{
			// check for the link
			if ( helmet.HasSuit() && helmet.HasPower() && !helmet.IsVisorUp() )
				return true;
			else
				return false;
		}
	}

	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		if ( GetGame().IsServer() )
		{
			dzn_lehs_helmet helmet = dzn_lehs_helmet.Cast( action_data.m_Target.GetObject() );
			if ( helmet )
			{
				helmet.ToggleNVG();
			}
		}
	}
};
