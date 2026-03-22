/*
*
*	EVR Emission Storms for the Namalsk Survival Pack
*
*	Few notes on how this works:
*		- All objects created are client side. (Due to the massive lag that would occur if
*		we move a server-side object at 200km/h)
*		The server sends all commands to the client on what to do to the "Object"
*
*		- MidPhaseServer is the actual event occuring. IF you plan on modifying it,
*		then m_MidPhaseLength MUST be adjusted accordingly. 
*
*	You are free to mod this as you please. 
*	Just credit @InclementDab for the scripting and @NamalskSurvivor for the modeling
*	
*	That being said you are NOT free to redistribute / repack this into
*	your own mod. 
*
*/

//#define NAMALSK_SURVIVAL_DEBUG

static const ref array<string> LIGHTNING_BOLT_TYPES = { "vfx_thunderboltnorm", "vfx_thunderboltheavy" };

class EVRStorm: NamWeatherEvent
{	
	static const int COLORIZE_SKY = 99;
	static const int COLORIZE_FLASH = 150;
			
	protected vector m_Position;
	protected vector m_TeleportPosition;
	
	protected ref array<Object> m_TeleportedObjects = {};
	protected float m_AnomalyScale;
	protected vector m_AnomalyPosition;
	protected bool m_CanTeleport = true;
	protected bool m_ForceRandomTeleport = false;
	protected ref Timer m_ServerScaleTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
	//protected ref Timer m_AreaDamageTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
	protected ContaminatedArea_Athena1 m_A1_Contamination;
	protected float m_Colour_SkyOrang;
	
	// Client side only
	protected PPERequester_NamEVRColours m_PPE_EVRColours;
	
	protected ref MaterialEffect m_MatGlow;
	protected ref MaterialEffect m_MatBlur;
	protected ref MaterialEffect m_MatChroma;
		
	protected vfx_the_thing m_BlowoutAnomalyObject;
	protected BlowoutStaticLight m_BlowoutStaticLight;
	protected ref array<AbstractWave> m_AlarmSounds = {};
	protected ref array<ref EVRRockEntry> m_EVRRockEntries = {};
	protected ref Timer m_HitPhaseTimer = new Timer(CALL_CATEGORY_SYSTEM);
	protected ref Timer m_VoicesTimer = new Timer(CALL_CATEGORY_SYSTEM);
		
	void EVRStorm()
	{		
		m_Position = GetEventPosition();
		m_AnomalyPosition = m_Position;
		if (GetGame().IsClient() || !GetGame().IsMultiplayer()) {
			
			m_MatBlur = new MaterialEffect("graphics/materials/postprocess/gauss");
			m_MatGlow = new MaterialEffect("graphics/materials/postprocess/glow");
			m_MatChroma = new MaterialEffect("graphics/materials/postprocess/chromaber");
			//PPEffects.m_ColorizeEffects.Set(COLORIZE_SKY, {0.0, 0.0, 0.0});
			//PPEffects.m_ColorizeEffects.Set(COLORIZE_FLASH, {0.0, 0.0, 0.0});
			// register PPE
			Class.CastTo( m_PPE_EVRColours, PPERequesterBank.GetRequester( PPERequester_NamEVRColours ) );
			// start PPE
			if ( m_PPE_EVRColours )
				m_PPE_EVRColours.Start();
			
			// Starts light at site
			vector ground_pos = m_Position;
			ground_pos[1] = GetGame().SurfaceY(m_Position[0], m_Position[2]) + 1;
			m_BlowoutStaticLight = BlowoutStaticLight.Cast(BlowoutStaticLight.CreateLight(BlowoutStaticLight, ground_pos, 10));
				
			// Create Anomaly Object at height of platform
			m_BlowoutAnomalyObject = vfx_the_thing.Cast(GetGame().CreateObjectEx("vfx_the_thing", m_Position, ECE_LOCAL));
			m_BlowoutAnomalyObject.SetScale(0.01);
			m_BlowoutAnomalyObject.SetVolume(0.01);
			
			// Set Zero-G buildings
			array<Object> object_data = {};
			array<CargoBase> proxy_data = {};
			GetGame().GetObjectsAtPosition3D(m_Position, 75, object_data, proxy_data);
			
			foreach (Object obj: object_data) {
				// Store initial positions
				if (obj && obj.GetType().Contains("Static_stone")) {
					m_EVRRockEntries.Insert(new EVRRockEntry(obj));
				}
			}
			
			// Set Alarm Positions
			array<vector> alarm_positions = {};
			GetAlarmPositions(alarm_positions);
			foreach (vector pos: alarm_positions) {
				//m_AlarmSounds.Insert(PlayEnvironmentSound(BlowoutSound.Blowout_Alarm, pos, 1, 0));
			}
		} 
		
		if (GetGame().IsServer()) {
			m_TeleportPosition = GetRandomTeleportPosition();
			//m_AreaDamageTimer.Run(6.0, this, "OnAreaDamageUpdate", null, true);
			m_Colour_SkyOrang = 0.23;
		}
	}
	
	void ~EVRStorm()
	{
		EventInfo("~EVRStorm");
				
		if (GetGame().IsClient() || !GetGame().IsMultiplayer()) {
											
			if (m_BlowoutStaticLight) {
				m_BlowoutStaticLight.FadeOut(10);
			}
			
			if (m_BlowoutAnomalyObject) {
				m_BlowoutAnomalyObject.DeleteOnClient();
			}
			
			delete m_MatGlow;
			delete m_MatChroma;
			delete m_MatBlur;
			
			// stop PPE
			if ( m_PPE_EVRColours )
				m_PPE_EVRColours.Stop();
		}
		
		delete m_HitPhaseTimer;
		delete m_VoicesTimer;
		delete m_EVRRockEntries;
		delete m_AlarmSounds;
		delete m_TeleportedObjects;
		delete m_ServerScaleTimer;
		//delete m_AreaDamageTimer;
	}
		
	override void EventInit()
	{
		super.EventInit();

		m_InitPhaseLength = EVRConstants.STARTUP_LENGTH;
		// Must represent the total time of all the Sleep() calls in MidPhaseServer (or more)
		m_MidPhaseLength = 90.0; 
		m_EndPhaseLength = 220.0;
	
#ifdef NAMALSK_SURVIVAL_DEBUG
		m_InitPhaseLength *= 0.15;
		m_EndPhaseLength *= 0.15;
#endif
	}
		
	override void UpdateServer()
	{		
		// This code is really messy and could probably be optimized alot
		// you have been warned - dab				
		if (m_CanTeleport && EVRConstants.ALLOW_TELEPORTING) {
			array<Object> objects = {};
			array<CargoBase> cargos = {};
			GetGame().GetObjectsAtPosition3D(m_AnomalyPosition, m_AnomalyScale * 4.5 * 1.5, objects, cargos);
			foreach (Object object: objects) {
				Transport transport;
				PlayerBase player;
				
				// Dont want to teleport buildings, or things like #animator and #particlesourceenf
				if (object.IsInherited(BuildingSuper) || object.GetType()[0] == "#") {
					continue;
				}
				
				// Get HierarchyRoot
				Object parent = object;
				while (parent) {
					object = parent;
					parent = Object.Cast(object.GetParent());
				}
								
				// Save object that was teleported so sound is only played once
				if (m_TeleportedObjects.Find(object) == -1) {
					m_TeleportedObjects.Insert(object);
					PlayEnvironmentSoundServer(BlowoutSound.Blowout_Teleport, object.GetPosition()); // m_Position... probably switch to m_AnomalyPosition
					PlayTeleportFlashServer(object.GetPosition() + vector.Direction(m_AnomalyPosition, object.GetPosition()) * 0.25);
				}

				int apsi_warning_color = COLOR_RED;
				string apsi_warning_text = "LINK NOT ESTABLISHED";
				// bout tree fiddy
				// Stable anomaly, A3 exists, we arent already in A3, and its not a car.... phew
				if (IsAnomalyStable() && !m_ForceRandomTeleport && GetA3Chamber() && !Class.CastTo(transport, object)) {
					TeleportObject( object, GetA3Chamber().GetTeleportPosition(), IsAnomalyStable() );
					apsi_warning_color = COLOR_GREEN;
					apsi_warning_text = "TELEPORTATION SUCCESSFUL";
				} else {
					TeleportObject( object, m_TeleportPosition, IsAnomalyStable() );
				}
				
				// This is only the case when the orb is in A3 already
				if (m_ForceRandomTeleport) {
					continue;
				}
				
				if (Class.CastTo(player, object)) {
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PlayAPSIWarningSignalToPlayer, 2000, false, apsi_warning_text, apsi_warning_color, player);
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PlayLEHSWarningSignalToPlayer, 2000, false, apsi_warning_text, apsi_warning_color, player);
				}
				
				// This is edge-casey, but if you are wearing an APSI in a car, we want to
				// warn the passengers
				if (Class.CastTo(transport, object)) {
					
					// We arent teleporting to A3 if you are in a car
					if (IsAnomalyStable()) {
						apsi_warning_color = COLOR_YELLOW;
						apsi_warning_text = "TELEPORTATION OVERLOAD";
					}
					
					for (int i = 0; i < transport.CrewSize(); i++) {
						Human human = transport.CrewMember(i);
						if (human) {
							GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PlayAPSIWarningSignalToPlayer, 2000, false, apsi_warning_text, apsi_warning_color, human);
							GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PlayLEHSWarningSignalToPlayer, 2000, false, apsi_warning_text, apsi_warning_color, human);
						}
					}
				}
			}
		}
	}
	
	override void UpdateClient()
	{
		super.UpdateClient();
		
		// *should* make loading faster for people jumping through the stable wormhole
		if (DistanceFromCenter(GetGame().GetPlayer()) < 200) {
			GetGame().PreloadObject("land_a3_chamber", 3000);
			GetGame().PreloadObject("land_a3_flaxbt", 3000);
			GetGame().PreloadObject("land_a3_lab_sink", 3000);
			GetGame().PreloadObject("land_a3_tube_sec_upper", 3000);
			GetGame().PreloadObject("land_a3_tube_sec_bottom", 3000);
			GetGame().PreloadObject("land_a3_tube_main_upper", 3000);
			GetGame().PreloadObject("land_a3_tube_main_bottom", 3000);
			GetGame().PreloadObject("land_a3_offices", 3000);
			GetGame().PreloadObject("land_a3_expedition", 3000);
		}
	}
			
	override void InitPhaseServer()
	{			
		EventInfo("InitPhaseServer");
		
		thread PlayAPSIWarningSignal("! ANOMALY DETECTED !", COLOR_GREEN);
		thread PlayLEHSWarningSignal("A1 activity", COLOR_GREEN);
		
		// Turn A3 Chamber On
		if (GetA3Chamber()) {
			GetA3Chamber().SetBuildingState(AthenaBuildingState.STARTUP);
		}
		
		// Turn A2 into startup
		if (GetBasement3rd()) {
			GetBasement3rd().SetBuildingState(AthenaBuildingState.STARTUP);
		}
		
		// This is like a manual way of lerping the server scale properly, since the client calls LerpScale on InitPhaseClient
		if (m_ServerScaleTimer.IsRunning()) {
			m_ServerScaleTimer.Stop();
		}
		
		m_ServerScaleTimer.Run(0.01, this, "RunScaleLerp", new Param4<float, float, float, int>(0.01, 0.65, m_InitPhaseLength, GetGame().GetTime()), true);
				
		super.InitPhaseServer();
	}
			
	override void MidPhaseServer()
	{		
		EventInfo("MidPhaseServer");	
		thread StartHitPhaseServer(m_MidPhaseLength);
		Sleep(25000);
		thread PlayEnvironmentSoundServer(BlowoutSound.Blowout_NearImpact, m_Position); 
		
		if (GetA3Chamber()) {
			GetA3Chamber().SetBuildingState(AthenaBuildingState.ACTIVE);
		}
		
		Sleep(5000);
		
		if (GetA3Chamber()) {
			thread PlayEnvironmentSoundServer(BlowoutSound.A3_Preboom, GetA3Chamber().GetAnomalyPosition());
		}
		
		Sleep(1000);
		
		// Lerp anomaly into the air
		thread LerpAnomalyPositionServer(m_Position + "0 2000 0", 1);
		// Additional safety check
		m_CanTeleport = false;
		//m_AreaDamageTimer.Stop();
		
		// Starts sky color change
		thread PlayAPSIWarningSignal("! DANGER !", COLOR_YELLOW);
		thread PlayLEHSWarningSignal("EVR danger", COLOR_YELLOW);
		
		Sleep(3600);
		thread LerpColorizationServer(Vector(0.5, 0.15, 0.15), 100, Vector(1, 1, 1)); //Vector(0.5, 0.15, 0.15)
		thread CreateBlowoutServer(0.8);
		
		// Move anomaly to ???
		if (GetA3Chamber()) {
			m_CanTeleport = true;
			m_ForceRandomTeleport = true;
			thread LerpAnomalyScaleServer(0.01, 0.375, 0.5);
			thread SetAnomalyPosition(GetA3Chamber().GetAnomalyPosition());
			thread PlayEnvironmentSoundServer(BlowoutSound.Blowout_Wormhole_AttackA3, GetA3Chamber().GetAnomalyPosition());
		}
				
		Sleep(6000);
		
		// Turn sky orange
		//Print( m_Colour_SkyOrang );
		thread LerpColorizationServer(Vector( m_Colour_SkyOrang, 0.05, 0.005 ), 14600, Vector(0.5, 0.15, 0.15));
		thread PlaySoundOnPlayerServer(BlowoutSound.Blowout_Bass, 1); 
		
		Sleep(25000);
		
		// Actual Blowout Event		
		thread CreateBlowoutServer(0.25);
		Sleep(4500); // 3600
		thread CreateBlowoutServer(0.5);
		Sleep(4500);
		thread CreateBlowoutServer(0.75);
		
		// Bring anomaly back from ???
		if (GetA3Chamber()) {
			GetA3Chamber().SetBuildingState(AthenaBuildingState.INACTIVE);
			m_CanTeleport = false;
			m_ForceRandomTeleport = false;
			thread SetAnomalyPosition(m_Position + "0 2000 0");
			thread PlayEnvironmentSoundServer(BlowoutSound.Blowout_Wormhole_AttackA3, GetA3Chamber().GetAnomalyPosition());
		}
		
		
		Sleep(4500 - 2200); // Timing
		
		// Slam anomaly back into ground
		thread PlayAPSIWarningSignal("! DANGER !", COLOR_RED);
		thread PlayLEHSWarningSignal("EVR danger", COLOR_RED);
		thread LerpAnomalyPositionServer(Vector(m_Position[0], GetGame().SurfaceY(m_Position[0], m_Position[2]) - 15, m_Position[2]), 1.6);
		thread PlayEnvironmentSoundServer(BlowoutSound.Blowout_Reentry, m_Position);		
		Sleep(2200);
			
		// Final blowout
		CreateBlowoutFinalServer(); // dont thread this
		Sleep(100); // Avoids crashes
		
		// Shutdown flax core on final BOOM
		if (IsAnomalyStable() && GetFlaxCore()) {
			GetFlaxCore().SetBuildingState(AthenaBuildingState.INACTIVE);
		}
				
		array<Man> players = {};
		GetGame().GetPlayers(players);	
		foreach (Man player: players) {
			SurvivorBase survivor;
			if (Class.CastTo(survivor, player)) {
				OnEVRFinished(survivor);
			}
		}
	}
		
	override void EndPhaseServer()
	{
		EventInfo("EndPhaseServer");
		
		//m_wObject.GetOvercast().Set(m_ovFore, m_EndPhaseLength, 0);
		
		thread LerpColorizationServer(Vector(1, 1, 1), m_EndPhaseLength * 1000, Vector( m_Colour_SkyOrang, 0.05, 0.005 ));
		Sleep(100);
		thread LerpAnomalyScaleServer(0.25, 0.001, 5);
		
		// Turn reactor after storm finishes		
		if (GetBasement3rd()) {
			GetBasement3rd().SetBuildingState(AthenaBuildingState.INACTIVE);
		}
		
		if ( GetContaminatedA1() )
		{
			GetGame().ObjectDelete( GetContaminatedA1() );
		}
		
		// We are a NamWeatherEvent :)
		super.EndPhaseServer();
	}
			
	override void InitPhaseClient(float phase_time) 
	{
		EventInfo("InitPhaseClient");
		foreach (EVRRockEntry entry: m_EVRRockEntries) {
			entry.LerpToSky(phase_time / 2);
		}
		
		m_BlowoutAnomalyObject.LerpScale(0.01, 0.65, phase_time);
		
		// 25 is the number of times we want to hear voices
		m_VoicesTimer.Run(m_InitPhaseLength / 25, this, "OnVoicesUpdate", new Param1<float>(m_InitPhaseLength / 25), true);
	}
	
	override void MidPhaseClient(float phase_time)
	{
		EventInfo("MidPhaseClient");
		m_VoicesTimer.Stop();		
		
		// This is important for JIP users to feel synced properly
		m_BlowoutAnomalyObject.SetScale(0.65);
		m_BlowoutAnomalyObject.SetVolume(1);
	}
	
	override void EndPhaseClient(float phase_time)
	{
		EventInfo("EndPhaseClient");
				
		foreach (AbstractWave alarm: m_AlarmSounds) {
			if (alarm) {
				//alarm.Stop();
			}
		}
	}
	
	void OnEVRFinished( notnull SurvivorBase player )
	{
		float current_shock = player.GetHealth("", "Shock");
			
		// Knock players out on event
		float shock_value = EVRConstants.SHOCK_WHEN_UNSAFE;
		
		//Print("---------------");
		if ( APSICheck( player ) || LEHSCheck( player ) )
		{
			shock_value += 20.0;
			//Print( player );
			//Print( " PROTECTED ");
		} 
		
		if ( player.m_Environment.IsSafeFromEVR() )
		{
			shock_value += 7.0;
			//Print( player );
			//Print( " SAFE ENVIRO ");
		}
		
		if ( player.IsPlayerInStance( DayZPlayerConstants.STANCEMASK_PRONE ) )
		{
			shock_value += 2.0;
			//Print( player );
			//Print( " IS PRONE ");
		}
		
		// If you are in the "safe bunkers"
		if ( UndergroundCheck( player ) )
		{
			shock_value += 20.0;
			//Print( player );
			//Print( " IS UNDERGROUND ");
		}

		//! Shock value handled
		if ( shock_value < current_shock )
		{
			/*Print("---------------");
			Print( shock_value );
			Print( current_shock );
			Print("---------------");*/
			player.SetHealth("", "Shock", shock_value);
		}
		
		//! Handle Stamina Value
		player.GetStaminaHandler().SetStamina( 0 );
					
		//! Handle health value
		if ( APSICheck( player ) || LEHSCheck( player ) )
		{
			dzn_apsi apsi = GetAPSI( player );
			if ( apsi && apsi.IsSwitchedOn() )
				apsi.AddHealth( "", "Health", -EVRConstants.APSI_DAMAGE );
			return;
		}
		
		if ( player.m_Environment.IsSafeFromEVR() )
		{
			return;
		}
		
		// if you are not safe during the event, yeet some HP
		player.AddHealth( "", "Health", -EVRConstants.DAMAGE_WHEN_UNSAFE );
		
		// Welp son. you fucked up
		if ( DistanceFromCenter( player ) < EVRConstants.ANOMALY_KILL_RADIUS )
		{
			player.SetHealth( "", "Health", 0 );
		}
	}
		
	void CreateBlowoutServer( float intensity )
	{
		EventDebug( "CreateBlowoutServer" );
		array<Man> players = {};
		GetGame().GetPlayers( players );
		foreach ( Man man: players )
		{		
			PlayerBase player;
			if ( Class.CastTo( player, man ) && !player.m_Environment.IsSafeFromEVR() && !APSICheck( player ) && !LEHSCheck( player ) )
			{
				if (intensity >= 0.50)
				{
					player.RequestSoundEvent( EPlayerSoundEventID.TAKING_DMG_HEAVY );
				} 
				else
				{
					player.RequestSoundEvent( EPlayerSoundEventID.TAKING_DMG_LIGHT );
				}
				
				player.AddHealth( "", "Health", -intensity * ( EVRConstants.DAMAGE_WHEN_UNSAFE / 10 ) );
				// Deplete stamina when the stomps come to town
				player.GetStaminaHandler().SetStamina( player.GetStaminaHandler().GetStamina() - ( intensity * 100 ) );
			}
		}
		
		GetGame().RPCSingleParam( null, ERPCsBlowoutStorm.BLOWOUT_RPC_CREATE_BLOWOUT, new Param1<float>(intensity), true );
	}
	
	void PlayEnvironmentSoundServer(BlowoutSound sound, vector position, float volume = 1)
	{
		EventDebug("PlayEnvironmentSoundServer");
		GetGame().RPCSingleParam(null, ERPCsBlowoutStorm.BLOWOUT_RPC_PLAY_SOUND_ENVIRONMENT, new Param3<BlowoutSound, vector, float>(sound, position, volume), true);
	}
	
	void PlaySoundOnPlayerServer(BlowoutSound sound, float volume = 1)
	{
		EventDebug("PlaySoundOnPlayerServer");
		GetGame().RPCSingleParam(null, ERPCsBlowoutStorm.BLOWOUT_RPC_PLAY_SOUND_PLAYER, new Param2<BlowoutSound, float>(sound, volume), true);
	}
	
	void CreateBlowoutFinalServer()
	{	
		EventDebug("CreateBlowoutFinalServer");
		array<Man> players = {};
		GetGame().GetPlayers(players);
		foreach (Man player: players) {	
			PlayerBase pb;
			if (Class.CastTo(pb, player)) {
				GetGame().RPCSingleParam(null, ERPCsBlowoutStorm.BLOWOUT_RPC_CREATE_BLOWOUT_FINAL, new Param1<bool>(pb.m_Environment.IsSafeFromEVR()), true, pb.GetIdentity());
			}
		}
	}
	
	void PlayTeleportFlashServer(vector position)
	{
		EventDebug("PlayTeleportFlashServer");
		GetGame().RPCSingleParam(null, ERPCsBlowoutStorm.BLOWOUT_RPC_TELEPORT_FLASH, new Param1<vector>(position), true);
	}
		
	void OnTeleportServer(Object object)
	{
		EventDebug("OnTeleportServer");
		PlayerBase pb;
		if (Class.CastTo(pb, object)) {
			GetGame().RPCSingleParam(null, ERPCsBlowoutStorm.BLOWOUT_RPC_TELEPORT, new Param1<bool>(IsAnomalyStable()), true, pb.GetIdentity());
			// not resiliant enough? VOMIT (maybe find a better way to do this. perhaps if the user is sick at all?)
			//if (pb.GetColdResistance() <= 40) {
				if (Math.RandomIntInclusive(0, 2) == 1 && !IsAnomalyStable()) {
					if ( !APSICheck( pb ) && !LEHSCheck( pb ) ) {
						GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TeleportationSickness, Math.RandomInt(5000, 13000), false, pb);
					}
				}
			//}
		}
		/*
		array<Object> objects = {};
		array<CargoBase> cargos = {};
		GetGame().GetObjectsAtPosition(m_AnomalyPosition, 150, objects, cargos);
		
		foreach (Object found_object: objects) {
			ZombieBase infected;
			if (Class.CastTo(infected, found_object)) {
				// Thanks Jacob_Mango
				//infected.JumpIntoOrb(m_AnomalyPosition); // PauseChamp zombies in orb????
			}
		}*/
	}
		
	private void TeleportationSickness(PlayerBase player)
	{
		SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
		symptom.SetDuration(Math.RandomIntInclusive(4, 8));

		if (player.GetStatWater().Get() > EVRConstants.WATER_DRAIN_FROM_TELEPORTATION_SICKNESS) {
			player.GetStatWater().Add(-1 * EVRConstants.WATER_DRAIN_FROM_TELEPORTATION_SICKNESS);
		}
		
		if (player.GetStatEnergy().Get() > EVRConstants.ENERGY_DRAIN_FROM_TELEPORTATION_SICKNESS) {
			player.GetStatEnergy().Add(-1 * EVRConstants.ENERGY_DRAIN_FROM_TELEPORTATION_SICKNESS);
		}
	}
	
	void OnTeleportClient(bool is_stable)
	{
		PlayerBase pb;
		if ( is_stable ) {
			if ( Class.CastTo( pb, GetGame().GetPlayer() ) )
			{
				if ( pb.GetDynamicMusic() )
				{
					pb.GetDynamicMusic().PlayA3JumpMusic();
				}
			}
		}
		
		if ( !APSICheck() && LEHSCheck() && !is_stable )
		{
			thread PlayEarRingSound();
			thread LerpTeleportEffect( 1, 0, 8000 );
		}
	}
	
	void PlayAPSIWarningSignal( string text, int color )
	{
		EventDebug( "PlayAPSIWarningSignal" );
		array<Man> players = {};
		GetGame().GetPlayers( players );
		foreach ( Man player: players )
		{
			PlayAPSIWarningSignalToPlayer( text, color, PlayerBase.Cast( player ) );
		}
	}
	void PlayLEHSWarningSignal( string text, int color )
	{
		array<Man> players = {};
		GetGame().GetPlayers( players );
		foreach ( Man player: players )
		{
			PlayLEHSWarningSignalToPlayer( text, color, PlayerBase.Cast( player ) );
		}
	}

	void PlayAPSIWarningSignalToPlayer( string text, int color, PlayerBase player )
	{
		EventDebug( "PlayAPSIWarningSignal" );
		dzn_apsi apsi = GetAPSI( player );
		if ( apsi )
		{
			GetGame().RPCSingleParam( apsi, ERPCsAPSI.APSI_RPC_DANGER_MOMENT, new Param2<string, int>( text, color ), true, player.GetIdentity() );
		}
	}
	void PlayLEHSWarningSignalToPlayer( string text, int color, PlayerBase receiver )
	{
		if ( receiver.IsWearingLEHSHelmet() )
		{
			GetGame().RPCSingleParam( receiver.GetLEHSHelmet(), ERPCsLantiaLink.NAM_RPC_LEHS_WARNING, new Param2<string, int>( text, color ), true, receiver.GetIdentity() );
		}
	}

	float m_VoicesTimePassed;
	void OnVoicesUpdate(float timeslice)
	{
		m_VoicesTimePassed += timeslice;
		float pregame_phase = 1 / (m_InitPhaseLength * 1000) * (m_VoicesTimePassed * 1000);
		float inverse_phase = Math.AbsFloat(pregame_phase - 1);
		inverse_phase *= 100;
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

		if ( player && !APSICheck() && !LEHSCheck() )
		{
			//player.RegisterAnomalyVoice(PlayEnvironmentSound(BlowoutSound.Blowout_Drone, m_Position, pregame_phase * 0.1));
			vector voice_pos = RandomizeVector(GetGame().GetPlayer().GetPosition(), inverse_phase + 10, inverse_phase + 75);
			voice_pos[1] = GetGame().SurfaceY(voice_pos[0], voice_pos[2]);
			player.RegisterAnomalyVoice(PlayEnvironmentSound(BlowoutSound.Blowout_Voices, voice_pos, pregame_phase * 0.1));				
		}
				
		// 1 in 4 shot of getting a distant thunder sound
		if (Math.RandomIntInclusive(1, 4) == 3) {
			PlayEnvironmentSound(BlowoutSound.Blowout_Wave, RandomizeVector(GetGame().GetPlayer().GetPosition(), 1, 10), pregame_phase * 0.25);
		}
	}
	
	void OnAreaDamageUpdate()
	{
		array<Man> players = {};
		GetGame().GetPlayers(players);
		
		foreach (Man man: players) {
			PlayerBase player = PlayerBase.Cast(man);
			
			if (!player) {
				continue;
			}
			
			if (vector.Distance(player.GetPosition(), m_AnomalyPosition) > EVRConstants.ANOMALY_KILL_RADIUS * m_AnomalyScale * 1.5) {
				continue;
			}
			
			if ( APSICheck( PlayerBase.Cast( player ) ) || LEHSCheck( PlayerBase.Cast( player ) ) )
			{
				continue;
			}
			
			player.RequestSoundEvent(EPlayerSoundEventID.TAKING_DMG_LIGHT);
			player.AddHealth("", "Shock", -20.0);
			player.AddHealth("GlobalHealth", "Health", -0.45);
		}
	}
			
	void CreateBlowoutClient(float intensity)
	{	
		EventDebug("CreateBlowoutClient " + intensity);
		// Maybe one day
		//ParticleManager.GetInstance().PlayInWorld(ParticleList.VFX_SHOCKWAVE, m_BlowoutAnomalyObject.GetPosition());	
		PlaySoundOnPlayer(BlowoutSound.Blowout_Contact, intensity);
		CreateCameraShake(intensity * 2);
		
		if ( APSICheck() || LEHSCheck() ) {
			return;
		}
		
		m_MatBlur.LerpParam("Intensity", 1 * intensity, 0, 0.75);
		m_MatGlow.LerpParam("Vignette", 0.75 * intensity, 0, 0.75);
		m_MatChroma.LerpParam("PowerX", 0.3 * intensity, -1, 2.5);
		m_MatGlow.LerpParam("Saturation", 0.2, 1, 1);
	}
	
	void CreateBlowoutFinalClient(bool is_safe)
	{
		EventDebug("CreateBlowoutFinalClient");
		thread CreateBlowoutClient(1);
		Sleep(100); // Sound overlap
		PlayEnvironmentSound(BlowoutSound.Blowout_FullWave, GetGame().GetPlayer().GetPosition(), 0.5);
		ParticleManager.GetInstance().PlayInWorld( ParticleList.BLOWOUT_SHOCKWAVE, GetEventPosition() );

		foreach (EVRRockEntry entry: m_EVRRockEntries) {
			entry.LerpToGround(0.2);
		}
		
		if ( !APSICheck() && !is_safe && !LEHSCheck() )
		{			
			thread PlayEarRingSound();
		}
	}
		
	void CreateCameraShake(float intensity)
	{
		GetGame().GetPlayer().GetCurrentCamera().SpawnCameraShake(Math.Clamp(intensity, 0.2, 1), 2, 5, 3.5);
	}
	
	bool IsAnomalyStable()
	{		
		return (GetFlaxCore() && GetFlaxCore().IsReactorOn());
	}
	
	void StartHitPhase(float time)
	{
		EventDebug("Starting hit phase " + time);
		
		m_HitPhaseDeltaTime = 0;
		m_HitPhaseTimer.Run(0.01, this, "RunHitPhase", new Param1<float>(time), true);
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(m_HitPhaseTimer.Stop, time * 1000);
	}
	
	private float m_HitPhaseDeltaTime;
	private void RunHitPhase(float time)
	{
		m_HitPhaseDeltaTime += 0.01;
		
		int factor = Math.Clamp(time / m_HitPhaseDeltaTime, 0, 100);
		int rand = Math.RandomInt(0, 150);
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (factor == rand && player) {
			
			CreateBolt(player.GetPosition());

			//! If player is within the "Danger Zone".... they're gonna die of they stick around
			//! we warn them with chrom abber
			if ( DistanceFromCenter( player ) < EVRConstants.ANOMALY_KILL_RADIUS && !APSICheck() && !LEHSCheck() ) {
				float intensity = Math.Clamp(factor, 0.3, 1);
				m_MatBlur.LerpParam("Intensity", 0.2 * intensity, 0, Math.RandomFloat01());
				m_MatChroma.LerpParam("PowerX", 0.25 * intensity, 0, Math.RandomFloat01());
				//m_MatGlow.LerpParam("Vignette", 0.5 * intensity, 0,  Math.RandomFloat01());
			}			
		}
	}

	void CreateBolt(vector position)
	{		
		EventDebug("CreateBolt " + position);
		
		position = RandomizeVector(position, 100, 350);
		position[1] = GetGame().SurfaceY(position[0], position[2])/* + 500*/;
		
		vector direction = vector.Direction(GetGame().GetPlayer().GetPosition(), position);
		direction.Normalize();
		
		PlayEnvironmentSound(BlowoutSound.Blowout_Hit, GetGame().GetPlayer().GetPosition() + (direction * 100), 0.25);
		
		// If the user is underground, just play the sounds
		bool underground = UndergroundCheck(GetGame().GetPlayer());

		if (!underground) {
			ScriptedLightBase.CreateLight(BlowoutLightFlash, position - ((position - GetGame().GetPlayer().GetPosition()) * 0.5));
			ScriptedLightBase.CreateLight(BlowoutLightning, position);
			
			PlayRandomLightningParticle( position );

			/*Object bolt = GetGame().CreateObjectEx(LIGHTNING_BOLT_TYPES.GetRandomElement(), position, ECE_LOCAL);
			bolt.SetOrientation(Vector(Math.RandomFloat(0, 360), 90, 0));
			bolt.SetScale(Math.RandomFloatInclusive(6, 10));
			bolt.Update();
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, Math.RandomInt(50, 150), false, bolt);*/
		}

		// Randomly create secondary bolt		
		if (Math.RandomIntInclusive(0, 4) != 1) {
			return;
		}
		
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CreateSecondaryBolt, Math.RandomInt(50, 300), false, position.ToString(false), underground);
	}
	
	private void CreateSecondaryBolt(string s_position, bool underground)
	{		
		vector position = s_position.ToVector();
		vector direction = vector.Direction(GetGame().GetPlayer().GetPosition(), position);
		direction.Normalize();
		for (int i = 0; i < Math.RandomIntInclusive(0, 2); i++) {
			
			PlayEnvironmentSound(BlowoutSound.Blowout_Hit, GetGame().GetPlayer().GetPosition() + (direction * 100), 0.1);	
			
			if (!underground) {
				ScriptedLightBase.CreateLight(BlowoutLightFlash, position - ((position - GetGame().GetPlayer().GetPosition()) * 0.5));
				ScriptedLightBase.CreateLight(BlowoutLightning, position);
				
				PlayRandomLightningParticle( position );
				/*Object bolt = GetGame().CreateObjectEx(LIGHTNING_BOLT_TYPES.GetRandomElement(), position, ECE_LOCAL);
				bolt.SetOrientation(Vector(Math.RandomFloat(0, 360), 0, 0));
				bolt.SetScale(Math.RandomFloatInclusive(2, 10));
				bolt.Update();
				
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, Math.RandomInt(50, 250), false, bolt);*/
			}
		}
	}
	
	protected void PlayRandomLightningParticle( vector position )
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

        ParticleManager.GetInstance().PlayInWorld( lightningID, position );
	}

	static void GetAlarmPositions(out array<vector> alarm_positions)
	{
		string world_name;
		GetGame().GetWorldName(world_name);
		string cfg = "CfgWorlds " + world_name + " Names";		
		
		string allowed_types = "Capital City";
		
		for (int i = 0; i < GetGame().ConfigGetChildrenCount(cfg); i++) {
			string city;
			GetGame().ConfigGetChildName(cfg, i, city);			
			vector city_position = GetGame().ConfigGetVector(string.Format("%1 %2 position", cfg, city));
			if (allowed_types.Contains(GetGame().ConfigGetTextOut(string.Format("%1 %2 type", cfg, city)))) {
				alarm_positions.Insert(city_position);
			}
		}
	}
		
	float DistanceFromCenter(Object object)
	{
		return (vector.Distance(object.GetPosition(), m_Position));	
	}

	bool LEHSCheck()
	{
		dzn_lehs_helmet lehs_helmet;
		Class.CastTo( lehs_helmet, GetGame().GetPlayer().GetInventory().FindAttachment( InventorySlots.HEADGEAR ) );

		if ( lehs_helmet && lehs_helmet.HasPower() && !lehs_helmet.IsVisorUp() )
			return true;
		else
			return false;
	}

	static bool LEHSCheck( notnull PlayerBase player )
	{
		dzn_lehs_helmet lehs_helmet;
		Class.CastTo( lehs_helmet, player.GetInventory().FindAttachment( InventorySlots.HEADGEAR ) );

		if ( lehs_helmet && lehs_helmet.HasPower() && !lehs_helmet.IsVisorUp() )
			return true;
		else
			return false;
	}
	
	bool APSICheck()
	{
		dzn_apsi apsi;
		Class.CastTo( apsi, GetGame().GetPlayer().GetInventory().FindAttachment( InventorySlots.EYEWEAR ) );
		if ( apsi && apsi.GetHud() )
			return true;
		else
			return false;
	}
	
	static bool APSICheck(notnull PlayerBase player)
	{
		dzn_apsi apsi;
		Class.CastTo( apsi, player.GetInventory().FindAttachment( InventorySlots.EYEWEAR ) );
		if ( apsi && apsi.IsSwitchedOn() )
			return true;
		else
			return false;
	}
	
	static dzn_apsi GetAPSI(notnull PlayerBase player)
	{
		return dzn_apsi.Cast( player.GetInventory().FindAttachment( InventorySlots.EYEWEAR ) );
	}
		
	void TeleportObject( notnull Object object, vector position, bool stability )
	{
		EventInfo( "Teleporting " + object.GetType() );
		PlayerBase p_base;
		if ( Class.CastTo( p_base, object ) )
		{
			p_base.SetTeleporting( true );
			if ( !stability && !APSICheck( p_base ) && !LEHSCheck( p_base ) )
			{
				object.SetHealth( "", "Shock", PlayerConstants.UNCONSCIOUS_THRESHOLD );
			}

            ScriptJunctureData pCtx = new ScriptJunctureData;
            pCtx.Write( position );
            p_base.SendSyncJuncture( 67, pCtx );
		}
		else
		{
			if ( dBodyIsDynamic( object ) )
			{
				object.SetDynamicPhysicsLifeTime(0.001);
				dBodyActive(object, ActiveState.INACTIVE);
			}
		
			SetWorldPosition(object, position);
			dBodyActive(object, ActiveState.ACTIVE);
			//object.CreateDynamicPhysics(PhxInteractionLayers.DYNAMICITEM);
		}
				
		OnTeleportServer(object);
		
		thread FinishRandomTeleportObject(object, position);
	}
	
	void FinishRandomTeleportObject(Object object, vector position)
	{
		OLinkT link = new OLinkT(object);

		Sleep(500);
		PlayEnvironmentSoundServer(BlowoutSound.Blowout_Teleport, object.GetPosition());
		thread PlayTeleportFlashServer(position);
		
		// Safe
		Sleep(10000);
		PlayerBase p_base;
		if (Class.CastTo(p_base, object))
		{
			p_base.SetTeleporting(false);
		}		
		
		int find_index = m_TeleportedObjects.Find(object);
		if (find_index != -1)
		{
			m_TeleportedObjects.Remove(find_index);
		}
	}
	
	// Grabbed from Community-Online-Tools (thanks Jacob_Mango)
    static void SetWorldPosition(Object object, vector position)
    {
        Object parent = object;
        while (parent) { // find world entity parent, incase we are passing a player on a vehicle or a mod has entities as children.
            object = parent;
            parent = Object.Cast(parent.GetParent());
        }

        Transport transport;
        if (Class.CastTo(transport, object)) { // cast to a transport, they require special logic to lower the chance of collision issues
            vector transform[4];
            transport.GetTransform(transform);

            transform[3] = position;
            
            transport.PlaceOnSurfaceRotated(transform, position, 0, 0, 0, false);
            transport.SetTransform(transform);
            transport.Synchronize();

            return;
        }

        object.SetPosition(position);
    }
	
	vector GetRandomTeleportPosition(float x = 5467, float z = 8660, float radius = 5250)
	{				
		vector position;
		position[0] = Math.RandomInt(x - radius, x + radius);
		position[2] = Math.RandomInt(z - radius, z + radius);
		position[1] = GetGame().SurfaceY(position[0], position[2]) + 1;
		
		if (GetGame().SurfaceIsSea(position[0], position[2]) || GetGame().SurfaceIsPond(position[0], position[2])) {
			// try again
			EventDebug("Landed in water, trying again");
			return GetRandomTeleportPosition(x, z, radius); 
		}
		
		array<Object> position_objects = {};
		array<CargoBase> position_cargos = {};
		GetGame().GetObjectsAtPosition(position, 6, position_objects, position_cargos);
		if (position_objects.Count() > 0) {
			// try again
			EventDebug("Landed in object, trying again");
			return GetRandomTeleportPosition(x, z, radius);
		}
		
		return position;
	}

	// Want to change the orb position? do it here
	override vector GetEventPosition()
	{
		string world_name;
		GetGame().GetWorldName(world_name);
		world_name.ToLower();
		
		switch (world_name) {
			
			case "namalsk": return Vector(3777, 152.5, 8244);
			
			case "deerisle": return Vector(688.5, 60.5, 15488.1);			
			
			// Its like a treasure hunt :)
			case "chernarusplus": return GetRandomTeleportPosition(7500, 7500, 10000) + "0 10 0";
			
			case "enoch": return Vector(7349.5, 290.5, 2649.4);
		}
		
		return vector.Zero;
	}
	
	void StartHitPhaseServer(float time)
	{
		EventDebug("StartHitPhaseServer");
		GetGame().RPCSingleParam(null, ERPCsBlowoutStorm.BLOWOUT_RPC_HIT_PHASE, new Param1<float>(time), true);
	}
	
	void LerpAnomalyScaleServer(float start, float end, float time)
	{
		EventDebug("LerpAnomalyScaleServer");
		GetGame().RPCSingleParam(null, ERPCsBlowoutStorm.BLOWOUT_RPC_LERP_ANOMALY_SCALE, new Param3<float, float, float>(start, end, time), true);
		
		if (m_ServerScaleTimer.IsRunning()) {
			m_ServerScaleTimer.Stop();
		}
		
		m_ServerScaleTimer.Run(0.01, this, "RunScaleLerp", new Param4<float, float, float, int>(start, end, time, GetGame().GetTime()), true);
	}
	
	void LerpAnomalyPositionServer(vector position, float time)
	{
		EventDebug("LerpAnomalyPositionServer");
		GetGame().RPCSingleParam(null, ERPCsBlowoutStorm.BLOWOUT_RPC_LERP_ANOMALY_POSITION, new Param2<vector, float>(position, time), true);
		//thread LerpPositionInternal(m_AnomalyPosition, position, time);
	}
	
	void SetAnomalyPosition(vector position)
	{
		EventDebug("SetAnomalyPosition");
		m_AnomalyPosition = position; // only way to get teleporting to update
		GetGame().RPCSingleParam(null, ERPCsBlowoutStorm.BLOWOUT_RPC_SET_ANOMALY_POSITION, new Param1<string>(position.ToString(false)), true);
	}

	void LerpColorizationServer(vector rgb, int time, vector start_rgb = "0 0 0")
	{
		EventDebug("LerpColorizationServer");
		GetGame().RPCSingleParam(null, ERPCsBlowoutStorm.BLOWOUT_RPC_COLOR_SKY, new Param3<vector, int, vector>(rgb, time, start_rgb), true);
	}
	
	override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, target, rpc_type, ctx);
		
		if (GetGame().IsClient()) {
			
			switch (rpc_type) {
								
				case ERPCsBlowoutStorm.BLOWOUT_RPC_HIT_PHASE: {
					Param1<float> hitphase_param;
					if (!ctx.Read(hitphase_param)) {
						break;
					}
					
					StartHitPhase(hitphase_param.param1);
					break;
				}
				
				case ERPCsBlowoutStorm.BLOWOUT_RPC_CREATE_BLOWOUT: {
					Param1<float> intensity;
					if (!ctx.Read(intensity)) {
						break;
					}
					
					thread CreateBlowoutClient(intensity.param1);				
					break;
				}
				
				case ERPCsBlowoutStorm.BLOWOUT_RPC_CREATE_BLOWOUT_FINAL: {
					Param1<bool> blowout_final_param;
					if (!ctx.Read(blowout_final_param)) {
						break;
					}
					
					thread CreateBlowoutFinalClient(blowout_final_param.param1);
					break;
				}
				
				case ERPCsBlowoutStorm.BLOWOUT_RPC_PLAY_SOUND_ENVIRONMENT: {
					Param3<BlowoutSound, vector, float> sound_param;
					if (!ctx.Read(sound_param)) {
						break;
					}
					
					thread PlayEnvironmentSound(sound_param.param1, sound_param.param2, sound_param.param3);
					break;
				}			
				
				case ERPCsBlowoutStorm.BLOWOUT_RPC_PLAY_SOUND_PLAYER: {
					Param2<BlowoutSound, float> sound_player_param;
					if (!ctx.Read(sound_player_param)) {
						break;
					}
					
					thread PlaySoundOnPlayer(sound_player_param.param1, sound_player_param.param2);
					break;
				}				
				
				case ERPCsBlowoutStorm.BLOWOUT_RPC_COLOR_SKY: {
					Param3<vector, int, vector> color_params;
					if (!ctx.Read(color_params)) {
						break;
					}
					
					thread LerpColorizationEx(m_PPE_EVRColours, color_params.param1[0], color_params.param1[1], color_params.param1[2], color_params.param2, color_params.param3[0], color_params.param3[1], color_params.param3[2]);
					break;
				}
				
				case ERPCsBlowoutStorm.BLOWOUT_RPC_LERP_ANOMALY_POSITION: {
					Param2<vector, float> position_params;
					if (!ctx.Read(position_params)) {
						break;
					}
					
					if (m_BlowoutAnomalyObject) {
						m_BlowoutAnomalyObject.LerpPosition(m_BlowoutAnomalyObject.GetPosition(), position_params.param1, position_params.param2);
					}
					break;
				}
				
				case ERPCsBlowoutStorm.BLOWOUT_RPC_LERP_ANOMALY_SCALE: {
					Param3<float, float, float> scale_params;
					if (!ctx.Read(scale_params)) {
						break;
					}
					
					if (m_BlowoutAnomalyObject) {
						m_BlowoutAnomalyObject.LerpScale(scale_params.param1, scale_params.param2, scale_params.param3);
					}
					break;
				}
				
				case ERPCsBlowoutStorm.BLOWOUT_RPC_TELEPORT_FLASH: {
					Param1<vector> flash_params;
					if (!ctx.Read(flash_params)) {
						break;
					}
					
					ScriptedLightBase.CreateLight(BlowoutTeleportLight, flash_params.param1);
					break;
				}
				
				case ERPCsBlowoutStorm.BLOWOUT_RPC_BRIGHTNESS: {
					Param1<float> brightness_params;
					if (!ctx.Read(flash_params)) {
						break;
					}
					
					GetGame().SetEVUser(brightness_params.param1);
					break;
				}
				
				case ERPCsBlowoutStorm.BLOWOUT_RPC_TELEPORT: {
					Param1<bool> teleport_params;
					if (!ctx.Read(teleport_params)) {
						break;
					}
					
					OnTeleportClient(teleport_params.param1);
					break;
				}
				
				case ERPCsBlowoutStorm.BLOWOUT_RPC_SET_ANOMALY_POSITION: {
					Param1<string> anomaly_position_params;
					if (!ctx.Read(anomaly_position_params)) {
						break;
					}
					
					m_AnomalyPosition = anomaly_position_params.param1.ToVector();
					if (m_BlowoutAnomalyObject) {
						m_BlowoutAnomalyObject.SetPosition(m_AnomalyPosition);
						m_BlowoutAnomalyObject.Update();
					}
					
					break;
				}
			}
		}
	}
	
	protected void PlayEarRingSound()
	{
		vector pos;

		MiscGameplayFunctions.GetHeadBonePos(PlayerBase.Cast(GetGame().GetPlayer()), pos);

		EffectSound flashbang_sound = SEffectManager.CreateSound("Tinnitus_SoundSet", pos);
		flashbang_sound.SetParent(GetGame().GetPlayer());
		flashbang_sound.SetAttachedLocalPos(GetGame().GetPlayer().WorldToModel(pos));
		flashbang_sound.SetSoundWaveKind(WaveKind.WAVEEFFECTEX);
		flashbang_sound.SetSoundFadeIn(2);
		flashbang_sound.SetSoundFadeOut(4);
		flashbang_sound.SoundPlay();
		
		flashbang_sound.GetSoundObject().SetVolume(0.2);
		
		GetGame().GetPlayer().SetMasterAttenuation("FlashbangAttenuation");
		
		Sleep(2000);
		
		flashbang_sound.SoundStop();
		
		Sleep(4000);
		
		GetGame().GetPlayer().SetMasterAttenuation("");
	}
	
	void PlayStabilizeEffect()
	{
		if (!GetGame().IsServer()) {
			return;
		}
		
		//PlayEnvironmentSoundServer(BlowoutSound.Blouwout_Stabilizer, m_Position - "0 1000 0", 1.2);
	}
	
	void ContaminateAthena1Site()
	{
		m_A1_Contamination = ContaminatedArea_Athena1.Cast( GetGame().CreateObjectEx( "ContaminatedArea_Athena1", Vector( 3777, 145, 8242 ), ECE_NONE ) );
		PlayAPSIWarningSignal("! A1 stabilized !", COLOR_GREEN);
		PlayLEHSWarningSignal("A1 stabilized", COLOR_GREEN);	
	}

	ContaminatedArea_Athena1 GetContaminatedA1()
	{
		return m_A1_Contamination;
	}
	
	bool UndergroundCheck(Object player)
	{
		string world_name;
		GetGame().GetWorldName(world_name);
		world_name.ToLower();
		
		// Namalsk only feature
		if (world_name != "namalsk") return false;
		
		vector player_pos = player.GetPosition();
		string surface;
		GetGame().SurfaceGetType(player_pos[0], player_pos[2], surface);
			

		// a2
		if (((player_pos[0]) > 4944) && ((player_pos[0]) < 5030)) {
			if (((player_pos[2]) > 6580) && ((player_pos[2]) < 6653)) {
				if ((player_pos[1]) < 44) {
					return true;
				}
			}
		}
		
		// a1
		if (((player_pos[0]) > 4186) && ((player_pos[0]) < 4287)) {
			if (((player_pos[2]) > 8170) && ((player_pos[2]) < 8280)) {
				if ((player_pos[1]) < 77) {
					return true;
				}
			}
		}
		
		// athena
		if (((player_pos[0]) > 4293) && ((player_pos[0]) < 4313)) {
			if (((player_pos[2]) > 8068) && ((player_pos[2]) < 8084)) {
				if ((player_pos[1]) < 76) {
					return true;
				}
			}
		}
		
		// rail tunnel
		if (((player_pos[0]) > 3571) && ((player_pos[0]) < 3741)) {
			if (((player_pos[2]) > 6195) && ((player_pos[2]) < 7175)) {
				if (((player_pos[1]) < 55) && (surface == "gravel_small_int")) {
					// Odd circumstance
					if (((player_pos[0]) > 3574) && ((player_pos[0]) < 3721)) {
						if (((player_pos[2]) > 6219) && ((player_pos[2]) < 7101)) {
							return false;
						}
					}
					return true;
				}
			}
		}
		
		// ???
		if (((player_pos[0]) > 3555) && ((player_pos[0]) < 3636)) {
			if (((player_pos[2]) > 6594) && ((player_pos[2]) < 6823)) {
				if (player_pos[1] < 200 && player_pos[1] > 60) {
					return true;
				}
			}
		}
		
		return false;
	}
		
	private void RunScaleLerp(float start, float finish, float duration, int start_time)
	{		
		float tfactor = (1 / duration) * (GetGame().GetTime() - start_time) / 1000;
		//CF.Debugger.Display("vfx_the_thing", this, "tfactor", tfactor);
		//CF.Debugger.Display("vfx_the_thing", this, "scale", SmoothLerp(start, finish, tfactor));
		m_AnomalyScale = SmoothLerp(start, finish, tfactor);
			
		if (tfactor >= 1) {
			m_ServerScaleTimer.Stop();
		}
	}
	
	override bool JIPRunPreviousPhases()
	{	
		return false;
	}
	
	override float GetServerTick()
	{
		return 0.06;
	}
	
	override float GetClientTick()
	{
		return 5.0;
	}
	
	vector GetAnomalyPosition()
	{
		return m_AnomalyPosition;
	}
	
	vfx_the_thing GetOrb()
	{
		return m_BlowoutAnomalyObject;
	}
		
	// Namalsk Helpers
	land_a3_chamber GetA3Chamber()
	{
		// Server only
		if (!GetGame().IsServer()) return null;
		
		land_a3_chamber chamber;
		EnScript.GetClassVar(GetGame().GetMission(), "m_LandA3Chamber", 0, chamber);
		return chamber;
	}
		
	land_a2_flaxcore GetFlaxCore()
	{
		if (GetGame().IsClient()) {
			Error("Cannot get FlaxCore on client!");
			return null;
		}
		
		land_a2_flaxcore flaxcore;
		EnScript.GetClassVar(GetGame().GetMission(), "m_LandA2FlaxCore", 0, flaxcore);
		return flaxcore;
	}
	
	land_a2_basement_3rd GetBasement3rd()
	{
		if (GetGame().IsClient()) {
			Error("Cannot get FlaxCore on client!");
			return null;
		}
		
		land_a2_basement_3rd basement;
		EnScript.GetClassVar(GetGame().GetMission(), "m_LandA2Basement3rd", 0, basement);
		return basement;
	}
	
	override TTypenameArray GetDisallowedEvents()
	{
		return { Aurora, LantiaLink, EVRStormDeadly };
	}
	
	override float GetFogTarget()
	{
		return 0.4;
	}
	
	override float GetOvercastTarget()
	{
		return 1.0;
	}
	
	// 45 minutes
	override int GetEventCooldown()
	{
		return 2700;
	}
}
