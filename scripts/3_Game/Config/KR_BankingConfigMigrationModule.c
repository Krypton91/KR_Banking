class KR_BankingMigrationModule
{
    protected ref KR_BankingConfigManager m_loadedConfig;
    void KR_BankingMigrationModule()
    {

    }

    void CheckConfig(KR_BankingConfigManager config)
    {
        m_loadedConfig = config;

        if(!IsConfigOutdated())
        {
            Print("[Advanced Banking Config Migration Module] -> config version machted! no migration required.");
            return;
        }

        Print("[Advanced Banking Config Migration Module] -> config version mismatch detected!");

        InitAddedDefaultValues();
        m_loadedConfig.Save();
    }

    private void InitAddedDefaultValues()
    {
        //Initalize here all new added config params.
    }

    protected bool IsConfigOutdated() 
    {
		if(m_loadedConfig.ModVersion != GetModVersion())
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