class ActionFixPhoenixPipesCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.BASEBUILDING_REPAIR_SLOW );
	}
};

class ActionFixPhoenixPipes: ActionContinuousBase
{
	void ActionFixPhoenixPipes()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CUTBARK;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_CallbackClass = ActionFixPhoenixPipesCB;
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor;
	}
	
	override string GetText()
	{
		return "#nam_phoenix_pipes";
	}

	override void OnFinishProgressServer( ActionData action_data )
	{		
		land_sea_oilrig_derrick_base oilrig;
		if ( !Class.CastTo( oilrig, action_data.m_Target.GetObject() ) )
		{
			return;
		}
		
		MissionBaseWorld mission = MissionBaseWorld.Cast( GetGame().GetMission() );
		if ( mission )
		{
			mission.DecontaminatePhoenix();
		}
	}
		
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{		
		land_sea_oilrig_derrick_base oilrig;
		if ( !Class.CastTo( oilrig, target.GetObject() ) )
		{
			return false;
		}

		bool is_in_range = vector.Distance( oilrig.WorldToModel( player.GetPosition() ), oilrig.GetMemoryPointPos("fix_pipes") ) < 2;
		return is_in_range /*&& ( oilrig.GetBuildingState() == AthenaBuildingState.INACTIVE )*/;
	}
}