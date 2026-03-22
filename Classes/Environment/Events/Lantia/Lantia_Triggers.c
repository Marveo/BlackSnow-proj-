class LantiaLavaTrigger: Trigger
{
    override void OnEnterServerEvent( TriggerInsider insider )
    {
        PlayerBase player = PlayerBase.Cast( insider.GetObject() );
        if ( player )
        {
            player.SetHealth( 0.0 );
        }
    }
}
class TeleportToLantiaTrigger: Trigger
{
    override void OnEnterServerEvent( TriggerInsider insider )
    {
        PlayerBase player = PlayerBase.Cast( insider.GetObject() );
        if ( player )
        {
            LantiaEC_Server lantiaController = MissionBaseWorld.Cast( GetGame().GetMission() ).GetLantiaControllerServer();

            if ( lantiaController )
            {
                lantiaController.TeleportPlayer( player, 0 );
            }
        }
    }
}
class TeleportFromLantiaTrigger: Trigger
{
    override void OnEnterServerEvent( TriggerInsider insider )
    {
        PlayerBase player = PlayerBase.Cast( insider.GetObject() );
        if ( player )
        {
            LantiaEC_Server lantiaController = MissionBaseWorld.Cast( GetGame().GetMission() ).GetLantiaControllerServer();

            if ( lantiaController )
            {
                if ( lantiaController.GetLantiaStateServer() <= ELantia.PROBES_DETECT )
                {
                    // teleport back to A3
                    lantiaController.TeleportPlayer( player, 1 );
                }
                else
                {
                    if ( lantiaController.GetLantiaStateServer() >= ELantia.LINK_ATTACK )
                    {
                        // teleport to random place, deal damage
                        lantiaController.TeleportPlayer( player, 3 );
                    }
                    else
                    {
                        // teleport to a random place
                        lantiaController.TeleportPlayer( player, 2 ); 
                    }
                }
            }
        }
    }
}