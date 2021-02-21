class PluginKrBankingClientManager extends PluginBase
{
    protected ref KR_BankingMenu                    m_BankingMenu;
    protected int                                   m_PlayersCurrency;
    protected bool                                  m_IsFirstRequest = true;
    protected bool                                  m_IsWaitingForServersResponse;
    protected string                                m_ClanID;
    protected ref KR_BankingClientConfig            m_clientSettings
    protected ref array<ref bankingplayerlistobj> m_BankingPlayers = new ref array<ref bankingplayerlistobj>;

    void PluginKrBankingClientManager()
    {
        Init();
    }

    void Init()
    {
        GetRPCManager().AddRPC("KR_BANKING","PlayerDataResponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("KR_BANKING","ServerConfigResponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("KR_BANKING","PlayeristResponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().SendRPC("KR_BANKING", "ServerConfigRequest", null, true);
    }

    //!Gets called when client opens the Banking Menu
    void PlayerDataResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            Param2<int, string> data;
            if ( !ctx.Read( data ) ) return;
            m_PlayersCurrency = data.param1;
            m_ClanID          = data.param2;
            if(m_BankingMenu)
            {
                m_BankingMenu.UpdateUI();//Invoke an Update
            }
        }
    }

    void ServerConfigResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            Param1<ref KR_BankingClientConfig> data;
            if ( !ctx.Read( data ) ) return;
            m_clientSettings = new KR_BankingClientConfig(data.param1.MaxCurrency, data.param1.InteractDelay, data.param1.isRobActive, data.param1.isBankCardNeeded, data.param1.BankingCurrency);
        }
    }

    void PlayeristResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            Param1<ref array<ref bankingplayerlistobj>> data;
            if ( !ctx.Read( data ) ) return;
            m_BankingPlayers.Clear();
            m_BankingPlayers = data.param1;
            if(m_BankingMenu)
                m_BankingMenu.InvokePlayerList();
        }
    }

    void OpenBankingMenu()
    {
        GetRPCManager().SendRPC("KR_BANKING", "PlayerDataRequest", null, true); //Send RPC for data.
        if(GetGame().GetUIManager().GetMenu() == NULL && m_BankingMenu == null)
        {
            m_BankingMenu = KR_BankingMenu.Cast(GetGame().GetUIManager().EnterScriptedMenu(KR_BANKING_MENU, null));
            m_BankingMenu.SetIsBankingMenuOpen(false);
        }
        else
        {
            if(GetGame().GetUIManager().GetMenu() == NULL && !m_BankingMenu.IsBankingMenuOpen())
            {
                GetGame().GetUIManager().ShowScriptedMenu(m_BankingMenu, NULL);
                m_BankingMenu.SetIsBankingMenuOpen(true);
            }
        }
    }

    void RequestOnlinePlayers()
    {
        GetRPCManager().SendRPC("KR_BANKING", "PlayerListRequst", null, true);
    }

    void RequestRemoteToWitdraw(int ammount, int mode)
    {
        GetRPCManager().SendRPC("KR_BANKING", "WithdrawRequest", new Param2<int, int>(ammount, mode), true);
        Print("Sucessfully requested remote to Witdraw Ammount: " + ammount);
    }

    void RequestRemoteToDeposit(int ammount, int mode)
    {
        GetRPCManager().SendRPC("KR_BANKING", "DepositRequest", new Param2<int, int>(ammount, mode), true);
        Print("Sucessfully requested remote to Deposit Ammount: " + ammount);
    }

    void RequestRemoteForTransfer(ref bankingplayerlistobj target, int ammount)
    {
        GetRPCManager().SendRPC("KR_BANKING", "TransferRequest", new Param2<ref bankingplayerlistobj, int>(target, ammount), true);
        Print("Sendet Transfer request to remote!");
    }
    
    array<ref CurrencySettings> GetServersCurrencyData()
    {
        return m_clientSettings.BankingCurrency;
    }

    int GetBankCredits()
    {
        return m_PlayersCurrency;
    }
    
    //!Returns the currency Ammount in Players Invenory
    int GetPlayerCurrencyAmount()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		
		int currencyAmount = 0;
		
		array<EntityAI> itemsArray = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

		ItemBase item;
		
		for (int i = 0; i < itemsArray.Count(); i++)
		{
			Class.CastTo(item, itemsArray.Get(i));

			if (!item)
				continue;

			for (int j = 0; j < GetServersCurrencyData().Count(); j++)
			{
				if(item.GetType() == GetServersCurrencyData().Get(j).CurrencyName)
				{
					currencyAmount += GetItemAmount(item) * GetServersCurrencyData().Get(j).CurrencyValue;
				}
			}
		}
		return currencyAmount;
	}

    ref array<ref bankingplayerlistobj> GetOnlinePlayers()
    {
        return m_BankingPlayers;
    }

    bool hasClan()
    {
        if(m_ClanID && m_ClanID != "NONE")
            return true;
        return false;
    }

    int GetItemAmount(ItemBase item)
	{
		Magazine mgzn = Magazine.Cast(item);
				
		int itemAmount = 0;
		if( item.IsMagazine() )
		{
			itemAmount = mgzn.GetAmmoCount();
		}
		else
		{
			itemAmount = QuantityConversions.GetItemQuantity(item);
		}
		
		return itemAmount;
	}
    
    bool WaitForServerResponse()
    {
        return m_IsWaitingForServersResponse;
    }
};

PluginKrBankingClientManager GetBankingClientManager()
{
	return PluginKrBankingClientManager.Cast(GetPluginManager().GetPluginByType(PluginKrBankingClientManager));
}