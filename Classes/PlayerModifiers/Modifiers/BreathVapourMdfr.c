modded class BreathVapourMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if ( player.IsWearingLEHSHelmet() )
		{
			dzn_lehs_helmet helmet = dzn_lehs_helmet.Cast( player.FindAttachmentBySlotName( "Headgear" ) );
			if ( !helmet.IsVisorUp() )
			{
				int level = 0;
				if ( level != player.m_BreathVapour )
				{
					player.m_BreathVapour = level;
					player.SetSynchDirty();
				}
				return;
			}
		}

		super.OnTick( player, deltaT );
	}
}