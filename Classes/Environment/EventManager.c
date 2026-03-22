typedef map<typename, ref NamEventBase> TNamEventMap;

class NamEventManager
{
	// Enable / Disable the multiple event system
	protected int m_MaxEventCount;
	
	protected ref map<typename, float> m_PossibleEventTypes = new map<typename, float>();
	protected ref TNamEventMap m_ActiveEvents = new TNamEventMap();
	
	protected float m_NextEventIn;
	protected int m_EventFreqMin;
	protected int m_EventFreqMax;
	
	private typename m_LastEventType;
	
	protected ref Timer m_ServerEventTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
	
	protected ref Timer m_EventCooldownTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
	protected ref map<typename, int> m_EventCooldowns = new map<typename, int>();
	
	void NamEventManager()
	{		
		DayZGame.Event_OnRPC.Insert(OnRPC);

		if (GetGame().IsServer()) {
			m_EventFreqMin = 550;
			m_EventFreqMax = 3500;
		}
	}
	
	void ~NamEventManager()
	{
		delete m_ActiveEvents;
		delete m_PossibleEventTypes;
		delete m_EventCooldownTimer;
		delete m_EventCooldowns;
		delete m_ServerEventTimer;
	}

	// 0 (bool): Enabled or disabled
	// 1 (int): Minimum time between events
	// 2 (int): Maximum time between events
	// 3 (int): Maximum amount of parallel events
	void OnInitServer(bool state, int min, int max, int max_event_count = 2)
	{
		if (!state) {
			NamEventManagerInfo("NamEventManager is now disabled");
			return;
		}
		
		m_MaxEventCount = max_event_count;
		m_EventFreqMin = min;
		m_EventFreqMax = max;
		
#ifdef NAMALSK_SURVIVAL_DEBUG
		m_EventFreqMin *= 0.05;
		m_EventFreqMax *= 0.05;
#endif
		
		m_EventCooldownTimer.Run(1.0, this, "ServerCooldownThread", null, true);
		
		m_NextEventIn = GetNextEventTime();
		NamEventManagerInfo("Next selection will occur in %1 seconds", m_NextEventIn.ToString());	
		m_ServerEventTimer.Run(m_NextEventIn, this, "ServerEventThread");
		
		NamEventManagerInfo("NamEventManager is now running");
	}
	
	private void ServerCooldownThread()
	{
		foreach (typename event_type, int event_cooldown: m_EventCooldowns) {
			m_EventCooldowns[event_type] = event_cooldown - 1;
			if (event_cooldown <= 0) {
				m_EventCooldowns.Remove(event_type);
			}
		}
	}
			
	private void ServerEventThread()
	{	
		NamEventManagerInfo("Trying to select a new event...");												
		// Just a quick check to make sure we dont run the same event twice
		typename current_type = GetRandomEvent();
		while (current_type == m_LastEventType && m_PossibleEventTypes.Count() > 1) {
			NamEventManagerDebug("Random event selected was the same as last %1", current_type.ToString());
			current_type = GetRandomEvent();
		}
		
		m_LastEventType = current_type;
		
		//! Start new event
		StartEvent(current_type);
		
		m_NextEventIn = GetNextEventTime();
		NamEventManagerInfo("Next selection will occur in %1 seconds", m_NextEventIn.ToString());
		
		m_ServerEventTimer.Run(m_NextEventIn, this, "ServerEventThread");
	}
	
	void RegisterEvent(typename event_type, float frequency = 1.0)
	{
		if (frequency <= 0) {
			return;
		}
		
		m_PossibleEventTypes.Insert(event_type, frequency);
	}

	void StartEvent(typename event_type, bool force = false)
	{
		if (!GetGame().IsServer()) {
			NamEventManagerInfo("StartEvent must be called on SERVER, exiting");
			return;
		}
		
		thread _StartEvent(event_type, force);
	}
	
	void CancelEvent(typename event_type)
	{
		if (!GetGame().IsServer()) {
			NamEventManagerInfo("CancelEvent must be called on SERVER, exiting");
			return;
		}
		
		if (!m_ActiveEvents[event_type]) {
			NamEventManagerInfo("Event %1 is not active", event_type.ToString());
			return;
		}
		
		SendActiveEventData(event_type, 3, 0); // 3 is cancel for clients
		DeleteEvent(event_type);
	}

	// kinda hacky, but will work since we gotta cancel all weather events before getting to Lantia
	void CancelLantiaEvents()
	{
		if ( GetEvent( Aurora ) )
			CancelEvent( Aurora );

		if ( GetEvent( HeavyFog ) )
			CancelEvent( HeavyFog );

		if ( GetEvent( SnowfallE ) )
			CancelEvent( SnowfallE );

		if ( GetEvent( Blizzard ) )
		{
			CancelEvent( Blizzard );
			GetGame().GetMission().GetWorldData().SetTemperatureEvent(0.0);
		}
	}
	void CancelEVREvents()
	{
		if ( GetEvent( Aurora ) )
			CancelEvent( Aurora );
	}

	private void DeleteEvent(typename event_type)
	{
		if (!m_ActiveEvents) {
			return;
		}
		
		delete m_ActiveEvents[event_type];
		m_ActiveEvents.Remove(event_type);
	}
	
	private void _StartEvent(typename event_type, bool force = false)
	{
		if (m_ActiveEvents[event_type]) { 
			NamEventManagerInfo("Could not start %1 as one is already active", event_type.ToString());
			return;
		}
				
		if (m_ActiveEvents.Count() >= m_MaxEventCount && !force) {
			NamEventManagerInfo("Could not start event as we reached the maximum event limit %1", m_MaxEventCount.ToString());
			return;
		}
		
		if (m_EventCooldowns.Contains(event_type) && !force) {
			NamEventManagerInfo("Could not start event %1 as it is on cooldown for %2 more seconds", event_type.ToString(), m_EventCooldowns[event_type].ToString());
			return;
		}
		
		NamEventManagerInfo("Starting event %1", event_type.ToString());
		m_ActiveEvents[event_type] = SpawnEvent(event_type);
		
		if (!m_ActiveEvents[event_type]) {
			NamEventManagerInfo("Failed to start event %1", event_type.ToString());
			return;
		}
		
		foreach (typename etype, NamEventBase ebase: m_ActiveEvents) {
			if (m_ActiveEvents[event_type].GetDisallowedEvents().Find(etype) != -1 && !force) {
				NamEventManagerInfo("Could not run event %1 because it conflicts with event %2...", event_type.ToString(), etype.ToString());
				DeleteEvent(event_type);
				return;
			}
		}
		
		if (m_ActiveEvents[event_type] && m_ActiveEvents[event_type].EventActivateCondition()) {
			// Register event for cooldown
			m_EventCooldowns.Insert(event_type, m_ActiveEvents[event_type].GetEventCooldown());
			
			// Phase updates
			// 3rd phase calls event_end
			for (int i = 0; i < 4; i++) {
				
				// Server event
				m_ActiveEvents[event_type].SwitchPhase(i);
				
				// Dispatch data to all clients
				SendActiveEventData(event_type, i, m_ActiveEvents[event_type].GetCurrentPhaseTimeRemaining());
				
				if (m_ActiveEvents[event_type]) {
					NamEventManagerInfo("%1: Phase Length %2", event_type.ToString(), m_ActiveEvents[event_type].GetCurrentPhaseLength().ToString());
					Sleep(m_ActiveEvents[event_type].GetCurrentPhaseLength() * 1000);
				}
				
				// Event was cancelled
				if (!m_ActiveEvents[event_type]) {
					return;
				}
			}
		}

		DeleteEvent(event_type);
	}
	
	void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{	
		switch (rpc_type) {
			
			case ERPCsNamalsk.NAM_RPC_EVENT_UPDATE: {
				
				NamEventManagerUpdateParams event_update_params;
				if (!ctx.Read(event_update_params)) {
					break;
				}
								
				if (GetGame().IsClient() || !GetGame().IsMultiplayer()) {
					
					NamEventManagerInfo("Client received event manager update %1: %2", event_update_params.param1, event_update_params.param2.ToString());
					typename event_type = event_update_params.param1.ToType();
					int event_phase = event_update_params.param2;
					float event_phase_time = event_update_params.param3;
					
					// Case for JIP players	
					if (!m_ActiveEvents[event_type]) {
						m_ActiveEvents[event_type] = SpawnEvent(event_type);
						if (!m_ActiveEvents[event_type]) return;
					}						
					
					// Event finished
					if (event_phase == NamEventPhase.DELETE) {
						DeleteEvent(event_type);
						break;
					}
					
					// Play catch-up to the current phase
					if (m_ActiveEvents[event_type].JIPRunPreviousPhases()) {
						for (int i = m_ActiveEvents[event_type].GetActivePhaseID(); i < event_phase; i++) {
							m_ActiveEvents[event_type].SwitchPhase(i);
						}					
					}
					
					m_ActiveEvents[event_type].SwitchPhase(event_phase, event_phase_time);
				}
				
				break;
			}				
		}
		
		foreach (typename et, NamEventBase event_base: m_ActiveEvents) {
			if (event_base) {
				event_base.OnRPC(sender, target, rpc_type, ctx);
			}
		}
	}
	
	void DispatchEventInfo(PlayerBase player)
	{
		NamEventManagerDebug("Sending In Progress info to %1", player.ToString());
		foreach (typename event_type, NamEventBase event_base: m_ActiveEvents) {
			if (event_base) {
				GetGame().RPCSingleParam(player, ERPCsNamalsk.NAM_RPC_EVENT_UPDATE, new NamEventManagerUpdateParams(event_type.ToString(), event_base.GetActivePhaseID(), event_base.GetCurrentPhaseTimeRemaining()), true, player.GetIdentity());
			}
		}
	}
	
	protected void SendActiveEventData(typename event_type, int phase_id, float time_remaining)
	{
		NamEventManagerDebug("Sending active Event Data: %1, Phase: %2", event_type.ToString(), phase_id.ToString());
		GetGame().RPCSingleParam(null, ERPCsNamalsk.NAM_RPC_EVENT_UPDATE, new NamEventManagerUpdateParams(event_type.ToString(), phase_id, time_remaining), true, null);
	}
	
	bool IsEventActive(typename event_type)
	{
		return m_ActiveEvents[event_type] != null;
	}
	
	NamEventBase SpawnEvent(typename event_type)
	{
		if (!event_type.IsInherited(NamEventBase))
			return null;
		
		return NamEventBase.Cast(event_type.Spawn());
	}
		
	typename GetRandomEvent()
	{
		if (m_PossibleEventTypes.Count() == 0) return typename;
		
		array<typename> possible_types = {};
		foreach (typename type, float freq: m_PossibleEventTypes) {
			for (int i = 0; i < freq * 100; i++) {
				possible_types.Insert(type);
			}
		}
				
		return possible_types.GetRandomElement();
	}
	
	float GetNextEventTime()
	{
		return Math.Clamp(Math.RandomInt(m_EventFreqMin, m_EventFreqMax), 10, int.MAX);
	}
	
	NamEventBase GetEvent(typename event_type)
	{
		return m_ActiveEvents[event_type];
	}
	
	array<NamEventBase> GetActiveEvents()
	{
		return m_ActiveEvents.GetValueArray();
	}
	
	void DumpInfo()
	{
		if (m_PossibleEventTypes.Count() == 0) {
			NamEventManagerInfo("Cannot debug Event Percentages with no events registered");
			return;
		}
		
		NamEventManagerInfo("New events will be selected between %1 and %2 minutes", (m_EventFreqMin / 60).ToString(), (m_EventFreqMax / 60).ToString());
		NamEventManagerInfo("There is a maximum event count of %1", m_MaxEventCount.ToString());
		float total_freq;
		foreach (typename typet, float freqt: m_PossibleEventTypes) {
			total_freq += freqt;
		}
		
		foreach (typename type, float freq: m_PossibleEventTypes) {
			float value = ((1 / total_freq) * freq) * 100;
			NamEventManagerInfo("Chance of %1 is %2 percent", type.ToString(), value.ToString());
		}
	}
	
	// This is really arbitrary but the NamEventManager is a singleton and im afraid
	// of breaking old code in mission files and destroying every server in existence :)
	static NamEventManager GetInstance()
	{
		NamEventManager manager;
		g_Script.CallFunction(GetGame().GetMission(), "GetNamEventManager", manager, null);
		return manager;
	}
		
	static void NamEventManagerDebug(string msg, string param1 = "", string param2 = "", string param3 = "", string param4 = "", string param5 = "", string param6 = "", string param7 = "", string param8 = "", string param9 = "")
	{
#ifdef NAMALSK_SURVIVAL_DEBUG		
		PrintFormat("[Namalsk][NamEventManager]: " + msg, param1, param2, param3, param4, param5, param6, param7, param8, param9);
#endif
	}
	
	static void NamEventManagerInfo(string msg, string param1 = "", string param2 = "", string param3 = "", string param4 = "", string param5 = "", string param6 = "", string param7 = "", string param8 = "", string param9 = "")
	{
		PrintFormat("[Namalsk][NamEventManager]: " + msg, param1, param2, param3, param4, param5, param6, param7, param8, param9);
	}
};