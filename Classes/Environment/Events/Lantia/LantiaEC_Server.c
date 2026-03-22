class LantiaEC_Server
{
    protected NamEventManager               m_EventManager;
    protected int                           m_LantiaState = ELantia.DORMANT;
    static int                              m_LantiaFinalState = ELantia.WINDOW_END;
    static int                              m_LantiaMode = ELantiaMode.INACTIVE;
    protected int                           m_LantiaModePrev = ELantiaMode.INACTIVE;
    protected bool                          m_LantiaAlive = false;
    //                                                                   12/*0*/, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 60, 60
    protected int                           m_LantiaWindowStages[13] = { 120, 5, 30, 60, 30, 16, 4, 40, 62, 4, 120, 57, 60 };
    protected float                         m_Tick_BigSlice = 0.0;
    protected float                         m_Tick_Slice = 0.0;
    protected vector                        m_UnstableTeleportPosition; // only for unstable Lantia wh

    protected LantiaLavaTrigger             m_LavaTriggers[3];
    protected TeleportToLantiaTrigger       m_TriggerTeleport_To;
    protected TeleportFromLantiaTrigger     m_TriggerTeleport_From;

    static const ref array<vector> TELEPORT_LANTIA = {
        /*Vector( 11066.42, 1004.24, 1611.41 ),
        Vector( 11077.03, 1004.28, 1595.51 ),
        Vector( 11113.65, 1004.22, 1633.86 ),
        Vector( 11096.98, 1004.29, 1636.06 ),
        Vector( 11077.31, 1004.26, 1637.29 ),
        Vector( 11065.37, 1004.24, 1625.30 ),*/
        /*Vector( 11079.40, 1009.74, 1617.10 ),
        Vector( 11081.90, 1009.74, 1627.60 ),
        Vector( 11084.65, 1009.83, 1619.60 ),
        Vector( 11080.52, 1004.80, 1619.50 ),
        Vector( 11079.77, 1004.83, 1626.10 ),
        Vector( 11075.76, 1004.84, 1621.30 ),
        Vector( 11083.12, 1004.84, 1625.50 ),
        Vector( 11075.32, 1003.72, 1619.02 ),
        Vector( 11082.11, 1003.74, 1627.48 ),*/
        Vector( 11077.59, 1005.00, 1621.80 ),
        Vector( 11081.43, 1005.00, 1624.73 ),
        Vector( 11083.48, 1005.00, 1622.30 ),
        Vector( 11080.60, 1005.00, 1620.47 ),
        Vector( 11077.75, 1005.00, 1620.06 ),
        Vector( 11074.51, 1005.00, 1621.27 ),
        Vector( 11077.32, 1005.00, 1627.47 ),
        Vector( 11081.71, 1005.00, 1627.34 ),
        Vector( 11081.57, 1005.00, 1630.04 ),
        Vector( 11083.19, 1005.00, 1617.70 ),
    };

    void LantiaEC_Server( NamEventManager event_manager )
    {
        m_EventManager = event_manager;

        // Lantia ENVIRO EFFECTS - STATIC
        InitStaticLogic();

        InitDynamicLogic();
    }
    void ~LantiaEC_Server()
    {
        for ( int i = 0; i < 3; i++ )
        {
            if ( m_LavaTriggers[i] )
            {
                m_LavaTriggers[i].Delete();
            }
        }

        if ( m_TriggerTeleport_To )
        {
            m_TriggerTeleport_To.Delete();
        }
        if ( m_TriggerTeleport_From )
        {
            m_TriggerTeleport_From.Delete();
        }
    }

    protected void InitStaticLogic()
    {
        m_LavaTriggers[0] = LantiaLavaTrigger.Cast( GetGame().CreateObject( "LantiaLavaTrigger", Vector( 11001.74, 995.13, 1613.79 ), false, false, true ) );
        m_LavaTriggers[0].SetExtents( Vector( -80, -4, -30), Vector( 0, 0, 0) );
        m_LavaTriggers[1] = LantiaLavaTrigger.Cast( GetGame().CreateObject( "LantiaLavaTrigger", Vector( 10921.75, 995.17, 1683.82 ), false, false, true ) );
        m_LavaTriggers[1].SetExtents( Vector( -20, -4, -100), Vector( 0, 0, 0) );
        m_LavaTriggers[2] = LantiaLavaTrigger.Cast( GetGame().CreateObject( "LantiaLavaTrigger", Vector( 11041.75, 995.17, 1691.25 ), false, false, true ) );
        m_LavaTriggers[2].SetExtents( Vector( -120, -4, -30 ), Vector( 0, 0, 0 ) );
    }
    protected void InitDynamicLogic()
    {
        m_UnstableTeleportPosition = GetRandomTeleportPosition();
    }
    protected void CreateTeleportTriggers()
    {
        m_TriggerTeleport_To = TeleportToLantiaTrigger.Cast( GetGame().CreateObject( "TeleportToLantiaTrigger", Vector( 3596.12, 171.349, 6687.33 ), false, false, true ) );
        m_TriggerTeleport_From = TeleportFromLantiaTrigger.Cast( GetGame().CreateObject( "TeleportFromLantiaTrigger", Vector( 11080.20, 1008.47, 1622.61 ), false, false, true ) );
        m_TriggerTeleport_To.SetExtents( Vector( -2, -2, -2), Vector( 2, 2, 2) );
        m_TriggerTeleport_From.SetExtents( Vector( -2.5, -0.5, -2.5), Vector( 2.5, 4.0, 2.5) );
    }
    protected void DeleteTeleportTriggers()
    {
        if ( m_TriggerTeleport_To )
        {
            m_TriggerTeleport_To.Delete();
        }
        if ( m_TriggerTeleport_From )
        {
            m_TriggerTeleport_From.Delete();
        }
    }

    void Update( float timeslice )
    {
        if ( m_Tick_BigSlice > 1.0 )
        {
            if ( m_LantiaMode != m_LantiaModePrev )
            {
                m_LantiaModePrev = m_LantiaMode;
                if ( m_LantiaMode != ELantiaMode.INACTIVE)
                {
                    m_LantiaAlive = true;
                    Print( "[Lantia] ACTIVE" );
					
					m_LantiaState += 1;
					StateChange( m_LantiaState );

                    Param1<int> lantiaMode = new Param1<int>( m_LantiaMode );
                    GetGame().RPCSingleParam( null, ERPCsLantiaLink.NAM_RPC_LANTIA_MODE_UPDATE, lantiaMode, true );
                }
            }

            if ( m_LantiaAlive )
            {
                LantiaStateUpdate();
            }
            m_Tick_BigSlice = 0.0;
        }
        m_Tick_BigSlice += timeslice;
    }

    protected void LantiaStateUpdate()
    {
        m_Tick_Slice++;
        //Print( "Waiting" );
        //Print( m_Tick_Slice );
		
        if ( m_Tick_Slice > m_LantiaWindowStages[ m_LantiaState - 1 ] )
        {
			m_LantiaState += 1;
			StateChange( m_LantiaState );
            m_Tick_Slice = 0.0;
        }
        if ( m_LantiaState >= m_LantiaFinalState )
        {
            m_LantiaAlive = false;
            m_LantiaMode = ELantiaMode.INACTIVE;
			StateChange( ELantia.DORMANT );
            m_Tick_Slice = 0.0;
			Print("[Lantia] INACTIVE");
        }
    }

    void SendLantiaState( PlayerBase player )
    {
        Param1<int> lantiaState = new Param1<int>( m_LantiaState );
        GetGame().RPCSingleParam( null, ERPCsLantiaLink.NAM_RPC_LANTIA_STATE_UPDATE, lantiaState, true );

        Param1<int> lantiaMode = new Param1<int>( m_LantiaMode );
        GetGame().RPCSingleParam( null, ERPCsLantiaLink.NAM_RPC_LANTIA_MODE_UPDATE, lantiaMode, true );
    }

	protected void StateChange( int state )
	{
		Print( string.Format( "[Lantia] State changed to %1", m_LantiaState ) );
	    m_LantiaState = state;
		OnLantiaStateChangedServer( state );

		Param1<int> lantiaState = new Param1<int>( state );
		GetGame().RPCSingleParam( null, ERPCsLantiaLink.NAM_RPC_LANTIA_STATE_UPDATE, lantiaState, true );
	}
	
    protected void OnLantiaStateChangedServer( int state )
    {
        switch ( state )
        {
            case ELantia.DORMANT:
            {
                if ( GetA3Chamber() )
                {
                    GetA3Chamber().SetBuildingState( AthenaBuildingState.INACTIVE );
                }
                break;
            }
            case ELantia.A3INIT:
            {
                PlayLEHSWarningSignalToAll( "A3 startup", COLOR_GREEN );

                if ( GetA3Chamber() )
                {
                    GetA3Chamber().SetBuildingState( AthenaBuildingState.STARTUP );
                }
                break;
            }
            case ELantia.A3ACTIVE:
            {
                PlayLEHSWarningSignalToAll( "A3 FLAX charged", COLOR_GREEN );

                CreateTeleportTriggers();
                if ( GetA3Chamber() )
                {
                    GetA3Chamber().SetBuildingState( AthenaBuildingState.ACTIVE );
                }
                break;
            }
            case ELantia.WINDOW_OPEN:
            {
                PlayLEHSWarningSignalToAll( "Lantia link initiated", COLOR_GREEN );
                break;
            }
            case ELantia.PROBES_DETECT:
            {
                PlayLEHSWarningSignalToAll( "LOP activity", COLOR_YELLOW );
                break;
            }
            case ELantia.LINK_WEAKENING:
            {
                PlayLEHSWarningSignalToAll( "Link stabilized", COLOR_YELLOW );
                break;
            }
            case ELantia.LINK_UNSTABLE:
            {
                PlayLEHSWarningSignalToAll( "Stabilization failed", COLOR_RED );
                break;
            }
            case ELantia.LINK_PREATTACK:
            {
                if ( m_LantiaMode == ELantiaMode.NOEXT )
                {
                    PlayLEHSWarningSignalToAll( "Link lost", COLOR_RED );
                    DeleteTeleportTriggers();
                    if ( GetA3Chamber() )
                    {
                        GetA3Chamber().SetBuildingState( AthenaBuildingState.INACTIVE );
                    }
                }
                else
                {
                    PlayLEHSWarningSignalToAll( "Energy spike detected", COLOR_RED );
                }
                break;
            }
            case ELantia.LINK_ATTACK:
            {
                PlayLEHSWarningSignalToAll( "LOP active", COLOR_RED );
                break;
            }
            case ELantia.LINK_PHASING:
            {
                if ( m_LantiaMode == ELantiaMode.EXT )
                {
                    PlayLEHSWarningSignalToAll( "Link lost", COLOR_RED );
                    DeleteTeleportTriggers();
                    if ( GetA3Chamber() )
                    {
                        GetA3Chamber().SetBuildingState( AthenaBuildingState.INACTIVE );
                    }
                }
                else
                {
                    PlayLEHSWarningSignalToAll( "A3 FLAX phasing active", COLOR_YELLOW );
                }
                break;
            }
            case ELantia.EARTHQUAKE:
            {
                PlayLEHSWarningSignalToAll( "Tremors detected", COLOR_RED );
                break;
            }
            case ELantia.SANDSTORM_LIGHT:
            {
                PlayLEHSWarningSignalToAll( "C3 Sandstorm inbound", COLOR_YELLOW );
                break;
            }
            case ELantia.SANDSTORM_HEAVY:
            {
                PlayLEHSWarningSignalToAll( "C3 Sandstorm in effect", COLOR_RED );
                break;
            }
            case ELantia.WINDOW_END:
            {
                PlayLEHSWarningSignalToAll( "Link lost", COLOR_RED );
                DeleteTeleportTriggers();
                if ( GetA3Chamber() )
                {
                    GetA3Chamber().SetBuildingState( AthenaBuildingState.INACTIVE );
                }
                break;
            }
            case ELantia.UNDEFINED:
            {
                break;
            }
        }
    }

    int GetLantiaStateServer()
    {
        return m_LantiaState;
    }
    int GetLantiaModeServer()
    {
        return m_LantiaMode;
    }

    bool IsSafezonePresent()
    {
        if ( m_LantiaMode == ELantiaMode.NOEXT )
        {
            if ( ( m_LantiaState >= ELantia.WINDOW_OPEN ) && ( m_LantiaState < ELantia.LINK_PREATTACK ) )
                return true;
            else
                return false;
        }
        if ( m_LantiaMode == ELantiaMode.EXT )
        {
            if ( ( m_LantiaState >= ELantia.WINDOW_OPEN ) && ( m_LantiaState < ELantia.LINK_PHASING ) )
                return true;
            else
                return false;
        }
        if ( m_LantiaMode == ELantiaMode.PHASED )
        {
            if ( ( m_LantiaState >= ELantia.WINDOW_OPEN ) && ( m_LantiaState < ELantia.WINDOW_END ) )
                return true;
            else
                return false;
        }

        return false;
    }

    // eeewww, but ITS OK, alright?
    static void OpenLantiaLink( ELantiaMode mode )
    {
        m_LantiaMode = mode;
        switch ( m_LantiaMode )
        {
            case ELantiaMode.NOEXT:
                m_LantiaFinalState = ELantia.LINK_PREATTACK;
                break;
            case ELantiaMode.EXT:
                m_LantiaFinalState = ELantia.LINK_PHASING;
                break;
            case ELantiaMode.PHASED:
                m_LantiaFinalState = ELantia.UNDEFINED;
                break;
        }
    }
    void PlayEnvironmentSoundServer(BlowoutSound sound, vector position, float volume = 1)
    {
        GetGame().RPCSingleParam( null, ERPCsBlowoutStorm.BLOWOUT_RPC_PLAY_SOUND_ENVIRONMENT, new Param3<BlowoutSound, vector, float>( sound, position, volume ), true );
    }
    protected vector GetLantiaTeleportPosition()
    {
        return TELEPORT_LANTIA.GetRandomElement();
    }
    protected land_a3_chamber GetA3Chamber()
    {
        land_a3_chamber chamber;
        EnScript.GetClassVar( GetGame().GetMission(), "m_LandA3Chamber", 0, chamber );
        return chamber;
    }
    protected vector GetRandomTeleportPosition( float x = 5467, float z = 8660, float radius = 5250 )
    {               
        vector position;
        position[0] = Math.RandomInt(x - radius, x + radius);
        position[2] = Math.RandomInt(z - radius, z + radius);
        position[1] = GetGame().SurfaceY(position[0], position[2]) + 1;
        
        if (GetGame().SurfaceIsSea(position[0], position[2]) || GetGame().SurfaceIsPond(position[0], position[2]))
        {
            // try again
            return GetRandomTeleportPosition(x, z, radius); 
        }
        
        array<Object> position_objects = {};
        array<CargoBase> position_cargos = {};
        GetGame().GetObjectsAtPosition(position, 6, position_objects, position_cargos);
        if (position_objects.Count() > 0)
        {
            // try again
            return GetRandomTeleportPosition(x, z, radius);
        }
        
        return position;
    }
    protected void TeleportSafely( Object obj, vector position )
    {    
        PlayerBase player = PlayerBase.Cast( obj );
        if ( player )
        {
            player.SetAllowDamage( false );
            GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( TeleportDamageHandler, 2000, false, player );

            ScriptJunctureData pCtx = new ScriptJunctureData;
            pCtx.Write( position );
            player.SendSyncJuncture( 67, pCtx );
        }
    }
    protected void TeleportDamageHandler( PlayerBase player )
    {
        player.SetAllowDamage( true );

        bool lehs = player.IsWearingLEHS() && player.IsWearingLEHSHelmet();

        if ( m_LantiaState <= ELantia.PROBES_DETECT )
        {
            if ( !lehs )
                player.AddHealth( "", "Shock", -42.0 );
            else
                player.AddHealth( "", "Shock", -22.0 );
        }
        else
        {
            if ( m_LantiaState >= ELantia.LINK_ATTACK )
            {
                if ( !lehs )
                {
                    player.AddHealth( "", "Health", -50.0 );
                    player.AddHealth( "", "Shock", -100.0 );
                }
                else
                {
                    player.AddHealth( "", "Shock", -50.0 );
                }
            }
            else 
            {
                if ( !lehs )
                    player.AddHealth( "", "Shock", -65.0 );
                else
                    player.AddHealth( "", "Shock", -35.0 );
            }
        }
    }
    void TeleportPlayer( PlayerBase player, int mode )
    {
        string lehs_message_content = "ERROR";
        int lehs_message_colour = COLOR_YELLOW;

        switch ( mode )
        {
            case 0:
            {
                // a3 -> lantia
                lehs_message_content = "Connected to LNT-KEYHOLE";
                lehs_message_colour = COLOR_GREEN;
                vector destination0 = GetLantiaTeleportPosition();
                TeleportSafely( player, destination0 );
                PlayEnvironmentSoundServer( BlowoutSound.Blowout_Teleport, destination0 );
                break;
            }
            case 1:
            {
                // lantia -> a3
                lehs_message_content = "Connected to A3-SYS-4";
                lehs_message_colour = COLOR_GREEN;
                vector destination1 = GetA3Chamber().GetTeleportPosition();
                TeleportSafely( player, destination1 );
                PlayEnvironmentSoundServer( BlowoutSound.Blowout_Teleport, destination1 );
                break;
            }
            case 2:
            {
                // lantia -> random Namalsk
                lehs_message_content = "Link precision lost";
                lehs_message_colour = COLOR_YELLOW;
                TeleportSafely( player, m_UnstableTeleportPosition );
                PlayEnvironmentSoundServer( BlowoutSound.Blowout_Teleport, m_UnstableTeleportPosition );
                break;
            }
            case 3:
            {
                // lantia -> random Namalsk, damage dealt
                lehs_message_content = "Link unstable";
                lehs_message_colour = COLOR_RED;
                TeleportSafely( player, m_UnstableTeleportPosition );
                PlayEnvironmentSoundServer( BlowoutSound.Blowout_Teleport, m_UnstableTeleportPosition );
                break;
            }
            default:
            {
                lehs_message_content = "ERROR";
                lehs_message_colour = COLOR_YELLOW;
                break;
            }
        }
        GetGame().RPCSingleParam( null, ERPCsLantiaLink.NAM_RPC_LANTIA_LINK_TELEPORT, null, true, player.GetIdentity() );

        PlayLEHSWarningSignalToSpecific( lehs_message_content, lehs_message_colour, player );
    }

    void PlayLEHSWarningSignalToAll( string text, int color )
    {
        array<Man> players = {};
        GetGame().GetPlayers( players );
        foreach ( Man player: players )
        {
            PlayLEHSWarningSignalToSpecific( text, color, PlayerBase.Cast( player ) );
        }
    }
    void PlayLEHSWarningSignalToSpecific( string text, int color, PlayerBase receiver )
    {
        if ( receiver.IsWearingLEHSHelmet() )
        {
            GetGame().RPCSingleParam( receiver.GetLEHSHelmet(), ERPCsLantiaLink.NAM_RPC_LEHS_WARNING, new Param2<string, int>( text, color ), true, receiver.GetIdentity() );
        }
    }
}