class RadiationManager_Clustered
{
    const float CELL_SIZE = 100.0;
    float m_CheckInterval = 3.0;
    float m_Timer = 0;

    const string SEVA_STALKER_SET = "seva_stalker";
    const string SEVA_DUTY_SET = "seva_dolg";
    const string SEVA_FREEDOM_SET = "seva_freedom";
    const float SEVA_FULL_SET_RESISTANCE = 60.0;

    ref map<string, ref array<PlayerBase>> m_PlayerCells;
    ref array<Man> m_AllPlayers;
    ref array<ref RadiationZoneConfig> m_Zones;
    ref array<ref RadiationResistanceConfig> m_ResistanceConfigs;

    void RadiationManager_Clustered()
    {
        m_PlayerCells = new map<string, ref array<PlayerBase>>;
        m_AllPlayers = new array<Man>();
        m_Zones = GetRadiationZoneConfigManager().GetZones();
        m_ResistanceConfigs = GetRadiationResistanceConfigManager().GetConfigs();
    }

    void OnTick(float deltaT)
    {
        m_Timer += deltaT;
        if (m_Timer < m_CheckInterval)
            return;

        m_Timer = 0;
        m_PlayerCells.Clear();

        GetGame().GetPlayers(m_AllPlayers);

        foreach (Man man : m_AllPlayers)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (!player || !player.GetAllowDamage())
                continue;

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

        foreach (RadiationZoneConfig zone : m_Zones)
        {
            ProcessZone(zone);
        }
    }

    void ProcessZone(RadiationZoneConfig zone)
    {
        // === РОЗСІЯНА ГЛОБАЛЬНА ЗОНА (zoneType=0, isGlobal=1) ===
        if (zone.zoneType == 0 && zone.isGlobal == 1)
        {
            float varRange = 0.15; // дефолт ±15%
            if (zone.variation > 0.0)
            {
                varRange = zone.variation / 100.0;
            }

            // застосовуємо до всіх гравців (не дивимось на координати)
            for (int gi = 0; gi < m_AllPlayers.Count(); gi++)
            {
                PlayerBase gp = PlayerBase.Cast(m_AllPlayers[gi]);
                if (!gp || !gp.GetAllowDamage())
                    continue;

                // не заважаємо пріоритету сильної зони
                if (gp.IsInStrongZone())
                    continue;

                float totalResistanceG = CalculatePlayerResistance(gp);
                float resistanceFactorG = 1.0 - (totalResistanceG / 100.0);

                float variationMulG = Math.RandomFloatInclusive(1.0 - varRange, 1.0 + varRange);
                float agentsG = zone.power * variationMulG * resistanceFactorG;

                if (agentsG > 0.0)
                {
                    gp.InsertAgent(RadZoneAgents.RADIATION, agentsG);
                    gp.SetExternalRadiation(zone.power * variationMulG);

                    float totalG = gp.GetSingleAgentCount(RadZoneAgents.RADIATION);
                    Print("[RAD DEBUG] ГЛОБАЛЬНА розсіяна: +" + agentsG.ToString() + " агентів (всього: " + totalG.ToString() + ")");
                }
            }
            return; // глобальну розсіяний блок опрацювали повністю
        }

        // === Далі — локальні зони (розсіяні та сильні), як у тебе було, але з "плаваючою" розсіяною ===
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
                    float distance = vector.Distance(playerPos, zonePos);

                    if (distance > radius)
                        continue;

                    int zoneType = zone.zoneType; // 0 - розсіяна, 1 - сильна
                    bool alreadyInStrongZone = player.IsInStrongZone();

                    // Якщо гравець уже в сильній зоні, і це розсіяна — ігноруємо
                    if (alreadyInStrongZone && zoneType == 0)
                        continue;

                    if (zoneType == 1)
                        player.SetInStrongZone(true);

                    float absorptionRate;
                    if (zoneType == 1)
                    {
                        absorptionRate = 1.0 - (distance / radius);
                        if (absorptionRate < 0.0)
                            absorptionRate = 0.0;
                    }
                    else
                    {
                        // розсіяна локальна — сила скрізь однакова, але "плаває"
                        absorptionRate = 1.0;
                    }

                    float totalResistance = CalculatePlayerResistance(player);
                    float resistanceFactor = 1.0 - (totalResistance / 100.0);

                    float agentsPerTick;
                    if (zoneType == 0)
                    {
                        // "плаваюча" потужність для розсіяної
                        float varRangeL = 0.15; // дефолт ±15%
                        if (zone.variation > 0.0)
                        {
                            varRangeL = zone.variation / 100.0;
                        }
                        float varMulL = Math.RandomFloatInclusive(1.0 - varRangeL, 1.0 + varRangeL);
                        agentsPerTick = zone.power * varMulL * resistanceFactor;
                    }
                    else
                    {
                        // сильна зона як було
                        agentsPerTick = zone.power * absorptionRate * resistanceFactor;
                    }

                    float newCount = 0;

                    if (agentsPerTick > 0)
                    {
                        if (zoneType == 1)
                        {
                            // === СИЛЬНА ЗОНА ===
                            player.InsertAgent(RadZoneAgents.RADIATION, agentsPerTick);
                            player.SetExternalRadiation(zone.power * absorptionRate);

                            newCount = player.GetSingleAgentCount(RadZoneAgents.RADIATION);
                            Print("[RAD DEBUG] у СИЛЬНІЙ зоні: +" + agentsPerTick.ToString() + " агентів (всього: " + newCount.ToString() + ")");

                            // Якщо без костюма — наносимо шкоду
                            if (!HasProtectiveSuit(player))
                            {
                                player.AddHealth("GlobalHealth", "", -5);
                                Print("[RAD DEBUG] без костюма — -5 HP");
                            }
                        }
                        else
                        {
                            // === РОЗСІЯНА ЛОКАЛЬНА ===
                            if (!alreadyInStrongZone)
                            {
                                player.InsertAgent(RadZoneAgents.RADIATION, agentsPerTick);
                                // для дозиметра показуємо саме "плаваюче" значення
                                // (щоб кліки/показники були синхронні з тим, що реально отримує гравець)
                                // тут absorptionRate = 1.0
                                float shown = agentsPerTick / resistanceFactor; // приблизно = zone.power * varMul
                                player.SetExternalRadiation(shown);

                                newCount = player.GetSingleAgentCount(RadZoneAgents.RADIATION);
                                Print("[RAD DEBUG] у РОЗСІЯНІЙ зоні: +" + agentsPerTick.ToString() + " агентів (всього: " + newCount.ToString() + ")");
                            }
                        }
                    }
                    else
                    {
                        player.SetExternalRadiation(0);
                    }
                }
            }
        }

        // 🔄 Завершальна синхронізація прапорця сильної зони
        for (int j = 0; j < m_AllPlayers.Count(); j++)
        {
            PlayerBase p = PlayerBase.Cast(m_AllPlayers[j]);
            if (!p)
                continue;

            bool inStrong = false;
            bool inLocal = false;
            bool inGlobal = false;

            for (int k = 0; k < m_Zones.Count(); k++)
            {
                RadiationZoneConfig z = m_Zones[k];

                // === сильна зона ===
                if (z.zoneType == 1 && vector.Distance(p.GetPosition(), z.position) <= z.radius)
                {
                    inStrong = true;
                    break;
                }

                // === розсіяна локальна ===
                if (z.zoneType == 0 && z.isGlobal == 0 && vector.Distance(p.GetPosition(), z.position) <= z.radius)
                {
                    inLocal = true;
                }

                // === розсіяна глобальна ===
                if (z.zoneType == 0 && z.isGlobal == 1)
                {
                    inGlobal = true;
                }
            }

            // Встановлюємо прапорець сильної зони
            p.SetInStrongZone(inStrong);

            // Якщо гравець у локальній зоні — він "у зоні"
            // Якщо лише у глобальній — НЕ рахується як "у зоні"
            if (inStrong || inLocal)
            {
                p.SetIsInRadiationZone(true);
            }
            else
            {
                p.SetIsInRadiationZone(false);
            }

            // Для дебагу
            if (inGlobal && !inStrong && !inLocal)
            {
                Print("[RAD DEBUG] " + p.ToString() + " перебуває лише в ГЛОБАЛЬНІЙ зоні — випромінення інших враховується");
            }
        }
    }
 
    float CalculatePlayerResistance(PlayerBase player)
    {
        float totalResistance = 0;

        string sevaType = CheckFullSevaSet(player);
        if (sevaType != "")
        {
            totalResistance = Math.Clamp(totalResistance + SEVA_FULL_SET_RESISTANCE, 0.0, 100.0);
            return totalResistance;
        }

        foreach (RadiationResistanceConfig config : m_ResistanceConfigs)
        {
            if (config.classname == SEVA_STALKER_SET || config.classname == SEVA_DUTY_SET || config.classname == SEVA_FREEDOM_SET)
                continue;

            if (config.slotname == "Back")
            {
                EntityAI backpack = player.GetInventory().FindAttachment(InventorySlots.BACK);
                if (backpack && backpack.GetType() == config.classname)
                {
                    totalResistance = Math.Clamp(totalResistance + config.resist, 0.0, 100.0);
                    continue;
                }
            }
            else if (player.HasItemInSlot(config.classname, config.slotname))
            {
                if (config.slotname == "Mask")
                {
                    EntityAI mask = player.FindAttachmentBySlotName("Mask");
                    if (mask)
                    {
                        ItemBase filterItem = mask.FindAttachmentBySlotName("GasMaskFilter");
                        if (!filterItem || filterItem.IsRuined() || filterItem.GetQuantity() == 0)
                            continue;
                    }
                    else
                        continue;
                }

                totalResistance = Math.Clamp(totalResistance + config.resist, 0.0, 100.0);
            }
        }

        return totalResistance;
    }

    string CheckFullSevaSet(PlayerBase player)
    {
        if (CheckSevaComponents(player, "SGE_Stalker_Seva_Vest", "SGE_Stalker_Seva_Jacket", "SGE_Stalker_Seva_BackPack", "SGE_Stalker_Seva_Cape", "SGE_Stalker_Seva_Pants"))
            return SEVA_STALKER_SET;
        if (CheckSevaComponents(player, "SGE_Duty_Seva_Vest", "SGE_Duty_Seva_Jacket", "SGE_Duty_Seva_BackPack", "SGE_Duty_Seva_Cape", "SGE_Duty_Seva_Pants"))
            return SEVA_DUTY_SET;
        if (CheckSevaComponents(player, "SGE_Freedom_Seva_Vest", "SGE_Freedom_Seva_Jacket", "SGE_Freedom_Seva_BackPack", "SGE_Freedom_Seva_Cape", "SGE_Freedom_Seva_Pants"))
            return SEVA_FREEDOM_SET;

        return "";
    }

    bool CheckSevaComponents(PlayerBase player, string vestType, string jacketType, string backpackType, string capeType, string pantsType)
    {
        EntityAI vest = player.GetInventory().FindAttachment(InventorySlots.VEST);
        if (!vest || vest.GetType() != vestType || vest.IsRuined())
            return false;

        EntityAI jacket = player.GetInventory().FindAttachment(InventorySlots.BODY);
        if (!jacket || jacket.GetType() != jacketType || jacket.IsRuined())
            return false;

        EntityAI backpack = player.GetInventory().FindAttachment(InventorySlots.BACK);
        if (!backpack || backpack.GetType() != backpackType || backpack.IsRuined())
            return false;

        EntityAI cape = player.GetInventory().FindAttachment(InventorySlots.HEADGEAR);
        if (!cape || cape.GetType() != capeType || cape.IsRuined())
            return false;

        ItemBase glassItem = cape.FindAttachmentBySlotName("Stalker_Seva_Glass");
        if (!glassItem || glassItem.IsRuined())
            return false;

        EntityAI pants = player.GetInventory().FindAttachment(InventorySlots.LEGS);
        if (!pants || pants.GetType() != pantsType || pants.IsRuined())
            return false;

        return true;
    }

    bool HasProtectiveSuit(PlayerBase player)
    {
        string requiredJacket = "NBCJacket";
        string requiredPants = "NBCPants";
        string requiredGloves = "NBCGloves";
        string requiredBoots = "NBCBoots";

        EntityAI jacket = player.FindAttachmentBySlotName("Body");
        EntityAI pants = player.FindAttachmentBySlotName("Legs");
        EntityAI gloves = player.FindAttachmentBySlotName("Gloves");
        EntityAI boots = player.FindAttachmentBySlotName("Feet");

        if (!jacket || jacket.GetType() != requiredJacket) return false;
        if (!pants || pants.GetType() != requiredPants) return false;
        if (!gloves || gloves.GetType() != requiredGloves) return false;
        if (!boots || boots.GetType() != requiredBoots) return false;

        return true;
    }

};
