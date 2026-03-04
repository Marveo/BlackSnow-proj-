class RadiationEffectManager
{
    protected PlayerBase m_Player;
    protected float m_RadiationLevel;
    protected const float RADGRAIN_THRESHOLD = 400.0;
    protected float m_grainSize = 1.0;

    protected ref Param1<float> m_grainSizeParam;

    void RadiationEffectManager(PlayerBase player)
    {
        m_Player = player;
        m_grainSizeParam = new Param1<float>(0);

        PPERequester_RadGrain m_RequesterRadGrain;
        Class.CastTo(m_RequesterRadGrain, PPERequester_RadGrain.Cast(PPERequesterBank.GetRequester(PPERequester_RadGrain)));
    }

    void SetRadiationLevel(float level)
    {
        m_RadiationLevel = level;
    }

    void UpdatePPE()
    {
        if (m_RadiationLevel > RADGRAIN_THRESHOLD)
        {
            m_grainSizeParam.param1 = m_grainSize;
            PPERequesterBank.GetRequester(PPERequester_RadGrain).Start(m_grainSizeParam);
        }
        else
        {
            PPERequesterBank.GetRequester(PPERequester_RadGrain).Stop();
        }
    }
}