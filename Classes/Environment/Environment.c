enum EEnvironmentHeatcomfortBehaviorCategoryNamalsk: EEnvironmentHeatcomfortBehaviorCategory
{
	LEHS,
}

modded class Environment
{
	protected float m_PlayerColdResist;

	override void Init(PlayerBase pPlayer)
	{
		super.Init(pPlayer);

		m_PlayerColdResist = 0.0;
	}

	override protected void CollectAndSetPlayerData()
	{
		super.CollectAndSetPlayerData();

		// cold resistance stuff
		m_PlayerColdResist = m_Player.GetStatColdResistance().Get();
		if ((m_HeatComfort < 0) && (m_Player.GetStatColdResistance().Get() < 100000) )
		{
			// heatcomfort is below zero and cold resistance has not yet been maxed
			m_Player.GetStatColdResistance().Add((Math.AbsFloat(m_HeatComfort) * 0.00015) * 100000);
			m_Player.UpdateColdResistance();
		}

		// frostbite heatcomfort sync to client (ui)
		m_Player.UpdateExactHeatComfort();
	}

	bool IsSafeFromEVR()
	{
		return ( IsInsideBuilding() || IsInsideVehicle() );
	}

	// 126 overrides
	override void ProcessHeatComfort()
	{
		float hcPenaltyTotal //! Heat Comfort Penalty
		
		// NEW body parts => splitted
		float hcBodyPartTotal, hcBodyPart;
		float hBodyPartTotal, hBodyPart;
		
		float heatComfortSum = 0.0;
		float heatItems = 0.0;
		
		#ifdef ENABLE_LOGGING
		LogItemHeat("====================");
		#endif
		if ( m_Player.IsWearingLEHS() )
		{
			hBodyPartTotal = 0.0;
			hcPenaltyTotal = 0.0;
			hcBodyPartTotal = 0.65;

			// if helemet is present, make it even better
			dzn_lehs_helmet helmet = dzn_lehs_helmet.Cast( m_Player.FindAttachmentBySlotName( "Headgear" ) );
			if ( m_Player.IsWearingLEHSHelmet() )
			{
				if ( !helmet.IsVisorUp() )
					hcBodyPartTotal = 0.85;
			}
		}
		else
		{
			BodyPartHeatProperties(InventorySlots.HEADGEAR, GameConstants.ENVIRO_HEATCOMFORT_HEADGEAR_WEIGHT, hcBodyPart, hBodyPart);
			hcBodyPartTotal += hcBodyPart; hBodyPartTotal += hBodyPart;
			BodyPartHeatProperties(InventorySlots.MASK, GameConstants.ENVIRO_HEATCOMFORT_MASK_WEIGHT, hcBodyPart, hBodyPart);
			hcBodyPartTotal += hcBodyPart; hBodyPartTotal += hBodyPart;
			BodyPartHeatProperties(InventorySlots.VEST, GameConstants.ENVIRO_HEATCOMFORT_VEST_WEIGHT, hcBodyPart, hBodyPart);
			hcBodyPartTotal += hcBodyPart; hBodyPartTotal += hBodyPart;
			BodyPartHeatProperties(InventorySlots.BODY, GameConstants.ENVIRO_HEATCOMFORT_BODY_WEIGHT, hcBodyPart, hBodyPart);
			hcBodyPartTotal += hcBodyPart; hBodyPartTotal += hBodyPart;
			BodyPartHeatProperties(InventorySlots.BACK, GameConstants.ENVIRO_HEATCOMFORT_BACK_WEIGHT, hcBodyPart, hBodyPart);
			hcBodyPartTotal += hcBodyPart; hBodyPartTotal += hBodyPart;
			BodyPartHeatProperties(InventorySlots.GLOVES, GameConstants.ENVIRO_HEATCOMFORT_GLOVES_WEIGHT, hcBodyPart, hBodyPart);
			hcBodyPartTotal += hcBodyPart; hBodyPartTotal += hBodyPart;
			BodyPartHeatProperties(InventorySlots.LEGS, GameConstants.ENVIRO_HEATCOMFORT_LEGS_WEIGHT, hcBodyPart, hBodyPart);
			hcBodyPartTotal += hcBodyPart; hBodyPartTotal += hBodyPart;
			BodyPartHeatProperties(InventorySlots.FEET, GameConstants.ENVIRO_HEATCOMFORT_FEET_WEIGHT, hcBodyPart, hBodyPart);
			hcBodyPartTotal += hcBodyPart; hBodyPartTotal += hBodyPart;
			BodyPartHeatProperties(InventorySlots.HIPS, GameConstants.ENVIRO_HEATCOMFORT_HIPS_WEIGHT, hcBodyPart, hBodyPart);
			hcBodyPartTotal += hcBodyPart; hBodyPartTotal += hBodyPart;
			
			hcPenaltyTotal += NakedBodyPartHeatComfortPenalty(InventorySlots.HEADGEAR, GameConstants.ENVIRO_HEATCOMFORT_HEADGEAR_WEIGHT);
			hcPenaltyTotal += NakedBodyPartHeatComfortPenalty(InventorySlots.MASK, GameConstants.ENVIRO_HEATCOMFORT_MASK_WEIGHT);
			hcPenaltyTotal += NakedBodyPartHeatComfortPenalty(InventorySlots.BODY, GameConstants.ENVIRO_HEATCOMFORT_BODY_WEIGHT);
			hcPenaltyTotal += NakedBodyPartHeatComfortPenalty(InventorySlots.GLOVES, GameConstants.ENVIRO_HEATCOMFORT_GLOVES_WEIGHT);
			hcPenaltyTotal += NakedBodyPartHeatComfortPenalty(InventorySlots.LEGS, GameConstants.ENVIRO_HEATCOMFORT_LEGS_WEIGHT);
			hcPenaltyTotal += NakedBodyPartHeatComfortPenalty(InventorySlots.FEET, GameConstants.ENVIRO_HEATCOMFORT_FEET_WEIGHT);
		}

		heatItems = hBodyPartTotal;
		heatComfortSum = hcBodyPartTotal;
		heatComfortSum += hcPenaltyTotal; //! heatcomfort body parts penalties
		
		//! Stomach temperature influence to heatcomfort		
		{
			if (m_Player.GetStomach().GetStomachVolume() > 0.0)
			{
				float stomachContentTemperature = m_Player.GetStomach().GetStomachTemperature();
				if (stomachContentTemperature < GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_LOWER_LIMIT)
				{
					stomachContentTemperature = Math.Remap(
						-10.0,
						GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_LOWER_LIMIT,
						-GameConstants.ENVIRO_STOMACH_WEIGHT,
						0.0,
						stomachContentTemperature,
					);
				}
				else if (stomachContentTemperature > GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_UPPER_LIMIT)
				{
					stomachContentTemperature = Math.Remap(
						GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_UPPER_LIMIT,
						70.0,
						0.0,
						GameConstants.ENVIRO_STOMACH_WEIGHT,
						stomachContentTemperature,
					);
				}
				else
					stomachContentTemperature = 0.0;

				heatComfortSum += stomachContentTemperature * GameConstants.ENVIRO_STOMACH_WEIGHT;
			}
		}

		float targetHeatComfort = (heatComfortSum + heatItems + (GetPlayerHeat() / 100)) + EnvTempToCoef(m_EnvironmentTemperature);
		
		//! uses the raw targetHeatComfort data
		m_EnvironmentSnapshot.m_ClothingHeatComfort = hcBodyPartTotal;
		m_EnvironmentSnapshot.m_TargetHeatComfort 	= targetHeatComfort;
		ProcessHeatBuffer(m_EnvironmentSnapshot);
		
		if (m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_HEATBUFFER))
			targetHeatComfort = Math.Clamp(targetHeatComfort, 0.0, m_Player.GetStatHeatComfort().GetMax());
 		else
 			targetHeatComfort = Math.Clamp(targetHeatComfort, m_Player.GetStatHeatComfort().GetMin(), m_Player.GetStatHeatComfort().GetMax());

		targetHeatComfort = Math.Min( (Math.Round(targetHeatComfort * 100) * 0.01) + (0.37 * (m_PlayerColdResist * 0.00001)), 0.14 );

		float dynamicHeatComfort;
		
		{
			float direction = 1.0;
			if (targetHeatComfort < 0.0)
				direction = -1.0;
			
			if (Math.AbsFloat(targetHeatComfort - m_HeatComfort) <= GameConstants.ENVIRO_HEATCOMFORT_MAX_STEP_SIZE)
				dynamicHeatComfort = m_AverageHeatComfortBuffer.Add(targetHeatComfort);
			else
				dynamicHeatComfort = m_AverageHeatComfortBuffer.Add((Math.AbsFloat(targetHeatComfort) - GameConstants.ENVIRO_HEATCOMFORT_MAX_STEP_SIZE) * direction);
		}
		
		dynamicHeatComfort = Math.Round(dynamicHeatComfort * 100) * 0.01;

		m_HeatComfort = dynamicHeatComfort;

		SetTargetHeatComfort(targetHeatComfort);
		m_Player.GetStatHeatComfort().Set(m_HeatComfort);
	}

	override void SetHeatcomfortDirectly()
	{
		if (m_HeatComfortBehaviorCategory == EEnvironmentHeatcomfortBehaviorCategoryNamalsk.LEHS)
		{
 			float targetHeatComfort = 0.0;
 			float dynamicHeatComfort = m_AverageHeatComfortBuffer.Add(targetHeatComfort);
 			
 			m_HeatComfort = dynamicHeatComfort;
 
 			SetTargetHeatComfort(0.0);
 			m_Player.GetStatHeatComfort().Set(dynamicHeatComfort);
		}

		super.SetHeatcomfortDirectly();
	}

	override bool DetermineHeatcomfortBehavior()
	{
		if ( m_Player.IsWearingLEHSHelmet() )
		{
			dzn_lehs_helmet helmet = dzn_lehs_helmet.Cast( m_Player.FindAttachmentBySlotName( "Headgear" ) );
			if ( helmet.IsPressurized() )
			{
				m_HeatComfortBehaviorCategory = EEnvironmentHeatcomfortBehaviorCategoryNamalsk.LEHS;
				return true;
			}
		}

		if ( m_Player.GetModifiersManager().IsModifierActive( eModifiersNamalsk.NAM_MDF_AREAEXPOSURE_PHOENIX ) )
		{
			m_HeatComfortBehaviorCategory = EEnvironmentHeatcomfortBehaviorCategoryNamalsk.LEHS;
			return true;
		}

		return super.DetermineHeatcomfortBehavior();
	}
};