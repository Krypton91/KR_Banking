class ClanDataBaseManager
{
    const protected static string m_ProfilesDIR = "$profile:";
    const protected static string m_ClansFolder = "KR_BANKING/ClanDataBase";

    protected string    m_ClanID;
    protected string    m_OwnersPlainID;
    protected string    m_ClanName;
    protected string    m_Prefix;
    protected int       m_BankAmount;

    protected ref array<ref ClanMemberObject> m_Members;

    protected ref TStringArray m_ClanLogs;

    void ClanDataBaseManager(string ClanID = "", string OwnersPlainID = "", string ClanName = "")
    {
        m_ClanID = ClanID;
        m_OwnersPlainID = OwnersPlainID;
        m_ClanName = ClanName;
        m_Members = new array<ref ClanMemberObject>;
        m_ClanLogs = new TStringArray();
        m_BankAmount = 0;
    }

    static ClanDataBaseManager LoadClanData(string ClanID, string username = "")
    {
        if(ClanID == "")
            return null;

        ClanDataBaseManager database = new ClanDataBaseManager();

        //Make sure to have all numbers
        if(!FileExist(m_ProfilesDIR + m_ClansFolder + "/"))
            MakeDirectory(m_ProfilesDIR + m_ClansFolder + "/");

        if(FileExist(m_ProfilesDIR + m_ClansFolder + "/" + ClanID + ".json"))
        {
            JsonFileLoader<ClanDataBaseManager>.JsonLoadFile(m_ProfilesDIR + m_ClansFolder + "/" + ClanID + ".json", database);
        }
        else
        {
            return null;
        }

        return database;
    }

    bool DeleteClan()
    {
        if(FileExist(m_ProfilesDIR + m_ClansFolder + "/" + GetClanID() + ".json"))
        {
            DeleteFile(m_ProfilesDIR + m_ClansFolder + "/" + GetClanID() + ".json");
            return true;
        }
        else
        {
            Error("Cant find Clan data error on Delete()");
            return false;
        }
    }

    void AddMember(string PlainID, string PlayerName, PermissionObject Permission)
    {
        m_Members.Insert(new ref ClanMemberObject(PlainID, PlayerName, Permission));
        SaveClanData(this);
    }

    void SetMembers(array<ref ClanMemberObject> members)
    {
        if(!m_Members)
            new array<ref ClanMemberObject>;
        
        m_Members = members;
    }

    void RemoveMember(int index)
    {
        m_Members.Remove(index);
        
        if(GetGame().IsServer())
            SaveClanData(this);
    }

    void SetPrefix(string Prefix)
    {
        m_Prefix = Prefix;
        
        if(GetGame().IsServer())
            SaveClanData(this);
    }

    void SetMoney(int Amount)
    {
        m_BankAmount = Amount;
    }

    string GetClanTag()
    {
        return m_Prefix;
    }

    //BE SURE CLANDATA GETS SAVED FROM LOGS!
    void DepositMoney(int Money)
    {
        m_BankAmount += Money;
    }

    //BE SURE CLANDATA GETS SAVED FROM LOGS!
    void WithdrawMoney(int Money)
    {
        m_BankAmount -= Money;
    }

    int GetBankCredit()
    {
        return m_BankAmount;
    }

    TStringArray GetClanLogs()
    {
        return m_ClanLogs;
    }

    int GetMemberCount()
    {
        return m_Members.Count();
    }
    void WriteLog(string LogLine)
    {
        if(m_ClanLogs.Count() > 20)
        {
            m_ClanLogs.Remove(19);
        }
        m_ClanLogs.Insert(LogLine);
        SaveClanData(this);
    }

    static ClanDataBaseManager CreateClan(string ClanName, string ClanID, string ownersPlainID)
    {
        ClanDataBaseManager database = new ClanDataBaseManager(ClanID, ownersPlainID, ClanName);
        SaveClanData(database);
        return database;
    }

    static void SaveClanData(ClanDataBaseManager database, string username = "")
    {
        //Make sure to have all numbers
        if(!FileExist(m_ProfilesDIR + m_ClansFolder + "/"))
            MakeDirectory(m_ProfilesDIR + m_ClansFolder + "/");

        if(!database)
		{
			return;
		}

        JsonFileLoader<ClanDataBaseManager>.JsonSaveFile(m_ProfilesDIR + m_ClansFolder + "/" + database.GetClanID() + ".json", database);
    }

    void SetName(string newName)
    {
        m_ClanName = newName;
    }
    string GetName()
    {
        return m_ClanName;
    }

    string GetClanID()
    {
        return m_ClanID;
    }

    string GetOwnersID()
    {
        return m_OwnersPlainID;
    }

    ClanMemberObject GetMemberByPlainId(string id)
    {
        for(int i = 0; i < m_Members.Count(); i++)
        {
            if(m_Members.Get(i).GetPlainID() == id)
                return m_Members.Get(i);
        }
        return null;
    }

    array<ref ClanMemberObject> GetClanMembers()
    {
        return m_Members;
    }
}