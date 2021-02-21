class ClanDataBaseManager
{
    const protected static string m_ProfilesDIR = "$profile:";
    const protected static string m_ClansFolder = "KR_BANKING/ClanDataBase";

    protected string    m_ClanID;
    protected string    m_OwnersPlainID;
    protected string    m_ClanName;
    protected string    m_Prefix;

    protected ref array<ref ClanMemberObject> m_Members;

    void ClanDataBaseManager(string ClanID = "", string OwnersPlainID = "", string ClanName = "")
    {
        m_ClanID = ClanID;
        m_OwnersPlainID = OwnersPlainID;
        m_ClanName = ClanName;
        m_Members = new ref array<ref ClanMemberObject>;
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

    void AddMember(string PlainID, string PlayerName, PermissionObject Permission)
    {
        m_Members.Insert(new ref ClanMemberObject(PlainID, PlayerName, Permission));
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

    string GetClanID()
    {
        return m_ClanID;
    }
}