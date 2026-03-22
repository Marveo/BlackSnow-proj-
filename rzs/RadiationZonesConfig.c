class RadiationZoneConfig
{
    vector position;
    float radius;
    float power;
    int zoneType;   // 0 = розсіяна, 1 = сильна

    // ✅ нові поля
    int isGlobal;   // 0 = локальна, 1 = глобальна (для розсіяної зони)
    float MapSize;  // розмір мапи (для глобальної зони), якщо isGlobal=1
    float variation; // коливання потужності у %, напр. 15 = ±15%

    void RadiationZoneConfig(vector pos, float rad, float pow, int type = 0)
    {
        position = pos;
        radius = rad;
        power = pow;
        zoneType = type;

        // значення за замовчуванням (JSON може їх перезаписати)
        isGlobal = 0;
        MapSize = 0.0;
        variation = 0.0; // якщо 0 → у коді використаємо дефолт ±15%
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
    ref array<ref RadiationZoneConfig> zones = new array<ref RadiationZoneConfig>;

    void RadiationZoneConfigManager()
    {
        LoadConfig();
    }

    // Загрузка конфигурации
    void LoadConfig()
    {
        if (FileExist(configPath))
        {
            JsonFileLoader<array<ref RadiationZoneConfig>>.JsonLoadFile(configPath, zones);
        }
    }

    // Получение списка всех зон
    array<ref RadiationZoneConfig> GetZones()
    {
        return zones;
    }
}