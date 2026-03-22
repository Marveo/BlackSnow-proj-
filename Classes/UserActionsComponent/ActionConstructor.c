modded class ActionConstructor
{
	override void RegisterActions( TTypenameArray actions )
	{
		super.RegisterActions( actions );

		actions.Insert( ActionReadBook );
		actions.Insert( ActionToggleAPSI );
		actions.Insert( ActionMakeSnowball );
		actions.Insert( ActionInjectEnzymeSelf );
		actions.Insert( ActionInjectEnzymeTarget );
		actions.Insert( ActionEnableFlaxCore );
		actions.Insert( ActionFlaxControlPanel );
		actions.Insert( ActionFixPhoenixPipes );
		actions.Insert( ActionActivatePrinter );
		actions.Insert( ActionToggleLEHSLight );
		actions.Insert( ActionToggleLEHSVisor );
		actions.Insert( ActionToggleLEHSPressure );
		actions.Insert( ActionToggleLEHSNVG );
		actions.Insert( ActionInjectOxygenShotSelf );
		actions.Insert( ActionInjectOxygenShotTarget );
		actions.Insert( ActionPlayVinyl );
	}
};