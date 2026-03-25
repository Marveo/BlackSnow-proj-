modded class ItemBase
{
	private float m_terjeRadiationServer = 0;
	private int m_terjeRadiationSynch = 0;
	
	private bool m_terjeStaticRadioactive;
	private float m_terjeStaticRadiation;
	
	void ItemBase()
	{
		if (GetTerjeGameConfig().ConfigIsExisting("CfgVehicles " + GetType() + " terjeStaticRadiationValue"))
		{
			m_terjeStaticRadiation = GetTerjeGameConfig().ConfigGetFloat("CfgVehicles " + GetType() + " terjeStaticRadiationValue");
			m_terjeStaticRadioactive = true;
		}
		else
		{
			RegisterNetSyncVariableInt("m_terjeRadiationSynch", 0, TerjeRadiationConstants.RADIATION_ITEMS_ACCUMULATOR_SYNCH_MAX);
			m_terjeStaticRadioactive = false;
		}
	}
	
	override bool AddTerjeRadiation(float rAmount)
	{
		if (GetGame().IsDedicatedServer() && IsTerjeRadiationAccumulated())
		{
			if (m_terjeStaticRadioactive)
			{
				return false;
			}
			
			if (rAmount > 0)
			{
				float itemRadProtection = GetTerjeProtectionLevel("radiation");
				if (itemRadProtection > 0)
				{
					if (GetTerjeSettingBool(TerjeSettingsCollection.RADIATION_PROTECTIVE_CLOTHING_ACCUMULATE))
					{
						rAmount *= Math.Clamp(1.0 - itemRadProtection, 0, 1);
					}
					else
					{
						return false;
					}
				}
			}
			
			m_terjeRadiationServer = Math.Clamp(m_terjeRadiationServer + rAmount, 0, TerjeRadiationConstants.RADIATION_ITEMS_ACCUMULATOR_SERVER_MAX);
			
			int newRadiationSynchValue = (int)Math.Round(m_terjeRadiationServer / TerjeRadiationConstants.RADIATION_ITEMS_ACCUMULATOR_SYNCH_DIVIDER);
			if (newRadiationSynchValue != m_terjeRadiationSynch)
			{
				m_terjeRadiationSynch = newRadiationSynchValue;
				SetSynchDirty();
			}
			
			return true;
		}

		return false;
	}
	
	override float GetTerjeRadiation()
	{
		if (m_terjeStaticRadioactive)
		{
			return Math.Max(0, m_terjeStaticRadiation);
		}
		
		if (GetGame().IsDedicatedServer())
		{
			return m_terjeRadiationServer;
		}
		else
		{
			return m_terjeRadiationSynch * TerjeRadiationConstants.RADIATION_ITEMS_ACCUMULATOR_SYNCH_DIVIDER;
		}
	}
	
	override bool IsTerjeRadiationAccumulated()
	{
		return GetTerjeSettingBool(TerjeSettingsCollection.RADIATION_DO_ITEMS_ACCUMULATE);
	}
	
	override void ProcessVariables()
	{
		super.ProcessVariables();
		TerjeProcessRadiationVariables();
	}
	
	protected void TerjeProcessRadiationVariables()
	{
		if (GetGame().IsDedicatedServer())
		{
			PluginTerjeScriptableAreas plugin = GetTerjeScriptableAreas();
			if (plugin)
			{
				EntityAI parent;
				if (m_terjeStaticRadioactive && (m_terjeStaticRadiation < 0))
				{
					parent = this.GetHierarchyParent();
					if (parent != null)
					{
						float consumeAmount = m_terjeStaticRadiation * m_ElapsedSinceLastUpdate * GetTerjeSettingFloat(TerjeSettingsCollection.RADIATION_TRANSFER_THRESHOLD);
						if (HasQuantity())
						{
							consumeAmount *= GetQuantityNormalized();
						}
						
						if (consumeAmount < 0)
						{
							plugin.AddTerjeRadiationToEntity(parent, consumeAmount);
							
							PlayerBase rootPlayer = PlayerBase.Cast(this.GetHierarchyRootPlayer());
							if (rootPlayer != null && rootPlayer != parent)
							{
								rootPlayer.AddTerjeRadiation(consumeAmount);
							}
						}
					}
				}
				else if (IsTerjeRadiationAccumulated())
				{
					float currentRadiation = GetTerjeRadiation();
					float radioactiveGlobalModifier = GetTerjeSettingFloat(TerjeSettingsCollection.RADIATION_AREAS_POWER_MOD);
					float rAmount = plugin.CalculateTerjeEffectValue(this, "rad") * radioactiveGlobalModifier;
					rAmount -= GetTerjeSettingFloat(TerjeSettingsCollection.RADIATION_ITEM_LOSE_PER_SEC);
					
					if (m_ElapsedSinceLastUpdate > 0)
					{
						if (rAmount > 0)
						{
							float maxAccumulatedRadLimit = rAmount * GetTerjeSettingFloat(TerjeSettingsCollection.RADIATION_ZONE_POWER_TO_RAD_LIMIT);
							if (currentRadiation < maxAccumulatedRadLimit)
							{
								AddTerjeRadiation(Math.Clamp(rAmount * m_ElapsedSinceLastUpdate, 0, maxAccumulatedRadLimit - currentRadiation));
							}
						}
						else
						{
							AddTerjeRadiation(rAmount * m_ElapsedSinceLastUpdate);
						}
					}
					
					if (GetTerjeSettingBool(TerjeSettingsCollection.RADIATION_TRANSFER_WITH_PARENT))
					{
						parent = this.GetHierarchyParent();
						if (parent != null && !parent.IsInherited(PlayerBase))
						{
							float maxTransferAmount;
							float finalTransferAmount;
							float transferThreshold = GetTerjeSettingFloat(TerjeSettingsCollection.RADIATION_TRANSFER_THRESHOLD);
							float transferAmount = GetTerjeSettingFloat(TerjeSettingsCollection.RADIATION_TRANSFER_PER_SECOND) * m_ElapsedSinceLastUpdate;
							float isolation = Math.Clamp(1.0 - GetTerjeRadiationInventoryIsolation(), 0, 1);
							float parentRadiation = plugin.GetTerjeRadiationFromEntity(parent);
							
							if (HasQuantity())
							{
								transferAmount *= GetQuantityNormalized();
							}
							
							if (currentRadiation * transferThreshold > parentRadiation)
							{
								maxTransferAmount = Math.Max(0, (currentRadiation * transferThreshold) - parentRadiation);
								finalTransferAmount = Math.Clamp(transferAmount * isolation, 0, maxTransferAmount);
								plugin.AddTerjeRadiationToEntity(parent, finalTransferAmount);
							}
							else if (parentRadiation * transferThreshold > currentRadiation)
							{
								maxTransferAmount = Math.Max(0, (parentRadiation * transferThreshold) - currentRadiation);
								finalTransferAmount = Math.Clamp(transferAmount * isolation, 0, maxTransferAmount);
								AddTerjeRadiation(finalTransferAmount);
							}
						}
					}
				}
			}
		}
	}
	
	override void OnTerjeStoreSave(TerjeStorageWritingContext ctx)
	{
		super.OnTerjeStoreSave(ctx);
		ctx.WriteFloat("rad", m_terjeRadiationServer);
	}
	
	override void OnTerjeStoreLoad(TerjeStorageReadingContext ctx)
	{
		super.OnTerjeStoreLoad(ctx);
		ctx.ReadFloat("rad", m_terjeRadiationServer);
	}
	
	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();
		
		int newRadiationSynchValue = (int)Math.Round(m_terjeRadiationServer / TerjeRadiationConstants.RADIATION_ITEMS_ACCUMULATOR_SYNCH_DIVIDER);
		if (newRadiationSynchValue != m_terjeRadiationSynch)
		{
			m_terjeRadiationSynch = newRadiationSynchValue;
			SetSynchDirty();
		}
	}

	override void AddAction(typename actionName)
	{
		// Bind radiation cleaning action
		if (actionName == ActionDrainLiquid)
		{
			super.AddAction(ActionWashRadioactiveItems);
		}
		
		// Default
		super.AddAction(actionName);
	}
}