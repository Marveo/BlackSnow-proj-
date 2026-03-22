class LantiaEC_Client
{
    protected NamEventManager       m_EventManager;
    protected int                   m_LantiaState = ELantia.DORMANT;
    protected int                   m_LantiaMode = ELantiaMode.INACTIVE;
    protected PlayerBase            m_Player;
    protected float                 m_Tick_BigSlice = 0.0;

    // Lantia ENVIRO EFFECTS - STATIC
    protected bool                  m_EffectsInit = false;
    protected EffectSound           m_Xerxes_Sound;
    protected vector                m_XerxesPos;
    protected const int             m_FogCount = 25;
    protected const int             m_TubeLightCount = 93;
    protected GenericVFX            m_Fog[m_FogCount];
    protected GenericVFX            m_FogPortalAtm;
    protected LantiaTubeLight       m_TubeLights[m_TubeLightCount];
    protected vector                m_FogPos[m_FogCount];
    protected vector                m_TubeLightsPos[m_TubeLightCount];
    protected LantiaCageLight       m_CageLight;
    protected vector                m_CagePos;
    protected vector                m_ProbesPos;
    protected vector                m_ProbesRot;
    protected land_lanteran_probe   m_Probes;

    // Lantia ENVIRO EFFECTS - DYNAMIC
    protected vfx_the_thing         m_WH_Keyhole;
    protected vector                m_WH_KeyholePos;
    protected ScriptedLightBase     m_WH_Light;
    protected Particle              m_WH_StartupParticle;
    protected vector                m_OrbitPos;
    protected EffectSound           m_WH_Phasing_Sound;
    protected vector                m_spires1[6];
    protected vector                m_spires2[5];
    protected vector                m_spires3[30];
    protected vector                m_spires4[22];
    protected vector                m_sandstorm_outer[23];
    protected GenericVFX            m_SandstormOuter[23];
    protected vector                m_sandstorm_inner[18];
    protected GenericVFX            m_SandstormInner[18];
    protected vector                m_lightning[13];
    protected ref Timer             m_WormholeJitterTimer = new Timer( CALL_CATEGORY_SYSTEM );
    protected ref Timer             m_EarthquakeTimer = new Timer( CALL_CATEGORY_SYSTEM );
    protected float                 m_EarthquakeDensity = 0.0;
    protected ref Timer             m_TremorTimer = new Timer( CALL_CATEGORY_SYSTEM );
	protected bool 					m_TremorInProgress = false;
    protected int                   m_TremorCounter = 0;
    protected Particle              m_SandstormAroundPlayer;
    protected ref Timer             m_SandstormPlayerTimer = new Timer( CALL_CATEGORY_SYSTEM );
    protected float                 m_SandstormDensity = 0.0;
    protected int                   m_SandstormAroundPlayerState = 0;
    protected EffectSound           m_Sandstorm_Distant_Sound;
    protected EffectSound           m_Sandstorm_Heavy_Sound;
    protected ref Timer             m_LightningTimer = new Timer( CALL_CATEGORY_SYSTEM );
    protected float                 m_LightningDensity = 0.0;
    protected vector                m_KeyholeObservatoryPos;
    protected Building              m_KeyholeObservatory;
    protected Particle              m_KeyholeObservatoryAirlockSmoke;
    protected ScriptedLightBase     m_KeyholeObservatoryAirlockLight;
    protected ref Timer             m_KeyholeObservatoryAirlockTimer = new Timer( CALL_CATEGORY_SYSTEM );

    // Athena-3 EFFECTS
    protected vfx_the_thing         m_WH_Athena;
    protected vector                m_WH_AthenaPos;
    protected ScriptedLightBase     m_WH_AthenaLight;

    void LantiaEC_Client( NamEventManager event_manager )
    {
        m_EventManager = event_manager;
        m_Player = PlayerBase.Cast( GetGame().GetPlayer() );
		DayZGame.Event_OnRPC.Insert( OnRPC );
		
        // Lantia ENVIRO EFFECTS - STATIC
        InitStaticEffects();
        // Lantia ENVIRO EFFECTS - DYNAMIC
        InitDynamicEffects();
    }

    void ~LantiaEC_Client()
    {
        if ( m_WH_Athena )
            m_WH_Athena.DeleteOnClient();
        if ( m_WH_Keyhole )
            m_WH_Keyhole.DeleteOnClient();

        if ( m_WormholeJitterTimer.IsRunning() )
            m_WormholeJitterTimer.Stop();
        if ( m_EarthquakeTimer.IsRunning() )
            m_EarthquakeTimer.Stop();
        if ( m_TremorTimer.IsRunning() )
            m_TremorTimer.Stop();
        if ( m_SandstormPlayerTimer.IsRunning() )
            m_SandstormPlayerTimer.Stop();
        if ( m_LightningTimer.IsRunning() )
            m_LightningTimer.Stop();
        if ( m_KeyholeObservatoryAirlockTimer.IsRunning() )
            m_KeyholeObservatoryAirlockTimer.Stop();

        delete m_WormholeJitterTimer;
        delete m_EarthquakeTimer;
        delete m_TremorTimer;
        delete m_SandstormPlayerTimer;
        delete m_LightningTimer;
        delete m_KeyholeObservatoryAirlockTimer;
    }

    protected void InitStaticEffects()
    {
        // land_lanteran_probe
        m_ProbesPos = Vector( 11080.60, 1009.70, 1622.89 );
        m_ProbesRot = Vector( -27, 0, 0 );
        m_XerxesPos = Vector( 10588.72, 1139.85, 1477.09 );
        m_CagePos = Vector(11079.155273, 1008.983948, 1622.038696);
        // fog - volcanic
        m_FogPos[0] = Vector(10988.27, 980.61, 1601.53);
        m_FogPos[1] = Vector(10960.05, 980.01, 1600.54);
        m_FogPos[2] = Vector(10932.50, 980.60, 1604.61);
        m_FogPos[3] = Vector(11003.41, 986.56, 1679.54);
        m_FogPos[4] = Vector(10976.46, 984.99, 1680.47);
        m_FogPos[5] = Vector(10937.91, 983.77, 1680.83);
        m_FogPos[6] = Vector(11090.39, 950.76, 1615.35);
        m_FogPos[7] = Vector(10911.26, 1002.86, 1645.79);
        m_FogPos[8] = Vector(10913.89, 1001.86, 1618.74);
        // fog - atmospheric
        m_FogPos[9] = Vector(11054.91, 1024.44, 1656.67);
        m_FogPos[10] = Vector(10984.61, 1026.52, 1644.45);
        m_FogPos[11] = Vector(11032.85, 1010.67, 1606.16);
        m_FogPos[12] = Vector(11051.46, 1027.05, 1556.35);
        m_FogPos[13] = Vector(11122.12, 1003.44, 1629.08);
        m_FogPos[14] = Vector(11118.05, 1030.51, 1687.82);
        m_FogPos[15] = Vector(11073.44, 1014.12, 1577.12);
        m_FogPos[16] = Vector(11090.79, 991.12, 1590.66);
        m_FogPos[17] = Vector(10998.10, 1035.04, 1593.00);
        m_FogPos[18] = Vector(10975.56, 1043.50, 1618.19);
        m_FogPos[19] = Vector(10941.44, 1035.22, 1598.61);
        m_FogPos[20] = Vector(11019.77, 1041.55, 1664.93);
        m_FogPos[21] = Vector(11016.96, 1046.69, 1573.03);
        m_FogPos[22] = Vector(11123.11, 1040.67, 1657.55);
        m_FogPos[23] = Vector(10956.70, 1051.96, 1665.09);
        m_FogPos[24] = Vector(10885.92, 1025.98, 1616.59);
        // lights - tubes
        m_TubeLightsPos[0] = Vector(11094.583984, 1006.391113, 1637.756592);
        m_TubeLightsPos[1] = Vector(11104.466797, 1006.455811, 1632.331543);
        m_TubeLightsPos[2] = Vector(11113.000000, 1006.319519, 1633.742676);
        m_TubeLightsPos[3] = Vector(11126.524414, 1008.443054, 1644.737183);
        m_TubeLightsPos[4] = Vector(11121.154297, 1013.807983, 1652.208008);  
        m_TubeLightsPos[5] = Vector(11116.768555, 1022.098694, 1665.657227);
        m_TubeLightsPos[6] = Vector(11118.603516, 1029.752808, 1684.276733);
        m_TubeLightsPos[7] = Vector(11124.654297, 1006.617798, 1637.761230);
        m_TubeLightsPos[8] = Vector(11129.285156, 1006.218506, 1621.710815);
        m_TubeLightsPos[9] = Vector(11125.590820, 1006.367737, 1608.741211);
        m_TubeLightsPos[10] = Vector(11116.497070, 1006.301819, 1595.962158);
        m_TubeLightsPos[11] = Vector(11104.225586, 1006.563171, 1586.070313);
        m_TubeLightsPos[12] = Vector(11090.319336, 1006.488037, 1579.032837);
        m_TubeLightsPos[13] = Vector(11077.316406, 1006.615906, 1576.367798);
        m_TubeLightsPos[14] = Vector(11068.853516, 1009.018738, 1574.429199);
        m_TubeLightsPos[15] = Vector(11065.724609, 1011.264282, 1583.664185);
        m_TubeLightsPos[16] = Vector(11065.139648, 1010.985596, 1591.030762);
        m_TubeLightsPos[17] = Vector(11068.338867, 1014.777466, 1570.397095);
        m_TubeLightsPos[18] = Vector(11070.674805, 1020.470276, 1558.563721);
        m_TubeLightsPos[19] = Vector(11062.808594, 1023.338440, 1554.432983);
        m_TubeLightsPos[20] = Vector(11048.638672, 1026.619751, 1553.883545);
        m_TubeLightsPos[21] = Vector(11076.295898, 1006.747742, 1591.905640);
        m_TubeLightsPos[22] = Vector(11071.290039, 1006.361572, 1603.750610);
        m_TubeLightsPos[23] = Vector(11060.829102, 1006.294250, 1609.514526);
        m_TubeLightsPos[24] = Vector(11041.072266, 1007.489197, 1603.754883);
        m_TubeLightsPos[25] = Vector(11030.309570, 1011.126648, 1603.678833);
        m_TubeLightsPos[26] = Vector(11017.775391, 1013.122986, 1601.265625);
        m_TubeLightsPos[27] = Vector(11066.770508, 1006.599426, 1611.757935);
        m_TubeLightsPos[28] = Vector(11064.890625, 1006.319946, 1619.814575);
        m_TubeLightsPos[29] = Vector(11068.552734, 1006.193359, 1630.528076);
        m_TubeLightsPos[30] = Vector(11075.746094, 1006.212830, 1636.527100);
        m_TubeLightsPos[31] = Vector(11083.116211, 1006.102966, 1640.429688);
        m_TubeLightsPos[32] = Vector(11074.833008, 1006.639954, 1642.942627);
        m_TubeLightsPos[33] = Vector(11072.576172, 1007.808105, 1647.698853);
        m_TubeLightsPos[34] = Vector(11071.810547, 1013.705933, 1649.760010);
        m_TubeLightsPos[35] = Vector(11075.018555, 1017.739075, 1652.473755);
        m_TubeLightsPos[36] = Vector(11066.637695, 1020.016418, 1653.411621);
        m_TubeLightsPos[37] = Vector(11054.731445, 1021.949219, 1651.007690);
        m_TubeLightsPos[38] = Vector(11055.075195, 1026.424072, 1656.253418);
        m_TubeLightsPos[39] = Vector(11040.190430, 1024.897461, 1561.846191);
        m_TubeLightsPos[40] = Vector(11024.250977, 1024.809692, 1572.119019);
        m_TubeLightsPos[41] = Vector(11016.806641, 1025.797974, 1568.363281);
        m_TubeLightsPos[42] = Vector(11029.396484, 1024.172607, 1551.570068);
        m_TubeLightsPos[43] = Vector(11017.477539, 1022.635803, 1548.690796);
        m_TubeLightsPos[44] = Vector(10995.985352, 1013.941895, 1546.487793);
        m_TubeLightsPos[45] = Vector(10983.199219, 1010.973694, 1543.290649);
        m_TubeLightsPos[46] = Vector(10980.491211, 1009.251404, 1554.040161);
        m_TubeLightsPos[47] = Vector(10982.949219, 1011.286621, 1569.112305);
        m_TubeLightsPos[48] = Vector(10992.520508, 1010.635864, 1576.036133);
        m_TubeLightsPos[49] = Vector(10999.728516, 1009.265930, 1581.604126);
        m_TubeLightsPos[50] = Vector(11008.535156, 1009.312317, 1586.369629);
        m_TubeLightsPos[51] = Vector(11131.048828, 1035.961304, 1702.463989);
        m_TubeLightsPos[52] = Vector(11116.882813, 1035.581909, 1709.635986);
        m_TubeLightsPos[53] = Vector(11114.200195, 1035.102905, 1700.888672);
        m_TubeLightsPos[54] = Vector(11115.923828, 1034.024536, 1694.219116);
        m_TubeLightsPos[55] = Vector(11110.408203, 1037.633423, 1680.724121);
        m_TubeLightsPos[56] = Vector(11106.930664, 1042.354370, 1674.988647);
        m_TubeLightsPos[57] = Vector(11103.402344, 1037.998779, 1665.052368);
        m_TubeLightsPos[58] = Vector(11098.353516, 1035.417480, 1657.909302);
        m_TubeLightsPos[59] = Vector(11085.737305, 1031.171387, 1656.346924);
        m_TubeLightsPos[60] = Vector(11072.825195, 1027.995850, 1655.101318);
        m_TubeLightsPos[61] = Vector(11060.577148, 1028.022949, 1658.092407);
        m_TubeLightsPos[62] = Vector(11053.123047, 1026.602905, 1661.726563);
        m_TubeLightsPos[63] = Vector(11049.902344, 1025.643311, 1655.877563);
        m_TubeLightsPos[64] = Vector(11040.505859, 1025.786133, 1653.447266);
        m_TubeLightsPos[65] = Vector(11025.720703, 1026.640869, 1657.052734);
        m_TubeLightsPos[66] = Vector(11018.935547, 1027.959961, 1659.201050);
        m_TubeLightsPos[67] = Vector(11015.685547, 1034.149414, 1658.014893);
        m_TubeLightsPos[68] = Vector(11011.019531, 1034.065552, 1654.215210);
        m_TubeLightsPos[69] = Vector(10994.529297, 1031.223022, 1648.061646);
        m_TubeLightsPos[70] = Vector(10987.808594, 1030.819702, 1652.747803);
        m_TubeLightsPos[71] = Vector(10984.714844, 1031.085327, 1662.837891);
        m_TubeLightsPos[72] = Vector(10970.425781, 1032.900879, 1653.919189);
        m_TubeLightsPos[73] = Vector(10956.444336, 1034.235840, 1663.048462);
        m_TubeLightsPos[74] = Vector(10984.634766, 1027.400269, 1645.141724);
        m_TubeLightsPos[75] = Vector(10981.878906, 1027.576172, 1641.672974);
        m_TubeLightsPos[76] = Vector(10986.824219, 1023.432678, 1631.584839);
        m_TubeLightsPos[77] = Vector(10994.287109, 1022.354980, 1620.537109);
        m_TubeLightsPos[78] = Vector(11003.818359, 1020.753052, 1610.773071);
        m_TubeLightsPos[79] = Vector(10993.927734, 1021.132751, 1610.684082);
        m_TubeLightsPos[80] = Vector(10993.382813, 1015.669006, 1608.976074);
        m_TubeLightsPos[81] = Vector(10982.111328, 1013.954651, 1609.416748);
        m_TubeLightsPos[82] = Vector(10966.148438, 1010.390015, 1607.997803);
        m_TubeLightsPos[83] = Vector(10957.296875, 1007.837280, 1609.107544);
        m_TubeLightsPos[84] = Vector(11009.958984, 1013.668335, 1605.208862);
        m_TubeLightsPos[85] = Vector(11010.264648, 1010.072754, 1597.375610);
        m_TubeLightsPos[86] = Vector(11085.769531, 1019.762268, 1563.149048);
        m_TubeLightsPos[87] = Vector(11088.361328, 1016.856384, 1574.734009);
        m_TubeLightsPos[88] = Vector(11097.268555, 1011.450073, 1578.229492);
        m_TubeLightsPos[89] = Vector(11111.715820, 1006.130615, 1590.230347);
        m_TubeLightsPos[90] = Vector(11030.111328, 1026.106812, 1557.744629);
        m_TubeLightsPos[91] = Vector(11025.592773, 1026.102173, 1558.473877);
        m_TubeLightsPos[92] = Vector(11022.143555, 1026.363770, 1561.979614);
    }
    protected void CreateStaticEffects()
    {
        m_Xerxes_Sound = SEffectManager.PlaySound( "Nam_Lantia_Xerxes_SoundSet", m_XerxesPos, 0, 0, true );

        m_Probes = land_lanteran_probe.Cast( GetGame().CreateObjectEx( "land_lanteran_probe", m_ProbesPos, ECE_LOCAL ) );
        if ( m_Probes )
            m_Probes.SetOrientation( m_ProbesRot );

        int i = 0;
        for ( i = 0; i < 9; i++ )
        {
            m_Fog[i] = GenericVFX.Cast( GetGame().CreateObjectEx( "vfx_lantia_fog", m_FogPos[i], ECE_LOCAL ) );
            if ( m_Fog[i] )
                m_Fog[i].vfx_start();
        }
        for ( i = 9; i < m_FogCount; i++ )
        {
            m_Fog[i] = GenericVFX.Cast( GetGame().CreateObjectEx( "vfx_lantia_portal_atm", m_FogPos[i], ECE_LOCAL ) );
            if ( m_Fog[i] )
                m_Fog[i].vfx_start();
        }

        for ( int j = 0; j < m_TubeLightCount; j++ )
        {
            m_TubeLights[j] = LantiaTubeLight.Cast( ScriptedLightBase.CreateLight( LantiaTubeLight, m_TubeLightsPos[j], 1 ) );
            m_TubeLights[j].FadeBrightnessTo( m_TubeLights[j].m_DefaultBrightness, 0 );
            m_TubeLights[j].FadeRadiusTo( m_TubeLights[j].m_DefaultRadius, 0 );
        }

        m_CageLight = LantiaCageLight.Cast( ScriptedLightBase.CreateLight( LantiaCageLight, m_CagePos, 1 ) );
        m_CageLight.FadeBrightnessTo( m_CageLight.m_DefaultBrightness, 0 );
        m_CageLight.FadeRadiusTo( m_CageLight.m_DefaultRadius, 0 );

        m_FogPortalAtm = GenericVFX.Cast( GetGame().CreateObjectEx( "vfx_lantia_portal_atm", m_CagePos, ECE_LOCAL ) );
        if ( m_FogPortalAtm )
            m_FogPortalAtm.vfx_start();

        // PROBES STATE
        switch ( GetLantiaState() )
        {
            case ELantia.DORMANT:
            case ELantia.A3INIT:
            case ELantia.A3ACTIVE:
            case ELantia.WINDOW_OPEN:
            case ELantia.PROBES_DETECT:
            case ELantia.LINK_WEAKENING:
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.INACTIVE );
                }
                break;
            case ELantia.LINK_UNSTABLE:
            case ELantia.LINK_PREATTACK:
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.PROBES_ACTIVE );
                }
                break;
            case ELantia.LINK_ATTACK:
            case ELantia.LINK_PHASING:
            case ELantia.EARTHQUAKE:
            case ELantia.SANDSTORM_LIGHT:
            case ELantia.SANDSTORM_HEAVY:
            case ELantia.WINDOW_END:
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.PROBES_BEAMS );
                }
                break;
            default:
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.INACTIVE );
                }
                break;
        }
        // GLOBAL LIGHTING
        switch ( GetLantiaState() )
        {
            case ELantia.DORMANT:
            case ELantia.A3INIT:
            case ELantia.A3ACTIVE:
            case ELantia.WINDOW_OPEN:
            case ELantia.PROBES_DETECT:
            case ELantia.LINK_WEAKENING:
            case ELantia.LINK_UNSTABLE:
            case ELantia.LINK_PREATTACK:
            case ELantia.LINK_ATTACK:
            case ELantia.LINK_PHASING:
            case ELantia.EARTHQUAKE:
                GetGame().GetWorld().LoadNewLightingCfg( WorldLighting.lighting_namalsk_lantia );
                break;
            case ELantia.SANDSTORM_LIGHT:
                GetGame().GetWorld().LoadNewLightingCfg( WorldLighting.lighting_namalsk_lantia_mid );
                break;
            case ELantia.SANDSTORM_HEAVY:
            case ELantia.WINDOW_END:
                GetGame().GetWorld().LoadNewLightingCfg( WorldLighting.lighting_namalsk_lantia_dark );
                break;
            default:
                GetGame().GetWorld().LoadNewLightingCfg( WorldLighting.lighting_namalsk_lantia );
                break;
        }
    }
    protected void RemoveStaticEffects()
    {
        WorldLighting wLighting = MissionBaseWorld.Cast( GetGame().GetMission() ).GetWorldLighting();
        if ( wLighting && GetGame().IsClient() )
        {
            // for the offline use
            wLighting.SetGlobalLighting( wLighting.GetGlobalLighting() );
        }
        else
        {
            // standard client (mp)
            GetGame().GetWorld().LoadNewLightingCfg( WorldLighting.lighting_namalsk_dark );
        }

        m_Xerxes_Sound.SoundStop();
        
        for ( int l = 0; l < m_FogCount; l++ )
        {
            if ( m_Fog[l] )
                GetGame().ObjectDelete( m_Fog[l] );
        }

        for ( int k = 0; k < m_TubeLightCount; k++ )
        {
            if ( m_TubeLights[k] )
                GetGame().ObjectDelete( m_TubeLights[k] );
        }

        if ( m_CageLight )
            GetGame().ObjectDelete( m_CageLight );

        if ( m_FogPortalAtm )
            GetGame().ObjectDelete( m_FogPortalAtm );

        if ( m_Probes )
            GetGame().ObjectDelete( m_Probes );
    }

    protected void InitDynamicEffects()
    {
        // WH positions
        m_WH_KeyholePos = Vector( 11080.20, 1008.47, 1622.61 );
        m_WH_AthenaPos = Vector( 3596.12, 171.349, 6687.33 );

        // Misc
        m_OrbitPos = Vector( 11074.23, 1578.78, 1604.55 );
        m_KeyholeObservatoryPos = Vector( 11025.531, 1023.573, 1558.438 );

        // Spires
        m_spires1[0] = Vector( 11102.44, 0, 1710.62 );
        m_spires1[1] = Vector( 11127.31, 0, 1560.31 );
        m_spires1[2] = Vector( 10804.27, 0, 1678.26 );
        m_spires1[3] = Vector( 10881.48, 0, 1654.92 );
        m_spires1[4] = Vector( 10969.41, 0, 1721.89 );
        m_spires1[5] = Vector( 10803.40, 0, 1635.80 );
        m_spires2[0] = Vector( 10962.08, 0, 1640.40 );
        m_spires2[1] = Vector( 10946.29, 0, 1516.46 );
        m_spires2[2] = Vector( 10843.25, 0, 1346.50 );
        m_spires2[3] = Vector( 11019.80, 0, 1481.10 );
        m_spires2[4] = Vector( 10667.74, 0, 1610.66 );
        m_spires3[0] = Vector( 10894.20, 0, 1581.43 );
        m_spires3[1] = Vector( 10876.59, 0, 1543.82 );
        m_spires3[2] = Vector( 10952.08, 0, 1564.80 );
        m_spires3[3] = Vector( 10993.53, 0, 1477.02 );
        m_spires3[4] = Vector( 10647.67, 0, 1369.34 );
        m_spires3[5] = Vector( 10747.38, 0, 1507.05 );
        m_spires3[6] = Vector( 10945.63, 0, 1504.51 );
        m_spires3[7] = Vector( 10917.88, 0, 1427.22 );
        m_spires3[8] = Vector( 11066.53, 0, 1682.05 );
        m_spires3[9] = Vector( 11017.08, 0, 1628.92 );
        m_spires3[10] = Vector( 11157.60, 0, 1643.92 );
        m_spires3[11] = Vector( 11047.42, 0, 1578.74 );
        m_spires3[12] = Vector( 11247.72, 0, 1646.19 );
        m_spires3[13] = Vector( 11019.84, 0, 1452.67 );
        m_spires3[14] = Vector( 11146.92, 0, 1516.68 );
        m_spires3[15] = Vector( 11155.46, 0, 1452.65 );
        m_spires3[16] = Vector( 11218.31, 0, 1451.92 );
        m_spires3[17] = Vector( 11237.66, 0, 1548.79 );
        m_spires3[18] = Vector( 11199.00, 0, 1452.12 );
        m_spires3[19] = Vector( 10948.04, 0, 1633.20 );
        m_spires3[20] = Vector( 10973.63, 0, 1765.51 );
        m_spires3[21] = Vector( 10988.71, 0, 1840.59 );
        m_spires3[22] = Vector( 10824.09, 0, 1934.56 );
        m_spires3[23] = Vector( 11072.93, 0, 1989.82 );
        m_spires3[24] = Vector( 11013.76, 0, 1974.00 );
        m_spires3[25] = Vector( 11259.19, 0, 1806.87 );
        m_spires3[26] = Vector( 10825.49, 0, 1676.31 );
        m_spires3[27] = Vector( 10962.27, 0, 1753.52 );
        m_spires3[28] = Vector( 10775.99, 0, 1620.10 );
        m_spires3[29] = Vector( 10768.58, 0, 1630.88 );
        m_spires4[0] = Vector( 11131.71, 0, 1592.51 );
        m_spires4[1] = Vector( 10957.74, 0, 1561.89 );
        m_spires4[2] = Vector( 10993.42, 0, 1426.61 );
        m_spires4[3] = Vector( 10912.79, 0, 1504.32 );
        m_spires4[4] = Vector( 10877.82, 0, 1374.08 );
        m_spires4[5] = Vector( 11087.75, 0, 1689.28 );
        m_spires4[6] = Vector( 11264.40, 0, 1643.26 );
        m_spires4[7] = Vector( 11025.82, 0, 1645.11 );
        m_spires4[8] = Vector( 11041.27, 0, 1574.67 );
        m_spires4[9] = Vector( 11298.09, 0, 1637.08 );
        m_spires4[10] = Vector( 11231.39, 0, 1556.58 );
        m_spires4[11] = Vector( 11135.54, 0, 1578.39 );
        m_spires4[12] = Vector( 11306.07, 0, 1558.03 );
        m_spires4[13] = Vector( 11153.01, 0, 1513.49 );
        m_spires4[14] = Vector( 10829.15, 0, 1751.73 );
        m_spires4[15] = Vector( 10908.75, 0, 1970.34 );
        m_spires4[16] = Vector( 11167.14, 0, 1916.52 );
        m_spires4[17] = Vector( 10963.24, 0, 1768.14 );
        m_spires4[18] = Vector( 11101.92, 0, 1861.19 );
        m_spires4[19] = Vector( 10819.03, 0, 1696.10 );
        m_spires4[20] = Vector( 10988.90, 0, 1748.94 );
        m_spires4[21] = Vector( 10872.04, 0, 1670.97 );

        // sandstorm circle - outer
        m_sandstorm_outer[0] = Vector( 10908.91, 1041.36, 1764.5 );
        m_sandstorm_outer[1] = Vector( 11030.75, 1060.71, 1783.28 );
        m_sandstorm_outer[2] = Vector( 11119.41, 1051.75, 1771.13 );
        m_sandstorm_outer[3] = Vector( 10820.56, 1050.06, 1728.39 );
        m_sandstorm_outer[4] = Vector( 10731.52, 1041.70, 1654.84 );
        m_sandstorm_outer[5] = Vector( 10965.65, 1115.85, 1434.77 );
        m_sandstorm_outer[6] = Vector( 10736.63, 1044.42, 1542.16 );
        m_sandstorm_outer[7] = Vector( 10778.76, 1025.65, 1460.53 );
        m_sandstorm_outer[8] = Vector( 10853.38, 1048.64, 1501.07 );
        m_sandstorm_outer[9] = Vector( 10932.02, 1079.67, 1494.18 );
        m_sandstorm_outer[10] = Vector( 11223.71, 1051.74, 1713.73 );
        m_sandstorm_outer[11] = Vector( 11250.94, 1054.32, 1620.90 );
        m_sandstorm_outer[12] = Vector( 11202.51, 1082.51, 1539.52 );
        m_sandstorm_outer[13] = Vector( 11031.61, 1087.33, 1454.04 );
        m_sandstorm_outer[14] = Vector( 11135.73, 1084.27, 1472.85 );
        m_sandstorm_outer[15] = Vector( 11175.15, 1059.87, 1744.45 );
        m_sandstorm_outer[16] = Vector( 11088.39, 1087.19, 1457.08 );
        m_sandstorm_outer[17] = Vector( 11297.38, 1058.56, 1557.11 );
        m_sandstorm_outer[18] = Vector( 10869.79, 1073.45, 1741.68 );
        m_sandstorm_outer[19] = Vector( 10761.53, 1061.31, 1598.43 );
        m_sandstorm_outer[20] = Vector( 10724.67, 1057.94, 1594.98 );
        m_sandstorm_outer[21] = Vector( 10965.26, 1090.45, 1802.36 );
        m_sandstorm_outer[22] = Vector( 10782.17, 1082.88, 1688.34 );

        // sandstorm circle - inner
        m_sandstorm_inner[0] = Vector( 10819.94, 1126.46, 1623.5 );
        m_sandstorm_inner[1] = Vector( 10991.34, 1129.71, 1614.25 );
        m_sandstorm_inner[2] = Vector( 10909.04, 1131.90, 1579.87 );
        m_sandstorm_inner[3] = Vector( 11012.16, 1109.54, 1686.3 );
        m_sandstorm_inner[4] = Vector( 11109.41, 1122.38, 1677.46 );
        m_sandstorm_inner[5] = Vector( 11073.67, 1118.79, 1597.26 );
        m_sandstorm_inner[6] = Vector( 11151.84, 1117.12, 1615.2 );
        m_sandstorm_inner[7] = Vector( 11042.39, 1120.34, 1550.04 );
        m_sandstorm_inner[8] = Vector( 11075.46, 1095.36, 1730.27 );
        m_sandstorm_inner[9] = Vector( 10991.76, 1098.52, 1739.03 );
        m_sandstorm_inner[10] = Vector( 10913.09, 1099.70, 1670.29 );
        m_sandstorm_inner[11] = Vector( 10993.30, 1113.98, 1543.30 );
        m_sandstorm_inner[12] = Vector( 10850.72, 1115.49, 1559.09 );
        m_sandstorm_inner[13] = Vector( 10791.08, 1090.44, 1523.48 );
        m_sandstorm_inner[14] = Vector( 10853.06, 1113.28, 1677.54 );
        m_sandstorm_inner[15] = Vector( 10969.13, 1088.16, 1667.15 );
        m_sandstorm_inner[16] = Vector( 11126.16, 1116.80, 1532.63 );
        m_sandstorm_inner[17] = Vector( 11186.48, 1096.90, 1669.72 );
        
        // sandstorm lightning
        m_lightning[0] = Vector( 11020.15, 1120.75, 1625.23 );
        m_lightning[1] = Vector( 10956.36, 1136.65, 1632.94 );
        m_lightning[2] = Vector( 10861.53, 1138.55, 1665.76 );
        m_lightning[3] = Vector( 10969.26, 1148.75, 1734.18 );
        m_lightning[4] = Vector( 11090.37, 1168.92, 1675.12 );
        m_lightning[5] = Vector( 11118.27, 1146.10, 1576.49 );
        m_lightning[6] = Vector( 11122.34, 1169.66, 1481.35 );
        m_lightning[7] = Vector( 11000.27, 1206.29, 1464.16 );
        m_lightning[8] = Vector( 10986.74, 1125.26, 1641.79 );
        m_lightning[9] = Vector( 11078.58, 1119.27, 1622.27 );
        m_lightning[10] = Vector( 11049.86, 1118.94, 1585.68 );
        m_lightning[11] = Vector( 11146.41, 1095.76, 1688.29 );
        m_lightning[12] = Vector( 11160.45, 1107.74, 1613.63 );
    }
    protected void UpdateGlobalLighting()
    {
        if ( m_Player && m_Player.IsAtLantia() )
        {
            switch ( GetLantiaState() )
            {
                case ELantia.DORMANT:
                case ELantia.WINDOW_OPEN:
                case ELantia.PROBES_DETECT:
                case ELantia.LINK_WEAKENING:
                case ELantia.LINK_UNSTABLE:
                case ELantia.LINK_PREATTACK:
                case ELantia.LINK_ATTACK:
                case ELantia.LINK_PHASING:
                case ELantia.EARTHQUAKE:
                    GetGame().GetWorld().LoadNewLightingCfg( WorldLighting.lighting_namalsk_lantia );
                    break;
                case ELantia.SANDSTORM_LIGHT:
                    GetGame().GetWorld().LoadNewLightingCfg( WorldLighting.lighting_namalsk_lantia_mid );
                    break;
                case ELantia.SANDSTORM_HEAVY:
                case ELantia.WINDOW_END:
                    GetGame().GetWorld().LoadNewLightingCfg( WorldLighting.lighting_namalsk_lantia_dark );
                    break;
                default:
                    GetGame().GetWorld().LoadNewLightingCfg( WorldLighting.lighting_namalsk_lantia );
                    break;
            }
        }
    }
    protected void CreateLocalEffects()
    {
        switch ( GetLantiaState() )
        {
            case ELantia.DORMANT:
            case ELantia.A3INIT:
            case ELantia.A3ACTIVE:
            case ELantia.WINDOW_OPEN:
            case ELantia.PROBES_DETECT:
            case ELantia.LINK_WEAKENING:
            case ELantia.LINK_UNSTABLE:
            case ELantia.LINK_PREATTACK:
            case ELantia.LINK_ATTACK:
            case ELantia.LINK_PHASING:
            case ELantia.EARTHQUAKE:
            {
                break;
            }
            case ELantia.SANDSTORM_LIGHT:
            {
                // SOUNDS
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                m_Sandstorm_Distant_Sound = PlaySoundOnPlayerStr( "Nam_Lantia_Sandstorm_Distant_SoundSet", 0, true );
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();

                // SANDSTORM SOUND CONTROL
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                m_SandstormPlayerTimer.Run( 1.0, this, "SandstormAroundPlayer", NULL, true );

                // SANDSTORM PARTICLE
                SandstormLocalParticle();

                // EARTHQUAKES
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                m_EarthquakeTimer.Run( 5.0, this, "Earthquakes", NULL, true );
                m_TremorTimer.Run( 0.5, this, "Tremors", NULL, true );

                break;
            }
            case ELantia.SANDSTORM_HEAVY:
            {
                // SOUNDS
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                m_Sandstorm_Distant_Sound = PlaySoundOnPlayerStr( "Nam_Lantia_Sandstorm_Distant_SoundSet", 400.0 / 1200.0, true );
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();
                m_Sandstorm_Heavy_Sound = PlaySoundOnPlayerStr( "Nam_Lantia_Sandstorm_Heavy_SoundSet", 0, true );

                // SANDSTORM SOUND CONTROL
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                m_SandstormPlayerTimer.Run( 1.0, this, "SandstormAroundPlayer", NULL, true );

                // SANDSTORM PARTICLE
                SandstormLocalParticle();

                // EARTHQUAKES
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                m_EarthquakeTimer.Run( 5.0, this, "Earthquakes", NULL, true );
                m_TremorTimer.Run( 0.5, this, "Tremors", NULL, true );

                break;
            }
            case ELantia.WINDOW_END:
            {
                // SOUNDS
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                m_Sandstorm_Distant_Sound = PlaySoundOnPlayerStr( "Nam_Lantia_Sandstorm_Distant_SoundSet", 400.0 / 1200.0, true );
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();
                m_Sandstorm_Heavy_Sound = PlaySoundOnPlayerStr( "Nam_Lantia_Sandstorm_Heavy_SoundSet", 80.0 / 240.0, true );

                // SANDSTORM SOUND CONTROL
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                m_SandstormPlayerTimer.Run( 1.0, this, "SandstormAroundPlayer", NULL, true );

                // SANDSTORM PARTICLE
                SandstormLocalParticle();

                // EARTHQUAKES
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                m_EarthquakeTimer.Run( 5.0, this, "Earthquakes", NULL, true );
                m_TremorTimer.Run( 0.5, this, "Tremors", NULL, true );

                break;
            }
            default:
            {
                break;
            }
        }

        // MISC
        InitKeyholeObservatory();
    }
    protected void RemoveLocalEffects()
    {
        // kill sounds
        if ( m_Sandstorm_Distant_Sound )
            m_Sandstorm_Distant_Sound.SoundStop();
        if ( m_Sandstorm_Heavy_Sound )
            m_Sandstorm_Heavy_Sound.SoundStop();

        // kill timers running local earthquakes
        if ( m_EarthquakeTimer.IsRunning() )
            m_EarthquakeTimer.Stop();
        if ( m_TremorTimer.IsRunning() )
            m_TremorTimer.Stop();

        // kill timer that runs local sandstorm sounds
        if ( m_SandstormPlayerTimer.IsRunning() )
            m_SandstormPlayerTimer.Stop();

        // kill the local sandstorm particle
        if ( m_SandstormAroundPlayer )
            m_SandstormAroundPlayer.Stop();

        // kill keyhole observatory effects
        if ( m_KeyholeObservatoryAirlockTimer.IsRunning() )
            m_KeyholeObservatoryAirlockTimer.Stop();
        if ( m_KeyholeObservatoryAirlockSmoke )
        {
            m_KeyholeObservatoryAirlockSmoke.Stop();
            m_KeyholeObservatoryAirlockSmoke = NULL;
        }
        if ( m_KeyholeObservatoryAirlockLight )
            m_KeyholeObservatoryAirlockLight.Destroy();
    }
    protected void InitKeyholeObservatory()
    {
        array<Object> keyhole_observatory = new array<Object>;
        GetGame().GetObjectsAtPosition( m_KeyholeObservatoryPos, 1, keyhole_observatory, null );
        if ( keyhole_observatory[0] )
            m_KeyholeObservatory = Building.Cast( keyhole_observatory[0] );

        if ( m_KeyholeObservatory )
        {
            m_KeyholeObservatoryAirlockTimer.Run( 0.25, this, "KeyholeObservatoryTick", NULL, true );
        }
    }
    protected void KeyholeObservatoryTick()
    {
        if ( m_KeyholeObservatory )
        {
            if ( !m_KeyholeObservatoryAirlockSmoke )
                m_KeyholeObservatoryAirlockSmoke = ParticleManager.GetInstance().PlayInWorld( ParticleList.LANTIA_KEYHOLE_AIRLOCK, Vector( 11032.61, 1026.73, 1559.87 ) );

            float rollDice = Math.RandomFloat( 0.0, 1.0 );
            if ( rollDice < 0.5 )
            {
                m_KeyholeObservatory.SetObjectMaterial( 0, "nst\\ns3\\structures\\keyhole\\data\\pallas_airlock_lights.rvmat" );

                if ( !m_KeyholeObservatoryAirlockLight )
                    m_KeyholeObservatoryAirlockLight = ScriptedLightBase.CreateLight( KeyholeObservatoryAirlockLight, Vector( 11032.48, 1026.39, 1559.88 ), 0.1 );;
            }
            else
            {
                m_KeyholeObservatory.SetObjectMaterial( 0, "nst\\ns3\\structures\\keyhole\\data\\pallas_airlock_lights_off.rvmat" );

                if ( m_KeyholeObservatoryAirlockLight )
                    m_KeyholeObservatoryAirlockLight.Destroy();
            }
        }
    }

    void Update( float timeslice )
    {
        if ( m_Tick_BigSlice > 1.0 )
        {
            // ask event manager if there is LantiaLink event running
            // todo..
            /*
            
            is link active?
             yes - what state it is?
             no - stay dormant

            enviro update - per player position

            */

            m_Tick_BigSlice = 0.0;
        }


        // STATIC EFFECTS (fast tick)
        if ( m_Player )
        {
            if ( m_Player.IsAtLantia() && !m_EffectsInit )
            {
                // enable static effects
                CreateStaticEffects();
                CreateLocalEffects();
                m_EffectsInit = true;
            }
            else
            {
                if ( !m_Player.IsAtLantia() && m_EffectsInit )
                {
                    // kill static effects
                    RemoveStaticEffects();
                    RemoveLocalEffects();
                    m_EffectsInit = false;
                }
            }
        }
        else
        {
            m_Player = PlayerBase.Cast( GetGame().GetPlayer() );
            // TODO - PLAYER DEATH
        }


        // fast tick
        m_Tick_BigSlice += timeslice;
    }

    void SwitchLantiaState( int state )
    {
        m_LantiaState = state;

        switch ( state )
        {
            case ELantia.DORMANT:
            {
                SetLantiaMode( ELantiaMode.INACTIVE );

                // SPIRES
                ToggleLanteranSpires( false );

                // LIGHTING
                UpdateGlobalLighting();

                // WH
                if ( m_WH_Keyhole )
                {
                    m_WH_Keyhole.Delete();
                }
                if ( m_WH_Athena )
                {
                    m_WH_Athena.Delete();
                }
                if ( m_WH_Light )
                {
                    m_WH_Light.Destroy();
                }

                // WH LIGHT
                if ( m_WH_Light )
                {
                    m_WH_Light.Destroy();
                }
                if ( m_WH_AthenaLight )
                {
                    m_WH_AthenaLight.Destroy();
                }

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.INACTIVE );
                }

                // SOUNDS
                StopLantiaMusic();
                if ( m_WH_Phasing_Sound )
                    m_WH_Phasing_Sound.SoundStop();
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();

                // PARTICLES
                DespawnSandstormOuterCircle();
                DespawnSandstormInnerCircle();
                if ( m_SandstormAroundPlayer )
                    m_SandstormAroundPlayer.Stop();

                // TIMERS
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                break;
            }

            case ELantia.A3INIT:
            {
                // SPIRES
                ToggleLanteranSpires( false );

                // LIGHTING
                UpdateGlobalLighting();

                // WH
                if ( m_WH_Keyhole )
                {
                    m_WH_Keyhole.Delete();
                }
                if ( m_WH_Athena )
                {
                    m_WH_Athena.Delete();
                }
                if ( m_WH_Light )
                {
                    m_WH_Light.Destroy();
                }

                // WH LIGHT
                if ( m_WH_Light )
                {
                    m_WH_Light.Destroy();
                }
                if ( m_WH_AthenaLight )
                {
                    m_WH_AthenaLight.Destroy();
                }

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.INACTIVE );
                }

                // SOUNDS
                if ( m_WH_Phasing_Sound )
                    m_WH_Phasing_Sound.SoundStop();
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();

                // PARTICLES
                DespawnSandstormOuterCircle();
                DespawnSandstormInnerCircle();
                if ( m_SandstormAroundPlayer )
                    m_SandstormAroundPlayer.Stop();
                if ( m_WH_StartupParticle )
                    m_WH_StartupParticle.Stop();
                m_WH_StartupParticle = ParticleManager.GetInstance().PlayInWorld( ParticleList.ANOMALY_AURA, m_WH_KeyholePos );

                // TIMERS
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                break;
            }

            case ELantia.A3ACTIVE:
            {
                // SPIRES
                ToggleLanteranSpires( false );

                // LIGHTING
                UpdateGlobalLighting();

                // WH
                if ( m_WH_Keyhole )
                {
                    m_WH_Keyhole.Delete();
                }
                if ( m_WH_Athena )
                {
                    m_WH_Athena.Delete();
                }
                if ( m_WH_Light )
                {
                    m_WH_Light.Destroy();
                }

                // WH LIGHT
                if ( m_WH_Light )
                {
                    m_WH_Light.Destroy();
                }
                if ( m_WH_AthenaLight )
                {
                    m_WH_AthenaLight.Destroy();
                }

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.INACTIVE );
                }

                // SOUNDS
                PlayEnvironmentSound( BlowoutSound.A3_Preboom, m_WH_KeyholePos, 1.0 );
                PlayEnvironmentSound( BlowoutSound.A3_Preboom, m_WH_AthenaPos, 1.0 );
                if ( m_WH_Phasing_Sound )
                    m_WH_Phasing_Sound.SoundStop();
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();

                // PARTICLES
                DespawnSandstormOuterCircle();
                DespawnSandstormInnerCircle();
                if ( m_SandstormAroundPlayer )
                    m_SandstormAroundPlayer.Stop();
                if ( m_WH_StartupParticle )
                    m_WH_StartupParticle.Stop();
                m_WH_StartupParticle = ParticleManager.GetInstance().PlayInWorld( ParticleList.ANOMALY_AURA, m_WH_KeyholePos );

                // TIMERS
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                break;
            }

            case ELantia.WINDOW_OPEN:
            {
                // SPIRES
                ToggleLanteranSpires( false );

                // LIGHTING
                UpdateGlobalLighting();

                // WH 
                if ( !m_WH_Keyhole )
                {
                    m_WH_Keyhole = vfx_the_thing.Cast ( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_KeyholePos, ECE_LOCAL ) );
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetUnderAttack( false );
                }
                else
                {
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetUnderAttack( false );
                }
                if ( !m_WH_Athena )
                {
                    m_WH_Athena = vfx_the_thing.Cast( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_AthenaPos, ECE_LOCAL ) );
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetUnderAttack( false );
                } 
                else
                {
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetUnderAttack( false );
                }

                // WH LIGHT
                if ( !m_WH_Light )
                {
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkActiveLight, m_WH_KeyholePos, 0.1 );
                }
                else
                {
                    m_WH_Light.Destroy();
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkActiveLight, m_WH_KeyholePos, 0.1 );
                }
                if ( !m_WH_AthenaLight )
                {
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkActiveLight, m_WH_AthenaPos, 0.1 );
                }
                else
                {
                    m_WH_AthenaLight.Destroy();
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkActiveLight, m_WH_AthenaPos, 0.1 );
                }

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.INACTIVE );
                }

                // SOUNDS
                PlayLantiaMusic();
                PlayEnvironmentSound( BlowoutSound.Blowout_Wormhole_AttackA3, m_WH_KeyholePos, 1.0 );
                PlayEnvironmentSound( BlowoutSound.Blowout_Wormhole_AttackA3, m_WH_AthenaPos, 1.0 );
                if ( m_WH_Phasing_Sound )
                    m_WH_Phasing_Sound.SoundStop();
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();
                if ( m_WH_StartupParticle )
                    m_WH_StartupParticle.Stop();

                // PARTICLES
                DespawnSandstormOuterCircle();
                DespawnSandstormInnerCircle();
                if ( m_SandstormAroundPlayer )
                    m_SandstormAroundPlayer.Stop();

                // TIMERS
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                break;
            }

            //! ADDITIVE STATE
            case ELantia.LINK_WEAKENING:
            {
                // SPIRES
                ToggleLanteranSpires( false );

                // LIGHTING
                UpdateGlobalLighting();

                // WH 
                if ( !m_WH_Keyhole )
                {
                    m_WH_Keyhole = vfx_the_thing.Cast ( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_KeyholePos, ECE_LOCAL ) );
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetUnderAttack( false );
                }
                else
                {
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetUnderAttack( false );
                }
                if ( !m_WH_Athena )
                {
                    m_WH_Athena = vfx_the_thing.Cast( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_AthenaPos, ECE_LOCAL ) );
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetUnderAttack( false );
                } 
                else
                {
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetUnderAttack( false );
                }

                // WH LIGHT
                if ( !m_WH_Light )
                {
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkActiveLight, m_WH_KeyholePos, 0.1 );
                }
                else
                {
                    m_WH_Light.Destroy();
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkActiveLight, m_WH_KeyholePos, 0.1 );
                }
                if ( !m_WH_AthenaLight )
                {
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkActiveLight, m_WH_AthenaPos, 0.1 );
                }
                else
                {
                    m_WH_AthenaLight.Destroy();
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkActiveLight, m_WH_AthenaPos, 0.1 );
                }

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.INACTIVE );
                }

                // SOUNDS
                PlayEnvironmentSound( BlowoutSound.Blouwout_Stabilizer, m_WH_KeyholePos, 1.0 );
                PlayEnvironmentSound( BlowoutSound.Blowout_Probes_Ping1, m_OrbitPos, 1.0 );
                if ( m_WH_Phasing_Sound )
                    m_WH_Phasing_Sound.SoundStop();
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();

                // PARTICLES
                DespawnSandstormOuterCircle();
                DespawnSandstormInnerCircle();
                if ( m_SandstormAroundPlayer )
                    m_SandstormAroundPlayer.Stop();
                if ( m_WH_StartupParticle )
                    m_WH_StartupParticle.Stop();

                // TIMERS
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                break;
            }

            case ELantia.LINK_UNSTABLE:
            {
                // SPIRES
                ToggleLanteranSpires( false );

                // LIGHTING
                UpdateGlobalLighting();

                // WH
                if ( !m_WH_Keyhole )
                {
                    m_WH_Keyhole = vfx_the_thing.Cast ( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_KeyholePos, ECE_LOCAL ) );
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( false );
                }
                else
                {
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( false );
                }
                if ( !m_WH_Athena )
                {
                    m_WH_Athena = vfx_the_thing.Cast( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_AthenaPos, ECE_LOCAL ) );
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( false );
                } 
                else
                {
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( false );
                }

                // WH LIGHT
                if ( !m_WH_Light )
                {
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkActiveLight, m_WH_KeyholePos, 0.1 );
                }
                else
                {
                    m_WH_Light.Destroy();
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkActiveLight, m_WH_KeyholePos, 0.1 );
                }
                if ( !m_WH_AthenaLight )
                {
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkActiveLight, m_WH_AthenaPos, 0.1 );
                }
                else
                {
                    m_WH_AthenaLight.Destroy();
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkActiveLight, m_WH_AthenaPos, 0.1 );
                }

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.PROBES_ACTIVE );
                }

                // PARTICLES
                DespawnSandstormOuterCircle();
                DespawnSandstormInnerCircle();
                if ( m_SandstormAroundPlayer )
                    m_SandstormAroundPlayer.Stop();
                if ( m_WH_StartupParticle )
                    m_WH_StartupParticle.Stop();

                // SOUNDS
                PlayEnvironmentSound( BlowoutSound.Blowout_Probes_Active, m_OrbitPos, 1.0 );
                if ( m_WH_Phasing_Sound )
                    m_WH_Phasing_Sound.SoundStop();
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();

                // TIMERS
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                break;
            }

            //! ADDITIVE STATE
            case ELantia.LINK_PREATTACK:
            {
                // if the link ends here
                if ( GetLantiaMode() == ELantiaMode.NOEXT )
                {
                    PlayEnvironmentSound( BlowoutSound.Blowout_Wormhole_AttackA3, m_WH_KeyholePos, 1.0 );
                    Particle m_WH_DestructNOEXT = ParticleManager.GetInstance().PlayInWorld( ParticleList.BLOWOUT_SHOCKWAVE, m_WH_KeyholePos );
                }

                // SPIRES
                ToggleLanteranSpires( false );

                // LIGHTING
                UpdateGlobalLighting();

                // WH
                if ( !m_WH_Keyhole )
                {
                    m_WH_Keyhole = vfx_the_thing.Cast ( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_KeyholePos, ECE_LOCAL ) );
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( false );
                }
                else
                {
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( false );
                }
                if ( !m_WH_Athena )
                {
                    m_WH_Athena = vfx_the_thing.Cast( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_AthenaPos, ECE_LOCAL ) );
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( false );
                } 
                else
                {
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( false );
                }

                // WH LIGHT
                if ( !m_WH_Light )
                {
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkActiveLight, m_WH_KeyholePos, 0.1 );
                }
                else
                {
                    m_WH_Light.Destroy();
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkActiveLight, m_WH_KeyholePos, 0.1 );
                }
                if ( !m_WH_AthenaLight )
                {
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkActiveLight, m_WH_AthenaPos, 0.1 );
                }
                else
                {
                    m_WH_AthenaLight.Destroy();
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkActiveLight, m_WH_AthenaPos, 0.1 );
                }

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.PROBES_ACTIVE );
                }

                // PARTICLES
                DespawnSandstormOuterCircle();
                DespawnSandstormInnerCircle();
                if ( m_SandstormAroundPlayer )
                    m_SandstormAroundPlayer.Stop();
                if ( m_WH_StartupParticle )
                    m_WH_StartupParticle.Stop();

                // SOUNDS
                PlayEnvironmentSound( BlowoutSound.A3_Preboom, m_WH_KeyholePos, 1.0 );
                if ( m_WH_Phasing_Sound )
                    m_WH_Phasing_Sound.SoundStop();
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();

                // TIMERS
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                break;
            }

            case ELantia.LINK_ATTACK:
            {
                // SPIRES
                ToggleLanteranSpires( false );

                // LIGHTING
                UpdateGlobalLighting();

                // WH
                if ( !m_WH_Keyhole )
                {
                    m_WH_Keyhole = vfx_the_thing.Cast ( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_KeyholePos, ECE_LOCAL ) );
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( true );
                }
                else
                {
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( true );
                }
                if ( !m_WH_Athena )
                {
                    m_WH_Athena = vfx_the_thing.Cast( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_AthenaPos, ECE_LOCAL ) );
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( true );
                } 
                else
                {
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( true );
                }

                // WH LIGHT
                if ( !m_WH_Light )
                {
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkAttackLight, m_WH_KeyholePos, 0.1 );
                }
                else
                {
                    m_WH_Light.Destroy();
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkAttackLight, m_WH_KeyholePos, 0.1 );
                }
                if ( !m_WH_AthenaLight )
                {
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkAttackLight, m_WH_AthenaPos, 0.1 );
                }
                else
                {
                    m_WH_AthenaLight.Destroy();
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkAttackLight, m_WH_AthenaPos, 0.1 );
                }

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.PROBES_BEAMS );
                }

                // PARTICLES
                DespawnSandstormOuterCircle();
                DespawnSandstormInnerCircle();
                if ( m_SandstormAroundPlayer )
                    m_SandstormAroundPlayer.Stop();
                if ( m_WH_StartupParticle )
                    m_WH_StartupParticle.Stop();

                // SOUNDS
                PlayEnvironmentSound( BlowoutSound.Blowout_Wormhole_Attack, m_WH_KeyholePos, 1.0 );
                if ( m_WH_Phasing_Sound )
                    m_WH_Phasing_Sound.SoundStop();
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();

                // TIMERS
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                break;
            }

            //! ADDITIVE STATE
            case ELantia.LINK_PHASING:
            {
                // if the link ends here
                if ( GetLantiaMode() == ELantiaMode.EXT )
                {
                    PlayEnvironmentSound( BlowoutSound.Blowout_Wormhole_AttackA3, m_WH_KeyholePos, 1.0 );
                    Particle m_WH_DestructEXT = ParticleManager.GetInstance().PlayInWorld( ParticleList.BLOWOUT_SHOCKWAVE, m_WH_KeyholePos );
                }
                
                // SPIRES
                ToggleLanteranSpires( false );

                // LIGHTING
                UpdateGlobalLighting();

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.PROBES_BEAMS );
                }

                // WH
                if ( !m_WH_Keyhole )
                {
                    m_WH_Keyhole = vfx_the_thing.Cast ( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_KeyholePos, ECE_LOCAL ) );
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( true );
                }
                else
                {
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( true );
                }
                if ( !m_WH_Athena )
                {
                    m_WH_Athena = vfx_the_thing.Cast( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_AthenaPos, ECE_LOCAL ) );
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( true );
                } 
                else
                {
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( true );
                }

                // WH LIGHT -> PHASED
                if ( !m_WH_Light )
                {
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkAttackLight, m_WH_KeyholePos, 0.1 );
                }
                else
                {
                    m_WH_Light.Destroy();
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkPhasedLight, m_WH_KeyholePos, 0.1 );
                }
                if ( !m_WH_AthenaLight )
                {
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkPhasedLight, m_WH_AthenaPos, 0.1 );
                }
                else
                {
                    m_WH_AthenaLight.Destroy();
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkPhasedLight, m_WH_AthenaPos, 0.1 );
                }

                // PARTICLES
                DespawnSandstormOuterCircle();
                DespawnSandstormInnerCircle();
                if ( m_SandstormAroundPlayer )
                    m_SandstormAroundPlayer.Stop();
                if ( m_WH_StartupParticle )
                    m_WH_StartupParticle.Stop();

                // SOUNDS
                if ( m_WH_Phasing_Sound )
                    m_WH_Phasing_Sound.SoundStop();
                m_WH_Phasing_Sound = SEffectManager.PlaySound( "Blowout_Wormhole_Phased", m_WH_KeyholePos, 0, 0, true );
                if ( m_Sandstorm_Distant_Sound )
                    m_Sandstorm_Distant_Sound.SoundStop();
                if ( m_Sandstorm_Heavy_Sound )
                    m_Sandstorm_Heavy_Sound.SoundStop();

                // TIMERS
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                m_WormholeJitterTimer.Run( 0.01, this, "WormholeJitter", NULL, true );
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                break;
            } 

            //! ADDITIVE STATE
            case ELantia.EARTHQUAKE:
            {
                if ( m_Player && m_Player.IsAtLantia() )
                {
                    thread ScriptedTremors();
                }
                break;
            }

            case ELantia.SANDSTORM_LIGHT:
            {
                // SPIRES
                ToggleLanteranSpires( true );

                // LIGHTING
                UpdateGlobalLighting();

                // RADIAL BLUR
                if ( m_Player.IsAtLantia() )
                {
                    thread LerpTeleportEffect( 0.55, 0, 5000 );
                }

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.PROBES_BEAMS );
                }

                // WH
                if ( !m_WH_Keyhole )
                {
                    m_WH_Keyhole = vfx_the_thing.Cast ( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_KeyholePos, ECE_LOCAL ) );
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( true );
                }
                else
                {
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( true );
                }
                if ( !m_WH_Athena )
                {
                    m_WH_Athena = vfx_the_thing.Cast( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_AthenaPos, ECE_LOCAL ) );
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( true );
                } 
                else
                {
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( true );
                }

                // WH LIGHT -> PHASED
                if ( !m_WH_Light )
                {
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkAttackLight, m_WH_KeyholePos, 0.1 );
                }
                else
                {
                    m_WH_Light.Destroy();
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkPhasedLight, m_WH_KeyholePos, 0.1 );
                }
                if ( !m_WH_AthenaLight )
                {
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkPhasedLight, m_WH_AthenaPos, 0.1 );
                }
                else
                {
                    m_WH_AthenaLight.Destroy();
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkPhasedLight, m_WH_AthenaPos, 0.1 );
                }

                // PARTICLES
                SpawnSandstormOuterCircle();
                DespawnSandstormInnerCircle();
                if ( m_Player.IsAtLantia() )
                {
                    SandstormLocalParticle();
                }
                if ( m_WH_StartupParticle )
                    m_WH_StartupParticle.Stop();

                // SOUNDS
                PlayEnvironmentSound( BlowoutSound.Blowout_Begin, m_WH_KeyholePos, 1.0 );
                if ( m_Player.IsAtLantia() )
                {
                    if ( m_Sandstorm_Distant_Sound )
                        m_Sandstorm_Distant_Sound.SoundStop();
                    m_Sandstorm_Distant_Sound = PlaySoundOnPlayerStr( "Nam_Lantia_Sandstorm_Distant_SoundSet", 0, true );
                    if ( m_Sandstorm_Heavy_Sound )
                        m_Sandstorm_Heavy_Sound.SoundStop();
                }

                // TIMERS
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                m_WormholeJitterTimer.Run( 0.01, this, "WormholeJitter", NULL, true );
                //  earthquakes
                m_EarthquakeDensity = 0.25;
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_Player.IsAtLantia() )
                {
                    m_EarthquakeTimer.Run( 5.0, this, "Earthquakes", NULL, true );
                    m_TremorTimer.Run( 0.5, this, "Tremors", NULL, true );
                }
                //  sandstorms
                m_SandstormDensity = 0;
                m_SandstormAroundPlayerState = 0;
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                if ( m_Player.IsAtLantia() )
                {
                    m_SandstormPlayerTimer.Run( 1.0, this, "SandstormAroundPlayer", NULL, true );
                }
                //  lightning
                m_LightningDensity = 0.05;
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                m_LightningTimer.Run( 1.0, this, "SandstormLightning", NULL, true );
                break;
            }

            case ELantia.SANDSTORM_HEAVY:
            {
                // SPIRES
                ToggleLanteranSpires( true );

                // LIGHTING
                UpdateGlobalLighting();

                // RADIAL BLUR
                if ( m_Player.IsAtLantia() )
                {
                    thread LerpTeleportEffect( 0.75, 0, 5000 );
                }

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.PROBES_BEAMS );
                }

                // WH
                if ( !m_WH_Keyhole )
                {
                    m_WH_Keyhole = vfx_the_thing.Cast ( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_KeyholePos, ECE_LOCAL ) );
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( true );
                }
                else
                {
                    m_WH_Keyhole.SetScale( 0.65 );
                    m_WH_Keyhole.SetVolume( 0.01 );
                    m_WH_Keyhole.SetLantiaLink( true );
                    m_WH_Keyhole.SetStable( false );
                    m_WH_Keyhole.SetUnderAttack( true );
                }
                if ( !m_WH_Athena )
                {
                    m_WH_Athena = vfx_the_thing.Cast( GetGame().CreateObjectEx( "vfx_the_thing", m_WH_AthenaPos, ECE_LOCAL ) );
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( true );
                } 
                else
                {
                    m_WH_Athena.SetScale( 0.375 );
                    m_WH_Athena.SetVolume( 0.01 );
                    m_WH_Athena.SetLantiaLink( true );
                    m_WH_Athena.SetStable( false );
                    m_WH_Athena.SetUnderAttack( true );
                }

                // WH LIGHT -> PHASED
                if ( !m_WH_Light )
                {
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkAttackLight, m_WH_KeyholePos, 0.1 );
                }
                else
                {
                    m_WH_Light.Destroy();
                    m_WH_Light = ScriptedLightBase.CreateLight( LantiaLinkPhasedLight, m_WH_KeyholePos, 0.1 );
                }
                if ( !m_WH_AthenaLight )
                {
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkPhasedLight, m_WH_AthenaPos, 0.1 );
                }
                else
                {
                    m_WH_AthenaLight.Destroy();
                    m_WH_AthenaLight = ScriptedLightBase.CreateLight( Athena3LinkPhasedLight, m_WH_AthenaPos, 0.1 );
                }

                // PARTICLES
                SpawnSandstormOuterCircle();
                SpawnSandstormInnerCircle();
                if ( m_Player.IsAtLantia() )
                {
                    SandstormLocalParticle();
                }
                if ( m_WH_StartupParticle )
                    m_WH_StartupParticle.Stop();

                // SOUNDS
                if ( m_WH_Phasing_Sound )
                    m_WH_Phasing_Sound.SoundStop();
                PlayEnvironmentSound( BlowoutSound.Blowout_Begin, m_WH_KeyholePos, 1.0 );
                if ( m_Player.IsAtLantia() )
                {
                    if ( m_Sandstorm_Distant_Sound )
                        m_Sandstorm_Distant_Sound.SoundStop();
                    m_Sandstorm_Distant_Sound = PlaySoundOnPlayerStr( "Nam_Lantia_Sandstorm_Distant_SoundSet", 400.0 / 1200.0, true );
                    if ( m_Sandstorm_Heavy_Sound )
                        m_Sandstorm_Heavy_Sound.SoundStop();
                    m_Sandstorm_Heavy_Sound = PlaySoundOnPlayerStr( "Nam_Lantia_Sandstorm_Heavy_SoundSet", 0, true );
                }

                // TIMERS
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                m_WormholeJitterTimer.Run( 0.01, this, "WormholeJitter", NULL, true );
                m_LightningDensity = 0.25;
                m_EarthquakeDensity = 0.45;
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_Player.IsAtLantia() )
                {
                    m_EarthquakeTimer.Run( 5.0, this, "Earthquakes", NULL, true );
                    m_TremorTimer.Run( 0.5, this, "Tremors", NULL, true );
                }
                m_SandstormDensity = 0;
                m_SandstormAroundPlayerState = 1;
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                if ( m_Player.IsAtLantia() )
                {
                    m_SandstormPlayerTimer.Run( 1.0, this, "SandstormAroundPlayer", NULL, true );
                }
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                m_LightningTimer.Run( 1.0, this, "SandstormLightning", NULL, true );

                break;
            }

            case ELantia.WINDOW_END:
            {
                // SPIRES
                ToggleLanteranSpires( true );

                // LIGHTING
                UpdateGlobalLighting();

                // PROBES
                if ( m_Probes )
                {
                    m_Probes.SetProbesState( AthenaBuildingState.PROBES_ACTIVE );
                }

                // PARTICLES
                SpawnSandstormOuterCircle();
                SpawnSandstormInnerCircle();
                if ( m_Player.IsAtLantia() )
                {
                    SandstormLocalParticle();
                }
                if ( m_WH_StartupParticle )
                    m_WH_StartupParticle.Stop();

                // WH
                if ( m_WH_Keyhole )
                {
                    m_WH_Keyhole.Delete();
                }
                if ( m_WH_Athena )
                {
                    m_WH_Athena.Delete();
                }
                if ( m_WH_Light )
                {
                    m_WH_Light.Destroy();
                }
                if ( m_WH_AthenaLight )
                {
                    m_WH_AthenaLight.Destroy();
                }
                Particle m_WH_Destruct = ParticleManager.GetInstance().PlayInWorld( ParticleList.BLOWOUT_SHOCKWAVE, m_WH_KeyholePos );

                // SOUNDS
                PlayEnvironmentSound( BlowoutSound.Blowout_Begin, m_WH_KeyholePos, 1.0 );
                if ( m_Player.IsAtLantia() )
                {
                    if ( m_Sandstorm_Distant_Sound )
                        m_Sandstorm_Distant_Sound.SoundStop();
                    m_Sandstorm_Distant_Sound = PlaySoundOnPlayerStr( "Nam_Lantia_Sandstorm_Distant_SoundSet", 400.0 / 1200.0, true );
                    if ( m_Sandstorm_Heavy_Sound )
                        m_Sandstorm_Heavy_Sound.SoundStop();
                    m_Sandstorm_Heavy_Sound = PlaySoundOnPlayerStr( "Nam_Lantia_Sandstorm_Heavy_SoundSet", 80.0 / 240.0, true );
                }

                // TIMERS
                m_LightningDensity = 0.15;
                m_EarthquakeDensity = 0.35;
                if ( m_WormholeJitterTimer.IsRunning() )
                    m_WormholeJitterTimer.Stop();
                if ( m_EarthquakeTimer.IsRunning() )
                    m_EarthquakeTimer.Stop();
                if ( m_TremorTimer.IsRunning() )
                    m_TremorTimer.Stop();
                if ( m_Player.IsAtLantia() )
                {
                    m_EarthquakeTimer.Run( 5.0, this, "Earthquakes", NULL, true );
                    m_TremorTimer.Run( 0.5, this, "Tremors", NULL, true );
                }
                if ( m_SandstormPlayerTimer.IsRunning() )
                    m_SandstormPlayerTimer.Stop();
                if ( m_LightningTimer.IsRunning() )
                    m_LightningTimer.Stop();
                m_LightningTimer.Run( 1.0, this, "SandstormLightning", NULL, true );

                break;
            }

            default:
            {
                break;
            }
        }
    }

	void OnRPC( PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx )
	{	
		switch ( rpc_type )
		{	
			case ERPCsLantiaLink.NAM_RPC_LANTIA_STATE_UPDATE:
			{
                Print( "[Lantia] State update received" );

				Param1<int> state = new Param1<int>( 0 );
				
				if ( ctx.Read( state ) )
				{
                    m_Player = PlayerBase.Cast( GetGame().GetPlayer() );
					SwitchLantiaState( state.param1 );
				}
                break;
			}
            case ERPCsLantiaLink.NAM_RPC_LANTIA_MODE_UPDATE:
            {
                Print( "[Lantia] Mode update received" );

                Param1<int> mode = new Param1<int>( 0 );
                
                if ( ctx.Read( mode ) )
                {
                    m_Player = PlayerBase.Cast( GetGame().GetPlayer() );
                    SetLantiaMode( mode.param1 );
                }
                break;
            }
            case ERPCsBlowoutStorm.BLOWOUT_RPC_PLAY_SOUND_ENVIRONMENT:
            {
                Param3<BlowoutSound, vector, float> sound_param;
                if ( !ctx.Read( sound_param ) )
                {
                    break;
                }
                
                PlayEnvironmentSound( sound_param.param1, sound_param.param2, sound_param.param3 );
                break;
            }
            case ERPCsLantiaLink.NAM_RPC_LANTIA_LINK_TELEPORT:
            {
                OnTeleportClient();
                break;
            }
		}
	}
    int GetLantiaState()
    {
        return m_LantiaState;
    }
    int GetLantiaMode()
    {
        return m_LantiaMode;
    }
    void SetLantiaMode( int mode )
    {
        m_LantiaMode = mode;
    }

    protected void WormholeJitter()
    {
        if ( m_WH_Keyhole )
        {
            vector whPos = m_WH_Keyhole.GetPosition();
			vector whNewPos = Vector( whPos[0] + Math.RandomFloat( -0.1, 0.1 ), whPos[1] + Math.RandomFloat( -0.1, 0.1 ), whPos[2] + Math.RandomFloat( -0.1, 0.1 ) );
			if ( vector.DistanceSq( whNewPos, m_WH_KeyholePos) < 0.1 )
            	m_WH_Keyhole.SetPosition( whNewPos );
        }
    }
    protected void Earthquakes()
    {
        float rollDice = Math.RandomFloat( 0.0, 1.0 );
        if ( ( rollDice < m_EarthquakeDensity ) && !m_TremorInProgress )
        {
            m_TremorInProgress = true;
            PlaySoundOnPlayerStr( "Nam_Lantia_Tremors_SoundSet", 1.0 );
        }
    }
    protected void SandstormLocalParticle()
    {
        if ( !m_SandstormAroundPlayer )
        {
            m_SandstormAroundPlayer = ParticleManager.GetInstance().PlayOnObject( ParticleList.NAM_P_LANTIA_SANDSTORM, m_Player, vector.Zero );
            m_SandstormAroundPlayer.SetParameter( 0, EmitorParam.BIRTH_RATE, 0 );
            m_SandstormAroundPlayer.SetParameter( 1, EmitorParam.BIRTH_RATE, 0 );
            m_SandstormAroundPlayer.SetParameter( 2, EmitorParam.BIRTH_RATE, 0 );
        }
        else
        {
            m_SandstormAroundPlayer.Stop();

            m_SandstormAroundPlayer = ParticleManager.GetInstance().PlayOnObject( ParticleList.NAM_P_LANTIA_SANDSTORM, m_Player, vector.Zero );
            m_SandstormAroundPlayer.SetParameter( 0, EmitorParam.BIRTH_RATE, 0 );
            m_SandstormAroundPlayer.SetParameter( 1, EmitorParam.BIRTH_RATE, 0 );
            m_SandstormAroundPlayer.SetParameter( 2, EmitorParam.BIRTH_RATE, 0 );
        }
    }
    protected void SandstormAroundPlayer( )
    {
        if ( m_SandstormAroundPlayerState == 0 )
        {
            m_SandstormDensity += 4;
            m_SandstormAroundPlayer.SetParameter( 0, EmitorParam.BIRTH_RATE, Math.Min( m_SandstormDensity, 400 ) );

            if ( m_SandstormDensity > 400 )
                m_SandstormPlayerTimer.Stop();

            float volume = ( m_SandstormDensity / 1200.0 );
            if ( m_Sandstorm_Distant_Sound )
                m_Sandstorm_Distant_Sound.GetSoundObject().SetVolume( volume );
        }
        if ( m_SandstormAroundPlayerState == 1)
        {
            m_SandstormDensity += 2;
            m_SandstormAroundPlayer.SetParameter( 1, EmitorParam.BIRTH_RATE, Math.Min( m_SandstormDensity, 80 ) );
            m_SandstormAroundPlayer.SetParameter( 2, EmitorParam.BIRTH_RATE, Math.Min( m_SandstormDensity, 80 ) );

            if ( m_SandstormDensity > 80 )
                m_SandstormPlayerTimer.Stop();

            float volume_heavy = ( m_SandstormDensity / 240.0 );
            if ( m_Sandstorm_Heavy_Sound )
                m_Sandstorm_Heavy_Sound.GetSoundObject().SetVolume( volume_heavy );
        }
    }
    protected void Tremors()
    {
        if ( m_TremorInProgress )
        {
            GetGame().GetPlayer().GetCurrentCamera().SpawnCameraShake( Math.RandomFloat( 0.2, 1.6 ), 2.0, 5.0, Math.RandomFloat( 3.2, 8.6 ) );
            m_TremorCounter++;
        }

        if ( m_TremorCounter > 25 )
        {
            m_TremorInProgress = false;
            m_TremorCounter = 0;
        }
    }
    protected void SandstormLightning()
    {
        float rollDice = Math.RandomInt( 0, 100 );
        if ( rollDice < ( m_LightningDensity * 100 ) )
        {
            int lightningID = Math.RandomInt( 1, 10 );
            switch ( lightningID )
            {
                case 1:
                    lightningID = ParticleList.LIGHTNING_1;
                    break;
                case 2:
                    lightningID = ParticleList.LIGHTNING_2;
                    break;
                case 3:
                    lightningID = ParticleList.LIGHTNING_3;
                    break;
                case 4:
                    lightningID = ParticleList.LIGHTNING_4;
                    break;
                case 5:
                    lightningID = ParticleList.LIGHTNING_5;
                    break;
                case 6:
                    lightningID = ParticleList.LIGHTNING_6;
                    break;
                case 7:
                    lightningID = ParticleList.LIGHTNING_7;
                    break;
                case 8:
                    lightningID = ParticleList.LIGHTNING_8;
                    break;
                case 9:
                    lightningID = ParticleList.LIGHTNING_9;
                    break;
                default:
                    lightningID = ParticleList.LIGHTNING_6;
                    break;
            }
            // choose position
            int index;
            float volume;
            vector lightningPos;
            if ( GetLantiaState() == ELantia.SANDSTORM_LIGHT )
            {
                index = Math.RandomInt( 0, 23 );
                volume = 0.15;
                lightningPos = m_sandstorm_outer[ index ];
            }
            else
            {
                index = Math.RandomInt( 0, 13 );
                volume = 0.25;
                lightningPos = m_lightning[ index ];
            }

            // create particle bolt + sound + light
            Particle lightning = ParticleManager.GetInstance().PlayInWorld( lightningID, lightningPos );
            lightning.SetOrientation( Vector( 0, Math.RandomFloat( 0, 180 ), 0 ) );
            PlayEnvironmentSoundStr( "ThunderHeavy_Near_SoundSet", lightningPos, volume );
            ScriptedLightBase.CreateLight( BlowoutLightning, lightningPos );
        }
    }
    protected void ScriptedTremors()
    {
        if ( m_Player )
        {
            m_Player.GetCurrentCamera().SpawnCameraShake( Math.RandomFloat( 0.2, 0.4 ), 2.0, 5.0, Math.RandomFloat( 3.2, 8.6 ) );
            Sleep( 1500 );
            m_Player.GetCurrentCamera().SpawnCameraShake( Math.RandomFloat( 0.4, 1.2 ), 2.0, 5.0, Math.RandomFloat( 3.2, 8.6 ) );
            Sleep( 1500 );
            m_Player.GetCurrentCamera().SpawnCameraShake( Math.RandomFloat( 1.2, 1.6 ), 2.0, 5.0, Math.RandomFloat( 3.2, 8.6 ) );
            Sleep( 1000 );
            m_Player.GetCurrentCamera().SpawnCameraShake( Math.RandomFloat( 1.6, 3.6 ), 2.0, 5.0, Math.RandomFloat( 3.2, 8.6 ) );
        }
    }
    protected void SpawnSandstormOuterCircle()
    {
        int s = 0;
        // outer
        for ( s = 0; s < 23; s++ )
        {
            if ( !m_SandstormOuter[s] )
            {
                m_SandstormOuter[s] = GenericVFX.Cast( GetGame().CreateObjectEx( "vfx_lantia_sandstorm", m_sandstorm_outer[s], ECE_LOCAL ) );
                if ( m_SandstormOuter[s] )
                    m_SandstormOuter[s].vfx_start();
            }
        }
    }
    protected void DespawnSandstormOuterCircle()
    {
        int s = 0;
        // outer
        for ( s = 0; s < 23; s++ )
        {
            if ( m_SandstormOuter[s] )
                GetGame().ObjectDelete( m_SandstormOuter[s] );
        }
    }
    protected void SpawnSandstormInnerCircle()
    {
        int s = 0;
        // inner
        for ( s = 0; s < 18; s++ )
        {
            if ( !m_SandstormInner[s] )
            {
                m_SandstormInner[s] = GenericVFX.Cast( GetGame().CreateObjectEx( "vfx_lantia_sandstorm", m_sandstorm_inner[s], ECE_LOCAL ) );
                if ( m_SandstormInner[s] )
                    m_SandstormInner[s].vfx_start();
            }
        }
    }
    protected void DespawnSandstormInnerCircle()
    {
        int s = 0;
        // inner
        for ( s = 0; s < 18; s++ )
        {
            if ( m_SandstormInner[s] )
                GetGame().ObjectDelete( m_SandstormInner[s] );
        }
    }
    protected void ToggleLanteranSpires( bool state )
    {
        array<Object> lantia_spires = new array<Object>;
        int i = 0;

        for ( i = 0; i < 6; i++ )
        {
            GetGame().GetObjectsAtPosition( m_spires1[i], 1, lantia_spires, null );
            if ( lantia_spires[0] )
            {
                land_spire_large_1 spire_proper1 = land_spire_large_1.Cast( lantia_spires[0] );
                if ( state )
                    spire_proper1.SetObjectMaterial( 0, "nst\\ns3\\structures\\spikes\\data\\spike_large_1_em.rvmat" );
                else
                    spire_proper1.SetObjectMaterial( 0, "nst\\ns3\\structures\\spikes\\data\\spike_large_1.rvmat" );
            }
        }

        for ( i = 0; i < 5; i++ )
        {
            GetGame().GetObjectsAtPosition( m_spires2[i], 1, lantia_spires, null );
            if ( lantia_spires[0] )
            {
                land_spire_large_2 spire_proper2 = land_spire_large_2.Cast( lantia_spires[0] );
                if ( state )
                    spire_proper2.SetObjectMaterial( 0, "nst\\ns3\\structures\\spikes\\data\\spike_large_1_em.rvmat" );
                else
                    spire_proper2.SetObjectMaterial( 0, "nst\\ns3\\structures\\spikes\\data\\spike_large_1.rvmat" );
            }
        }

        for ( i = 0; i < 30; i++ )
        {
            GetGame().GetObjectsAtPosition( m_spires3[i], 1, lantia_spires, null );
            if ( lantia_spires[0] )
            {
                land_spire_large_3 spire_proper3 = land_spire_large_3.Cast( lantia_spires[0] );
                if ( state )
                    spire_proper3.SetObjectMaterial( 0, "nst\\ns3\\structures\\spikes\\data\\spike_large_1_em.rvmat" );
                else
                    spire_proper3.SetObjectMaterial( 0, "nst\\ns3\\structures\\spikes\\data\\spike_large_1.rvmat" );
            }
        }
    
        for ( i = 0; i < 22; i++ )
        {
            GetGame().GetObjectsAtPosition( m_spires4[i], 1, lantia_spires, null );
            if ( lantia_spires[0] )
            {
                land_spire_large_4 spire_proper4 = land_spire_large_4.Cast( lantia_spires[0] );
                if ( state )
                    spire_proper4.SetObjectMaterial( 0, "nst\\ns3\\structures\\spikes\\data\\spike_large_1_em.rvmat" );
                else
                    spire_proper4.SetObjectMaterial( 0, "nst\\ns3\\structures\\spikes\\data\\spike_large_1.rvmat" );
            }
        }
    }
    void PlayLantiaMusic()
    {
        if ( m_Player )
        {
            if ( m_Player.GetDynamicMusic() )
            {
                m_Player.GetDynamicMusic().PlayLantiaMusic();
            }
        }
    }
    void StopLantiaMusic()
    {
        if ( m_Player )
        {
            if ( m_Player.GetDynamicMusic() )
            {
                m_Player.GetDynamicMusic().StopLantiaMusic();
            }
        }
    }
    EffectSound PlaySoundOnPlayerStr(string sound, float volume = 1, bool loop = false)
    {
        EffectSound effect_sound;
        if (GetGame().IsClient() || !GetGame().IsMultiplayer()) {
            effect_sound = SEffectManager.PlaySoundOnObject(sound, GetGame().GetPlayer(), 0, 0, loop);
            if (!effect_sound)
            {
                return effect_sound;
            }
            
            effect_sound.SetParent(GetGame().GetPlayer());
            effect_sound.SetSoundAutodestroy(true);
            effect_sound.GetSoundObject().SetVolume(volume);    
        }
        
        return effect_sound;
    }
    EffectSound PlayEnvironmentSoundStr(string sound, vector position, float volume, bool loop = false, Object parent = null)
    {
        EffectSound effect_sound;
        if (GetGame().IsClient() || !GetGame().IsMultiplayer()) {   
                
            effect_sound = SEffectManager.PlaySound( sound, position, 0, 0, loop);
            if (!effect_sound)
            {
                return effect_sound;
            }
            
            effect_sound.SetParent(parent);
            effect_sound.GetSoundObject().SetVolume(volume);
            effect_sound.SetSoundAutodestroy(true);

        }
        
        return effect_sound;
    }
    void OnTeleportClient()
    {
        thread LerpTeleportEffect( 0.75, 0, 5000 );
    }

    // DEBUG
    // Call this for offline testing:
    //  MissionGameplay.Cast(GetGame().GetMission()).GetLantiaControllerClient().Debug_KICKITOFF();
    void Debug_KICKITOFF()
    {
        thread Debug_LantiaSequence();
    }
    void Debug_LantiaSequence()
    {
        SwitchLantiaState( ELantia.DORMANT );
        Sleep( 10000 );
        SwitchLantiaState( ELantia.A3INIT );
        Sleep( 120000 );
        SwitchLantiaState( ELantia.A3ACTIVE );
        Sleep( 5000 );
        SwitchLantiaState( ELantia.WINDOW_OPEN );
        Sleep( 30000 );
        SwitchLantiaState( ELantia.PROBES_DETECT );
        Sleep( 60000 );
        SwitchLantiaState( ELantia.LINK_WEAKENING );
        Sleep( 30000 );
        SwitchLantiaState( ELantia.LINK_UNSTABLE );
        Sleep( 20000 );
        SwitchLantiaState( ELantia.LINK_PREATTACK );
        Sleep( 4000 );
        SwitchLantiaState( ELantia.LINK_ATTACK );
        Sleep( 40000 );
        SwitchLantiaState( ELantia.LINK_PHASING );
        Sleep( 66000 );
        SwitchLantiaState( ELantia.EARTHQUAKE );
        Sleep( 4000 );
        SwitchLantiaState( ELantia.SANDSTORM_LIGHT );
        Sleep( 120000 );
        SwitchLantiaState( ELantia.SANDSTORM_HEAVY );
        Sleep( 60000 );
        SwitchLantiaState( ELantia.WINDOW_END );
    }
}

class LantiaLinkActiveLight: ScriptedLightBase
{
    void LantiaLinkActiveLight()
    {
        SetLightType( LightSourceType.PointLight );
        
        SetVisibleDuringDaylight( true );
        SetFlareVisible( false );
        SetCastShadow( true );
        
        SetDiffuseColor( 0.3, 0.3, 1 );
        SetAmbientColor( 0.3, 0.3, 1 );
        
        SetRadiusTo( 35 );
        SetBrightnessTo( 300 );
    }
}
class LantiaLinkAttackLight: ScriptedLightBase
{
    void LantiaLinkAttackLight()
    {
        SetLightType( LightSourceType.PointLight );
        
        SetVisibleDuringDaylight( true );
        SetFlareVisible( false );
        SetCastShadow( true );
        
        SetDiffuseColor( 1.0, 0.15, 0.05 );
        SetAmbientColor( 1.0, 0.15, 0.05 );
        
        SetRadiusTo( 35 );
        SetBrightnessTo( 300 );

        SetFlickerAmplitude( 0.55 );
        SetFlickerSpeed( 0.75 ); 
    }
}
class LantiaLinkPhasedLight: ScriptedLightBase
{
    void LantiaLinkPhasedLight()
    {
        SetLightType( LightSourceType.PointLight );
        
        SetVisibleDuringDaylight( true );
        SetFlareVisible( false );
        SetCastShadow( true );
        
        SetDiffuseColor( 1.0, 0.15, 0.05 );
        SetAmbientColor( 1.0, 0.15, 0.05 );
        
        SetRadiusTo( 35 );
        SetBrightnessTo( 300 );

        SetFlickerAmplitude( 0.85 );
        SetFlickerSpeed( 40.75 ); 
    }
}
class Athena3LinkActiveLight: ScriptedLightBase
{
    void Athena3LinkActiveLight()
    {
        SetLightType( LightSourceType.PointLight );
        
        SetVisibleDuringDaylight( true );
        SetFlareVisible( false );
        SetCastShadow( true );
        
        SetDiffuseColor( 0.3, 0.3, 1 );
        SetAmbientColor( 0.3, 0.3, 1 );
        
        SetRadiusTo( 15 );
        SetBrightnessTo( 30 );
    }
}
class Athena3LinkAttackLight: ScriptedLightBase
{
    void Athena3LinkAttackLight()
    {
        SetLightType( LightSourceType.PointLight );
        
        SetVisibleDuringDaylight( true );
        SetFlareVisible( false );
        SetCastShadow( true );
        
        SetDiffuseColor( 1.0, 0.15, 0.05 );
        SetAmbientColor( 1.0, 0.15, 0.05 );
        
        SetRadiusTo( 15 );
        SetBrightnessTo( 30 );

        SetFlickerAmplitude( 0.55 );
        SetFlickerSpeed( 0.75 ); 
    }
}
class Athena3LinkPhasedLight: ScriptedLightBase
{
    void Athena3LinkPhasedLight()
    {
        SetLightType( LightSourceType.PointLight );
        
        SetVisibleDuringDaylight( true );
        SetFlareVisible( false );
        SetCastShadow( true );
        
        SetDiffuseColor( 1.0, 0.15, 0.05 );
        SetAmbientColor( 1.0, 0.15, 0.05 );
        
        SetRadiusTo( 15 );
        SetBrightnessTo( 30 );

        SetFlickerAmplitude( 0.85 );
        SetFlickerSpeed( 40.75 ); 
    }
}
class KeyholeObservatoryAirlockLight extends PointLightBase
{
    static float m_DefaultBrightness = 3.5;
    static float m_DefaultRadius = 5.5;
    
    void KeyholeObservatoryAirlockLight()
    {
        SetVisibleDuringDaylight(true);
        SetRadiusTo(m_DefaultRadius);
        SetBrightnessTo(m_DefaultBrightness);
        SetCastShadow(false);
        SetAmbientColor(1.0, 0.2, 0.0);
        SetDiffuseColor(1.0, 0.2, 0.0);
        SetFlareVisible(false);
    }
}
class LantiaTubeLight extends PointLightBase
{
    static float m_DefaultBrightness = 3.5;
    static float m_DefaultRadius = 5.5;
    
    void LantiaTubeLight()
    {
        SetVisibleDuringDaylight(true);
        SetRadiusTo(m_DefaultRadius);
        SetBrightnessTo(m_DefaultBrightness);
        SetCastShadow(false);
        SetAmbientColor(0.7, 0.85, 1.0);
        SetDiffuseColor(0.7, 0.85, 1.0);
        SetFlareVisible(false);
    }
}
class LantiaCageLight extends PointLightBase
{
    static float m_DefaultBrightness = 2.5;
    static float m_DefaultRadius = 20.5;
    
    void LantiaCageLight()
    {
        SetVisibleDuringDaylight(true);
        SetRadiusTo(m_DefaultRadius);
        SetBrightnessTo(m_DefaultBrightness);
        SetCastShadow(false);
        SetAmbientColor(0.7, 0.85, 1.0);
        SetDiffuseColor(0.7, 0.85, 1.0);
        SetFlareVisible(false);
    }
}