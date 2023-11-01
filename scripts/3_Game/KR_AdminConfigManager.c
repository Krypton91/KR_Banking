class KR_AdminConfigManager
{
    ref array<ref AdminUsers> m_Admins;
    void KR_AdminConfigManager()
    {
        m_Admins = new ref array<ref AdminUsers>;
    }

    void LoadDefaultSettings()
    {
        //Thats how 
        AddAdmin("ExampleSteamID",          "ExampleAdminName", CreatePermissions(true, true, true, true, true), false);
        AddAdmin("AnotherExampleSteamID",   "ExampleAdminName", CreatePermissions(true, true, true, true, true), false);
        Save();
    }

    AdminPermissions CreatePermissions(bool CanUseServerConfig = false, bool CanUsePlayersBankData = false, bool CanUsePlayersClans = false, bool CanUseMiscServer = false, bool CanAddAdmins = false)
    {
        AdminPermissions newPermissions = new AdminPermissions(CanUseServerConfig, CanUsePlayersBankData, CanUsePlayersClans, CanUseMiscServer, CanAddAdmins);
        return newPermissions;
    }

    void AddAdmin(string PlainID, string Name, AdminPermissions perms, bool shouldSafe = true)
    {
        m_Admins.Insert(new ref AdminUsers(PlainID, Name, perms));

        if(shouldSafe)
            Save();
    }

    void Save()
    {
       if (!FileExist(m_BankingProfileDIR + m_BankingConfigDIR + "/"))
			MakeDirectory(m_BankingProfileDIR + m_BankingConfigDIR + "/");

        BankingJsonFileLoader<KR_AdminConfigManager>.JsonSaveFile("$profile:KR_BANKING/Admins.json", this);
    }

    static KR_AdminConfigManager Load()
    {
        KR_AdminConfigManager settings = new KR_AdminConfigManager();
        if(!FileExist(m_BankingConfigDIR))
            MakeDirectory(m_BankingConfigDIR);
        if(FileExist("$profile:KR_BANKING/Admins.json"))
        {
            Print("[Advanced Banking] -> Found Admin Config Loading existing config...");
            bool hasError = BankingJsonFileLoader<KR_AdminConfigManager>.JsonLoadFile("$profile:KR_BANKING/Admins.json", settings);
            if(hasError)
            {
                Print("JSON ERROR IN YOUR CONFIG CAN NOT START MOD!");
            }
        }
        else
        {
            Print("[Advanced Banking] -> No Admin Config Found Try to write default config...");
            settings.LoadDefaultSettings();
        }
        return settings;
    }
};


static ref KR_AdminConfigManager g_KR_AdminConfigManager;
static KR_AdminConfigManager GetKR_AdminConfig() 
{
    if (g_Game.IsServer() && !g_KR_AdminConfigManager) 
    {
        g_KR_AdminConfigManager = KR_AdminConfigManager.Load();
        g_KR_AdminConfigManager.Save();
    }
    return g_KR_AdminConfigManager;
};