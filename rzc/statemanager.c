enum RadDebuffSymptomIDs
{
	SYMPTOM_RAD3 = 10939,
    SYMPTOM_RAD4 = 10940,
};

modded class SymptomManager
{
    override void Init()
	{
		super.Init();

		RegisterSymptom(new RadSymptomStageThree);
        RegisterSymptom(new RadSymptomStageFour);
	}
}