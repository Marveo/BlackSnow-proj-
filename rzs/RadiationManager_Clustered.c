class RadiationManager_Clustered
{
    const float CELL_SIZE = 100.0;
    float m_CheckInterval = 3.0;
    float m_Timer = 0;
    float m_WarningTimer = 0;

    ref map<string, ref array<PlayerBase>> m_PlayerCells;
    ref array<Man> m_AllPlayers;
    ref array<ref RadiationZoneConfig> m_Zones;
    ref RadiationResistanceConfig m_ResistConfig;

    void RadiationManager_Clustered()
    {
        m_PlayerCells = new map<string, ref array<PlayerBase>>;
        m_AllPlayers = new array<Man>();
        m_Zones = GetRadiationZoneConfigManager().GetZones();
        m_ResistConfig = GetRadiationResistanceConfigManager().GetConfig();
    }

    void OnTick(float deltaT)
    {
        m_Timer += deltaT;
        m_WarningTimer += deltaT;

        if (m_Timer < m_CheckInterval)
            return;

        m_Timer = 0;
        m_PlayerCells.Clear();

        bool shouldWarn = false;
        if (m_WarningTimer >= 10.0)
        {
            shouldWarn = true;
            m_WarningTimer = 0;
        }

        GetGame().GetPlayers(m_AllPlayers);

        // Розподіляємо гравців по клітинах
        for (int i = 0; i < m_AllPlayers.Count(); i++)
        {
            PlayerBase player = PlayerBase.Cast(m_AllPlayers[i]);
            if (!player) continue;
            if (!player.GetAllowDamage()) continue;

            vector playerPos = player.GetPosition();
            int cellX = Math.Floor(playerPos[0] / CELL_SIZE);
            int cellZ = Math.Floor(playerPos[2] / CELL_SIZE);
            string cellKey = cellX.ToString() + "_" + cellZ.ToString();

            array<PlayerBase> cellPlayers;
            if (!m_PlayerCells.Find(cellKey, cellPlayers))
            {
                cellPlayers = new array<PlayerBase>;
                m_PlayerCells.Set(cellKey, cellPlayers);
            }
            cellPlayers.Insert(player);
        }

        // Обробка кожної зони
        for (int z = 0; z < m_Zones.Count(); z++)
        {
            ProcessZone(m_Zones[z], shouldWarn);
        }

        // Одноразова синхронізація статусів та повідомлення для кожного гравця
        for (int j = 0; j < m_AllPlayers.Count(); j++)
        {
            PlayerBase p = PlayerBase.Cast(m_AllPlayers[j]);
            if (!p) continue;
            if (!p.GetAllowDamage()) continue;

            bool inStrong = false;
            bool inLocal = false;

            for (int k = 0; k < m_Zones.Count(); k++)
            {
                RadiationZoneConfig zConfig = m_Zones[k];
                vector pPos = p.GetPosition();
                vector zPos = zConfig.position;
                
                // Рахуємо дистанцію як циліндр (ігноруємо Y)
                float dX = pPos[0] - zPos[0];
                float dZ = pPos[2] - zPos[2];
                float dist = Math.Sqrt((dX * dX) + (dZ * dZ));

                if (zConfig.zoneType == 1 && dist <= zConfig.radius)
                {
                    inStrong = true;
                }
                else if (zConfig.zoneType == 0 && zConfig.isGlobal == 0 && dist <= zConfig.radius)
                {
                    inLocal = true;
                }
            }

            bool wasInStrong = p.IsInStrongZone();
            bool wasInZone = p.IsInRadiationZone();
            
            bool isNowInZone = false;
            if (inStrong || inLocal)
            {
                isNowInZone = true;
            }

            if (!wasInZone && isNowInZone)
            {
                TraderMessage.PlayerWhite("Ви увійшли в радіоактивну зону.", p);
            }
            else if (wasInZone && !isNowInZone)
            {
                TraderMessage.PlayerWhite("Ви покинули радіоактивну зону.", p);
            }

            if (!wasInStrong && inStrong)
            {
                TraderMessage.PlayerWhite("Увага! Ви увійшли в зону СИЛЬНОЇ радіації!", p);
            }

            p.SetInStrongZone(inStrong);
            p.SetIsInRadiationZone(isNowInZone);

            // === ОБРОБКА ФОНЯЧОГО ОДЯГУ ===
            float dirtyRad = p.ProcessContaminatedClothing();
            if (dirtyRad > 0)
            {
                p.InsertAgent(RadZoneAgents.RADIATION, dirtyRad);
                if (shouldWarn)
                {
                    TraderMessage.PlayerWhite("Ваш одяг сильно забруднений і випромінює радіацію!", p);
                }
            }
        }
    }

    void ProcessZone(RadiationZoneConfig zone, bool shouldWarn)
    {
        // === РОЗСІЯНА ГЛОБАЛЬНА ЗОНА ===
        if (zone.zoneType == 0 && zone.isGlobal == 1)
        {
            float varRange = 0.15;
            if (zone.variation > 0.0)
            {
                varRange = zone.variation / 100.0;
            }

            for (int gi = 0; gi < m_AllPlayers.Count(); gi++)
            {
                PlayerBase gp = PlayerBase.Cast(m_AllPlayers[gi]);
                if (!gp) continue;
                if (!gp.GetAllowDamage()) continue;
                if (gp.IsInStrongZone()) continue;

                // Для глобальної зони використовуємо новий розрахунок (Біозахист / 2)
                float totalResistanceG = gp.GetTotalRadResistance(); 
                float resistanceFactorG = 1.0 - (totalResistanceG / 100.0);

                float variationMulG = Math.RandomFloatInclusive(1.0 - varRange, 1.0 + varRange);
                float agentsG = zone.power * variationMulG * resistanceFactorG;

                if (agentsG > 0.0)
                {
                    gp.InsertAgent(RadZoneAgents.RADIATION, agentsG);
                    gp.SetExternalRadiation(zone.power * variationMulG);
                }

                // Накопичуємо бруд від глобальної зони (звичайна швидкість)
                gp.AddClothingContamination(zone.contamSpeed * m_CheckInterval);
            }
            return;
        }

        // === ЛОКАЛЬНІ ЗОНИ (Сильні та Розсіяні) ===
        vector zonePos = zone.position;
        float radius = zone.radius;

        float minX = zonePos[0] - radius;
        float maxX = zonePos[0] + radius;
        float minZ = zonePos[2] - radius;
        float maxZ = zonePos[2] + radius;

        int minCellX = Math.Floor(minX / CELL_SIZE);
        int maxCellX = Math.Floor(maxX / CELL_SIZE);
        int minCellZ = Math.Floor(minZ / CELL_SIZE);
        int maxCellZ = Math.Floor(maxZ / CELL_SIZE);

        for (int cellX = minCellX; cellX <= maxCellX; cellX++)
        {
            for (int cellZ = minCellZ; cellZ <= maxCellZ; cellZ++)
            {
                string cellKey = cellX.ToString() + "_" + cellZ.ToString();
                array<PlayerBase> cellPlayers;
                if (!m_PlayerCells.Find(cellKey, cellPlayers))
                    continue;

                for (int i = 0; i < cellPlayers.Count(); i++)
                {
                    PlayerBase player = cellPlayers[i];
                    vector playerPos = player.GetPosition();
                    
                    // БЕЗКІНЕЧНИЙ ЦИЛІНДР (Ігноруємо висоту Y)
                    float distX = playerPos[0] - zonePos[0];
                    float distZ = playerPos[2] - zonePos[2];
                    float distance = Math.Sqrt((distX * distX) + (distZ * distZ));

                    if (distance > radius)
                        continue;

                    int zoneType = zone.zoneType;
                    bool alreadyInStrongZone = player.IsInStrongZone();

                    if (alreadyInStrongZone && zoneType == 0)
                        continue;

                    float absorptionRate = 1.0; 
                    float totalResistance = 0.0;
                    
                    if (zoneType == 1)
                    {
                        // В сильній зоні працює жорсткий конфіг екіпірування
                        totalResistance = ProcessStrongZoneProtection(player, shouldWarn);
                    }
                    else
                    {
                        // В локальній розсіяній зоні працює захист (Біозахист / 2)
                        totalResistance = player.GetTotalRadResistance();
                    }

                    float resistanceFactor = 1.0 - (totalResistance / 100.0);
                    float agentsPerTick;

                    if (zoneType == 0)
                    {
                        float varRangeL = 0.15;
                        if (zone.variation > 0.0)
                        {
                            varRangeL = zone.variation / 100.0;
                        }
                        float varMulL = Math.RandomFloatInclusive(1.0 - varRangeL, 1.0 + varRangeL);
                        agentsPerTick = zone.power * varMulL * resistanceFactor;
                    }
                    else
                    {
                        agentsPerTick = zone.power * absorptionRate * resistanceFactor;
                    }

                    if (agentsPerTick > 0)
                    {
                        if (zoneType == 1)
                        {
                            player.InsertAgent(RadZoneAgents.RADIATION, agentsPerTick);
                            player.SetExternalRadiation(zone.power); 
                        }
                        else
                        {
                            if (!alreadyInStrongZone)
                            {
                                player.InsertAgent(RadZoneAgents.RADIATION, agentsPerTick);
                                float shown = agentsPerTick / resistanceFactor;
                                player.SetExternalRadiation(shown);
                            }
                        }
                    }
                    else
                    {
                        player.SetExternalRadiation(0);
                    }

                    // Накопичуємо бруд (швидкість збільшена в 20 разів)
                    player.AddClothingContamination((zone.contamSpeed * m_CheckInterval) * 20.0);
                }
            }
        }
    }

    float ProcessStrongZoneProtection(PlayerBase player, bool shouldWarn)
    {
        float totalResistance = 0;
        float pieceProtection = m_ResistConfig.ClothingTotalProtection / 4.0; 

        bool maskOk = false;
        bool jacketOk = false;
        bool pantsOk = false;
        bool bootsOk = false;
        bool glovesOk = false;

        ItemBase mask = ItemBase.Cast(player.FindAttachmentBySlotName("Mask"));
        if (mask && !mask.IsRuined() && m_ResistConfig.MaskClassnames.Find(mask.GetType()) > -1)
        {
            if (mask.HasQuantity() && mask.GetQuantity() > 0)
            {
                mask.AddQuantity(-m_ResistConfig.MaskQuantityUsage);
                totalResistance += m_ResistConfig.MaskProtection;
                if (m_ResistConfig.MaskDamagePerTick > 0) mask.AddHealth("GlobalHealth", "Health", -m_ResistConfig.MaskDamagePerTick);
                maskOk = true;
            }
        }

        ItemBase jacket = ItemBase.Cast(player.FindAttachmentBySlotName("Body"));
        if (jacket && !jacket.IsRuined() && m_ResistConfig.JacketClassnames.Find(jacket.GetType()) > -1)
        {
            totalResistance += pieceProtection;
            if (m_ResistConfig.ClothingDamagePerTick > 0) jacket.AddHealth("GlobalHealth", "Health", -m_ResistConfig.ClothingDamagePerTick);
            jacketOk = true;
        }

        ItemBase pants = ItemBase.Cast(player.FindAttachmentBySlotName("Legs"));
        if (pants && !pants.IsRuined() && m_ResistConfig.PantsClassnames.Find(pants.GetType()) > -1)
        {
            totalResistance += pieceProtection;
            if (m_ResistConfig.ClothingDamagePerTick > 0) pants.AddHealth("GlobalHealth", "Health", -m_ResistConfig.ClothingDamagePerTick);
            pantsOk = true;
        }

        ItemBase boots = ItemBase.Cast(player.FindAttachmentBySlotName("Feet"));
        if (boots && !boots.IsRuined() && m_ResistConfig.BootsClassnames.Find(boots.GetType()) > -1)
        {
            totalResistance += pieceProtection;
            if (m_ResistConfig.ClothingDamagePerTick > 0) boots.AddHealth("GlobalHealth", "Health", -m_ResistConfig.ClothingDamagePerTick);
            bootsOk = true;
        }

        ItemBase gloves = ItemBase.Cast(player.FindAttachmentBySlotName("Gloves"));
        if (gloves && !gloves.IsRuined() && m_ResistConfig.GlovesClassnames.Find(gloves.GetType()) > -1)
        {
            totalResistance += pieceProtection;
            if (m_ResistConfig.ClothingDamagePerTick > 0) gloves.AddHealth("GlobalHealth", "Health", -m_ResistConfig.ClothingDamagePerTick);
            glovesOk = true;
        }

        if (shouldWarn)
        {
            array<string> warnings = new array<string>;
            
            if (!bootsOk) warnings.Insert("Ноги печуть!");
            if (!glovesOk) warnings.Insert("Руки обпікає радіацією!");
            if (!maskOk) warnings.Insert("Важко дихати, легені обпікає!");
            if (!jacketOk) warnings.Insert("Тіло пече від радіації!");
            if (!pantsOk) warnings.Insert("Шкіра на ногах пече!");

            if (warnings.Count() > 0)
            {
                string warnMsg = warnings.GetRandomElement();
                TraderMessage.PlayerWhite(warnMsg, player);
            }
        }

        // Шкода гравцю, якщо не вистачає захисту
        if (!bootsOk || !glovesOk || !maskOk || !jacketOk || !pantsOk)
        {
            player.AddHealth("GlobalHealth", "", -5);
        }

        if (totalResistance > 95.0)
            totalResistance = 95.0;

        return totalResistance;
    }
};