modded class PlayerBase
{
    override void Init()
    {
        super.Init();
    }
	
	override void OnConnect()
	{
		super.OnConnect();

        if (GetGame().IsServer())
		{
            // Агент радіації сам обробляється системою агентів.
        }
	}

    void IncreaseRadiation(float amount) {} // Застаріла

	void ResetRadiation()
    {
        if (GetGame().IsServer())
        {
            RemoveAgent(RadZoneAgents.RADIATION);
            SetExternalRadiation(0);
            SetSynchDirty();
        }
    }

    // ======================
    // Радіаційний фон гравця
    // ======================
    protected float m_ExternalRadiation = 0.0;
    protected bool m_IsInRadiationZone = false; // <-- додано
    protected int m_InStrongZone = 0;

    void SetExternalRadiation(float level)
    {
        m_ExternalRadiation = level;
        if (GetGame().IsServer()) SetSynchDirty();
    }

    float GetExternalRadiation()
    {
        return m_ExternalRadiation;
    }

    void SetIsInRadiationZone(bool state)
    {
        m_IsInRadiationZone = state;
        SetSynchDirty();
    }

    bool IsInRadiationZone()
    {
        return m_IsInRadiationZone;
    }

    bool IsInStrongZone()
    {
        return (m_InStrongZone == 1);
    }

    void SetInStrongZone(bool state)
    {
        if (state)
        {
            m_InStrongZone = 1;
        }
        else
        {
            m_InStrongZone = 0;
        }
        SetSynchDirty();
    }


    bool HasItemInSlot(string classname, string slotname)
    {
        ItemBase item = FindAttachmentBySlotName(slotname);
        return (item && item.GetType() == classname && !item.IsRuined());
    }

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version))
            return false;
        return true;
    }

    // ==========================
    // === ДОЗИМЕТР / ГЕІГЕР ===
    // ==========================
   void UpdateDosimeterSound()
    {
        if (!GetGame().IsClient())
            return;

        ItemBase dosimeter = ItemBase.Cast(FindAttachmentBySlotName("RadiationDosimeter"));
        if (!dosimeter)
            return;

        Radiation_Geiger_Pripyat geiger = Radiation_Geiger_Pripyat.Cast(dosimeter);
        if (!geiger || !geiger.m_IsActive)
            return;

        float zoneRadiation = GetExternalRadiation();
        bool isInZone = IsInRadiationZone();
        bool isInStrong = IsInStrongZone();
        float selfAgents = GetSingleAgentCount(RadZoneAgents.RADIATION);
        bool shouldClick = false;

        // === DEBUG HEADER ===
        Print("[RAD DEBUG] --- ГЕЙГЕР ОНОВЛЕННЯ ---");
        Print("[RAD DEBUG] isInZone=" + isInZone.ToString() + " | isInStrong=" + isInStrong.ToString());
        Print("[RAD DEBUG] selfAgents=" + selfAgents + " | zoneRadiation=" + zoneRadiation);

        // === 1️⃣ Реакція в локальних / сильних зонах ===
        if (isInZone && isInStrong)
        {
            if (zoneRadiation > 40)
            {
                Print("[RAD DEBUG] СИЛЬНА зона → тріщить від фону");
                shouldClick = true;
            }
        }
        else if (isInZone && !isInStrong)
        {
            // Локальна розсіяна зона (але не глобальна)
            if (zoneRadiation > 40)
            {
                Print("[RAD DEBUG] ЛОКАЛЬНА зона → тріщить від фону");
                shouldClick = true;
            }
        }

        // === 2️⃣ Якщо поза сильною зоною (тобто глобальна або взагалі поза зонами) ===
        if (!isInStrong)
        {
            // Власне зараження
            if (selfAgents > 40)
            {
                Print("[RAD DEBUG] ОПРОМІНЕННЯ гравця → тріщить від власних агентів");
                shouldClick = true;
            }
            else
            {
                Print("[RAD DEBUG] selfAgents < 40 → не тріщить від себе");
            }

            // Заражені поблизу
            array<Man> players = new array<Man>;
            GetGame().GetPlayers(players);
            vector myPos = GetPosition();

            for (int i = 0; i < players.Count(); i++)
            {
                PlayerBase other = PlayerBase.Cast(players[i]);
                if (!other || other == this)
                    continue;

                float dist = vector.Distance(myPos, other.GetPosition());
                float otherAgents = other.GetSingleAgentCount(RadZoneAgents.RADIATION);

                if (dist <= 8.0)
                {
                    Print("[RAD DEBUG] Поблизу гравець " + other.ToString() + " | dist=" + dist + " | agents=" + otherAgents);
                    if (otherAgents > 40)
                    {
                        Print("[RAD DEBUG] ОПРОМІНЕНИЙ гравець поруч → тріщить");
                        shouldClick = true;
                        break;
                    }
                }
            }
        }

        // === 3️⃣ Відтворення звуку ===
        if (shouldClick)
        {
            PlayGeigerClick();
            Print("[RAD DEBUG] 🎧 ГЕЙГЕР клацнув!");
        }
        else
        {
            Print("[RAD DEBUG] 🔇 ГЕЙГЕР мовчить");
        }
    }



    void PlayGeigerClick()
    {
        string soundName = "geiger_" + Math.RandomInt(1, 5).ToString() + "_SoundSet";
        EffectSound s;
        PlaySoundSet(s, soundName, 0, 0);
        if (s)
        {
            s.SetSoundVolume(1.0);
            s.SetSoundAutodestroy(true);
        }
    }

    override void OnScheduledTick(float deltaTime)
    {
        super.OnScheduledTick(deltaTime);

        if (GetGame().IsClient())
        {
            static float m_DosimeterTimer = 0;
            m_DosimeterTimer += deltaTime;

            if (m_DosimeterTimer >= 2.0) // як у стабільній версії
            {
                UpdateDosimeterSound();
                m_DosimeterTimer = 0;
            }
        }
    }
}
