class Frostbite extends ModifierBase
{
	protected const int 			m_SlotCount = 6;
	protected int 					m_SlotsOfInterest[m_SlotCount] = { InventorySlots.FEET, InventorySlots.GLOVES, InventorySlots.MASK, InventorySlots.HEADGEAR, InventorySlots.BODY, InventorySlots.LEGS };
	protected float 				m_FrostbiteTimers[m_SlotCount] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	protected float 				m_SlotImportance[m_SlotCount] = { 20.0, 20.0, 20.0, 14.0, 8.0, 8.0 };
	protected int 					m_LastFrostbiteValue = 0;

	override void Init()
	{
		m_TrackActivatedTime 	= false;
		m_ID 					= eModifiersNamalsk.NAM_MDF_FROSTBITE;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	override bool ActivateCondition( PlayerBase player )
	{
		return true;
	}

	override void OnActivate( PlayerBase player )
	{
	}

	override void OnReconnect( PlayerBase player )
	{

	}
	
	override bool DeactivateCondition( PlayerBase player )
	{
		return false;
	}

	override void OnTick( PlayerBase player, float deltaT )
	{	
		float currHeatComf = player.GetStatHeatComfort().Get();
		bool isFrostiteApplicable = false;
		if ( ( currHeatComf < PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING ) && !( player.IsWearingLEHS() ) )
		{
			isFrostiteApplicable = true;
		}
		float frostbite_timer = ( /*0.1675*/0.01675 * currHeatComf );

		for ( int i = 0; i < m_SlotCount; i++ )
		{
			// check if the slot of interest does not have frostbite already
			if ( m_FrostbiteTimers[i] > -1.0 )
			{
				// lets check if all is good for this slot and the player character is in lower heat comfort
				if ( !CheckSlotOfInterest( player, m_SlotsOfInterest[i] ) && isFrostiteApplicable )
				{
					m_FrostbiteTimers[i] = m_FrostbiteTimers[i] + ( frostbite_timer * ( ( m_SlotImportance[i] ) / 100.0 ) );
					if ( m_FrostbiteTimers[i] <= -1.0 )
					{
						// timer has reached target value of -1, apply frostbite
						ApplyFrostbite( player, m_SlotsOfInterest[i] );
					}
				} else {
					// does it have timer != 0.0?
					if ( m_FrostbiteTimers[i] < 0.0 )
					{
						// lower it slowly to 0 again
						m_FrostbiteTimers[i] = m_FrostbiteTimers[i] - ( frostbite_timer * ( ( m_SlotImportance[i] ) / 100.0 ) );
					}
					else
					{
						// frostbite risk is gone
					}
				}
			}
			else
			{
				// slot can be ignored, frostbite has been already applied
			}
		}

		if ( m_LastFrostbiteValue != player.GetStatFrostbiteZones().Get() )
		{
			player.ForceUpdateFrostbiteZones();
			player.IncreaseDiseaseCount();
			m_LastFrostbiteValue = player.GetStatFrostbiteZones().Get();
		}

		//string fb_debug = " " + m_FrostbiteTimers[0].ToString() + " " + m_FrostbiteTimers[1].ToString() + " " + m_FrostbiteTimers[2].ToString() + " " + m_FrostbiteTimers[3].ToString() + " " + m_FrostbiteTimers[4].ToString() + " " + m_FrostbiteTimers[5].ToString();
		//Print( fb_debug );
	}

	protected bool CheckSlotOfInterest(PlayerBase player, int slotID)
	{
		// true = item is present and not damaged
		// false = item is either not present, ruined or wet enough
		EntityAI att = player.GetInventory().FindAttachment( slotID );
		if ( att )
		{
			ItemBase item = ItemBase.Cast(att);
			if ( ( item.GetHealth() <= 0 ) || ( item.GetWet() >= 0.25 ) )
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	protected void ApplyFrostbite( PlayerBase player, int slotID )
	{
		int currentZones = player.GetStatFrostbiteZones().Get();
		switch ( slotID )
		{
			case InventorySlots.FEET:
			{
				player.GetStatFrostbiteZones().Set( currentZones | 1 );
				player.GetStatFrostbiteEffect().Add( ( 100 - player.GetStatFrostbiteEffect().Get() ) * ( m_SlotImportance[0] / 100.0 ) );
				break;
			}
			case InventorySlots.GLOVES:
			{
				player.GetStatFrostbiteZones().Set( currentZones | 2 );
				player.GetStatFrostbiteEffect().Add( ( 100 - player.GetStatFrostbiteEffect().Get() ) * ( m_SlotImportance[1] / 100.0 ) );
				break;
			}
			case InventorySlots.MASK:
			{
				player.GetStatFrostbiteZones().Set( currentZones | 4 );
				player.GetStatFrostbiteEffect().Add( ( 100 - player.GetStatFrostbiteEffect().Get() ) * ( m_SlotImportance[2] / 100.0 ) );
				break;
			}
			case InventorySlots.HEADGEAR:
			{
				player.GetStatFrostbiteZones().Set( currentZones | 8 );
				player.GetStatFrostbiteEffect().Add( ( 100 - player.GetStatFrostbiteEffect().Get() ) * ( m_SlotImportance[3] / 100.0 ) );
				break;
			}
			case InventorySlots.BODY:
			{
				player.GetStatFrostbiteZones().Set( currentZones | 16 );
				player.GetStatFrostbiteEffect().Add( ( 100 - player.GetStatFrostbiteEffect().Get() ) * ( m_SlotImportance[4] / 100.0 ) );
				break;
			}
			case InventorySlots.LEGS:
			{
				player.GetStatFrostbiteZones().Set( currentZones | 32 );
				player.GetStatFrostbiteEffect().Add( ( 100 - player.GetStatFrostbiteEffect().Get() ) * ( m_SlotImportance[5] / 100.0 ) );
				break;
			}
		}
	}
};