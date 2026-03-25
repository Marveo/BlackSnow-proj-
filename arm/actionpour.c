class ActionPourCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(5);
	}
};

class ActionPour: ActionContinuousBase
{	
	string action_type = "";
	void ActionPour()
	{
		m_CallbackClass = ActionPourCB;
		m_CommandUID = DayZPlayerConstants.CMD_GESTUREFB_CAMPFIRE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
	}

	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTSelf;
		m_ConditionItem = new CCINone;
	}

	override string GetText()
	{
		return "";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
		
	override bool ActionCondition (PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return false;
	}
	
	bool ActionConditionPour(PlayerBase player, ActionTarget target, ItemBase item)
	{
		// 1. Перевірка інструмента в руках
		MoldaPliers pliers = MoldaPliers.Cast(player.GetItemInHands());
		
		// 2. Перевірка цільового Tigel
		Tigel tigel = Tigel.Cast(target.GetObject());
		
		if (tigel && pliers)
		{
			// 3. Перевірка кількості свинцю у Tigel
			float leadAmount = tigel.GetQuantity();
			if (leadAmount >= 100)
			{
				// 4. Перевірка температури Tigel
				float temp = tigel.GetTemperature();
				if (temp > 328)
				{
					return true; // УМОВИ ВИКОНАНІ
				}
			}
		}

		return false;
	}


	void PourResult(PlayerBase player, ActionTarget target, string bulletType, int baseLeadCost, int baseOutput)
	{
		Tigel tigel = Tigel.Cast(target.GetObject());
		if (!tigel)
			return;

		// 🔵 1. Прочитати навичку
		int skill = player.GetSingleAgentCount(SkillAgent.AMMO_CRAFTING);

		float skillMult = 1.0;
		float failChance = 0.15;   // 15% шанс браку для новачка
		float bonusChance = 0.0;

		// 🔥 2. Залежність від навички
		if (skill < 200)           // Новачок
		{
			skillMult = 1.0;
			failChance = 0.15;
			bonusChance = 0.00;
		}
		else if (skill < 500)      // Учень
		{
			skillMult = 1.10;
			failChance = 0.10;
			bonusChance = 0.10;
		}
		else if (skill < 1000)     // Профі
		{
			skillMult = 1.20;
			failChance = 0.05;
			bonusChance = 0.20;
		}
		else                       // Мастер
		{
			skillMult = 1.30;
			failChance = 0.00;
			bonusChance = 0.30;
		}

		// 🔵 3. Перевірити наявність свинцю
		int leadCost = baseLeadCost;
		if (tigel.GetQuantity() < leadCost)
			return;

		// Знімаємо свинець
		tigel.AddQuantity(-leadCost);

		// 🔵 4. Шанс браку через низьку навичку
		float rnd = Math.RandomFloat01();
		if (rnd < failChance)
		{
			// бракована куля
			GetGame().CreateObject("Paper", player.GetPosition());
			// прокачка навички
			player.InsertAgent(SkillAgent.AMMO_CRAFTING, 5);
			return;
		}

		// 🔵 5. Визначити кількість куль (бонус за навичку)
		int outputCount = baseOutput;
		float rndBonus = Math.RandomFloat01();
		if (rndBonus < bonusChance)
		{
			outputCount += 1; // +1 бонусна куля
		}

		// 🔵 6. Створення куль
		vector pos = player.GetPosition();

		for (int i = 0; i < outputCount; i++)
		{
			GetGame().CreateObject(bulletType, pos);
		}

		// 🔵 7. Прокачування навички
		player.InsertAgent(SkillAgent.AMMO_CRAFTING, 10);
	}


}

class ActionPourBullets_9x19: ActionPour
{
	override string GetText()
	{
		return "Виплавити 9x19";
	}
	
	override bool ActionCondition (PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return ActionConditionPour (player, target, item);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		PourResult(
			action_data.m_Player, action_data.m_Target,"Bullets_9x19", 90, 1);
	}
}

class ActionPourBullets_9x17: ActionPour
{
	override string GetText()
	{
		return "Виплавити 9x17";
	}
	
	override bool ActionCondition (PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return ActionConditionPour (player, target, item);
	}
}

class ActionPourBullets_45ACP: ActionPour
{
	override string GetText()
	{
		return "Виплавити 45 ACP";
	}
	
	override bool ActionCondition (PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return ActionConditionPour (player, target, item);
	}
}

class ActionPourBullets_357Magnum: ActionPour
{
	override string GetText()
	{
		return "Виплавити 357 Magnum";
	}
	
	override bool ActionCondition (PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return ActionConditionPour (player, target, item);
	}
}

class ActionPourBullets_545x39: ActionPour
{
	override string GetText()
	{
		return "Виплавити 5.45×39";
	}
	
	override bool ActionCondition (PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return ActionConditionPour (player, target, item);
	}
}

class ActionPourBullets_556x45: ActionPour
{
	override string GetText()
	{
		return "Виплавити 5.56×45";
	}
	
	override bool ActionCondition (PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return ActionConditionPour (player, target, item);
	}
}

class ActionPourBullets_762x39: ActionPour
{
	override string GetText()
	{
		return "Виплавити 7.62×39";
	}
	
	override bool ActionCondition (PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return ActionConditionPour (player, target, item);
	}
}

class ActionPourBullets_9x39: ActionPour
{
	override string GetText()
	{
		return "Виплавити 9×39";
	}
	
	override bool ActionCondition (PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return ActionConditionPour (player, target, item);
	}
}

class ActionPourBullets_308Win: ActionPour
{
	override string GetText()
	{
		return "Виплавити 308 Win";
	}
	
	override bool ActionCondition (PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return ActionConditionPour (player, target, item);
	}
}

class ActionPourBullets_762x54: ActionPour
{
	override string GetText()
	{
		return "Виплавити 7.62×54";
	}
	
	override bool ActionCondition (PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return ActionConditionPour (player, target, item);
	}
}