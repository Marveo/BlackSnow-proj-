class RadiationResistanceConfig
{
    string classname;
    string slotname;
    float resist;

    void RadiationResistanceConfig(string className, string slotName, float resistance)
    {
        classname = className;
        slotname = slotName;
        resist = resistance;
    }
}

static ref RadiationResistanceConfigManager g_RadiationResistanceConfigManager;

static ref RadiationResistanceConfigManager GetRadiationResistanceConfigManager()
{
    if (!g_RadiationResistanceConfigManager)
    {
        g_RadiationResistanceConfigManager = new RadiationResistanceConfigManager();
    }
    return g_RadiationResistanceConfigManager;
}

class RadiationResistanceConfigManager
{
    const static string configPath = "$profile:radiation_resistance.json";
    ref array<ref RadiationResistanceConfig> resistanceConfigs = new array<ref RadiationResistanceConfig>;

    void RadiationResistanceConfigManager()
    {
        LoadConfig();
    }

    // Загрузка конфигурации
    void LoadConfig()
    {
        if (FileExist(configPath))
        {
            JsonFileLoader<array<ref RadiationResistanceConfig>>.JsonLoadFile(configPath, resistanceConfigs);
            Print("Radiation resistance config file loaded: " + configPath);
        }
    }

    array<ref RadiationResistanceConfig> GetConfigs()
    {
        return resistanceConfigs;
    }
}