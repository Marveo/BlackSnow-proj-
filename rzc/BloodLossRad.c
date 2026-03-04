modded class BloodLoss
{
    protected float MAXIMUM_DESATURATION = 490; 
    protected float radToSaturationValue;
    protected float m_RadiationLevel;

    override void OnUpdateClient(PlayerBase player, float deltatime)
    {
        if(player.IsPlayerSelected() && player.GetTransferValues()) 
        {
            m_BloodSet = player.GetTransferValues().GetBlood();
            m_RadiationLevel = player.GetExternalRadiation();

            if ( m_BloodSet < 1.0 || m_RadiationLevel > 0 )
            {
                radToSaturationValue = 1 - ( m_RadiationLevel / MAXIMUM_DESATURATION );

                if ( m_BloodSet > radToSaturationValue )
                    m_BloodSet = radToSaturationValue;

                if ( m_RadiationLevel > MAXIMUM_DESATURATION || radToSaturationValue < 0 )
                    m_BloodSet = 0;

                m_RequesterBloodLoss.SetBloodLossLevel(m_BloodSet);
            }
            else
            {
                m_RequesterBloodLoss.Stop();
            }
        }
    }
}