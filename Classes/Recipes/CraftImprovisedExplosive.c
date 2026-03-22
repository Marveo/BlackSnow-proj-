modded class CraftImprovisedExplosive
{	
	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		if ( ingredients[0].IsKindOf( "dzn_phoenix_stash" ) )
			return false;
		else
			return ingredients[0].IsEmpty();
	}
};
