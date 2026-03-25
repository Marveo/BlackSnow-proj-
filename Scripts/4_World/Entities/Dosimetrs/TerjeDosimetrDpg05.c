class TerjeDosimetrDpg05 : TerjeDosimetrBase
{
	override int TerjeGeigerSoundsCount()
	{
		return 4;
	}
	
	override int TerjeGeigerMaxLimit()
	{
		return 999;
	}
	
	override float TerjeGeigerRadiationModifier()
	{
		return 100;
	}
	
	override float GetTerjeSensitivityRadius()
	{
		return 1.5;
	}
	
	override string TerjeGeigerGetSoundSet(int index)
	{
		return "TerjeGeigerElectro" + index + "_SoundSet";
	}
}