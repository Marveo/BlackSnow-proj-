class Server_AntiRadTablets_mdfr: ModifierBase
{
    int cooldown_tablets = 5;

    override void Init()
    {
        m_TrackActivatedTime    = true;
        m_IsPersistent          = false;
        m_ID                    = RadModifiers.MDF_SERVER_ANTIRAD_TABLETS;
        m_TickIntervalInactive  = DEFAULT_TICK_TIME_INACTIVE;
        m_TickIntervalActive    = 1;
    }

    override bool ActivateCondition(PlayerBase player)
    {
        return false;
    }

    override bool DeactivateCondition(PlayerBase player)
    {
        float attached_time = GetAttachedTime();

        return attached_time > cooldown_tablets;
    }

    override void OnActivate(PlayerBase player)
    {
		if (player.GetAccumulatedRadiation() < 5)
		{
			player.IncreaseRadiation(-player.GetAccumulatedRadiation());
			
			return;
		}
        player.IncreaseRadiation(-5);
    }

    override void OnDeactivate(PlayerBase player)
    {
    }

    override void OnTick(PlayerBase player, float deltaT)
    {
    }
};