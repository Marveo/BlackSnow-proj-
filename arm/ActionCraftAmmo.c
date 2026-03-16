class ActionCraftAmmoCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        // тривалість дії — 4 секунди
        m_ActionData.m_ActionComponent = new CAContinuousTime(4);
    }
}

class ActionCraftAmmo : ActionContinuousBase
{
    void ActionCraftAmmo()
    {
        m_CallbackClass = ActionCraftAmmoCB;
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
        m_FullBody = true;
    }

    override void CreateConditionComponents()  
	{
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
        m_ConditionItem = new CCINone;
	}

    override string GetText()
    {
        return "Виготовити набої ";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        // перевірка, чи це наш станок
        Object target_object = target.GetObject();
        if (!target_object)
            return false;

        AmmoCraft_Compressor compressor = AmmoCraft_Compressor.Cast(target_object);
        if (!compressor)
            return false;

        return true;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        PlayerBase player = action_data.m_Player;
        AmmoCraft_Compressor compressor = AmmoCraft_Compressor.Cast(action_data.m_Target.GetObject());
        if (!compressor || !player)
            return;

        // отримуємо вміст інвентаря станка
        array<EntityAI> items = new array<EntityAI>;
        compressor.GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, items);

        ItemBase bullet = null;
        ItemBase shell = null;

        foreach (EntityAI item : items)
        {
            if (item && item.IsInherited(ItemBase))
            {
                string type = item.GetType();
                if (type.Contains("Bullets_")) bullet = ItemBase.Cast(item);
                if (type.Contains("Cartrige_powered_")) shell = ItemBase.Cast(item);
            }
        }

        // якщо нічого немає — повідомлення і вихід
        if (!bullet && !shell)
        {
            player.MessageStatus("Гільза і куля відсутні, немає чого обтискати!");
            return;
        }

        // перевірка рецепту
        string result_type = compressor.GetRecipeResult(bullet, shell);
        if (result_type == "")
        {
            player.MessageStatus("Комбінація непридатна!");

            // 40% шанс, що зламається один або обидва предмети
            if (Math.RandomFloat01() < 0.4)
            {
                float rnd = Math.RandomFloat01();
                if (rnd < 0.33 && bullet) bullet.SetHealth(0);
                else if (rnd < 0.66 && shell) shell.SetHealth(0);
                else
                {
                    if (bullet) bullet.SetHealth(0);
                    if (shell) shell.SetHealth(0);
                }
                player.MessageStatus("Деталі були пошкоджені при спробі!");
            }
            return;
        }

        // перевірка шансів успіху з урахуванням навички
        float skill = player.GetSingleAgentCount(SkillAgent.AMMO_CRAFTING);
        float successChance = Math.Clamp(0.6 + (skill / 2000) * 0.4, 0.6, 1.0);
        float quantityMult = 1.0 + (skill / 2000) * 0.5;

        if (Math.RandomFloat01() > successChance)
        {
            player.MessageStatus("Виготовлення не вдалося.");
            if (Math.RandomFloat01() < 0.25)
            {
                if (bullet) bullet.SetHealth(0);
                if (shell) shell.SetHealth(0);
            }
            return;
        }

        int amount = Math.Floor(5 * quantityMult);

        // руйнуємо використані предмети (по одному)
        if (bullet) bullet.AddQuantity(-1);
        if (shell) shell.AddQuantity(-1);

        // створюємо патрони на позиції гравця
        vector pos = player.GetPosition();
        Ammunition_Base ammo = Ammunition_Base.Cast(GetGame().CreateObjectEx(result_type, pos, ECE_PLACE_ON_SURFACE));
        if (ammo)
        {
            ammo.ServerSetAmmoCount(amount);
            player.MessageStatus("Виготовлено: " + amount.ToString() + " патронів.");
        }

        // додаємо досвід
        player.InsertAgent(SkillAgent.AMMO_CRAFTING, 10);
    }
}