class AmmoCraft_Drill extends ItemBase {};

class AmmoCraft_Drill_notakeable: AmmoCraft_Drill 
{	
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		return false;
	}
	
	override bool CanPutInCargo( EntityAI parent )
		return false;

	override bool CanRemoveFromCargo( EntityAI parent )
		return false;
	
	override bool CanPutIntoHands( EntityAI parent )
		return false;
};

class Cartrige_Base extends ItemBase 
{
	override bool CanBeCombined( EntityAI other_item, bool reservation_check = true, bool stack_max_limit = false )
	{
		if ( this.GetHierarchyParent() && this.GetHierarchyParent().IsInherited(AmmoCraft_Drill) )
			return false;
		
		if ( this.GetHealthLevel() != other_item.GetHealthLevel() )
			return false;
		
		return super.CanBeCombined( other_item, true, false );
	}
	
	override bool HasQuantity()
		return true;
};

class Cartrige_Base_capsuled extends ItemBase 
{
	override bool CanBeCombined( EntityAI other_item, bool reservation_check = true, bool stack_max_limit = false )
	{
		if ( this.GetHierarchyParent() && this.GetHierarchyParent().IsInherited(AmmoCraft_Drill) )
			return false;
		
		if ( this.GetHealthLevel() != other_item.GetHealthLevel() )
			return false;
		
		return super.CanBeCombined( other_item, true, false );
	}
	
	override bool HasQuantity()
		return true;
};

class Cartrige_powered_Base extends ItemBase 
{
	override bool CanBeCombined( EntityAI other_item, bool reservation_check = true, bool stack_max_limit = false )
	{
		if ( this.GetHierarchyParent() && this.GetHierarchyParent().IsInherited(AmmoCraft_Drill) )
			return false;
		
		if ( this.GetHealthLevel() != other_item.GetHealthLevel() )
			return false;
		
		return super.CanBeCombined( other_item, true, false );
	}
	
	override bool HasQuantity()
		return true;
};

class Bullets_Base extends ItemBase 
{
	override bool CanBeCombined( EntityAI other_item, bool reservation_check = true, bool stack_max_limit = false )
	{
		if ( this.GetHierarchyParent() && this.GetHierarchyParent().IsInherited(AmmoCraft_Drill) )
			return false;
		
		if ( this.GetHealthLevel() != other_item.GetHealthLevel() )
			return false;
		
		return super.CanBeCombined( other_item, true, false );
	}
	
	override bool HasQuantity()
		return true;
};

class MoldaPliers: ItemBase {};

class Pb_plate extends Edible_Base
{
	override bool CanBeCombined( EntityAI other_item, bool reservation_check = true, bool stack_max_limit = false )
	{		
		ItemBase oi = ItemBase.Cast(other_item);
		if ( (this.GetTemperature() - oi.GetTemperature()) > 100 || (this.GetTemperature() - oi.GetTemperature()) < -100 || (oi.GetTemperature() - this.GetTemperature()) > 100 || (oi.GetTemperature() - this.GetTemperature()) < -100)
			return false;
		
		return super.CanBeCombined( other_item, true, false );
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if ( GetGame().IsClient() && parent )
		{
			if ( parent.IsMan() || parent.IsClothing() )
			{
				if (this.GetTemperature() > 100)
					return false;					
			}
		}		
		return super.CanPutInCargo(parent);
	}	
	
	override bool HasQuantity()
		return true;
	
	override bool CanBeCooked()
		return false;
	
	override bool CanBeCookedOnStick()
		return false;
};

class GunPowder extends ItemBase
{
};
class GunPowder_Box extends Box_Base{};