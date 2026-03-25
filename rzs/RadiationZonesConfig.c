class RadiationZoneConfig
{
    vector position;
    float radius;
    float power;
    int zoneType;   // 0 = розсіяна, 1 = сильна

    int isGlobal;   // 0 = локальна, 1 = глобальна (для розсіяної зони)
    float MapSize;  // розмір мапи (для глобальної зони), якщо isGlobal=1
    float variation; // коливання потужності у відсотках, напр. 15 = 15 відсотків

    float contamSpeed; 

    void RadiationZoneConfig(vector pos, float rad, float pow, int type = 0)
    {
        position = pos;
        radius = rad;
        power = pow;
        zoneType = type;

        isGlobal = 0;
        MapSize = 0.0;
        variation = 0.0; 
        contamSpeed = 0.0035; // 8 годин для 100%
    }
}

static ref RadiationZoneConfigManager g_RadiationZoneConfigManager;

static ref RadiationZoneConfigManager GetRadiationZoneConfigManager()
{
    if (!g_RadiationZoneConfigManager)
    {
        g_RadiationZoneConfigManager = new RadiationZoneConfigManager();
    }
    return g_RadiationZoneConfigManager;
}

class RadiationZoneConfigManager
{
    const static string configPath = "$profile:radiationzones.json";
    ref array<ref RadiationZoneConfig> zones;

    void RadiationZoneConfigManager()
    {
        zones = new array<ref RadiationZoneConfig>;
        LoadConfig();
    }

    void LoadConfig()
    {
        if (FileExist(configPath))
        {
            Print("[RAD DEBUG] Знайдено файл конфігу зон, завантажуємо: " + configPath);
            JsonFileLoader<array<ref RadiationZoneConfig>>.JsonLoadFile(configPath, zones);
        }
        else
        {
            Print("[RAD DEBUG] Файл конфігу зон НЕ знайдено! Створюємо стандартний...");
            SaveDefaultConfig();
        }
    }

    void SaveDefaultConfig()
    {
        // Приклад 1: Глобальна розсіяна зона
        RadiationZoneConfig globalZone = new RadiationZoneConfig("0 0 0", 0, 5.0, 0);
        globalZone.isGlobal = 1;
        globalZone.MapSize = 15360.0; // Приблизний розмір Чорнарусі
        globalZone.variation = 15.0;
        zones.Insert(globalZone);

        // Приклад 2: Локальна сильна зона
        RadiationZoneConfig strongZone = new RadiationZoneConfig("1500 0 14000", 200.0, 100.0, 1);
        strongZone.isGlobal = 0;
        strongZone.variation = 0.0;
        zones.Insert(strongZone);

        JsonFileLoader<array<ref RadiationZoneConfig>>.JsonSaveFile(configPath, zones);
        Print("[RAD DEBUG] Стандартний конфіг зон успішно збережено у: " + configPath);
    }

    array<ref RadiationZoneConfig> GetZones()
    {
        return zones;
    }
}