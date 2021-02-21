class PluginKrBankingClientManager extends PluginBase
{
    protected ref KR_BankingMenu                    m_BankingMenu;
    protected int                                   m_PlayersCurrency;
    protected bool                                  m_IsFirstRequest = true;
    protected bool                                  m_IsWaitingForServersResponse;
    protected ref KR_BankingClientConfig            m_clientSettings

    void PluginKrBankingClientManager()
    {
        Init();
    }

    void Init()
    {
        GetRPCManager().AddRPC("KR_BANKING","PlayerDataResponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("KR_BANKING","ServerConfigResponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().SendRPC("KR_BANKING", "ServerConfigRequest", null, true);
    }

    //!Gets called when client opens the Banking Menu
    void PlayerDataResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            Param1<int> data;
            if ( !ctx.Read( data ) ) return;
            m_PlayersCurrency = data.param1;
            if(m_BankingMenu)
            {
                m_BankingMenu.UpdateUI();//Ivoke an Update
            }
        }
    }

    void ServerConfigResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            Param5<int, float, bool, bool, ref array<ref CurrencySettings>> data;
            if ( !ctx.Read( data ) ) return;
            m_clientSettings = new KR_BankingClientConfig(data.param1, data.param2, data.param3, data.param4, data.param5);
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