class KR_BankingConfigManager
{

    string  ModVersion;
    float   MenuDelay;
    int     startCurrency;
    int     maxCurrency;
    bool    IsClanAccountActive;
    int     CostsToCreateClan;
    int     CostsToInviteAnPlayer;
    int     MaxClanAccountLimit;
    int     MaxPlayersInClan;
    int     MinAmountToTransfer;
    int     TransferfeesInProcent;
    bool    IsRobEventActive;
    int     MinPlayersForRob;
    int     MinMoneyForRob;
    int     MaxMoneyForRob;
    int     TimeInSecToRobATM;
    bool    RobMessagesActive;
    int     PayCheckValue;
    int     PayCheckTickTime;
    bool    IsPayCheckMessageActive;
    bool    CanAddPayCheckInSafezone;
    string  PayCheckMessage;
    int     MinPlayersForPayCheck;
    bool    CanAddToFullAcc;
    bool    NeedsBankCardToOpenMenu;
    ref LogSettings m_LoggingSettings;
    ref Webhookservice m_DiscordWebhook;
    ref array<ref ATMPosition> ATM;
    ref array<ref CurrencySettings> BankingCurrency;
    
    void KR_BankingConfigManager()
    {
        ATM = new array<ref ATMPosition>();
        BankingCurrency = new array<ref CurrencySettings>();
        m_LoggingSettings = new LogSettings();
        m_DiscordWebhook = new Webhookservice();
    }

    void LoadDefaultSettings()
    {

        ModVersion = GetModVersion();
        MenuDelay = 1.0;
        startCurrency = 0;
        maxCurrency = 10000;
        IsClanAccountActive = true;
        CostsToCreateClan = 5000;
        CostsToInviteAnPlayer = 1000;
        MaxClanAccountLimit = 200000;
        MaxPlayersInClan = -1;
        MinAmountToTransfer = 1000;
        TransferfeesInProcent = 3;
        IsRobEventActive = true;
        MinPlayersForRob = 5;
        MinMoneyForRob = 1000;
        MaxMoneyForRob = 5000;
        TimeInSecToRobATM = 120;
        RobMessagesActive = true;
        PayCheckValue = 500;
        PayCheckTickTime = 20;
        IsPayCheckMessageActive = true;
        CanAddPayCheckInSafezone = true;
        PayCheckMessage = "%Amount% added to your bank account! stay active to earn more money!";
        MinPlayersForPayCheck = 1;
        CanAddToFullAcc = false;
        NeedsBankCardToOpenMenu = false;

        ATM.Insert(new ref ATMPosition("KR_ATMBlack", "3689.35 402.012 5988.02", "-110 0 0"));
        ATM.Insert(new ref ATMPosition("KR_ATMBlack", "11475.6 342.984 11320", "210 0 0"));
        ATM.Insert(new ref ATMPosition("KR_ATMBlack", "3433.22 192.705 13063.299", "-114.01 0 0", true, "ATM Gets Robbed near Novaya Petrovka from Player: %PlayerName%"));
        ATM.Insert(new ref ATMPosition("KR_ATMBlack", "6045.77 300.76 7841.39", "52.99 0 0", true, "ATM Gets Robbed near Stary Sobor from Player: %PlayerName%"));
        ATM.Insert(new ref ATMPosition("KR_ATMBlack", "6597.81 8.300 2423.65", "133.0 0 0", true, "ATM Gets Robbed near Chernogorsk from Player: %PlayerName%"));

        //Importent to start with the highest Value of money!!! Todo: Make a sort function.
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
       if (!FileExist(m_BankingProfileDIR + m_BankingConfigDIR + "/"))
			MakeDirectory(m_BankingProfileDIR + m_BankingConfigDIR + "/");

        BankingJsonFileLoader<KR_BankingConfigManager>.JsonSaveFile(m_BankingConfigPath, this);
    }

    void RemoveATMWithIndex(int index)
    {
        if(ATM && ATM.Get(index))
        {
            ATM.Remove(index);
            Save();
        }
    }

    void AddATM(string classname, bool canRob, vector Position, vector Orientation)
    {
        ATM.Insert(new ref ATMPosition(classname, Position, Orientation, canRob, "ATM Gets Robbed near Novaya Petrovka from Player: %PlayerName%"));
        Save();
    }

    void SetConfig(KR_BankingConfigManager newConfig)
    {
        this = newConfig;
        g_KR_BankingConfigManager = newConfig;
        Save();
    }

    static ref KR_BankingConfigManager Load()
    {
        KR_BankingConfigManager settings = new KR_BankingConfigManager();
        if(!FileExist(m_BankingConfigDIR))
            MakeDirectory(m_BankingConfigDIR);
        if(FileExist(m_BankingConfigPath))
        {
            Print("[Advanced Banking] -> Found Config Loading existing config...");
            bool hasError = BankingJsonFileLoader<KR_BankingConfigManager>.JsonLoadFile(m_BankingConfigPath, settings);
            if(settings && settings.IsConfigOutdated())
            {
                settings.CreateNewConfig();
            }
            else
            {
                if(hasError)
                {
                    Print("Banking -> JSON ERROR ON YOUR CONFIG CAN NOT START!");
                }
            }
        }
        else
        {
            Print("[Advanced Banking] -> No Config Found Try to write default config...");
            settings.LoadDefaultSettings();
        }
        return settings;
    }

    void CreateNewConfig()
    {
        if (!FileExist(m_BankingProfileDIR + m_BankingConfigDIR + "/" + "OLDCONFIG/"))
			MakeDirectory(m_BankingProfileDIR + m_BankingConfigDIR + "/" + "OLDCONFIG/");

        string BKFileName = "OldConfig.AdvancedBanking";
        int TryCount = 1;
        while(FileExist(m_BankingProfileDIR + m_BankingConfigDIR + "/" + "OLDCONFIG/"+ BKFileName))
        {
            BKFileName = TryCount.ToString() + BKFileName;
            TryCount++;
        }

        CopyFile(m_BankingConfigPath, m_BankingProfileDIR + m_BankingConfigDIR + "/" + "OLDCONFIG/" + BKFileName);
        DeleteFile(m_BankingConfigPath);
        ATM.Clear();
        BankingCurrency.Clear();
        LoadDefaultSettings();
    }

    int GetCorrectPayCheckTime()
    {
        return PayCheckTickTime * 60000;
    }

    bool IsConfigOutdated() 
    {
		if(this.ModVersion != GetModVersion())
			return true;
		return false;
	}

    protected string GetModVersion()
    {
        string cfgversion = "CfgMods KR_Banking version";
        string mdversion;
        GetGame().ConfigGetText(cfgversion, mdversion);
        return mdversion;
    }
};


static ref KR_BankingConfigManager g_KR_BankingConfigManager;
static KR_BankingConfigManager GetKR_BankingServerConfig() 
{
    if (g_Game.IsServer() && !g_KR_BankingConfigManager) 
    {
        g_KR_BankingConfigManager = KR_BankingConfigManager.Load();
        g_KR_BankingConfigManager.Save();
    }
    return g_KR_BankingConfigManager;
};