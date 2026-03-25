modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionWashRadioactiveItems);
		actions.Insert(ActionTurnOnTerjeRadTent);
		actions.Insert(ActionTurnOffTerjeRadTent);
		actions.Insert(ActionTurnOnTerjeRadTentStatic);
		actions.Insert(ActionTurnOffTerjeRadTentStatic);
	}
}