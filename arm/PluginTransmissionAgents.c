modded class PluginTransmissionAgents extends PluginBase
{	
	void PluginTransmissionAgents()
	{
		m_AgentList.Insert(SkillAgent.AMMO_CRAFTING, new AmmoCraftingAgent);
	}
};

