modded class KTQuestBase
{
	private bool needCheckPos;

	void KTQuestBase(KTQuestProgress initData, KTQuestManager _parent)
	{
		needCheckPos = NeedPosCheck();
	}

	void Update()
	{
		CheckCondition();
	}

	void CheckCondition()
	{
		if (!NeedCheck())
			return;

		if (LeftTime() == 0)
		{
			OnQuestFailed();
			return;
		}

		if (needCheckPos)
		{
			CheckPos();
		}
	}

	bool NeedCheck()
	{
		return !QuestCompleted() && !QuestFailed();
	}

	bool NeedPosCheck()
	{
		for (int i = 0; i < GetQuestT().Targets.Count(); i++)
		{
			if (template.Targets[i].questType == KTQuestType.Exploration)
				return true;
		}
		return false;
	}

	void OnQuestFailed()
	{
		data.Completed = -1
		parent.OnQuestFailed(this);
	}

	bool QuestCompleted()
	{
		return data.Completed == 1;
	}

	bool QuestFailed()
	{
		return data.Completed == -1;
	}

	override KTQuestData GetQuestT()
	{
		if (!template)
			template = GetKTPluginQuestStore().GetQuests().Get(data.ID);
		return template;
	}

	bool UpdateProgress(string target, float count, string extra, int i)
	{
		bool statChanged = false;
		array<ref KTQuestDefaultData> Targets = template.Targets;
		array<float> goals = data.Progress;

		if (NeedCheck() && MainCondition(target, Targets[i].type) && ExtraCondition(Targets[i].extra, extra))
		{
			goals[i] = goals[i] + count;
			statChanged =  true;
		}

		if (statChanged)
			OnStatsUpdated();

		return statChanged;
	}

	void CheckPos()
	{
		array<float> goals = data.Progress;
		vector pos = parent.GetOwner().GetPosition();
		vector targetPos;
		float dist;
		bool needSave;
		for (int i = 0; i < template.Targets.Count(); i++)
		{
			if (template.Targets[i].questType == KTQuestType.Exploration && (goals[i] < 1))
			{
				targetPos = template.Targets[i].type.ToVector();
				if (targetPos == vector.Zero)
					continue;
				if (template.Targets[i].conditions[1] < 0)
					targetPos[1] = GetGame().SurfaceY(targetPos[0], targetPos[2]);
					
				dist = vector.Distance(pos, targetPos);

				if (dist <= template.Targets[i].conditions[0])
				{
					needSave = true;
					goals[i] = 1;
				}
			}
		}

		if (needSave)
		{
			OnStatsUpdated();
			parent.ForceSaveProgress();
		}
	}

	bool MainCondition(string has, string need)
	{
		bool kind = GetGame().IsKindOf(has, need);
		has.ToLower();
		need.ToLower();
		
		return (kind || has == need);
	}

	bool ExtraCondition(string from, string to)
	{
		if (!from)
			return true;
		
		return MainCondition(to, from);
	}

	void OnStatsUpdated()
	{
		array<ref KTQuestDefaultData> Targets = template.Targets;
		array<float> goals = data.Progress;

		for (int i = 0; i < Targets.Count(); i++)
		{
			if (goals[i] < Targets[i].count)
				return;
		}

		CompleteQuest();
	}

	void CompleteQuest()
	{
		data.Completed = 1;
		parent.OnCompleteQuest(this);
	}
}