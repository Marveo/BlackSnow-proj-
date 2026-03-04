modded class ActionUseTesterSelf
{	
	override void OnEndServer( ActionData action_data )
    {
		if (action_data.m_WasExecuted && action_data.m_MainItem)
        {
            inrFon = Math.Ceil(action_data.m_Player.GetAccumulatedRadiation()).ToString();
            
            DisplayDosimeterValue(action_data.m_MainItem, inrFon);
            
            inrFon = "";
			
			ItemBase mainItem = ItemBase.Cast( action_data.m_MainItem );
        
			ItemBase batteryItem = mainItem.FindAttachmentBySlotName("BatteryD");
			if ( batteryItem )
			{
				batteryItem.AddQuantity( -2.0 );
			}
        }
    }
};