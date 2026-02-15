class ActionOpenQuestMenu : ActionInteractBase
{
	void ActionOpenQuestMenu()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_HUDCursorIcon = CursorIcons.CloseHood;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINone;
	}

	override string GetText()
	{
		return "#use";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		PlayerBase targetPl;
		if (CastTo(targetPl, target.GetObject()))
		{
			return targetPl.IsQuestNpc();
		}
		return false;
	}

	override void OnExecuteClient( ActionData action_data )
	{
		PlayerBase targetPl;
		CastTo(targetPl, action_data.m_Target.GetObject());
		GetQuestMenuHandler().TryOpenUI(false, targetPl.GetQuestNpcID());
	}
}

modded class ActionConstructor
{
	
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionOpenQuestMenu);
	}
}