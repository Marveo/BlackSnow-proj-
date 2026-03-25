class TerjeDosimetrCdv700 : TerjeDosimetrBase
{
	override void TerjeEnableDisplayClient()
	{
		SetAnimationPhase("arrow", 0.5);
	}
	
	override void TerjeDisableDisplayClient()
	{
		SetAnimationPhase("arrow", 0);
	}
	
	override void TerjeUpdateDisplayValueClient(int value)
	{
		float arrowPos = Math.Clamp(value / TerjeGeigerMaxLimit(), 0, 1);
		SetAnimationPhase("arrow", arrowPos);
	}
	
	
	override int TerjeGeigerSoundsCount()
	{
		return 4;
	}
	
	override int TerjeGeigerMaxLimit()
	{
		return 5000;
	}
	
	override float TerjeGeigerRadiationModifier()
	{
		return 1000;
	}
	
	override float GetTerjeSensitivityRadius()
	{
		return 10;
	}
	
	override string TerjeGeigerGetSoundSet(int index)
	{
		return "TerjeGeigerClasic" + index + "_SoundSet";
	}
}