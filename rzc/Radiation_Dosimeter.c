class Radiation_Dosimeter_Pripyat extends ItemBase
{
	override void SetActions()
	{	
		super.SetActions();
		
		AddAction(ActionUseTesterSelf);
	}    
}