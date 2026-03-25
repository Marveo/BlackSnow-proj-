// AmmoCraft_Compressor.c — станок для виготовлення набоїв вручну через внутрішній інвентар

class AmmoCraft_Compressor : Inventory_Base
{
    ref array<ref array<string>> m_Recipes;

    void AmmoCraft_Compressor()
    {
        m_Recipes = new array<ref array<string>>;

        // приклади рецептів
        m_Recipes.Insert({"Bullets_9x19", "Cartrige_powered_Pistol", "Ammo_9x19"});
        m_Recipes.Insert({"Bullet_545x39", "545x39_cartrige", "Ammo_545x39"});
        m_Recipes.Insert({"Bullet_556x45", "556x45_cartrige", "Ammo_556x45"});
        m_Recipes.Insert({"Bullet_762x39", "762x39_cartrige", "Ammo_762x39"});
    }

    // Повертає тип готового патрона, якщо знайдено відповідний рецепт
    string GetRecipeResult(ItemBase bullets, ItemBase cartrige)
    {
        if (!bullets || !cartrige)
            return "";

        string bullets_type = bullets.GetType();
        string cartrige_type = cartrige.GetType();

        foreach (array<string> recipe : m_Recipes)
        {
            if (recipe[0] == bullets_type && recipe[1] == cartrige_type)
                return recipe[2];
        }
        return "";
    }

    override bool CanPutInCargo(EntityAI parent)
    {
        return false; // станок не можна класти в інвентар
    }

    override bool CanPutIntoHands(EntityAI parent)
    {
        return false; // не можна взяти в руки
    }

    override bool IsInventoryVisible()
    {
        return true; // інвентар відкривається як у контейнера
    }

    override bool IsHeavyBehaviour()
    {
        return true; // важкий об'єкт
    }

    // --- ДІЇ ---
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionCraftAmmo);
    }
}
