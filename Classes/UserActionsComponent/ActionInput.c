class ToggleAPSIActionInput: DefaultActionInput
{
	ref ActionTarget target_new;
	
	void ToggleAPSIActionInput( PlayerBase player )
	{
		SetInput( "UAToggleAPSI" );
		m_InputType = ActionInputType.AIT_HOLDSINGLE;
		m_Priority = 100;
	}
	
	override void UpdatePossibleActions( PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask )
	{
		if ( ForceActionCheck( player ) )
		{
			m_SelectAction = m_ForcedActionData.m_Action;
			return;
		}
		
		m_SelectAction = null;
		array<ActionBase_Basic> possible_actions;
		ActionBase action;
		int i;
		m_MainItem = null;
		
		if ( player )
		{
			dzn_apsi apsi = dzn_apsi.Cast( player.FindAttachmentBySlotName( "Eyewear" ) );

			if ( apsi )
			{
				target_new = new ActionTarget( apsi, null, -1, vector.Zero, -1 );
				ForceActionTarget( target_new );
			}
			else
			{
				ClearForcedTarget();
			}
		}
		
		target = m_ForcedTarget;
		m_Target = m_ForcedTarget;
		
		if ( target && target.GetObject() )
		{
			target.GetObject().GetActions( Type(), possible_actions );
			if ( possible_actions )
			{
				for ( i = 0; i < possible_actions.Count(); i++ )
				{
					action = ActionBase.Cast( possible_actions.Get( i ) );
					if ( action.Can( player, target, m_MainItem, action_condition_mask ) )
					{
						m_SelectAction = action;
						return;
					}
				}
			}
		}
	}
	
	override ActionBase GetAction()
	{
		return m_SelectAction;
	}
};

class ToggleLEHSLightActionInput : DefaultActionInput
{
	ref ActionTarget target_new;
	
	void ToggleLEHSLightActionInput(PlayerBase player)
	{
		SetInput("UAToggleHeadlight");
		m_InputType = ActionInputType.AIT_SINGLE;
		m_Priority = 100;
	}
	
	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		if( ForceActionCheck(player) )
		{
			m_SelectAction = m_ForcedActionData.m_Action;
			return;
		}
		
		m_SelectAction = NULL;
		array<ActionBase_Basic> possible_actions;
		ActionBase action;
		int i;

		m_MainItem = NULL;
		if ( player && !player.IsInVehicle() ) 
		{
			dzn_lehs_helmet helmet = dzn_lehs_helmet.Cast( player.FindAttachmentBySlotName("Headgear") );
			
			if ( helmet )
			{
				target_new = new ActionTarget(helmet, null, -1, vector.Zero, -1);
				ForceActionTarget(target_new);
			}
			else
				ClearForcedTarget();
		}
		else if ( player && player.IsInVehicle() )
		{
			HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
			if( vehCommand )
			{
				Transport trans = vehCommand.GetTransport();
				if( trans )
				{
					target_new = new ActionTarget(trans, null, -1, vector.Zero, -1);
					ForceActionTarget(target_new);
				}
			}
			
			if ( !target_new )
				ClearForcedTarget();
		}
		
		target = m_ForcedTarget;
		m_Target = m_ForcedTarget;
		
		if(target && target.GetObject())
		{
			target.GetObject().GetActions(this.Type(), possible_actions);
			if(possible_actions)
			{
				for (i = 0; i < possible_actions.Count(); i++)
				{
					action = ActionBase.Cast(possible_actions.Get(i));
					if ( action.Can(player, target, m_MainItem, action_condition_mask) )
					{
						m_SelectAction = action;
						return;
					}
				}
			}
		}
	}
	
	override ActionBase GetAction()
	{
		return m_SelectAction;
	}
};

class ToggleLEHSVisorActionInput : DefaultActionInput
{
	ref ActionTarget target_new;
	
	void ToggleLEHSVisorActionInput(PlayerBase player)
	{
		SetInput("UAToggleLEHSVisor");
		m_InputType = ActionInputType.AIT_SINGLE;
		m_Priority = 100;
	}
	
	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		if( ForceActionCheck(player) )
		{
			m_SelectAction = m_ForcedActionData.m_Action;
			return;
		}
		
		m_SelectAction = NULL;
		array<ActionBase_Basic> possible_actions;
		ActionBase action;
		int i;

		m_MainItem = NULL;
		if ( player ) 
		{
			dzn_lehs_helmet helmet = dzn_lehs_helmet.Cast( player.FindAttachmentBySlotName("Headgear") );
			
			if ( helmet )
			{
				target_new = new ActionTarget(helmet, null, -1, vector.Zero, -1);
				ForceActionTarget(target_new);
			}
			else
				ClearForcedTarget();
		}
		
		target = m_ForcedTarget;
		m_Target = m_ForcedTarget;
		
		if(target && target.GetObject())
		{
			target.GetObject().GetActions(this.Type(), possible_actions);
			if(possible_actions)
			{
				for (i = 0; i < possible_actions.Count(); i++)
				{
					action = ActionBase.Cast(possible_actions.Get(i));
					if ( action.Can(player, target, m_MainItem, action_condition_mask) )
					{
						m_SelectAction = action;
						return;
					}
				}
			}
		}
	}
	
	override ActionBase GetAction()
	{
		return m_SelectAction;
	}
};

class ToggleLEHSPressureActionInput : DefaultActionInput
{
	ref ActionTarget target_new;
	
	void ToggleLEHSPressureActionInput(PlayerBase player)
	{
		SetInput("UAToggleLEHSPressure");
		m_InputType = ActionInputType.AIT_HOLDSINGLE;
		m_Priority = 100;
	}
	
	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		if( ForceActionCheck(player) )
		{
			m_SelectAction = m_ForcedActionData.m_Action;
			return;
		}
		
		m_SelectAction = NULL;
		array<ActionBase_Basic> possible_actions;
		ActionBase action;
		int i;

		m_MainItem = NULL;
		if ( player ) 
		{
			dzn_lehs_helmet helmet = dzn_lehs_helmet.Cast( player.FindAttachmentBySlotName("Headgear") );
			
			if ( helmet )
			{
				target_new = new ActionTarget(helmet, null, -1, vector.Zero, -1);
				ForceActionTarget(target_new);
			}
			else
				ClearForcedTarget();
		}
		
		target = m_ForcedTarget;
		m_Target = m_ForcedTarget;
		
		if(target && target.GetObject())
		{
			target.GetObject().GetActions(this.Type(), possible_actions);
			if(possible_actions)
			{
				for (i = 0; i < possible_actions.Count(); i++)
				{
					action = ActionBase.Cast(possible_actions.Get(i));
					if ( action.Can(player, target, m_MainItem, action_condition_mask) )
					{
						m_SelectAction = action;
						return;
					}
				}
			}
		}
	}
	
	override ActionBase GetAction()
	{
		return m_SelectAction;
	}
};

class ToggleLEHSNVGActionInput : DefaultActionInput
{
	ref ActionTarget target_new;
	
	void ToggleLEHSNVGActionInput(PlayerBase player)
	{
		SetInput("UAToggleNVG");
		m_InputType = ActionInputType.AIT_HOLDSINGLE;
		m_Priority = 100;
	}
	
	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		if( ForceActionCheck(player) )
		{
			m_SelectAction = m_ForcedActionData.m_Action;
			return;
		}
		//ForceActionTarget(player.m_PlayerLightManager.
		
		m_SelectAction = NULL;
		array<ActionBase_Basic> possible_actions;
		ActionBase action;
		int i;

		m_MainItem = NULL;
		if ( player ) 
		{

			dzn_lehs_helmet helmet = dzn_lehs_helmet.Cast( player.FindAttachmentBySlotName("Headgear") );
			
			if ( helmet )
			{
				target_new = new ActionTarget(helmet, null, -1, vector.Zero, -1);
				ForceActionTarget(target_new);
			}
			else
				ClearForcedTarget();
		}
		
		target = m_ForcedTarget;
		m_Target = m_ForcedTarget;
		
		if(target && target.GetObject())
		{
			target.GetObject().GetActions(this.Type(), possible_actions);
			if(possible_actions)
			{
				for (i = 0; i < possible_actions.Count(); i++)
				{
					action = ActionBase.Cast(possible_actions.Get(i));
					if ( action.Can(player, target, m_MainItem, action_condition_mask) )
					{
						m_SelectAction = action;
						return;
					}
				}
			}
		}
	}
	
	override ActionBase GetAction()
	{
		return m_SelectAction;
	}
};