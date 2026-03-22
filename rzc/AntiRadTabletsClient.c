class Client_AntiRadModifier_mdfr: ModifierBase
{
    int cooldown_tablets = 5;

    override void Init()
    {
        m_TrackActivatedTime    = true;
        m_IsPersistent          = false;
        m_ID                    = RadModifiers.MDF_CLIENT_ANTIRAD_TABLETS;
        m_TickIntervalInactive  = DEFAULT_TICK_TIME_INACTIVE;
        m_TickIntervalActive    = 1;
    }

    override bool ActivateCondition(PlayerBase player)
    {
        return player.GetModifiersManager().IsModifierActive(RadModifiers.MDF_SERVER_ANTIRAD_TABLETS);
    }

    override bool DeactivateCondition(PlayerBase player)
    {
        return !player.GetModifiersManager().IsModifierActive(RadModifiers.MDF_SERVER_ANTIRAD_TABLETS);
    }

    override void OnActivate(PlayerBase player)
    {
        if(player.GetNotifiersManager())
        {
            player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_PILLS);
        }
    }

    override void OnDeactivate(PlayerBase player)
    {
        if(player.GetNotifiersManager())
        {
            player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_PILLS);
        }
    }

    override void OnTick(PlayerBase player, float deltaT)
    {
    }
};