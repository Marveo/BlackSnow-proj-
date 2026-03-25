// ─────────────────────────────────
//  TemperatureZones PlayerBase.c
// ─────────────────────────────────
modded class PlayerBase
{
	bool TZ_GetZoneTemperature(out float outTemp)
	{
		TZ_Settings cfg = GetTZ_Settings();
		if (!cfg || !cfg.Zones || cfg.Zones.Count() == 0) return false;

		vector pos = GetPosition();

		float bestDistSq = 1e12;
		bool  found      = false;

		foreach (TZ_Zone zone : cfg.Zones)
		{
			string shape = zone.TemperatureZoneShape;
			shape.ToLower();
			if (shape == "")
				shape = "sphere";

			float distSq;
				if (shape == "radial")
				{
					float dx = pos[0] - zone.Position[0];
					float dz = pos[2] - zone.Position[2];
					distSq = dx * dx + dz * dz;

					if (distSq <= zone.Radius * zone.Radius && distSq < bestDistSq)
					{
						if (zone.Height <= 0 || Math.AbsFloat(pos[1] - zone.Position[1]) <= zone.Height * 0.5)
						{
							outTemp    = zone.Temperature;
							bestDistSq = distSq;
							found      = true;
						}
					}
				}
				else if (shape == "rectangle")
				{
					vector pos1 = zone.Position;
					vector pos2 = zone.Position2;
					if (pos2[0] == 0 && pos2[2] == 0 && (zone.Size[0] != 0 || zone.Size[2] != 0))
					{
						float halfXLegacy = Math.AbsFloat(zone.Size[0]) * 0.5;
						float halfZLegacy = Math.AbsFloat(zone.Size[2]) * 0.5;
						pos1 = Vector(zone.Position[0] - halfXLegacy, zone.Position[1], zone.Position[2] - halfZLegacy);
						pos2 = Vector(zone.Position[0] + halfXLegacy, zone.Position[1], zone.Position[2] + halfZLegacy);
					}
					else if (pos2[0] == 0 && pos2[2] == 0)
					{
						continue;
					}

					float minX = Math.Min(pos1[0], pos2[0]);
					float maxX = Math.Max(pos1[0], pos2[0]);
					float minZ = Math.Min(pos1[2], pos2[2]);
					float maxZ = Math.Max(pos1[2], pos2[2]);

					if (pos[0] >= minX && pos[0] <= maxX && pos[2] >= minZ && pos[2] <= maxZ)
					{
						float centerX = (minX + maxX) * 0.5;
						float centerZ = (minZ + maxZ) * 0.5;
						float dxAbs = Math.AbsFloat(pos[0] - centerX);
						float dzAbs = Math.AbsFloat(pos[2] - centerZ);
						distSq = dxAbs * dxAbs + dzAbs * dzAbs;
						if (distSq < bestDistSq)
						{
							float centerY = (pos1[1] + pos2[1]) * 0.5;
							if (zone.Height <= 0 || Math.AbsFloat(pos[1] - centerY) <= zone.Height * 0.5)
							{
								outTemp    = zone.Temperature;
								bestDistSq = distSq;
								found      = true;
							}
						}
					}
				}
			else
			{
				distSq = vector.DistanceSq(pos, zone.Position);
				if (distSq <= zone.Radius * zone.Radius && distSq < bestDistSq)
				{
					outTemp    = zone.Temperature;
					bestDistSq = distSq;
					found      = true;
				}
			}
		}
		return found;
	}
}
