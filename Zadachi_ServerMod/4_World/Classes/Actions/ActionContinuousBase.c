modded class ActionContinuousBase
{
	override void OnFinishProgress( ActionData action_data )
	{
		EntityAI target;
		if (action_data.m_Target && action_data.m_Target.GetObject())
			target = action_data.m_Target.GetObject();
		KTQuestOnActionCompleted.Invoke(this.ClassName(), action_data.m_Player, target);

		super.OnFinishProgress(action_data);
	}
}