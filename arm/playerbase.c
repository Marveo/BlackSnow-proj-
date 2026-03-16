modded class PlayerBase
{	
	 
	override void SetActions(out TInputActionMap InputActionMap)
    {
        super.SetActions(InputActionMap);
		
		AddAction(ActionCraftAmmo				,InputActionMap);
		AddAction(ActionPour				 	,InputActionMap);
        AddAction(ActionPourBullets_9x19		,InputActionMap);
	}
	
}