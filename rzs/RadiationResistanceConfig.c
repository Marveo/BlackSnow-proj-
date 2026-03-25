class RadiationResistanceConfig
{
    float MaskProtection;
    float MaskQuantityUsage;
    float MaskDamagePerTick;
    ref array<string> MaskClassnames;

    float ClothingTotalProtection;
    float ClothingDamagePerTick;
    ref array<string> JacketClassnames;
    ref array<string> PantsClassnames;
    ref array<string> BootsClassnames;
    ref array<string> GlovesClassnames;

    void RadiationResistanceConfig()
    {
        MaskClassnames = new array<string>;
        JacketClassnames = new array<string>;
        PantsClassnames = new array<string>;
        BootsClassnames = new array<string>;
        GlovesClassnames = new array<string>;
        
        MaskProtection = 25.0;
        MaskQuantityUsage = 1.0;
        MaskDamagePerTick = 0.0;

        ClothingTotalProtection = 70.0;
        ClothingDamagePerTick = 1.0;
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
    const static string configPath = "$profile:radiation_lists_config.json";
    ref RadiationResistanceConfig configData;

    void RadiationResistanceConfigManager()
    {
        LoadConfig();
    }

    void LoadConfig()
    {
        // Обов'язково створюємо пустий об'єкт перед будь-якими діями з JSON
        configData = new RadiationResistanceConfig();

        if (FileExist(configPath))
        {
            Print("[RAD DEBUG] Знайдено файл конфігу захисту, завантажуємо: " + configPath);
            JsonFileLoader<RadiationResistanceConfig>.JsonLoadFile(configPath, configData);
        }
        else
        {
            Print("[RAD DEBUG] Файл конфігу захисту НЕ знайдено! Створюємо стандартний...");
            SaveDefaultConfig();
        }
    }

    void SaveDefaultConfig()
    {
        configData.MaskClassnames.Insert("GasMask");
        configData.JacketClassnames.Insert("NBCJacketGray");
        configData.PantsClassnames.Insert("NBCPantsGray");
        configData.BootsClassnames.Insert("NBCBootsGray");
        configData.GlovesClassnames.Insert("NBCGlovesGray");

        JsonFileLoader<RadiationResistanceConfig>.JsonSaveFile(configPath, configData);
        Print("[RAD DEBUG] Стандартний конфіг успішно збережено у: " + configPath);
    }

    RadiationResistanceConfig GetConfig()
    {
        return configData;
    }
}