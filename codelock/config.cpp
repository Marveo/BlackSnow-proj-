////////////////////////////////////////////////////////////////////
//DeRap: Codelock\config.bin
//Produced from mikero's Dos Tools Dll version 9.98
//https://mikero.bytex.digital/Downloads
//'now' is Thu Mar 19 20:52:46 2026 : 'file' last modified on Fri Nov 01 16:59:47 2024
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class GDZ_Codelock
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Gear_Camping"};
	};
};
class CfgVehicles
{
	class Container_Base;
	class CombinationLock;
	class CodeLock: CombinationLock
	{
		displayName = "CodeLock";
		descriptionShort = "An electronic lock to secure fence gates and tents with a 3-6 character passcode. Runs off of magical energy and doesn't require an external power source.";
		model = "Codelock\codelock\GDZ_codelock.p3d";
		repairableWithKits[] = {7};
		repairCosts[] = {50};
	};
	class MediumTent: Container_Base
	{
		attachments[] = {"CamoNet","Lights","Att_CombinationLock"};
		class GUIInventoryAttachmentsProps
		{
			class CodeLock
			{
				name = "CodeLock";
				description = "";
				attachmentSlots[] = {"Att_CombinationLock"};
				icon = "cat_bb_attachments";
				selection = "wall";
			};
		};
		class AnimationSources
		{
			class CodeLock
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 1;
			};
		};
	};
	class LargeTent: Container_Base
	{
		attachments[] = {"CamoNet","Lights","Att_CombinationLock"};
		class GUIInventoryAttachmentsProps
		{
			class CodeLock
			{
				name = "CodeLock";
				description = "";
				attachmentSlots[] = {"Att_CombinationLock"};
				icon = "cat_bb_attachments";
				selection = "wall";
			};
		};
		class AnimationSources
		{
			class CodeLock
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 1;
			};
		};
	};
	class CarTent: Container_Base
	{
		attachments[] = {"CamoNet","Lights","Att_CombinationLock"};
		class GUIInventoryAttachmentsProps
		{
			class CodeLock
			{
				name = "CodeLock";
				description = "";
				attachmentSlots[] = {"Att_CombinationLock"};
				icon = "cat_bb_attachments";
				selection = "wall";
			};
		};
		class AnimationSources
		{
			class CodeLock
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 1;
			};
		};
	};
};
