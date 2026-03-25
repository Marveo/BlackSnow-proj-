class ActionPlaceAmmoDrill : ActionPlaceObject
{
    void ActionPlaceAmmoDrill()
	{
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_PLACING_2HD;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_CallbackClass	= ActionPlaceObjectCB;
	}
	override void SetupAnimation( ItemBase item )
    {

    }
} 