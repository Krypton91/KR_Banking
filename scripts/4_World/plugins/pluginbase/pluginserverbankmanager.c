class PluginKRBankingManagerServer extends PluginBase
{
    protected ref KR_BankingConfigManager m_krserverconfig;
	protected ref array<ref bankingplayerlistobj> m_BankingPlayers = new ref array<ref bankingplayerlistobj>;
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
		//RegisterNewClan("RB Server is Gay", "76561198796326626");
    }

    protected void RegisterServersideRPCs()
    {
        GetRPCManager().AddRPC("KR_BANKING","PlayerDataRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "ServerConfigRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "WithdrawRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "DepositRequest", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("KR_BANKING", "PlayerListRequst", this, SingleplayerExecutionType.Server);
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
					   if(m_krserverconfig.maxCurrency < playerdata.GetBankCredit() + ammountTOAddForSpecialUser)
					   {
						   #ifdef NOTIFICATIONS
						   NotificationSystem.SimpleNoticiation("Error with adding Paycheck Bank is already full!", "Banking", "Notifications/gui/data/notifications.edds", ARGB(240, 255, 13, 55), 5, identity);
						   #endif
						   continue;
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
                GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param2< int, string>( playerdata.GetBankCredit(), playerdata.GetClanID() ), true, sender);
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

	void PlayerListRequst(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(type == CallType.Server)
        {
            if(type == CallType.Server)
            {
                UpdatePlayerList();
				GetRPCManager().SendRPC("KR_BANKING", "PlayeristResponse", new Param1< ref array<ref bankingplayerlistobj> >( m_BankingPlayers ), true, sender);
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

            GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param2< int, string >( playerdata.GetBankCredit(), playerdata.GetClanID() ), true, identity);
            //Todo add here logs.
        }
    }

    void DepositMoneyOnOwnBank(PlayerIdentity identity, int Ammount)
    {
        KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());
        if(playerdata)
        {
			
			int MaxPlaceAbleAmount = GetMaxPlaceAbleAmmountForBank(playerdata);
			int SumToInsert = Ammount + playerdata.GetBankCredit();
			if(SumToInsert > MaxPlaceAbleAmount)
				SumToInsert = MaxPlaceAbleAmount;
			
			Print("Player try to insert: " + Ammount + " number was parsed to: " + MaxPlaceAbleAmount);
            playerdata.DepositMoney(SumToInsert);
            Print("Sucessfully added: " + SumToInsert.ToString() + " to bank account!");
            RemoveCurrencyFromPlayer(RemoteFindPlayer(identity.GetPlainId()), SumToInsert);
            GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param2< int, string >( playerdata.GetBankCredit(), playerdata.GetClanID() ), true, identity);
        }
    }

	void UpdatePlayerList()
	{
		m_BankingPlayers.Clear();
		array<Man> olpl = new array<Man>;
		GetGame().GetPlayers(olpl);
		
		for(int i = 0; i < olpl.Count(); i++)
		{
			PlayerBase player = PlayerBase.Cast(olpl.Get(i));
			if(player && player.GetIdentity())
			{
				m_BankingPlayers.Insert(new ref bankingplayerlistobj(player.GetIdentity().GetPlainId(), player.GetIdentity().GetName()));
			}
		}
	}
	//!retuns sum of bc acc
	int GetMaxPlaceAbleAmmountForBank(KR_JsonDatabaseHandler playerdata)
	{
		int currentBankAmount = playerdata.GetBankCredit();
		int maxConfigAmount = m_krserverconfig.maxCurrency;

		int sum = maxConfigAmount - currentBankAmount;

		if(playerdata.GetBonusAmount())
			sum += playerdata.GetBonusAmount();
		
		return sum;
	}

	void RegisterNewClan(string ClanName, string ClanOwnersID)
	{
		//string ClanName, string ClanID, string ownersPlainID
		string ClanID = GenerateRandomClanID();
		ClanDataBaseManager clanDB = ClanDataBaseManager.CreateClan(ClanName, ClanID, ClanOwnersID);
		if(clanDB)
		{
			//string PlainID, string PlayerName, Permission
			PermissionObject perms = new PermissionObject();
			perms.GiveClanOwner();
			clanDB.AddMember("76561198796326626", "DevKrypton",perms);
		}
	}

	string GenerateRandomClanID()
	{
		const ref array<string> m_chars = { "A", "B", "C", "D", "E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","a", "b", "c", "d", "e","f","g","h","i","j","k","l","m","n","o","o","q","r","s","t","u","v","w","x","y","z","1","2","3","4","5","6","7","8","9" };
		string rndf = "";
		for(int i = 0; i < 16; i++)
			rndf += m_chars.GetRandomElement();
		return rndf;
	}

    /* THANKS TO DAEMON FORGE! <3 */
    int AddCurrencyToPlayer(PlayerBase player, int amountToAdd)
	{
		if(amountToAdd <= 0)
		{
			return 0;
		}

		int amountStillNeeded = amountToAdd;
		int quantityNeeded;
		int quantityLeft;
		
		for(int i = 0; i < GetKR_BankingServerConfig().BankingCurrency.Count(); i++)
		{
			quantityNeeded = Math.Floor(amountStillNeeded / GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue);
			if(quantityNeeded > 0)
			{
				quantityLeft = AddCurrencyToInventory(player, i, quantityNeeded);
                //Print("Adding " + GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyName + " " + quantityNeeded.ToString());
				amountStillNeeded -= (quantityNeeded - quantityLeft) * GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue;
				
				if(amountStillNeeded == 0)
				{
					return 0;
				}
			}
		}
		return amountStillNeeded;
	}

    int RemoveCurrencyFromPlayer(PlayerBase player, int amountToRemove)
	{
		int amountStillNeeded = amountToRemove;

        if(amountToRemove <= 0)
		{
			return 0;
		}

		array<EntityAI> invItems = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, invItems);
		
		ItemBase item;
		
		int currencyValue;
		int quantityNeeded;
		for(int i = GetKR_BankingServerConfig().BankingCurrency.Count() - 1; i >= 0; i--)
		{
			for(int j = 0; j < invItems.Count(); j++)
			{
				if(GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyName == invItems.Get(j).GetType())
				{
					if(Class.CastTo(item, invItems.Get(j))
					{
						currencyValue = GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue;
						quantityNeeded = amountStillNeeded / currencyValue;
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
    
    int AddCurrencyToInventory(PlayerBase player, int CurrencyArrayIndex, int ammountToAdd)
	{
		if(ammountToAdd <= 0)
		{
			return 0;
		}
		
		int quantityLeft = ammountToAdd;
		
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
						if(addableQuantity >= quantityLeft)
						{
							SetItemQuantity(currencyItem, GetItemQuantity(currencyItem) + quantityLeft);
							quantityLeft = 0;
						}
						else
						{
							SetItemQuantity(currencyItem, GetItemQuantityMax(currencyItem));
							quantityLeft -= addableQuantity;
						}
					}
					
					if(quantityLeft == 0)
					{
						return 0;
					}
				}
			}
		}
		
		EntityAI createdCurrencyEntity;
		int currencyItemMaxQuantity;
		InventoryLocation il = new InventoryLocation();
		while(player.GetInventory().FindFirstFreeLocationForNewEntity(currency.CurrencyName, FindInventoryLocationType.CARGO, il))
		{
			createdCurrencyEntity = player.GetHumanInventory().CreateInInventory(currency.CurrencyName);
			if(Class.CastTo(currencyItem, createdCurrencyEntity)
            {
				currencyItemMaxQuantity = GetItemQuantityMax(currencyItem);
				if(currencyItemMaxQuantity == 0)
				{
					SetItemQuantity(currencyItem, 0);
					quantityLeft -= 1;
				}
				else
				{
					if(quantityLeft <= currencyItemMaxQuantity)
					{
						SetItemQuantity(currencyItem, quantityLeft);
						quantityLeft = 0;
					}
					else
					{
						SetItemQuantity(currencyItem, currencyItemMaxQuantity);
						quantityLeft -= currencyItemMaxQuantity;
					}
				}
				
				if(quantityLeft == 0)
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
					quantityLeft -= 1;
				}
				else
				{
					if(quantityLeft <= currencyItemMaxQuantity)
					{
						SetItemQuantity(currencyItem, quantityLeft);
						quantityLeft = 0;
					}
					else
					{
						SetItemQuantity(currencyItem, currencyItemMaxQuantity);
						quantityLeft -= currencyItemMaxQuantity;
					}
				}
				
				if(quantityLeft == 0)
				{
					return 0;
				}
			}
		}
		return quantityLeft;
	}

    /* QUANTITY SECTION */ 
    void SetItemQuantity(ItemBase item, int quantity)
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

    int GetItemQuantity(ItemBase item)
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

    int GetItemQuantityMax(ItemBase item)
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