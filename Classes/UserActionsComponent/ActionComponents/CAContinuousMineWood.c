modded class CAContinuousMineWood
{
	override void DamagePlayersHands(PlayerBase player)
	{
		if ( !player.IsWearingLEHS() )
			super.DamagePlayersHands( player );
	}
};