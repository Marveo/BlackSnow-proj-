modded class PlayerBase
{
    ref RadiationEffectManager m_RadiationEffectManager;

    int m_AccumulatedRadiation;
	float m_ExternalRadiation;

    override void Init()
	{
        super.Init();

        RegisterNetSyncVariableFloat("m_ExternalRadiation");
        RegisterNetSyncVariableInt("m_AccumulatedRadiation");
    }

    int GetAccumulatedRadiation()
    {
        return m_AccumulatedRadiation;
    }

    float GetExternalRadiation()
    {
        return m_ExternalRadiation;
    }

    override void OnPlayerLoaded()
    {
        super.OnPlayerLoaded();

        if (GetGame().IsClient())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(EffectRadiation, 3000, true);
		}
    }

    void EffectRadiation()
    {
        if (!m_RadiationEffectManager)
        {
            m_RadiationEffectManager = new RadiationEffectManager(this);
        }

        m_RadiationEffectManager.SetRadiationLevel(GetExternalRadiation());
        m_RadiationEffectManager.UpdatePPE();
    }
}