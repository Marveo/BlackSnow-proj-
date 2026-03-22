modded class ActionUnrestrainSelf
{
	override void OnFinishProgressServer( ActionData action_data )
	{
		PlayerBase player = PlayerBase.Cast( action_data.m_Player );
		EntityAI item_in_hands = action_data.m_MainItem;

		if ( item_in_hands )
		{
			if ( item_in_hands.IsInherited( BarbedWireLocked ) && !player.IsWearingLEHS() )
			{
				int randNum = Math.RandomInt( 0, 3 ); // Values 0 to 3 represent fore arm and arm roll bleed sources
				player.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection( PlayerBase.m_BleedingSourcesUp[randNum] );
			}
			player.SetRestrained( false );
			item_in_hands.AddHealth( -GameConstants.CLEAN_UNRESTRAIN_DAMAGE );
			MiscGameplayFunctions.TransformRestrainItem( item_in_hands, null, null, player );
		}
	}
};