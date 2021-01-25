class KR_JsonDatabaseHandler
{
    const protected static string m_ProfilesDIR = "$profile:";
    const protected static string m_PlayersFolder = "KR_BANKING/PlayerDataBase";
    const protected static string m_ClansFolder = "KR_BANKING/ClanAccounts";

    protected string m_Steam64ID;
    protected string m_PlayerName;
    protected int  m_OwnedCurrency;
    protected int  m_BonusCurrency;
    protected string m_ClanID;

    void KR_JsonDatabaseHandler(string plainID = "", string username = "", int ownedcurrency = GetKR_BankingServerConfig().startCurrency, int bonuscurrency = 0, string clanid = "")
    {
        m_Steam64ID = plainID;
        m_PlayerName = username;
        m_OwnedCurrency = ownedcurrency;
        m_BonusCurrency = bonuscurrency;
        m_ClanID = clanid;
    }

    int GetBankCredit()
    {
        return m_OwnedCurrency;
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