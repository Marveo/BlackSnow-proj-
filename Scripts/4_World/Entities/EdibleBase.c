modded class Edible_Base
{
	override bool Consume(float amount, PlayerBase consumer)
	{
		if (super.Consume(amount, consumer))
		{
			float itemRadiation = GetTerjeRadiation();
			if (itemRadiation > 0)
			{
				if (HasQuantity() && GetQuantityMax() > 0)
				{
					consumer.AddTerjeRadiationAdvanced((itemRadiation / GetQuantityMax()) * amount, -1, true);
				}
				else
				{
					consumer.AddTerjeRadiationAdvanced(itemRadiation * amount, -1, true);
				}
			}
			
			return true;
		}
		
		return false;
	}
}