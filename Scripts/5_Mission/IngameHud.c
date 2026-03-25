modded class IngameHud
{
	int TERJE_BADGE_RADIATION = -1;
	int TERJE_BADGE_ANTIRAD = -1;

	override void InitBadgesAndNotifiers()
	{
		super.InitBadgesAndNotifiers();
		TERJE_BADGE_RADIATION = RegisterTerjeBadgetWidget("set:TerjeRadiation_icon image:tr_rad_sickness", "TerjeRadiation", TerjeBadgeType.LEVELED);
		TERJE_BADGE_ANTIRAD = RegisterTerjeBadgetWidget("set:TerjeRadiation_icon image:tr_antirad", "TerjeAntirad", TerjeBadgeType.LEVELED);
	}
	
	override bool GetTerjeBadgeColor( int key, int value, out int outputColor )
	{
		if (super.GetTerjeBadgeColor(key, value, outputColor))
		{
			return true;
		}
		else if (key == TERJE_BADGE_RADIATION)
		{
			if (value <= 2)
			{
				outputColor = GetTerjeBadgeColorWarning();
				return true;
			}
			else
			{
				outputColor = GetTerjeBadgeColorCritical();
				return true;
			}
		}
		
		outputColor = GetTerjeBadgeColorDefault();
		return false;
	}
}
