class vfx_the_thing: House
{	
	protected Particle m_AnomalyAura;
	protected BlowoutLight m_BlowoutLight;
	
	protected ref map<BlowoutSound, ref EffectSound> m_AnomalySounds = new map<BlowoutSound, ref EffectSound>();
	protected ref Timer m_UpdateTimer = new Timer(CALL_CATEGORY_SYSTEM);
	
	// Timers for lerping size
	protected ref Timer m_ScaleTimer = new Timer(CALL_CATEGORY_SYSTEM);
	protected ref Timer m_PositionTimer = new Timer(CALL_CATEGORY_SYSTEM);
		
	protected bool m_IsStable;
	protected bool m_IsLantiaLink = false;
	protected bool m_IsUnderAttack = false;
	protected bool m_ForcePaused;
	protected bool m_ParticleSwitched = false;

	protected EffectSound m_WHSound;
	
	void vfx_the_thing()
	{		
		//EVRStorm.EVRDebugPrint("vfx_the_thing");
		
		if (GetGame().IsClient() || !GetGame().IsMultiplayer()) {
			m_BlowoutLight = BlowoutLight.Cast(BlowoutLight.CreateLight(BlowoutLight, GetPosition(), 10));
			if (m_BlowoutLight) {
				m_BlowoutLight.AttachOnObject(this);
			}
						
			thread StartUpdateFrame();
			
			DayZGame.Event_OnRPC.Insert(OnGlobalRPC);
		}	
	}
	
		
	void ~vfx_the_thing()
	{
		//EVRStorm.EVRDebugPrint("~vfx_the_thing");

		if (m_UpdateTimer) {
			m_UpdateTimer.Stop();
		}
		
		delete m_UpdateTimer;
		
		if (m_BlowoutLight) {
			m_BlowoutLight.Destroy();
		}
		
		foreach (BlowoutSound name, EffectSound wave: m_AnomalySounds) {
			if (wave) {
				wave.SoundStop();
				delete wave;
			}
		}
		
		delete m_AnomalySounds;
		
		if ( m_WHSound )
		{
			m_WHSound.SoundStop();
		}

		if (m_AnomalyAura) {
			m_AnomalyAura.Stop();
			//m_AnomalyAura.Delete();
		}
		
		if (m_ScaleTimer) {
			m_ScaleTimer.Stop();
		}
		
		if (m_PositionTimer) {
			m_PositionTimer.Stop();
		}
		
		delete m_ScaleTimer;
		delete m_PositionTimer;
	}
	
	private void StartUpdateFrame()
	{
		Sleep(100);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		
		RegisterSound(BlowoutSound.Blowout_GraviIdle);
		if ( !IsLantiaLink() )
		{
			RegisterSound(BlowoutSound.Blowout_Ambient);
			RegisterSound(BlowoutSound.Blowout_Drone);
			player.RegisterAnomalyVoice(RegisterSound(BlowoutSound.Blowout_Voices_Loop));
		}
		
		SetVolume(1);
						
		m_UpdateTimer.Run(0.02, this, "UpdateFrame", null, true);
	}
	
	EffectSound RegisterSound(BlowoutSound sound)
	{	
		EffectSound wave = SEffectManager.PlaySoundOnObject(typename.EnumToString(BlowoutSound, sound), this, 0, 0, true);
		m_AnomalySounds.Insert(sound, wave);
		return wave;
	}

	void UpdateFrame()
	{		
		vector position = GetPosition();
		float scale = GetScale();

		if ( !m_AnomalyAura && m_IsStable )
		{
			m_AnomalyAura = ParticleManager.GetInstance().PlayOnObject( ParticleList.ANOMALY_AURA, this );
		}
		if ( m_AnomalyAura && m_IsUnderAttack && !m_ParticleSwitched )
		{
			m_AnomalyAura.Stop();
			m_AnomalyAura = ParticleManager.GetInstance().PlayOnObject( ParticleList.ANOMALY_AURA_LNT, this );
			m_ParticleSwitched = true;
		}

		vector size[2];
		vector size_result;
		ClippingInfo(size);

		size_result[0] = Math.AbsFloat(size[0][0]) + Math.AbsFloat(size[1][0]);
		size_result[1] = Math.AbsFloat(size[0][1]) + Math.AbsFloat(size[1][1]);
		size_result[2] = Math.AbsFloat(size[0][2]) + Math.AbsFloat(size[1][2]);

		if (m_BlowoutLight) {
			m_BlowoutLight.SetHeatHazeRadius((size_result[1] / 2 * scale) + scale * 1.5);
		}
				
		// Lightning Effect
		if (Math.RandomInt(0, 2) == 1 && !APSICheck() && !LEHSCheck() && !m_IsStable) {
			vector pos = scale * size_result[1] / 2 * Math3D.GetRandomDir();
			vector ori = 180 * Vector(Math.RandomFloat01(), Math.RandomFloat01(), Math.RandomFloat01());
			float scl = scale * 0.15;
			CreateBolt(position + pos, ori, scl);
			CreateBolt(position - pos, ori * -1, scl);
		}
	}
	
	bool APSICheck()
	{
		dzn_apsi apsi;
		return ( GetGame() && GetGame().GetPlayer() && Class.CastTo( apsi, GetGame().GetPlayer().GetInventory().FindAttachment( InventorySlots.EYEWEAR ) ) && apsi && apsi.GetHud() );
	}

	bool LEHSCheck()
	{
		dzn_lehs_helmet lehs_helmet;
		return ( GetGame() && GetGame().GetPlayer() && Class.CastTo( lehs_helmet, GetGame().GetPlayer().GetInventory().FindAttachment( InventorySlots.HEADGEAR ) ) && lehs_helmet && lehs_helmet.HasPower() && !lehs_helmet.IsVisorUp() );
	}

	private void CreateBolt(vector position, vector orientation, float scale)
	{
		Object bolt = GetGame().CreateObjectEx(LIGHTNING_BOLT_TYPES.GetRandomElement(), position, ECE_LOCAL);
		if (!bolt) return;
		bolt.SetOrientation(orientation);
		bolt.SetScale(scale);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().ObjectDelete, 25, false, bolt);
	}
		
	void SetVolume(float volume)
	{
		foreach (BlowoutSound name, EffectSound wave: m_AnomalySounds) {
			// weird edge case when dealing with APSI muting sounds
			if (wave && wave.GetSoundVolume() != 0) {
				wave.SetSoundVolume(volume);
			}
		}
	}
	
	void SetPaused(bool state)
	{
		m_ForcePaused = state;
		if (m_ForcePaused) {
			if (m_ScaleTimer) {
				m_ScaleTimer.Pause();
			}
			
			if (m_PositionTimer) {
				m_PositionTimer.Pause();
			}
		} else {
			if (m_ScaleTimer) {
				m_ScaleTimer.Continue();
			}
			
			if (m_PositionTimer) {
				m_PositionTimer.Continue();
			}
		}
	}
		
	bool IsPaused()
	{
		return m_ForcePaused;
	}
	
	EffectSound GetSound(BlowoutSound sound)
	{
		return m_AnomalySounds[sound];
	}
	
	void LerpScale(float start, float finish, float duration)
	{
		if (m_ScaleTimer.IsRunning()) {
			m_ScaleTimer.Stop();
		}
		
		m_ScaleTimer.Run(0.01, this, "RunScaleLerp", new Param4<float, float, float, int>(start, finish, duration, GetGame().GetTime()), true);
	}
	
	void LerpPosition(vector start, vector finish, float duration)
	{
		if (m_PositionTimer.IsRunning()) {
			m_PositionTimer.Stop();
		}
		
		m_PositionTimer.Run(0.01, this, "RunPositionLerp", new Param4<vector, vector, float, int>(start, finish, duration, GetGame().GetTime()), true);
	}
	
	private void RunScaleLerp(float start, float finish, float duration, int start_time)
	{		
		float tfactor = (1 / duration) * (GetGame().GetTime() - start_time) / 1000;
		//CF.Debugger.Display("vfx_the_thing", this, "tfactor", tfactor);
		//CF.Debugger.Display("vfx_the_thing", this, "scale", SmoothLerp(start, finish, tfactor));
		SetScale(SmoothLerp(start, finish, tfactor));
		SetVolume(SmoothLerp(start, finish, tfactor));
		Update();
				
		if (tfactor >= 1) {
			m_ScaleTimer.Stop();
		}
	}
	
	private void RunPositionLerp(vector start, vector finish, float duration, int start_time)
	{
		float tfactor = (1 / duration) * (GetGame().GetTime() - start_time) / 1000;
		SetPosition(Vector(SmoothLerp(start[0], finish[0], tfactor), SmoothLerp(start[1], finish[1], tfactor), SmoothLerp(start[2], finish[2], tfactor)));
		Update();
		
		if (tfactor >= 1) {
			m_PositionTimer.Stop();
		}
	}
	
	bool IsStable()
	{
		return m_IsStable;
	}
	void SetStable( bool level )
	{
		m_IsStable = level;
	}
	
	bool IsLantiaLink()
	{
		return m_IsLantiaLink;
	}
	void SetLantiaLink( bool state )
	{
		m_IsLantiaLink = state;
		SetStable( true );
	}
	
	bool IsUnderAttack()
	{
		return m_IsUnderAttack;
	}
	void SetUnderAttack( bool state )
	{
		m_IsUnderAttack = state;
		if ( m_IsUnderAttack )
		{
			SetObjectMaterial( GetHiddenSelectionIndex( "wormhole" ), "nst\\ns_dayz\\effects\\visual\\data\\thething_glow_attack.rvmat" );
			SetObjectTexture( GetHiddenSelectionIndex( "wormhole" ), "#(argb,8,8,3)color(0.960784,0.509804,0.313726,1.0,co)" );
			if ( m_WHSound )
				m_WHSound.SoundStop();
			m_WHSound = SEffectManager.PlaySoundOnObject(typename.EnumToString(BlowoutSound, BlowoutSound.Blowout_Wormhole_Underpressure), this, 0, 0, true);
		}
		else
		{
			SetObjectMaterial( GetHiddenSelectionIndex( "wormhole" ), "nst\\ns_dayz\\effects\\visual\\data\\thething_glow.rvmat" );
			SetObjectTexture( GetHiddenSelectionIndex( "wormhole" ), "#(argb,8,8,3)color(0.562745,0.57451,0.905882,0.8,ca)" );
			if ( m_WHSound )
				m_WHSound.SoundStop();
		}
	}	
	
	void OnGlobalRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{		
		switch (rpc_type) {
			case ERPCsReactor.REACTOR_ENABLE: {
				if ( !m_IsLantiaLink )
					m_IsStable = true;
				break;
			}
			
			case ERPCsReactor.REACTOR_DISABLE: {
				if ( !m_IsLantiaLink )
					m_IsStable = false;
				break;
			}
		}
	}
}