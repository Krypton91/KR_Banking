const protected static string m_ProfileDIR = "$profile:";
const protected static string m_ConfigDIR = "KR_BANKING";
const protected static string m_ConfigPath = m_ProfileDIR + m_ConfigDIR + "/" + "server-config.json";
class KR_BankingConfigManager
{

    float MenuDelay;
    float startCurrency;
    float maxCurrency;
    bool IsClanAccountActiv;
    bool MakeLogs;
    bool IsRobEventActiv;
    bool RobMessagesActiv;
    bool NeedsBankCardToOpenMenu;
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
        IsClanAccountActiv = true;
        MakeLogs = true;
        IsRobEventActiv = true;
        RobMessagesActiv = true;
        NeedsBankCardToOpenMenu = false;

        ATM.Insert(new ref ATMPosition("KR_ATM", "3689.35 402.012 5988.02", "-110 0 0"));
        ATM.Insert(new ref ATMPosition("KR_ATM", "11475.6 342.984 11320", "210 0 0"));
        ATMRobSpots.Insert(new ref RobAtmSpawns("7541.557 213.997 5137.169", "100 0 0", "An Atm gets robbed in Mogilevka!"));
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble1", 1.0));
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble5", 5.0));
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble10", 10.0));
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble25", 25.0));
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble50", 50.0));
        BankingCurrency.Insert(new ref CurrencySettings("MoneyRuble100", 100.0));
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
            Print("[KR Banking] -> Found Config Loading existing config...");
            JsonFileLoader<KR_BankingConfigManager>.JsonLoadFile(m_ConfigPath, settings);
        }
        else
        {
            Print("[KR Banking] -> No Config Found Try to write default config...");
            settings.LoadDefaultSettings();
        }
        return settings;
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

class ATMPosition
{
    string ATMSName;
    protected vector Position;
    protected vector Direction;

    void ATMPosition(string atmname, vector atmPos, vector atmDirection)
    {
        ATMSName = atmname;
        Position = atmPos;
        Direction = atmDirection;
    }

    vector GetATMPosition()
    {
        return Position;
    }

    vector GetATMDirectory()
    {
        return Direction;
    }
};

class RobAtmSpawns
{
    protected vector Position;
    protected vector Direction;
    string RobMessage;
    
    void RobAtmSpawns(vector atmPos, vector atmDirection, string robmsg)
    {
        Position = atmPos;
        Direction = atmDirection;
        RobMessage = robmsg;
    }

    vector GetATMPosition()
    {
        return Position;
    }

    vector GetATMDirectory()
    {
        return Direction;
    }
};

class CurrencySettings
{
    string CurrencyName;
    float CurrencyValue;

    void CurrencySettings(string curname, float curvalue)
    {
        CurrencyName = curname;
        CurrencyValue = curvalue;
    }
};