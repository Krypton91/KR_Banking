class KR_JsonDatabaseHandler
{
    const protected static string m_ProfilesDIR = "$profile:";
    const protected static string m_PlayersFolder = "KR_BANKING/PlayerDataBase";
    const protected static string m_ClansFolder = "KR_BANKING/ClanAccounts";

    protected string    m_Steam64ID;
    protected string    m_PlayerName;
    protected int       m_OwnedCurrency;
    protected int       m_PayCheckBonus;
    protected int       m_BonusCurrency;
    protected string    m_ClanID;

    void KR_JsonDatabaseHandler(string plainID = "", string username = "", int bonuscurrency = 0, string clanid = "")
    {
        m_Steam64ID = plainID;
        m_PlayerName = username;
        m_OwnedCurrency = GetKR_BankingServerConfig().startCurrency;
        m_BonusCurrency = bonuscurrency;
        m_ClanID = clanid;
        m_PayCheckBonus = 0;
    }

    int GetBankCredit()
    {
        return m_OwnedCurrency;
    }

    int GetPayCheckBonus()
    {
        return m_PayCheckBonus;
    }
    
    void UpdateCurrency(int newCurrency)
    {
        m_OwnedCurrency = newCurrency;
        SavePlayerData(this);
    }

    void WitdrawMoney(int moneyToWitdraw)
    {
        m_OwnedCurrency = m_OwnedCurrency - moneyToWitdraw;
        SavePlayerData(this);
    }

    void DepositMoney(int moneyToDeposit)
    {
        m_OwnedCurrency =  m_OwnedCurrency + moneyToDeposit;
        SavePlayerData(this);
    }

    static KR_JsonDatabaseHandler LoadPlayerData(string Steam_ID, string username = "")
    {
        if(Steam_ID == "")
            return null;

        KR_JsonDatabaseHandler database = new KR_JsonDatabaseHandler();

        //Make sure to have all numbers
        if(!FileExist(m_ProfilesDIR + m_PlayersFolder + "/"))
            MakeDirectory(m_ProfilesDIR + m_PlayersFolder + "/");

        if(FileExist(m_ProfilesDIR + m_PlayersFolder + "/" + Steam_ID + ".json"))
        {
            JsonFileLoader<KR_JsonDatabaseHandler>.JsonLoadFile(m_ProfilesDIR + m_PlayersFolder + "/" + Steam_ID + ".json", database);

            if(username != "")
                database.SetPlayerName(username);
        }
        else
        {
            database = CreateDefaultDataBaseData(Steam_ID, username);
        }

        return database;
    }


    protected static KR_JsonDatabaseHandler CreateDefaultDataBaseData(string id, string username = "")
    {
        KR_JsonDatabaseHandler database = new KR_JsonDatabaseHandler(id, username);
        SavePlayerData(database);
        return database;
    }

    static void SavePlayerData(KR_JsonDatabaseHandler database, string username = "")
    {
        if(!database)
		{
			return;
		}
		
		if(database.GetSteamID() == "")
		{
			return;
		}
		
		if(username != "")
		{
			database.SetPlayerName(username);
		}

        JsonFileLoader<KR_JsonDatabaseHandler>.JsonSaveFile(m_ProfilesDIR + m_PlayersFolder + "/" + database.GetSteamID() + ".json", database);
    }

    void SetPlayerName(string username)
    {
        m_PlayerName = username;
    }

    string GetSteamID()
    {
        return m_Steam64ID;
    }
}