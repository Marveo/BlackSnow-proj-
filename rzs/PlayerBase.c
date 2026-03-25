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

    bool IsGeigerTurnedOn()
    {
        array<EntityAI> itemsArray = new array<EntityAI>;
        GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
        
        for (int i = 0; i < itemsArray.Count(); i++)
        {
            ItemBase item = ItemBase.Cast(itemsArray[i]);
            if (item && (item.IsKindOf("Radiation_Geiger_Pripyat") || item.IsKindOf("Radiation_Dosimeter_Pripyat")))
            {
                // 1. Стандартна перевірка (якщо прилад має EnergyManager і він Увімкнений)
                if (item.GetCompEM() && item.GetCompEM().IsWorking()) return true;
                
                // 2. Надійна перевірка (Шукаємо батарейку всередині)
                if (item.GetInventory())
                {
                    for (int j = 0; j < item.GetInventory().AttachmentCount(); j++)
                    {
                        ItemBase attachment = ItemBase.Cast(item.GetInventory().GetAttachmentFromIndex(j));
                        if (attachment && attachment.IsKindOf("Battery9V")) // Знайшли крону
                        {
                            if (attachment.GetCompEM() && attachment.GetCompEM().GetEnergy() > 0)
                            {
                                return true; // Батарейка є і заряджена!
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    // --- ДЕБАГ ВЕРСІЯ СКАНУВАННЯ ---
    void UpdateDosimeterSound()
    {
        if (!GetGame().IsServer()) return;

        // Перевіряємо прилад
        if (!IsGeigerTurnedOn()) 
        {
            // Якщо розкоментувати рядок нижче, сервер буде флудити, коли прилад вимкнено
            // TraderMessage.PlayerWhite("Прилад не знайдено або він розряджений", this);
            return;
        }

        bool shouldClick = false;
        string debugSourceInfo = ""; // Для розуміння, кого саме ми знайшли

        // 1. Перевіряємо зону
        if (GetExternalRadiation() > 40.0)
        {
            shouldClick = true; 
            debugSourceInfo = "Радіаційна зона";
        }
        // 2. Перевіряємо себе
        else if (GetSingleAgentCount(RadZoneAgents.RADIATION) > 100.0)
        {
            shouldClick = true; 
            debugSourceInfo = "Сам гравець (кров)";
        }
        // 3. Шукаємо навколо
        else
        {
            array<Object> nearestObjects = new array<Object>;
            GetGame().GetObjectsAtPosition(GetPosition(), 10.0, nearestObjects, null);

            for (int i = 0; i < nearestObjects.Count(); i++)
            {
                Object obj = nearestObjects[i];
                if (obj == this) continue; // Себе пропускаємо

                // Чи це зомбі?
                ZombieBase zombie = ZombieBase.Cast(obj);
                if (zombie && zombie.m_RadiationAmount > 0)
                {
                    shouldClick = true; 
                    debugSourceInfo = "Радіоактивний Зомбі";
                    break;
                }

                // Чи це інший гравець або труп?
                PlayerBase otherPlayer = PlayerBase.Cast(obj);
                if (otherPlayer)
                {
                    float pRads = 0;
                    if (otherPlayer.IsAlive()) pRads = otherPlayer.GetSingleAgentCount(RadZoneAgents.RADIATION);
                    else pRads = otherPlayer.m_CorpseRadiation;

                    if (pRads > 100.0)
                    {
                        shouldClick = true; 
                        debugSourceInfo = "Інший гравець/Труп";
                        break; 
                    }
                }
            }
        }

        // 4. Відтворення звуку
        if (shouldClick)
        {
            // УВАГА: Цей рядок буде флудити вам в чат кожну секунду, коли ви біля джерела.
            // Це нормально для тесту. Коли переконаєтесь, що все працює - просто видаліть його.
            TraderMessage.PlayerWhite("[ГЕЙГЕР ТЕСТ] Звук! Джерело: " + debugSourceInfo, this);
            
            PlayGeigerClick() 
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

        if (GetGame().IsServer())
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
