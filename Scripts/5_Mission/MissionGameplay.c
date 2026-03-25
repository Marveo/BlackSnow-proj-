modded class MissionGameplay
{
	override void OnMissionStart()
	{
		super.OnMissionStart();
		PPERequesterBank.GetRequester(PPERequesterBank.REQ_TERJEMED_RADIATION).Start();
	}
	
	override void OnMissionFinish()
	{
		super.OnMissionFinish();
		PPERequesterBank.GetRequester(PPERequesterBank.REQ_TERJEMED_RADIATION).Stop();
	}
	
	override void OnUpdateTerjeCustomBadges(PlayerBase player, bool detailedDiseaseHudBadges, bool detailedHealingHudBadges)
	{
		super.OnUpdateTerjeCustomBadges(player, detailedDiseaseHudBadges, detailedHealingHudBadges);

		if (detailedDiseaseHudBadges)
		{
			m_Hud.DisplayBadge(m_Hud.TERJE_BADGE_RADIATION, player.GetTerjeStats().GetRadiationLevel());
		}
		
		if (detailedHealingHudBadges)
		{
			m_Hud.DisplayBadge(m_Hud.TERJE_BADGE_ANTIRAD, player.GetTerjeStats().GetAntiradLevel());
		}
	}
	
	override void OnUpdateTerjeCustomGUI(PlayerBase player, float deltaTime)
	{
		super.OnUpdateTerjeCustomGUI(player, deltaTime);
		PPERequester_TerjeMedRadiation.Cast(PPERequesterBank.GetRequester(PPERequesterBank.REQ_TERJEMED_RADIATION)).SetRadiationEffect(player.GetTerjeStats().GetRadiationLevel());
		PPERequesterBank.GetRequester(PPERequesterBank.REQ_TERJEMED_RADIATION).SetRequesterUpdating(true);
	}
}
