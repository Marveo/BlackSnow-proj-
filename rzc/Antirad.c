class Radiation_BottlePills: VitaminBottle
{
	override void InitItemVariables()
	{
		super.InitItemVariables();

		can_this_be_combined = true;
	}
	
	override void OnConsume(float amount, PlayerBase consumer)
	{
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionEatPillFromBottle);
	}
}