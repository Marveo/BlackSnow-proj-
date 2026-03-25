// ────────────────────────────
//  TemperatureZones settings
// ────────────────────────────
class TZ_Zone
{
	vector Position;
	vector Position2;
	float  Radius;
	float  Temperature;
	string Name;
	string TemperatureZoneShape;
	vector Size;
	float  Height;
};

class TZ_Settings
{
	private const static string CONFIG_ROOT     = "$profile:\\TemperatureZones\\";
	private const static string CONFIG_FILENAME = "TemperatureZones.json";

	ref array<ref TZ_Zone> Zones;

	void TZ_Settings()
	{
		Zones = new array<ref TZ_Zone>;
	}

	private void EnsureConfigDirectoryExists()
	{
		if (!FileExist(CONFIG_ROOT))
			MakeDirectory(CONFIG_ROOT);
	}

	private void GenerateDefaults()
	{
		TZ_Zone z;

		z = new TZ_Zone;
		z.Name        = "ColdZone";
		z.Position    = "1000 1000 1000";
		z.Radius      = 100;
		z.Temperature = -5;
		z.TemperatureZoneShape   = "SPHERE";
		Zones.Insert( z );

		z = new TZ_Zone;
		z.Name        = "HotZone";
		z.Position    = "1500 1500 1500";
		z.Radius      = 100;
		z.Temperature = 40;
		z.TemperatureZoneShape   = "SPHERE";
		Zones.Insert( z );

		z = new TZ_Zone;
		z.Name        = "CoolZone";
		z.Position    = "2000 2000 2000";
		z.Radius      = 100;
		z.Temperature = 10;
		z.TemperatureZoneShape   = "SPHERE";
		Zones.Insert( z );

		z = new TZ_Zone;
		z.Name        = "WarmZone";
		z.Position    = "2500 2500 2500";
		z.Radius      = 100;
		z.Temperature = 25;
		z.TemperatureZoneShape   = "SPHERE";
		Zones.Insert( z );

		z = new TZ_Zone;
		z.Name        = "RadialZone";
		z.Position    = "3000 0 3000";
		z.Radius      = 150;
		z.Temperature = 15;
		z.TemperatureZoneShape   = "RADIAL";
		Zones.Insert( z );

		z = new TZ_Zone;
		z.Name        = "SquareZone";
		z.Position    = "3500 0 3500";
		z.Position2   = "3550 0 3550";
		z.Temperature = 5;
		z.TemperatureZoneShape   = "RECTANGLE";
		Zones.Insert( z );
	}

	private void SaveDefaults()
	{
		string filePath = CONFIG_ROOT + CONFIG_FILENAME;

		EnsureConfigDirectoryExists();

		string json = "{\n    \"Zones\": [\n";
		for (int i = 0; i < Zones.Count(); i++)
		{
			TZ_Zone zone = Zones[i];
			bool isRectangle = zone.TemperatureZoneShape == "RECTANGLE";

			json += "        {\n";
			json += string.Format("            \"Position\": [%1, %2, %3],\n", zone.Position[0], zone.Position[1], zone.Position[2]);
			if (isRectangle)
				json += string.Format("            \"Position2\": [%1, %2, %3],\n", zone.Position2[0], zone.Position2[1], zone.Position2[2]);
			if (!isRectangle)
				json += string.Format("            \"Radius\": %1,\n", zone.Radius);
			json += string.Format("            \"Temperature\": %1,\n", zone.Temperature);
			json += string.Format("            \"Name\": \"%1\",\n", zone.Name);
			json += string.Format("            \"ZoneShape\": \"%1\"", zone.TemperatureZoneShape);

			if (zone.Height > 0)
			{
				json += ",\n";
				json += string.Format("            \"Height\": %1", zone.Height);
			}

			json += "\n";
			json += "        }";

			if (i < Zones.Count() - 1)
				json += ",\n";
			else
				json += "\n";
		}
		json += "    ]\n}\n";

		FileHandle handle = OpenFile(filePath, FileMode.WRITE);
		if (handle != 0)
		{
			FPrint(handle, json);
			CloseFile(handle);
		}
	}

	void Load()
	{
		string filePath = CONFIG_ROOT + CONFIG_FILENAME;

		EnsureConfigDirectoryExists();

		if (FileExist(filePath))
		{
			string fileContent;
			FileHandle handle = OpenFile(filePath, FileMode.READ);
			if (handle != 0)
			{
				ReadFile(handle, fileContent, 100000000);
				CloseFile(handle);
			}

			if (fileContent != "")
			{
				if (fileContent.Contains("\"Shape\""))
					fileContent.Replace("\"Shape\"", "\"ZoneShape\"");
				
				if (fileContent.Contains("\"ZoneShape\""))
					fileContent.Replace("\"ZoneShape\"", "\"TemperatureZoneShape\"");


				string errorMessage;
				if (!JsonFileLoader<TZ_Settings>.LoadData(fileContent, this, errorMessage))
				{
					Print("[TemperatureZones] Failed to load config, regenerating defaults: " + errorMessage);
					Zones.Clear();
					GenerateDefaults();
					Save();
					return;
				}
			}

			if (!Zones)
				Zones = new array<ref TZ_Zone>;
		}
		else
		{
			Zones.Clear();
			GenerateDefaults();
			SaveDefaults();
			Print("[TemperatureZones] First time launch detected! Generating config files.");
		}
	}

	void Save()
	{
		SaveDefaults();
	}
	};

	ref TZ_Settings g_TZ_Settings;

	static TZ_Settings GetTZ_Settings()
	{
		if (!g_TZ_Settings)
		{
			g_TZ_Settings = new TZ_Settings();

			if (!GetGame().IsClient())
				g_TZ_Settings.Load();
		}
		return g_TZ_Settings;
	}

	static void SetTZ_Settings(TZ_Settings settings)
	{
		g_TZ_Settings = settings;
	}
