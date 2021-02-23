const protected static string m_ProfileDIR = "$profile:";
const protected static string m_ConfigDIR = "KR_BANKING";
const protected static string m_ConfigPath = m_ProfileDIR + m_ConfigDIR + "/" + "server-config.json";
class KR_BankingConfigManager
{

    float   MenuDelay;
    int     startCurrency;
    int     maxCurrency;
    bool    IsClanAccountActive;
    int     CostsToCreateClan;
    int     CostsToInviteAnPlayer;
    int     MaxClanAccountLimit;
    bool    MakeLogs;
    bool    IsRobEventActive;
    bool    RobMessagesActive;
    int     PayCheckValue;
    float   PayCheckTickTime;
    int     MinPlayersForPayCheck;
    bool    CanAddToFullAcc;
    bool    NeedsBankCardToOpenMenu;
    ref array<ref ATMPosition> ATM;
    ref array<ref RobAtmSpawns> ATMRobSpots;
    ref array<ref CurrencySettings> BankingCurrency;
    void KR_BankingConfigManager()
    {
        ATM = new ref array<ref ATMPosition>();
        ATMRobSpots = new ref array<ref RobAtmSpawns>();
        BankingCurrency = new ref array<ref CurrencySettings>();
    }

    void LoadDefaultSettings()
    {
        MenuDelay = 1.0;
        startCurrency = 0;
        maxCurrency = 10000;
        IsClanAccountActive = true;
        CostsToCreateClan = 5000;
        CostsToInviteAnPlayer = 1000;
        MaxClanAccountLimit = 200000;
        MakeLogs = true;
        IsRobEventActive = true;
        RobMessagesActive = true;
        PayCheckValue = 500;
        PayCheckTickTime = 1;
        MinPlayersForPayCheck = 1;
        CanAddToFullAcc = true;
        NeedsBankCardToOpenMenu = false;

        ATM.Insert(new ref ATMPosition("KR_ATM", "3689.35 402.012 5988.02", "-110 0 0"));
        ATM.Insert(new ref ATMPosition("KR_ATM", "11475.6 342.984 11320", "210 0 0"));
        ATMRobSpots.Insert(new ref RobAtmSpawns("7541.557 213.997 5137.169", "100 0 0", "An Atm gets robbed in Mogilevka!"));

        //Importent to start with the highest Value of money because math.Floor.
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble100", 100));
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble50", 50));
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble25", 25));
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble10", 10));
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble5", 5));
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble1", 1));
        Save();
    }

    void Save()
    {
       if (!FileExist(m_ProfileDIR + m_ConfigDIR + "/"))
			MakeDirectory(m_ProfileDIR + m_ConfigDIR + "/");

         JsonFileLoader<KR_BankingConfigManager>.JsonSaveFile(m_ConfigPath, this);
    }

    static ref KR_BankingConfigManager Load()
    {
        ref KR_BankingConfigManager settings = new KR_BankingConfigManager();
        if(!FileExist(m_ConfigDIR))
            MakeDirectory(m_ConfigDIR);
        if(FileExist(m_ConfigPath))
        {
            Print("[Advanced Banking] -> Found Config Loading existing config...");
            JsonFileLoader<KR_BankingConfigManager>.JsonLoadFile(m_ConfigPath, settings);
        }
        else
        {
            Print("[Advanced Banking] -> No Config Found Try to write default config...");
            settings.LoadDefaultSettings();
        }
        return settings;
    }

    float GetCorrectPayCheckTime()
    {
        return PayCheckTickTime * 60000;
    }
};


static ref KR_BankingConfigManager g_KR_BankingConfigManager;
static ref KR_BankingConfigManager GetKR_BankingServerConfig() {
    if (g_Game.IsServer() && !g_KR_BankingConfigManager) {
        g_KR_BankingConfigManager = KR_BankingConfigManager.Load();
        g_KR_BankingConfigManager.Save();
    }
    return g_KR_BankingConfigManager;
};