  
class CfgPatches 
{
	class KR_Banking 
	{
		units[] = 
		{
			"KR_Banking"
		};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = 
		{
			"DZ_Data",
			"JM_CF_Scripts"
		};
	};
};

class CfgMods 
{
	class KR_Banking {
		dir = "KR_Banking";
		picture = "";
		action = "https://DeutscheBohrmaschine.de";
		hideName = 1;
		hidePicture = 1;
		name = "BANKING";
		credits = "Krypton91";
		author = "Krypton91";
		authorID = "0";
		version = 0.1;
		extra = 0;
		type = "mod";
		dependencies[] = {"Mission", "World", "Game"};

	    class defs
	    {
			class gameScriptModule
            {
                value = "";
                files[] = {"KR_Banking/scripts/3_Game"};
            };
			class worldScriptModule
            {
                value = "";
                files[] = {"KR_Banking/scripts/4_World"};
            };
			class missionScriptModule
            {
                value = "";
                files[] = {"KR_Banking/scripts/5_Mission"};
            };
        };
	};
};

class CfgVehicles
{
	class HouseNoDestruct;
	class Inventory_Base;
	class KeyCard_Base: Inventory_Base{};
	class KR_BankingATM: HouseNoDestruct
	{
		scope=1;
		model="\KR_Banking\data\ATM\KR_BankingATM.p3d";
		hiddenSelections[]=
		{
			"zybtek"
		};
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATM\data\ATM_co.paa"
		};
	};

	class KR_ATM: KR_BankingATM
	{
		scope=1;
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATM\data\ATM_co.paa"
		};
	};
	class KR_ATMRusty: KR_BankingATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATM\data\ATM_co1.paa"
		};
	};
	class KR_BankingCard: KeyCard_Base
	{
		scope=2;
		displayName="Banking Card";
		descriptionShort="For Logging in into ATM";
		model="\KR_Banking\data\ATMCard\KR_BankingCard.p3d";
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATMCard\data\ATMCard_co.paa"
		};
	};
};