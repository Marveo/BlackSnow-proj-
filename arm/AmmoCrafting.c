class AmmoCraftingAgent extends AgentBase
{
	override void Init()
	{
		m_Type 					= SkillAgent.AMMO_CRAFTING;
		m_Invasibility 			= 0;
		m_TransferabilityIn		= 0;
		m_TransferabilityOut	= 0;
		m_AntibioticsResistance = 1;
		m_MaxCount 				= 2000;
		m_DieOffSpeed           = 0;
	}
};
