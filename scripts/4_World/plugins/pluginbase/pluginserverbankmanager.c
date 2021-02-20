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
        GetRPCManager().AddRPC("KR_BANKING", "DepositRequest", this, SingleplayerExecutionType.Server);
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

    void DepositRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
        if(type == CallType.Server)
        {
            Param2<int, int> data;
            if(!ctx.Read(data)) return;
            if(type == CallType.Server)
            {
                if(data.param2 == 1)
                {
                    DepositMoneyOnOwnBank(sender, data.param1);
                }
            }
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
            
            playerdata.WitdrawMoney(CorrectAmountToWitdraw);

            AddCurrencyToPlayer(RemoteFindPlayer(identity.GetPlainId()), CorrectAmountToWitdraw);

            GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param1< int >( playerdata.GetBankCredit() ), true, identity);
            //Todo add here logs.
        }
    }


    void DepositMoneyOnOwnBank(PlayerIdentity identity, int Ammount)
    {
        KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());
        if(playerdata)
        {
            playerdata.DepositMoney(Ammount);
            Print("Sucessfully added: " + Ammount.ToString() + " to bank account!");
            RemoveCurrencyFromPlayer(RemoteFindPlayer(identity.GetPlainId()), Ammount);
            GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param1< int >( playerdata.GetBankCredit() ), true, identity);
        }
    }

    protected int AddCurrencyToPlayer(PlayerBase player, int amountToAdd)
	{
		if(amountToAdd <= 0)
		{
			return 0;
		}
        
		int amountStillNeeded = amountToAdd;
		int quantityNeeded;
		int quantityLeftToAdd;
		
		for(int i = 0; i < GetKR_BankingServerConfig().BankingCurrency.Count(); i++)
		{
			quantityNeeded = Math.Floor(amountStillNeeded / GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue);
			if(quantityNeeded > 0)
			{
				quantityLeftToAdd = AddCurrencyToInventory(player, i, quantityNeeded);
                //Print("Adding " + GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyName + " " + quantityNeeded.ToString());
				amountStillNeeded -= (quantityNeeded - quantityLeftToAdd) * GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue;
				
				if(amountStillNeeded == 0)
				{
					return 0;
				}
			}
		}
		return amountStillNeeded;
	}

    protected int RemoveCurrencyFromPlayer(PlayerBase player, int amountToRemove)
	{
		if(amountToRemove <= 0)
		{
			return 0;
		}
		
		int amountStillNeeded = amountToRemove;
		
		array<ref CurrencySettings> currency = GetKR_BankingServerConfig().BankingCurrency;
		
		array<EntityAI> inventory = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, inventory);
		
		ItemBase item;
		
		int currencyValue;
		int quantityNeeded;
		for(int i = currency.Count() - 1; i >= 0; i--)
		{
			for(int j = 0; j < inventory.Count(); j++)
			{
				if(currency.Get(i).CurrencyName == inventory.Get(j).GetType())
				{
					Class.CastTo(item, inventory.Get(j));
					if(item)
					{
						currencyValue = currency.Get(i).CurrencyValue;
						quantityNeeded = Math.Floor(amountStillNeeded / currencyValue); //floor is probably unnecessary since both are int
						if(GetItemQuantityMax(item) == 0)
						{
							GetGame().ObjectDelete(item);
							if(quantityNeeded >= 1)
							{
								amountStillNeeded -= currencyValue;
							}
							else
							{
								return AddCurrencyToPlayer(player, currencyValue - amountStillNeeded);							
							}
						}
						else
						{
							if(quantityNeeded >= GetItemQuantity(item))
							{
								amountStillNeeded -= GetItemQuantity(item) * currencyValue;
								GetGame().ObjectDelete(item);
							}
							else
							{
								SetItemQuantity(item, GetItemQuantity(item) - quantityNeeded);
								amountStillNeeded -= quantityNeeded * currencyValue;
								
								if(amountStillNeeded < currencyValue)
								{
									if(GetItemQuantity(item) == 1)
									{
										GetGame().ObjectDelete(item);
									}
									else
									{
										SetItemQuantity(item, GetItemQuantity(item) - 1);
									}
									return AddCurrencyToPlayer(player, currencyValue - amountStillNeeded);
								}
							}
						}
					}
				}
			}
		}
		return 0;
	}
    
    //!Creates Currency in invenory & returns the ammount what is not addable.
    protected int AddCurrencyToInventory(PlayerBase player, int CurrencyArrayIndex, int ammountToAdd)
	{
		if(ammountToAdd <= 0)
		{
			return 0;
		}
		
		int quantityLeftToAdd = ammountToAdd;
		
		CurrencySettings currency = GetKR_BankingServerConfig().BankingCurrency.Get(CurrencyArrayIndex);
		
		array<EntityAI> invArray = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, invArray);
		ItemBase currencyItem;
		
		int addableQuantity;
		
		for(int i = 0; i < invArray.Count(); i++) //Add currency to already in the inventory existing currency
		{
			if(invArray.Get(i).GetType() == currency.CurrencyName)
			{
				Class.CastTo(currencyItem, invArray.Get(i));
				if(currencyItem)
				{
					addableQuantity = GetItemQuantityMax(currencyItem) - GetItemQuantity(currencyItem);
					if(addableQuantity > 0)
					{
						if(addableQuantity >= quantityLeftToAdd)
						{
							SetItemQuantity(currencyItem, GetItemQuantity(currencyItem) + quantityLeftToAdd);
							quantityLeftToAdd = 0;
						}
						else
						{
							SetItemQuantity(currencyItem, GetItemQuantityMax(currencyItem));
							quantityLeftToAdd -= addableQuantity;
						}
					}
					
					if(quantityLeftToAdd == 0)
					{
						return 0;
					}
				}
			}
		}
		
		EntityAI createdCurrencyEntity;
		int currencyItemMaxQuantity;
		InventoryLocation invLocation = new InventoryLocation();
		while(player.GetInventory().FindFirstFreeLocationForNewEntity(currency.CurrencyName, FindInventoryLocationType.CARGO, invLocation)) //Create new currency in the inventory
		{
			createdCurrencyEntity = player.GetHumanInventory().CreateInInventory(currency.CurrencyName);
			if(Class.CastTo(currencyItem, createdCurrencyEntity)
            {
				currencyItemMaxQuantity = GetItemQuantityMax(currencyItem);
				if(currencyItemMaxQuantity == 0)
				{
					SetItemQuantity(currencyItem, 0);
					quantityLeftToAdd -= 1;
				}
				else
				{
					if(quantityLeftToAdd <= currencyItemMaxQuantity)
					{
						SetItemQuantity(currencyItem, quantityLeftToAdd);
						quantityLeftToAdd = 0;
					}
					else
					{
						SetItemQuantity(currencyItem, currencyItemMaxQuantity);
						quantityLeftToAdd -= currencyItemMaxQuantity;
					}
				}
				
				if(quantityLeftToAdd == 0)
				{
					return 0;
				}
			}
			else
			{
				break;
			}
		}
		
		if(!player.GetHumanInventory().GetEntityInHands()) //Create new currency in the hands of the player
		{
			createdCurrencyEntity = player.GetHumanInventory().CreateInHands(currency.CurrencyName);
			Class.CastTo(currencyItem, createdCurrencyEntity);
			if(currencyItem)
			{
				currencyItemMaxQuantity = GetItemQuantityMax(currencyItem);
				if(currencyItemMaxQuantity == 0)
				{
					SetItemQuantity(currencyItem, 0);
					quantityLeftToAdd -= 1;
				}
				else
				{
					if(quantityLeftToAdd <= currencyItemMaxQuantity)
					{
						SetItemQuantity(currencyItem, quantityLeftToAdd);
						quantityLeftToAdd = 0;
					}
					else
					{
						SetItemQuantity(currencyItem, currencyItemMaxQuantity);
						quantityLeftToAdd -= currencyItemMaxQuantity;
					}
				}
				
				if(quantityLeftToAdd == 0)
				{
					return 0;
				}
			}
		}
		return quantityLeftToAdd;
	}

    /* QUANTITY SECTION */ 
    protected void SetItemQuantity(ItemBase item, int quantity)
	{
		if(!item || quantity < 0)
		{
			return;
		}
		
		if(item.IsMagazine())
		{
			Magazine magazine = Magazine.Cast(item);
			if(magazine)
			{
				magazine.ServerSetAmmoCount(quantity);
			}
		}
		else
		{
			item.SetQuantity(quantity);
		}
	}

    int GetItemMaxQuantity(string itemClassname) // duplicate now useable for everyone!
	{
		TStringArray searching_in = new TStringArray;
		searching_in.Insert( CFG_MAGAZINESPATH  + " " + itemClassname + " count");
		//searching_in.Insert( CFG_WEAPONSPATH );
		searching_in.Insert( CFG_VEHICLESPATH + " " + itemClassname + " varQuantityMax");

		for ( int s = 0; s < searching_in.Count(); ++s )
		{
			string path = searching_in.Get( s );

			if ( GetGame().ConfigIsExisting( path ) )
			{
				return g_Game.ConfigGetInt( path );
			}
		}

		return 0;
	}

    protected int GetItemQuantity(ItemBase item)
	{
		if(!item)
		{
			return 0;
		}
		
		if(item.IsMagazine())
		{
			Magazine magazineItem = Magazine.Cast(item);
			if(magazineItem)
			{
				return magazineItem.GetAmmoCount();
			}
		}
		return item.GetQuantity();
	}

    protected int GetItemQuantityMax(ItemBase item)
	{
		if(!item)
		{
			return 0;
		}
		
		if(item.IsMagazine())
		{
			Magazine magazine = Magazine.Cast(item);
			if(magazine)
			{
				return magazine.GetAmmoMax();
			}
		}
		return item.GetQuantityMax();
	}

    //!retrurns PlayerBase with steam64ID if player is connected to server!
    PlayerBase RemoteFindPlayer(string Steam64ID)
    {
        array<Man> onlinePlayers = new array<Man>;
        GetGame().GetPlayers(onlinePlayers);

        for(int i = 0; i < onlinePlayers.Count(); i++)
        {
            if(onlinePlayers.Get(i).GetIdentity().GetPlainId() == Steam64ID)
            {
                return PlayerBase.Cast(onlinePlayers.Get(i));
            }
        }
        return NULL;
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