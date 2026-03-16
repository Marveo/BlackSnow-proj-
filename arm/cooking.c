modded class Cooking
{
	static const float DEFAULT_COOKING_TEMPERATURE 		= 800;		//default temperature for cooking (e.g. cooking on stick)
	static const float FOOD_MAX_COOKING_TEMPERATURE		= 800;		//

	override int CookWithEquipment( ItemBase cooking_equipment, float cooking_time_coef = 1 )
	{
		if (cooking_equipment.IsKindOf("Tigel"))
		{
			CargoBase cargo = cooking_equipment.GetInventory().GetCargo();
			if (cargo)
			{
				for (int i = cargo.GetItemCount() - 1; i >= 0; i--)
				{
					ItemBase item = cargo.GetItem(i);

					// умова плавлення
					if (item.IsKindOf("Lead_plate") && item.GetTemperature() > 327)
					{
						Tigel bucket = Tigel.Cast(cooking_equipment);
						if (bucket)
						{
							const int LEAD_PER_PLATE = 200;  // свинець за 1 пластину
							bucket.AddQuantity(LEAD_PER_PLATE);
						}

						// мінус 1 пластина зі стека
						item.AddQuantity(-1);

						// якщо стек закінчився — видаляємо
						if (item.GetQuantity() <= 0)
							item.Delete();

						continue;
					}

					// нагрів інших предметів
					AddTemperatureToItem(item, NULL, 0);
				}
			}
		}

		else
		{
			return super.CookWithEquipment(cooking_equipment, cooking_time_coef);
		}
		return 0;
	}


	override CookingMethodType GetCookingMethod( ItemBase cooking_equipment )
	{
		if (cooking_equipment.Type() == Tigel)
		{
 			return CookingMethodType.DRYING;
		}
		return super.GetCookingMethod(cooking_equipment);
	}
}