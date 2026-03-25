modded class TerjePlayerInspectableStats
{
	override void OnInit(PlayerBase player, int level)
	{
		super.OnInit(player, level);
		
		if (player && player.IsAlive() && player.GetTerjeStats())
		{
			int radSickness = player.GetTerjeStats().GetRadiationLevel();
			if (radSickness > 0)
			{
				if (level >= 2)
				{
					if (radSickness == 1)
					{
						AddInspectableStat("#STR_TERJERAD_PIS_RADSICK", "set:TerjeRadiation_icon image:tr_rad_sickness", "#STR_TERJERAD_PIS_RADSICK_L1");
					}
					else if (radSickness == 2)
					{
						AddInspectableStat("#STR_TERJERAD_PIS_RADSICK", "set:TerjeRadiation_icon image:tr_rad_sickness", "#STR_TERJERAD_PIS_RADSICK_L2");
					}
					else
					{
						AddInspectableStat("#STR_TERJERAD_PIS_RADSICK", "set:TerjeRadiation_icon image:tr_rad_sickness", "#STR_TERJERAD_PIS_RADSICK_L3");
					}
				}
				else if (level == 1)
				{
					AddInspectableStat("#STR_TERJERAD_PIS_RADSICK", "set:TerjeRadiation_icon image:tr_rad_sickness", "");
				}
			}
		}
	}
}