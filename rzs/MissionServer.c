modded class MissionServer
{
    ref RadiationManager_Clustered m_RadiationManager;

    override void OnInit()
    {
        super.OnInit();
        m_RadiationManager = new RadiationManager_Clustered();
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
		
        if (m_RadiationManager)
        {
            m_RadiationManager.OnTick(timeslice);
        }
    }
}