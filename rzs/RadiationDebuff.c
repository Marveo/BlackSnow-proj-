class Radiation_DeBuff_Mdfr: ModifierBase
{
	protected float m_RadLevel;

	static const int WATER_DRAIN_FROM_VOMIT 	= 70;
	static const int ENERGY_DRAIN_FROM_VOMIT 	= 55;

	static const int STAGE1_COUGHCHANCE 	 = 25;

	static const int STAGE2_COUGHCHANCE 	 = 35;
	static const int STAGE2_VOMITCHANCE 	 = 10;
	static const int STAGE2_VOMITDURATION 	 = 2;
	static const int STAGE2_STAMINADEPLETION = 5;

	static const int STAGE3_COUGHCHANCE 	 = 55;
	static const int STAGE3_VOMITCHANCE 	 = 15;
	static const int STAGE3_VOMITDURATION 	 = 3;
	static const int STAGE3_STAMINADEPLETION = 8;
	static const int STAGE3_BLEEDCHANCE		 = 10;

	static const int STAGE4_COUGHCHANCE 	 = 65;
	static const int STAGE4_VOMITCHANCE 	 = 20;
	static const int STAGE4_VOMITDURATION 	 = 5;
	static const int STAGE4_STAMINADEPLETION = 15;
	static const int STAGE4_BLEEDCHANCE		 = 20;
	static const int STAGE4_UNCONCHANCE 	 = 5;

	bool m_Stage3SymptomActive;
	bool m_Stage4SymptomActive;
		
	override void Init()
	{
		m_TrackActivatedTime 	= false;
		m_ID 					= RadServerModifiers.MDF_RADIATION_DEBUFF;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		// Активується завжди, система агентів сама контролює рівень впливу
		return true;
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}
	
	override void OnReconnect(PlayerBase player) {}

	override void OnTick(PlayerBase player, float deltaT)
	{
		if (!player.IsAlive())
			return;

		// Отримуємо поточну кількість агентів радіації
		m_RadLevel = player.GetSingleAgentCount(RadZoneAgents.RADIATION);
		if (m_RadLevel <= 0)
			return;

		int coughChance;
		int vomitChance;
		int unconsciousChance;
		float currentStamina;
		SymptomBase vomitsymptom;
		int bleedingChance;
		int randNum;
		float water_loss;

		// === СТАДІЯ 1 ===
		if (m_RadLevel < 25)
		{
			if (m_Stage3SymptomActive)
			{
				player.GetSymptomManager().RemoveSecondarySymptom(RadDebuffSymptomIDs.SYMPTOM_RAD3);
				m_Stage3SymptomActive = false;
			}
			if (m_Stage4SymptomActive)
			{
				player.GetSymptomManager().RemoveSecondarySymptom(RadDebuffSymptomIDs.SYMPTOM_RAD4);
				m_Stage4SymptomActive = false;
			}
			return;
		}
		// === СТАДІЯ 2 ===
		else if (m_RadLevel < 150)
		{
			coughChance = Math.RandomIntInclusive(0, 100);
			if (coughChance <= STAGE1_COUGHCHANCE)
				player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);

			if (m_Stage3SymptomActive)
			{
				player.GetSymptomManager().RemoveSecondarySymptom(RadDebuffSymptomIDs.SYMPTOM_RAD3);
				m_Stage3SymptomActive = false;
			}
			if (m_Stage4SymptomActive)
			{
				player.GetSymptomManager().RemoveSecondarySymptom(RadDebuffSymptomIDs.SYMPTOM_RAD4);
				m_Stage4SymptomActive = false;
			}
		}
		// === СТАДІЯ 3 ===
		else if (m_RadLevel < 200)
		{
			coughChance = Math.RandomIntInclusive(0, 100);
			if (coughChance <= STAGE2_COUGHCHANCE)
				player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);

			vomitChance = Math.RandomIntInclusive(0, 100);
			if (vomitChance <= STAGE2_VOMITCHANCE)
			{
				vomitsymptom = player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
				if (vomitsymptom)
				{
					vomitsymptom.SetDuration(STAGE2_VOMITDURATION);
					if (player.GetStatWater().Get() > WATER_DRAIN_FROM_VOMIT)
						player.GetStatWater().Add(-WATER_DRAIN_FROM_VOMIT);
					if (player.GetStatEnergy().Get() > ENERGY_DRAIN_FROM_VOMIT)
						player.GetStatEnergy().Add(-ENERGY_DRAIN_FROM_VOMIT);
				}
			}

			currentStamina = player.GetStatStamina().Get(); 
			player.GetStaminaHandler().SetStamina(currentStamina - (STAGE2_STAMINADEPLETION * deltaT));

			water_loss = deltaT * PlayerConstants.WATER_LOSS_FEVER;
			player.GetStatWater().Add(-water_loss);

			if (m_Stage3SymptomActive)
			{
				player.GetSymptomManager().RemoveSecondarySymptom(RadDebuffSymptomIDs.SYMPTOM_RAD3);
				m_Stage3SymptomActive = false;
			}
			if (m_Stage4SymptomActive)
			{
				player.GetSymptomManager().RemoveSecondarySymptom(RadDebuffSymptomIDs.SYMPTOM_RAD4);
				m_Stage4SymptomActive = false;
			}
		}
		// === СТАДІЯ 4 ===
		else if (m_RadLevel < 500)
		{
			coughChance = Math.RandomIntInclusive(0, 100);
			if (coughChance <= STAGE3_COUGHCHANCE)
				player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);

			vomitChance = Math.RandomIntInclusive(0, 100);
			if (vomitChance <= STAGE3_VOMITCHANCE)
			{
				vomitsymptom = player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
				if (vomitsymptom)
				{
					vomitsymptom.SetDuration(STAGE3_VOMITDURATION);
					if (player.GetStatWater().Get() > WATER_DRAIN_FROM_VOMIT)
						player.GetStatWater().Add(-WATER_DRAIN_FROM_VOMIT);
					if (player.GetStatEnergy().Get() > ENERGY_DRAIN_FROM_VOMIT)
						player.GetStatEnergy().Add(-ENERGY_DRAIN_FROM_VOMIT);
				}
			}

			currentStamina = player.GetStatStamina().Get();
			player.GetStaminaHandler().SetStamina(currentStamina - (STAGE3_STAMINADEPLETION * deltaT));

			bleedingChance = Math.RandomIntInclusive(0, 100);
			if (bleedingChance <= STAGE3_BLEEDCHANCE)
			{
				if (Math.RandomFloat01() < 0.5)
					player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesLow.GetRandomElement());
				else
					player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesUp.GetRandomElement());
			}

			water_loss = deltaT * PlayerConstants.WATER_LOSS_FEVER;
			player.GetStatWater().Add(-water_loss);

			if (m_Stage4SymptomActive)
			{
				player.GetSymptomManager().RemoveSecondarySymptom(RadDebuffSymptomIDs.SYMPTOM_RAD4);
				m_Stage4SymptomActive = false;
			}

			if (!m_Stage3SymptomActive)
			{
				player.GetSymptomManager().QueueUpSecondarySymptom(RadDebuffSymptomIDs.SYMPTOM_RAD3);
				m_Stage3SymptomActive = true;
			}
		}
		// === СТАДІЯ 5 ===
		else
		{
			coughChance = Math.RandomIntInclusive(0, 100);
			if (coughChance <= STAGE4_COUGHCHANCE)
				player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);

			vomitChance = Math.RandomIntInclusive(0, 100);
			if (vomitChance <= STAGE4_VOMITCHANCE)
			{
				vomitsymptom = player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
				if (vomitsymptom)
				{
					vomitsymptom.SetDuration(STAGE4_VOMITDURATION);
					if (player.GetStatWater().Get() > WATER_DRAIN_FROM_VOMIT)
						player.GetStatWater().Add(-WATER_DRAIN_FROM_VOMIT);
					if (player.GetStatEnergy().Get() > ENERGY_DRAIN_FROM_VOMIT)
						player.GetStatEnergy().Add(-ENERGY_DRAIN_FROM_VOMIT);
				}
			}

			currentStamina = player.GetStatStamina().Get();
			player.GetStaminaHandler().SetStamina(currentStamina - (STAGE4_STAMINADEPLETION * deltaT));

			unconsciousChance = Math.RandomIntInclusive(0, 100);
			if (unconsciousChance <= STAGE4_UNCONCHANCE)
				player.SetHealth("GlobalHealth", "Shock", 0);

			bleedingChance = Math.RandomIntInclusive(0, 100);
			if (bleedingChance <= STAGE4_BLEEDCHANCE)
			{
				if (Math.RandomFloat01() < 0.5)
					player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesLow.GetRandomElement());
				else
					player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesUp.GetRandomElement());
			}

			water_loss = deltaT * PlayerConstants.WATER_LOSS_FEVER;
			player.GetStatWater().Add(-water_loss);

			if (m_Stage3SymptomActive)
			{
				player.GetSymptomManager().RemoveSecondarySymptom(RadDebuffSymptomIDs.SYMPTOM_RAD3);
				m_Stage3SymptomActive = false;
			}

			if (!m_Stage4SymptomActive)
			{
				player.GetSymptomManager().QueueUpSecondarySymptom(RadDebuffSymptomIDs.SYMPTOM_RAD4);
				m_Stage4SymptomActive = true;
			}
		}
	}
}
