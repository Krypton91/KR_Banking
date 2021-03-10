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
	class KR_Banking 
	{
		dir = "KR_Banking";
		picture = "";
		action = "https://DeutscheBohrmaschine.de";
		hideName = 1;
		hidePicture = 1;
		name = "Advanced Banking";
		credits = "Krypton91";
		author = "Krypton91";
		authorID = "76561198796326626";
		version = "2.0";
		extra = 0;
		type = "mod";
		inputs = "KR_Banking\data\inputs.xml";
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
	class KR_ATMRed: KR_BankingATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATM\data\ATM_cored.paa"
		};
	};
	class KR_ATMBlue: KR_BankingATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATM\data\ATM_coblue.paa"
		};
	};
	class KR_ATMGreen: KR_BankingATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATM\data\ATM_cogreen.paa"
		};
	};
	class KR_ATMYellow: KR_BankingATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATM\data\ATM_coyellow.paa"
		};
	};
	class KR_ATMOrange: KR_BankingATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATM\data\ATM_coorange.paa"
		};
	};
	class KR_ATMBlackWhite: KR_BankingATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATM\data\ATM_cobw.paa"
		};
	};
	class KR_ATMBlack: KR_BankingATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATM\data\ATM_coblack.paa"
		};
	};
	class KR_BankingCard: KeyCard_Base
	{
		scope=2;
		displayName="Banking Card";
		descriptionShort="For Logging in into ATM";
		model="\KR_Banking\data\ATMCard\KR_BankingCard.p3d";
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"\KR_Banking\data\ATMCard\data\ATMCard_co.paa"
		};
	};
};
//MoneyDropSoundset
class CfgSoundShaders
{
	class MoneyDrop_SoundShaders
	{
		samples[]=
		{
			
			{
				"KR_Banking\data\Sounds\MoneyDrop.ogg",
				1
			}
		};
		range=5;
		volume=1.5;
	};
};

class CfgSoundSets
{
	class MoneyDrop_SoundSet
	{
		soundShaders[]=
		{
			"MoneyDrop_SoundShaders"
		};
	};
};