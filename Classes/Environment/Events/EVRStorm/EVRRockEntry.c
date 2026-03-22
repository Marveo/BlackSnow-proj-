class EVRRockEntry
{
	protected vector m_OriginalPostion;
	protected Object m_Rock;
	protected ref Timer m_PositionTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
	
	void EVRRockEntry(notnull Object rock)
	{
		m_Rock = rock;
		m_OriginalPostion = m_Rock.GetPosition();
	}
	
	void ~EVRRockEntry()
	{
		if (m_PositionTimer) {
			m_PositionTimer.Stop();
		}
		
		delete m_PositionTimer;
	}
	
	void LerpToSky(float duration)
	{
		LerpPosition(m_OriginalPostion, m_OriginalPostion + Vector(0, Math.RandomFloat(5, 15), 0), duration);
	}
	
	void LerpToGround(float duration)
	{
		if (m_Rock) {
			LerpPosition(m_Rock.GetPosition(), m_OriginalPostion, duration);
		}
	}
	
	protected vector m_Start, m_Finish;
	protected float m_Duration;
	protected int m_StartTime;
	
	void LerpPosition(vector start, vector finish, float duration)
	{
		if (m_PositionTimer.IsRunning()) {
			m_PositionTimer.Stop();
		}

		// This is a stupid solution since passing the Param to multiple different timers breaks??????
		// .... idfk
		m_Start = start;
		m_Finish = finish;		
		m_Duration = duration;
		m_StartTime = GetGame().GetTime();
		m_PositionTimer.Run(0.05, this, "RunPositionLerp", null, true);
	}
	
	private void RunPositionLerp()
	{
		if (!m_Rock) {
			return;
		}
				
		float tfactor = (1 / m_Duration) * (GetGame().GetTime() - m_StartTime) / 1000;
		m_Rock.SetPosition(Vector(SmoothLerp(m_Start[0], m_Finish[0], tfactor), SmoothLerp(m_Start[1], m_Finish[1], tfactor), SmoothLerp(m_Start[2], m_Finish[2], tfactor)));
		m_Rock.Update();

		if (tfactor >= 1) {
			m_PositionTimer.Stop();
		}
	}
	
	Object GetRock()
	{
		return m_Rock;
	}
}