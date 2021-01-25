class PluginKRBankingManagerServer extends PluginBase
{
    protected ref KR_BankingConfigManager m_krserverconfig;
    void PluginKRBankingManagerServer()
    {
        if(!m_krserverconfig)
            m_krserverconfig = GetKR_BankingServerConfig();
        
        Init();
    }

    void Init()
    {
        GetRPCManager().AddRPC("KR_BANKING","PlayerDataRequest", this, SingleplayerExecutionType.Server);
        SpawnATMs();
    }

    void PlayerDataRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
        if(type == CallType.Server)
        {
            KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());
            if(playerdata)
            {
                GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param1< int >( playerdata.GetBankCredit() ), true, sender);
            }
        }
    }

    int GetCurrencyFromPlayer(PlayerIdentity identity)
    {
        KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());
        if(playerdata)
        {
           return playerdata.GetBankCredit();
        }
        return -1;
    }

    protected void SpawnATMs()
    {
        for(int i = 0; i < m_krserverconfig.ATM.Count(); i++)
        {
            local vector tempPos = m_krserverconfig.ATM.Get(i).GetATMPosition();
            local vector tempDirection = m_krserverconfig.ATM.Get(i).GetATMDirectory();
            string ObjectName = m_krserverconfig.ATM.Get(i).ATMSName;
            if(!tempPos || !tempDirection || !ObjectName)
                continue;
            
            EntityAI SpawnedATM = EntityAI.Cast(GetGame().CreateObject(ObjectName, tempPos));
            SpawnedATM.SetPosition(tempPos);
            SpawnedATM.SetOrientation(tempDirection);
            Print("[KR Banking]-> Sucesfully spawned Object: " + ObjectName + " on: " + tempPos);
        }
    }
};
PluginKRBankingManagerServer GetBankingServerManager()
{
	return PluginKRBankingManagerServer.Cast(GetPluginManager().GetPluginByType(PluginKRBankingManagerServer));
}