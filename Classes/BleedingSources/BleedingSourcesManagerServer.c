modded class BleedingSourcesManagerServer
{
	//damage must be to "Blood" healthType
	override void ProcessHit(float damage, EntityAI source, int component, string zone, string ammo, vector modelPos)
	{
		float dmg_max = m_Player.GetMaxHealth(zone, "Blood");
		float bleed_threshold = GetGame().ConfigGetFloat("CfgAmmo " + ammo + " DamageApplied bleedThreshold");
		string damageTypeString = GetGame().ConfigGetTextOut("CfgAmmo " + ammo + " DamageApplied type");
		bleed_threshold = Math.Clamp(bleed_threshold,0,1);
		float bleedingChance;
		bool createBleedingSource = false;
		
		// 'true' only when the damageTypeString is handled there
		if (BleedChanceData.CalculateBleedChance(damageTypeString, damage, bleed_threshold,bleedingChance))
		{
			float roll = Math.RandomFloat01();
			createBleedingSource = bleedingChance != 0 && bleedingChance >= roll;
			
			#ifdef DEVELOPER
			if (LogManager.IsBleedingChancesLogEnable())
			{	
				Debug.BleedingChancesLog(roll.ToString(), "BleedingSourcesManagerServer" , "n/a", "bleeding random roll:");
			}
			#endif
		}
		else if (source && source.IsZombie() && !m_Player.IsWearingLEHS())
		{
			int chance = Math.RandomInt(0,100);
			if (chance <= damage)
			{
				createBleedingSource = true;
			}
		}
		else if (damage > (dmg_max * (1 - bleed_threshold)) )
		{
			createBleedingSource = true;
		}
		
		if (createBleedingSource)
		{
			#ifdef DEVELOPER
			if (LogManager.IsBleedingChancesLogEnable())
			{	
				Debug.BleedingChancesLog("true", "BleedingSourcesManagerServer" , "n/a", "Attempting to create bleeding source");
			}
			#endif
			AttemptAddBleedingSource(component);
		}
	}
}