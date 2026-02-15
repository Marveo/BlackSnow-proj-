class CfgPatches
{
	class Zadachi_ServerMod
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"Zadachi",
			"Zadachi_gr"
		};
	};
};

class CfgMods
{
	class Zadachi_ServerMod
	{
		dir="Zadachi_ServerMod";
		type="mod";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"Zadachi_ServerMod/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"Zadachi_ServerMod/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"Zadachi_ServerMod/5_Mission"
				};
			};
		};
	};
};
