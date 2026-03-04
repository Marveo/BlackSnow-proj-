class Radiation_Geiger_Pripyat extends ItemBase
{
    PlayerBase owner;

    bool m_IsActive = false;

    protected EffectSound m_GeigerSound;
    ref Timer m_SoundLoopStartTimer;
    float m_time = 0;
    EntityAI Owner;

    override void OnWorkStart()
    {	
        m_IsActive = true;

        if ( GetGame().IsClient()  ||  !GetGame().IsMultiplayer() )
        {
            Owner = GetHierarchyParent();
            if(!Owner) return;
            if(!Owner.IsPlayer()) return;

            if (!m_SoundLoopStartTimer)
                m_SoundLoopStartTimer = new Timer( CALL_CATEGORY_SYSTEM );	
                
            m_SoundLoopStartTimer.Run( 0.005, this, "GeigerLoopSound", NULL, true);
        }
    }
    
    void GeigerDisplay(string data)
	{
		string digits;
		int i;

		for (i = 0; i < data.Length(); ++i)
		{
			digits += data.Get(i);
		}

		// Сначала устанавливаем все цифры "off"
		for(int j = 0; j < 4; j++)
		{
			ChangeDigit(j, "off");
		}

		// Затем устанавливаем нужные цифры
		for(int k = 0; k < digits.Length() && k < 4; k++)
		{
			ChangeDigit(4 - digits.Length() + k, digits.Get(k));
		}
	}
    
    void GeigerLoopSound()
    {
        if ( GetGame().IsClient()  ||  !GetGame().IsMultiplayer() )
        {
            PlayerBase pl = PlayerBase.Cast(Owner);

            if (pl.GetExternalRadiation() > 0)
            {
                float koef = pl.GetExternalRadiation() / 35;
                float time = 0.012 * koef;
            
                m_time += time;
                
                if (m_time >= 0.2)
                {
                    if(Math.RandomInt(1,100) < 20 ) return;

                    PlaySoundSet( m_GeigerSound, "geiger_" + Math.RandomInt(1, 8).ToString() + "_SoundSet", 0, 0 );

                    // Преобразование float в int с округлением
                    int radiationInt = Math.Ceil(pl.GetExternalRadiation());
                    GeigerDisplay(radiationInt.ToString());

                    m_GeigerSound.SetSoundAutodestroy( true );
                    
                    m_time = 0;
                }
            }
            if (pl.GetExternalRadiation() == 0)
            {
                GeigerDisplay("0");
            }
        }
    }	
    
    void ChangeDigit(int digitIndex, string number)
    {
        int hiddenSelectionIndex = digitIndex + 1;
        SetObjectTexture(hiddenSelectionIndex, "RadiationClient\\Geiger\\data\\digits\\" + number + ".paa");
    }
    
    override void OnWorkStop()
    {
        m_IsActive = false;

        SetObjectTexture(GetHiddenSelectionIndex("num0"), "RadiationClient\\Geiger\\data\\digits\\nothing.paa");
        SetObjectTexture(GetHiddenSelectionIndex("num1"), "RadiationClient\\Geiger\\data\\digits\\nothing.paa");
        SetObjectTexture(GetHiddenSelectionIndex("num2"), "RadiationClient\\Geiger\\data\\digits\\nothing.paa");
        SetObjectTexture(GetHiddenSelectionIndex("num3"), "RadiationClient\\Geiger\\data\\digits\\nothing.paa");
        if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
        {
            m_SoundLoopStartTimer = NULL;
        }
    }

    override void SetActions()
    {
        super.SetActions();
        
        AddAction(ActionTurnOnWhileInHands);
        AddAction(ActionTurnOffWhileInHands);
    }

    override void OnDebugSpawn()
    {
        Battery9V.Cast(GetInventory().CreateInInventory("Battery9V"));
    }
}