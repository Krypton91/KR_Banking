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
        RegisterServersideRPCs();
        SpawnATMs();
        InitPayCheck();
    }

    protected void RegisterServersideRPCs()
    {
        GetRPCManager().AddRPC("KR_BANKING","PlayerDataRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "ServerConfigRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "WithdrawRequest", this, SingleplayerExecutionType.Server);
    }

    protected void InitPayCheck()
    {
        float TickTime = m_krserverconfig.GetCorrectPayCheckTime();

        if(TickTime && TickTime != -1)
        {
            //we sure he want to make paychecks.
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.TickPayCheck, TickTime, true);
        }
    }

    void TickPayCheck()
    {
        Print("[KR Banking] START GIVE PAYCHECKS!");
        array<Man> onlinePlayers = new array<Man>();
        GetGame().GetPlayers(onlinePlayers);
        if(m_krserverconfig.MinPlayersForPayCheck >= onlinePlayers.Count())
        {
            for(int i = 0; i < onlinePlayers.Count(); i++)
            {
               PlayerBase tempPlayer = PlayerBase.Cast(onlinePlayers.Get(i));
               PlayerIdentity identity = tempPlayer.GetIdentity();
               if(tempPlayer && identity)
               {
                   //Player & Identity is valid
                   KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());
                   if(playerdata)
                   {
                       //Json Loadet we can now add him the Money.
                       int ammountTOAddForSpecialUser = m_krserverconfig.PayCheckValue;
                       if(playerdata.GetPayCheckBonus() != 0)
                       {
                            ammountTOAddForSpecialUser =  (ammountTOAddForSpecialUser / 100) * playerdata.GetPayCheckBonus() + m_krserverconfig.PayCheckValue;
                       }
                       playerdata.DepositMoney(ammountTOAddForSpecialUser);
                       Print("Sucessfully added to players: " + identity.GetPlainId() + " bank " + ammountTOAddForSpecialUser.ToString() + " $");
                       #ifdef NOTIFICATIONS
                       NotificationSystem.SimpleNoticiation(ammountTOAddForSpecialUser.ToString() + " Added to your Bank Account! Stay active to get more paychecks!", "Banking", "Notifications/gui/data/notifications.edds", ARGB(240, 0, 175, 163), 10, identity);
                       #endif
                   }
               }
            }
        }
        Print("[KR Banking] END GIVE PAYCHECKS!");
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

    void ServerConfigRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
        if(type == CallType.Server)
        {
            GetRPCManager().SendRPC("KR_BANKING", "ServerConfigResponse", new Param5< int, float, bool, bool, ref array<ref CurrencySettings>  >( m_krserverconfig.maxCurrency, m_krserverconfig.MenuDelay, m_krserverconfig.IsRobEventActive, m_krserverconfig.NeedsBankCardToOpenMenu, m_krserverconfig.BankingCurrency ), true, sender);
        }
    }

    void WithdrawRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
        if(type == CallType.Server)
        {
            Param2<int, int> data;
            if(!ctx.Read(data)) return;
            if(type == CallType.Server)
            {
                if(data.param2 == 1)
                {
                    WitdrawMoneyFromBankAccount(sender, data.param1);
                }
            }
        }
    }

    void WitdrawMoneyFromBankAccount(PlayerIdentity identity, int Ammount)
    {
        KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());
        if(playerdata)
        {
            if(Ammount == 0) return;
            int CorrectAmountToWitdraw = Ammount;
            if(CorrectAmountToWitdraw > playerdata.GetBankCredit())
                CorrectAmountToWitdraw = playerdata.GetBankCredit();
            
            playerdata.WitdrawMoney(Ammount);
            AddCurrencyToPlayersInventory(identity.GetPlainId(), Ammount);

            GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param1< int >( playerdata.GetBankCredit() ), true, identity);
            //Todo add here logs.
        }
    }

    protected void AddCurrencyToPlayersInventory(string PlainID, int Ammount)
    {
        Print("Try to add " + PlainID + " Money with ammount:" + Ammount);
    }


    int GetPlayersBankAmount(PlayerIdentity identity)
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

/* Global Plugin Getter */ 
PluginKRBankingManagerServer GetBankingServerManager()
{
    if(GetGame().IsServer() && GetGame().IsMultiplayer())
    {
        return PluginKRBankingManagerServer.Cast(GetPluginManager().GetPluginByType(PluginKRBankingManagerServer));
    }
    return NULL;
}