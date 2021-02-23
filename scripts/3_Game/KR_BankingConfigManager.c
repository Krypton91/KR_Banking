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
    bool    IsRobEventActive;
    int     MinPlayersForRob;
    int     MinMoneyForRob;
    int     MaxMoneyForRob;
    int     TimeInSecToRobATM;
    bool    RobMessagesActive;
    int     PayCheckValue;
    int     PayCheckTickTime;
    bool    PayCheckMessage;
    int     MinPlayersForPayCheck;
    bool    CanAddToFullAcc;
    bool    NeedsBankCardToOpenMenu;
    ref LogSettings m_LoggingSettings;
    ref Webhookservice m_DiscordWebhook;
    ref array<ref ATMPosition> ATM;
    ref array<ref CurrencySettings> BankingCurrency;
    void KR_BankingConfigManager()
    {
        ATM = new ref array<ref ATMPosition>();
        BankingCurrency = new ref array<ref CurrencySettings>();
        m_LoggingSettings = new ref LogSettings();
        m_DiscordWebhook = new ref Webhookservice();
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
        IsRobEventActive = true;
        MinPlayersForRob = 5;
        MinMoneyForRob = 1000;
        MaxMoneyForRob = 5000;
        TimeInSecToRobATM = 120;
        RobMessagesActive = true;
        PayCheckValue = 500;
        PayCheckTickTime = 20;
        PayCheckMessage = true;
        MinPlayersForPayCheck = 1;
        CanAddToFullAcc = true;
        NeedsBankCardToOpenMenu = false;

        ATM.Insert(new ref ATMPosition("KR_ATM", "3689.35 402.012 5988.02", "-110 0 0"));
        ATM.Insert(new ref ATMPosition("KR_ATM", "11475.6 342.984 11320", "210 0 0"));
        ATM.Insert(new ref ATMPosition("KR_ATMBlack", "3433.22 192.705 13063.299", "-114.01 0 0", true, "ATM Gets Robbed near Snosnovy Militäry from Player: %PlayerName%  with: %id%"));
        ATM.Insert(new ref ATMPosition("KR_ATMBlack", "6045.77 300.76 7841.39", "52.99 0 0", true, "ATM Gets Robbed near Snosnovy Militäry from Player: %PlayerName%  with: %id%"));
        ATM.Insert(new ref ATMPosition("KR_ATMBlack", "6597.81 8.300 2423.65", "133.0 0 0", true, "ATM Gets Robbed near Snosnovy Militäry from Player: %PlayerName%  with: %id%"));

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

    static ref KR_BankingConfigManager Load()
    {
        ref KR_BankingConfigManager settings = new KR_BankingConfigManager();
        if(!FileExist(m_BankingConfigDIR))
            MakeDirectory(m_BankingConfigDIR);
        if(FileExist(m_BankingConfigPath))
        {
            Print("[Advanced Banking] -> Found Config Loading existing config...");
            bool hasError = BankingJsonFileLoader<KR_BankingConfigManager>.JsonLoadFile(m_BankingConfigPath, settings);
            if(settings && settings.IsConfigOutdated())
                Error("Your Config is Outdated! Please Read AdvancedBanking Changelog!");

            if(hasError)
            {
                bool WebSiteOpen = false;
                while(true)
                {
                    if(!WebSiteOpen)
                        GetGame().OpenURL("https://deutschebohrmaschine.de/AdvancedBanking/error.html");
                    WebSiteOpen = true;
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

    float GetCorrectPayCheckTime()
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
static ref KR_BankingConfigManager GetKR_BankingServerConfig() {
    if (g_Game.IsServer() && !g_KR_BankingConfigManager) {
        g_KR_BankingConfigManager = KR_BankingConfigManager.Load();
        g_KR_BankingConfigManager.Save();
    }
    return g_KR_BankingConfigManager;
};