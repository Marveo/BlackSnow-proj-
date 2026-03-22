class land_lanteran_probe: Building
{
	protected ref Timer m_BuildingUpdate;
	protected float m_Phase;
	protected bool m_BeamsActive;
	
	void land_lanteran_probe()
	{
		m_BuildingUpdate = new Timer( CALL_CATEGORY_SYSTEM );
		m_Phase = 0.0;
		m_BeamsActive = false;
		m_BuildingUpdate.Run( 0.1, this, "BuildingUpdate", null, true );

		// init
		DeactivateProbes();
		DeactivateBeams();
	}
	void ~land_lanteran_probe()
	{
		if ( m_BuildingUpdate )
		{
			m_BuildingUpdate.Stop();
			delete m_BuildingUpdate;
		}
	}
	
	void SetProbesState( AthenaBuildingState building_state )
	{
		switch ( building_state )
		{
			case AthenaBuildingState.INACTIVE:
			{
				DeactivateProbes();
				DeactivateBeams();
				break;
			}
			case AthenaBuildingState.PROBES_ACTIVE:
			{
				ActivateProbes();
				DeactivateBeams();
				break;
			}
			case AthenaBuildingState.PROBES_BEAMS:
			{
				ActivateBeams();
				ActivateProbes();
				break;
			}
		}
	}

	/*override void OnBuildingStateChangedServer( AthenaBuildingState building_state )
	{			
		switch ( building_state )
		{
			case AthenaBuildingState.INACTIVE:
			{
				DeactivateProbes();
				DeactivateBeams();
				break;
			}
			case AthenaBuildingState.PROBES_ACTIVE:
			{
				ActivateProbes();
				DeactivateBeams();
				break;
			}
			case AthenaBuildingState.PROBES_BEAMS:
			{
				ActivateBeams();
				ActivateProbes();
				break;
			}
		}
	}
	
	override void OnBuildingStateChangedClient( AthenaBuildingState building_state )
	{
		switch ( building_state )
		{
			case AthenaBuildingState.INACTIVE:
			{
				DeactivateBeams();
				break;
			}
			case AthenaBuildingState.PROBES_ACTIVE:
			{
				DeactivateBeams();
				break;
			}
			case AthenaBuildingState.PROBES_BEAMS:
			{
				ActivateBeams();
				break;
			}
		}
	}*/

	void ActivateProbes()
	{
		SetObjectMaterial( GetHiddenSelectionIndex( "probes" ), "nst\\ns3\\structures\\probes\\data\\lanteran_probe_active.rvmat" );
	}
	
	void DeactivateProbes()
	{
		SetObjectMaterial( GetHiddenSelectionIndex( "probes" ), "nst\\ns3\\structures\\probes\\data\\lanteran_probe_inactive.rvmat" );
	}
	
	void ActivateBeams()
	{
		m_BeamsActive = true;
	}
	
	void DeactivateBeams()
	{
		m_BeamsActive = false;
		SetObjectTexture( GetHiddenSelectionIndex( "beam1" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
		SetObjectTexture( GetHiddenSelectionIndex( "beam2" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
		SetObjectTexture( GetHiddenSelectionIndex( "beam3" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
		SetObjectTexture( GetHiddenSelectionIndex( "beam4" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
		SetObjectTexture( GetHiddenSelectionIndex( "beam5" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
		SetObjectTexture( GetHiddenSelectionIndex( "beam6" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
	}
	
	protected void BuildingUpdate()
	{
		// orbit loop of probes
		float phaseCheck = GetAnimationPhase( "ProbeMovement" ) / 10;
		int phaseCheckInt = phaseCheck;	
		if ( ( phaseCheckInt - phaseCheck ) == 0 )
		{
			m_Phase += 10.0;
			SetAnimationPhase( "ProbeMovement", m_Phase );
		}
		
		// visiblity of beams from probes
		if ( m_BeamsActive )
		{
			int whole = GetAnimationPhase( "ProbeMovement" );
			BeamVisibility( GetAnimationPhase( "ProbeMovement" ) - whole );
		}
	}
	
	void BeamVisibility( float zeroToOne )
	{
		/*
		 NAC observation data :-)
		
		 1: 0.88 - 0.18
		 2: 0.95 - 0.25
		 3: 0.08 - 0.41
		 4: 0.33 - 0.68
		 5: 0.42 - 0.78
		 6: 0.57 - 0.93	
		*/
		
		// beam1
		if ( ( zeroToOne >= 0.88 ) || ( zeroToOne <= 0.18 ) )
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam1" ), "#(argb,8,8,3)color(1,1,1,1.0,ca)" );
		}
		else
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam1" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
		}
		
		// beam2
		if ( ( zeroToOne >= 0.98 ) || ( zeroToOne <= 0.25 ) )
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam2" ), "#(argb,8,8,3)color(1,1,1,1.0,ca)" );
		}
		else
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam2" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
		}
		
		// beam3
		if ( ( zeroToOne >= 0.08 ) && ( zeroToOne <= 0.41 ) )
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam3" ), "#(argb,8,8,3)color(1,1,1,1.0,ca)" );
		}
		else
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam3" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
		}
		
		// beam4
		if ( ( zeroToOne >= 0.33 ) && ( zeroToOne <= 0.68 ) )
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam4" ), "#(argb,8,8,3)color(1,1,1,1.0,ca)" );
		}
		else
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam4" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
		}
		
		// beam5
		if ( ( zeroToOne >= 0.42 ) && ( zeroToOne <= 0.78 ) )
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam5" ), "#(argb,8,8,3)color(1,1,1,1.0,ca)" );
		}
		else
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam5" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
		}
		
		// beam6
		if ( ( zeroToOne >= 0.57 ) && ( zeroToOne <= 0.93 ) )
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam6" ), "#(argb,8,8,3)color(1,1,1,1.0,ca)" );
		}
		else
		{
			SetObjectTexture( GetHiddenSelectionIndex( "beam6" ), "#(argb,8,8,3)color(1,1,1,0.0,ca)" );
		}
	}
}