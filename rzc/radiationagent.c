class RadZoneAgent extends AgentBase
{
    override void Init()
    {
        m_Type = RadZoneAgents.RADIATION;
        m_Invasibility = 0.0;
        m_TransferabilityIn = 1.0;
        m_TransferabilityOut = 0.0;
        m_AntibioticsResistance = 1.0;
        m_MaxCount = 2000.0;
        m_Potency = EStatLevels.GREAT;
        m_DieOffSpeed = 0.0;
    }
} 
