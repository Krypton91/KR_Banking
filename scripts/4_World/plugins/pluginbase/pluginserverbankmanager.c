class PluginKRBankingManagerServer extends PluginBase
{
    protected ref KR_BankingConfigManager m_krserverconfig;
	protected ref array<ref bankingplayerlistobj> m_BankingPlayers = new ref array<ref bankingplayerlistobj>;
	autoptr TStringArray m_chars = new TStringArray();
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
		m_chars = { "A", "B", "C", "D", "E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","a", "b", "c", "d", "e","f","g","h","i","j","k","l","m","n","o","o","q","r","s","t","u","v","w","x","y","z","1","2","3","4","5","6","7","8","9" };
    }

    protected void RegisterServersideRPCs()
    {
        GetRPCManager().AddRPC("KR_BANKING", "PlayerDataRequest", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "ServerConfigRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "WithdrawRequest", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "DepositRequest", 		this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("KR_BANKING", "PlayerListRequst", 	this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("KR_BANKING", "TransferRequest", 	this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("KR_BANKING", "ClanCreateRequest", 	this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("KR_BANKING", "ClanSyncRequest", 	this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("KR_BANKING", "ClanAddMember", 		this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("KR_BANKING", "ClanRemoveMember", 	this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("KR_BANKING", "ClanUpdateMember", 	this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("KR_BANKING", "ClanMemberLeave", 	this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("KR_BANKING", "ClanUpdate", 			this, SingleplayerExecutionType.Server);
    }

    protected void InitPayCheck()
    {
        int TickTime = m_krserverconfig.GetCorrectPayCheckTime();

        if(TickTime && m_krserverconfig.PayCheckTickTime != -1)
        {
            //we sure he want to make paychecks.
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.TickPayCheck, TickTime, true);
        }
    }

    void TickPayCheck()
    {
        array<Man> onlinePlayers = new array<Man>();
        GetGame().GetPlayers(onlinePlayers);

        if(onlinePlayers.Count() >= m_krserverconfig.MinPlayersForPayCheck)
        {
            for(int i = 0; i < onlinePlayers.Count(); i++)
            {
               PlayerBase tempPlayer = PlayerBase.Cast(onlinePlayers.Get(i));
               PlayerIdentity identity = tempPlayer.GetIdentity();
               if(tempPlayer && identity)
               {
				   #ifdef TRADER
				   if(tempPlayer.m_Trader_IsInSafezone && !m_krserverconfig.CanAddPayCheckInSafezone)//Skip players in safezone.
				   		continue;
				   #endif

                   KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());
                   if(playerdata)
                   {
                       int ammountTOAddForSpecialUser = m_krserverconfig.PayCheckValue;
                       if(playerdata.GetPayCheckBonus() != 0)
                       {
                            ammountTOAddForSpecialUser =  (ammountTOAddForSpecialUser / 100) * playerdata.GetPayCheckBonus() + m_krserverconfig.PayCheckValue;
                       }
					   if(m_krserverconfig.maxCurrency < playerdata.GetBankCredit() + ammountTOAddForSpecialUser && !m_krserverconfig.CanAddToFullAcc)
					   {
						   if(m_krserverconfig.IsPayCheckMessageActive)
						   		SendNotification("#AB_BankAlreadyFullPayCheckError", identity, true);
						   continue;
					   }
                       playerdata.DepositMoney(ammountTOAddForSpecialUser);
					   if(m_krserverconfig.IsPayCheckMessageActive)
					   		SendNotification(ReplacedPlaceHolderWithValue(m_krserverconfig.PayCheckMessage), identity);
                   }
               }
            }
        }
    }

	//!Replace Placeholders from ConfigEntry Todo add some more Placeholders here.
	string ReplacedPlaceHolderWithValue(string Placeholder)
	{
		if(Placeholder.Contains("%Amount%"))
		{
			Placeholder.Replace("%Amount%", m_krserverconfig.PayCheckValue.ToString());
		}

		return Placeholder;
	}

    void PlayerDataRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
        if(type == CallType.Server)
        {
            KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());
            if(playerdata)
            {
                GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param3< int, string, int>( playerdata.GetBankCredit(), playerdata.GetClanID(), playerdata.GetBonusAmount() ), true, sender);
            }
        }
    }

    void ServerConfigRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
        if(type == CallType.Server)
        {
			SendConfigToClient(sender);
        }
    }

	void InternalConfigReload()
	{
		m_krserverconfig = GetKR_BankingServerConfig();
	}

	void SendConfigToClient(PlayerIdentity identity)
	{
		KR_BankingClientConfig clientsettings = new KR_BankingClientConfig(m_krserverconfig.maxCurrency, m_krserverconfig.MenuDelay, m_krserverconfig.IsRobEventActive, m_krserverconfig.NeedsBankCardToOpenMenu, m_krserverconfig.BankingCurrency, m_krserverconfig.CostsToCreateClan, m_krserverconfig.MaxClanAccountLimit, m_krserverconfig.IsClanAccountActive);
		clientsettings.TimeInSecToRobATM = m_krserverconfig.TimeInSecToRobATM;
        GetRPCManager().SendRPC("KR_BANKING", "ServerConfigResponse", new Param2< ref KR_BankingClientConfig, string >( clientsettings, identity.GetPlainId() ), true, identity);
	}

    void DepositRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
        Param2<int, int> data;
        if(!ctx.Read(data)) return;
        if(type == CallType.Server)
        {
			if(IsNegativ(data.param1)) return;
            if(data.param2 == BankType.OWNBANK)
            {
                DepositMoneyOnOwnBank(sender, data.param1);
            }
			else
			{
				DepositMoneyOnClanBank(sender, data.param1);
			}
        }
    }

	void TransferRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(type == CallType.Server)
        {
            Param2<string, int> data;
            if(!ctx.Read(data)) return;
			string TargetsPlainID = data.param1;
			int TransferAmount = data.param2;
			if(TransferAmount < m_krserverconfig.MinAmountToTransfer)
			{
				SendNotification("#AB_ErrorMinAmountNotReachedTransfer " + m_krserverconfig.MinAmountToTransfer, sender, true);
				return;
			}

			if(!TargetsPlainID || !TransferAmount)
			int currentFeec =  Math.Floor((TransferAmount / 100) * m_krserverconfig.TransferfeesInProcent);//Use Floor because Math.Round dont work lol.
			int AmountToTransfer = TransferAmount - currentFeec; //Amount with fees!
			PlayerBase targetPlayer = RemoteFindPlayer(TargetsPlainID);
			if(!targetPlayer || sender.GetPlainId() == TargetsPlainID)
			{
				SendNotification("#AB_PlayerNotAvaibleForTransfers", sender, true);
				return;
			}

			KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());
			if(playerdata)
			{
				if(playerdata.GetBankCredit() < TransferAmount)
				{
					SendNotification("You dont have enough money to do this!", sender, true);
				}
				else
				{
					//Player what will transfer has enough money to Transfer!
					KR_JsonDatabaseHandler targetdata = KR_JsonDatabaseHandler.LoadPlayerData(targetPlayer.GetIdentity().GetPlainId(), targetPlayer.GetIdentity().GetName());
					if(targetdata)
					{
						//Check if target can store this transfer!
						int TargetsMaxPlaceAbleAmount = GetMaxPlaceAbleAmmountForBank(targetdata);
						if(targetdata.GetBankCredit() < TargetsMaxPlaceAbleAmount)
						{
							//Target can store this transfer!
							playerdata.WitdrawMoney(AmountToTransfer);
							targetdata.DepositMoney(AmountToTransfer);
							SendNotification("#AB_TransferSucess " + AmountToTransfer + " #AB_ToPlayer " + targetPlayer.GetIdentity().GetName(), sender);
							SendNotification("#AB_TransferRecived " + AmountToTransfer + " #AB_FromPlayer " + sender.GetName(), targetPlayer.GetIdentity());

						}
						else
						{
							SendNotification("#AB_PlayerNotAvaibleForTransfers", sender, true);
						}
					}
					else
					{
						SendNotification("#AB_PlayerHasNoBankAccount", sender, true);
					}
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
            if(data.param2 == BankType.OWNBANK)
            {
                WitdrawMoneyFromBankAccount(sender, data.param1);
            }
			else
			{
				WitdrawMoneyFromClanBankAccount(sender, data.param1);
			}
        }
    }

	void PlayerListRequst(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(type == CallType.Server)
        {
            UpdatePlayerList();
			GetRPCManager().SendRPC("KR_BANKING", "PlayeristResponse", new Param1< ref array<ref bankingplayerlistobj> >( m_BankingPlayers ), true, sender);
        }
		else
		{
			Error("FATAL RPC ERROR CLIENT TRYED TO CALL SERVERSIDE CODE!");
		}
	}

	void ClanCreateRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(type == CallType.Server)
		{	
			Param2<string, string> data;
			if(!ctx.Read(data)) return;

			PlayerBase player = RemoteFindPlayer(sender.GetPlainId());
			if(!player) return;
			int CurrencyOnPlayer = GetPlayerCurrencyAmount(player);
			if(CurrencyOnPlayer < m_krserverconfig.CostsToCreateClan)
			{
				SendNotification("#AB_YouDontHave " + m_krserverconfig.CostsToCreateClan + " #AB_InYourInventory", sender, true);
				return;
			}

			ref ClanDataBaseManager usersNewClan = RegisterNewClan(data.param1, sender.GetPlainId());
			if(usersNewClan)
			{
				PermissionObject perms = new PermissionObject();
				perms.GiveClanOwner();
				AddClanMember(usersNewClan, perms, sender.GetPlainId(), sender.GetName());
				string ClanID = usersNewClan.GetClanID();
				KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());
				if(playerdata && ClanID)
				{
					RemoveCurrencyFromPlayer(player, m_krserverconfig.CostsToCreateClan);
					playerdata.SetClan(ClanID);
					GetRPCManager().SendRPC("KR_BANKING","UIQuitRequest", null, true, sender);
					usersNewClan.SetPrefix(data.param2);
					SendNotification("#AB_SucesfullyCreatedClan " + usersNewClan.GetName(), sender);

					if(m_krserverconfig.m_LoggingSettings.m_LogCreateClanAccount)
						GetBankingLogManager().Log("Player: " + sender.GetPlainId() + " Created Clan: " + usersNewClan.GetName());

					if(m_krserverconfig.m_DiscordWebhook.m_LogClanCreateToDiscord)
						GetWebhookManager().PlayerLog(sender, " Created an clan account with id: " + usersNewClan.GetClanID());
				}
			}
			else
			{
				Error("Failed to create clan please report this to the dev team!");
			}
		}
	}

	void ClanSyncRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(type == CallType.Server)
		{
			KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());
			if(playerdata)
			{
				ClanDataBaseManager clanDB = ClanDataBaseManager.LoadClanData(playerdata.GetClanID());
				if(clanDB)
				{
					//clanDB.CheckName(sender.GetName(), sender.GetPlainId());
					GetRPCManager().SendRPC("KR_BANKING", "ClanSyncRespose", new Param1< ref ClanDataBaseManager >( clanDB ), true, sender);
				}
			}
		}
		else
		{
			Error("Failed to create clan please report this to the dev team!");
		}
	}

	void ClanAddMember(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(type == CallType.Server)
		{	
			Param1<string> data;
			if(!ctx.Read(data)) return;

			PlayerBase t_player = RemoteFindPlayer(data.param1);
			if(!t_player) return;
			KR_JsonDatabaseHandler targetPlayer = KR_JsonDatabaseHandler.LoadPlayerData(t_player.GetIdentity().GetPlainId(), t_player.GetIdentity().GetName());
			KR_JsonDatabaseHandler currentPlayer = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());
			if(targetPlayer && currentPlayer)
			{
				if(targetPlayer.GetClanID() != "NONE")
				{
					SendNotification("#AB_ErrorPlayerHasAlreadyAnClan", sender, true);
					return;
				}

				ClanDataBaseManager clandata = ClanDataBaseManager.LoadClanData(currentPlayer.GetClanID());
				if(clandata)
				{
					PlayerBase player = RemoteFindPlayer(sender.GetPlainId());
					if(!player) return;
					int CurrencyOnPlayer = GetPlayerCurrencyAmount(player);
					if(CurrencyOnPlayer >= m_krserverconfig.CostsToInviteAnPlayer)
					{
						ref ClanMemberObject user = clandata.GetMemberByPlainId(sender.GetPlainId());
						if(!user || !user.GetPermission().m_CanInvite)
						{
							SendNotification("#AB_NotEnoughtPermissions", sender, true);
							return;
						}
						if(clandata.GetMemberCount() >= m_krserverconfig.MaxPlayersInClan && m_krserverconfig.MaxPlayersInClan != -1)
						{
							SendNotification("#AB_ClanMaxPlayerLimitReached", sender, true);
							return;
						}

						RemoveCurrencyFromPlayer(player, m_krserverconfig.CostsToInviteAnPlayer);
						targetPlayer.SetClan(clandata.GetClanID());
						PermissionObject perms = new PermissionObject();
						AddClanMember(clandata, perms, data.param1, targetPlayer.GetName());
						//Sync new clan data to both players!
						GetRPCManager().SendRPC("KR_BANKING", "ClanSyncRespose", new Param1< ref ClanDataBaseManager >( clandata ), true, t_player.GetIdentity());
						GetRPCManager().SendRPC("KR_BANKING", "ClanSyncRespose", new Param1< ref ClanDataBaseManager >( clandata ), true, sender);

						//Update the player data!
						GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param2< int, string >( targetPlayer.GetBankCredit(), targetPlayer.GetClanID() ), true, t_player.GetIdentity());
						SendNotification("#AB_InvitedToClan: " + t_player.GetIdentity().GetName(), sender);
						SendNotification("#AB_YouGotAnInviteFromClan " + clandata.GetName() + " #AB_FromPlayer " + sender.GetName(), t_player.GetIdentity());
					}
					else
					{
						SendNotification("#AB_YouDontHave: " + m_krserverconfig.CostsToInviteAnPlayer.ToString() + " #AB_InYourInventory", sender, true);
					}
				}
				else
				{
					Error("Cant load clan data! with clan id: " + currentPlayer.GetClanID());
				}
			}
			else
			{
				Error("Error cant load Internal Playerdata");
			}
		}
	}

	void ClanRemoveMember(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(type == CallType.Server)
		{	
			Param1<string> data;
			if(!ctx.Read(data)) return;
			if(sender.GetPlainId() == data.param1)
			{
				SendNotification("#AB_CanNotKickSelf", sender, true);
				return;
			}
			KR_JsonDatabaseHandler targetPlayer = KR_JsonDatabaseHandler.LoadPlayerData(data.param1);
			KR_JsonDatabaseHandler SendersData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());
			if(targetPlayer && SendersData)
			{
				if(targetPlayer.GetClanID() != SendersData.GetClanID())
				{
					SendNotification("#AB_ThisPlayerIsNotInYourClan", sender, true);
					return;
				}

				ClanDataBaseManager clandata = ClanDataBaseManager.LoadClanData(SendersData.GetClanID());
				if(clandata)
				{
					PermissionObject perms;
					for(int i = 0; i < clandata.GetClanMembers().Count(); i++)
					{
						if(clandata.GetClanMembers().Get(i).GetPlainID())
						{
							perms = clandata.GetClanMembers().Get(i).GetPermission();
							break;
						}
					}
					if(perms.m_CanKick)
					{
						if(data.param1 == clandata.GetOwnersID())
						{
							SendNotification("#AB_CanNotKickOwner", sender, true);
							return;
						}
						else
						{
							for(int n = 0; n < clandata.GetClanMembers().Count(); n++)
							{
								if(clandata.GetClanMembers().Get(n).GetPlainID() == data.param1)
								{
									clandata.RemoveMember(n);
									targetPlayer.SetClan("NONE");
									PlayerBase t_player = RemoteFindPlayer(data.param1);
									//GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param2< int, string >( targetPlayer.GetBankCredit(), targetPlayer.GetClanID() ), true, t_player.GetIdentity());
									SendNotification("#AB_SucessKicked " + data.param1, sender);
									if(t_player)
										SendNotification("#AB_YouHaveBeenKicked " + clandata.GetName() + " #AB_FromPlayer " + sender.GetName(), t_player.GetIdentity());

									if(m_krserverconfig.m_DiscordWebhook.m_LogClanKickMemberToDiscord)
										GetWebhookManager().PlayerLog(sender, " kicked member: " + data.param1 + " from clan: " + clandata.GetName());
									break;
								}
							}
						}
					}
					else
					{
						SendNotification("#AB_NotEnoughtPermissions", sender);
					}
				}
				else
				{
					Error("Cant Load Clan data of player: " + sender.GetName() + " please report this to the Banking Dev Team!");
				}
			}
			else
			{
				Error("Error cant load Internal Playerdata");
			}
		}
	}

	void ClanUpdateMember(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(type == CallType.Server)
		{	
			Param2<ref PermissionObject, string> data;
			if(!ctx.Read(data)) return;
			KR_JsonDatabaseHandler targetPlayer = KR_JsonDatabaseHandler.LoadPlayerData(data.param2);
			KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId());
			if(targetPlayer && playerdata)
			{
				if(targetPlayer.GetClanID() != playerdata.GetClanID())
				{
					SendNotification("#AB_ThisPlayerIsNotInYourClan", sender, true);
					return;
				}

				ClanDataBaseManager clandata = ClanDataBaseManager.LoadClanData(playerdata.GetClanID());
				if(clandata)
				{
					PermissionObject sendersPermissions = GetMemberPermission(clandata, sender.GetPlainId());
					if(sendersPermissions)
					{
						if(!sendersPermissions.m_CanEdit)
						{
							SendNotification("#AB_NotEnoughtPermissions", sender, true);
							return;
						}

						if(data.param2 == clandata.GetOwnersID())
						{
							SendNotification("#AB_TryToModifyClanOwnersPermission", sender, true);
							return;
						}

						for(int i = 0; i < clandata.GetClanMembers().Count(); i++)
						{
							if(clandata.GetClanMembers().Get(i).GetPlainID() == data.param2)
							{
								clandata.GetClanMembers().Get(i).SetPermission(data.param1);
								clandata.SaveClanData(clandata);

								if(m_krserverconfig.m_DiscordWebhook.m_LogClanUpdatePermissionToDiscord)
									GetWebhookManager().PlayerLog(sender, " updated Permissions for member: " + data.param2 + " from clan: " + clandata.GetName());
								
								break;
							}
						}

						PlayerBase t_player = RemoteFindPlayer(data.param2);
						GetRPCManager().SendRPC("KR_BANKING", "ClanSyncRespose", new Param1< ref ClanDataBaseManager >( clandata ), true, sender);
						SendNotification("#AB_PermissionsUpdated " + data.param2, sender);

						if(m_krserverconfig.m_LoggingSettings.m_LogUpdatePermission)
							GetBankingLogManager().Log("Player: " + data.param2 + " PermissionsUpdated! from clan: " + clandata.GetName());
						

						if(!t_player) return;
						//Sync new clan data to both players!
						GetRPCManager().SendRPC("KR_BANKING", "ClanSyncRespose", new Param1< ref ClanDataBaseManager >( clandata ), true, t_player.GetIdentity());

						//Update the player data!
						GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param2< int, string >( targetPlayer.GetBankCredit(), targetPlayer.GetClanID() ), true, t_player.GetIdentity());
						SendNotification("#AB_PermissionsHasBeenChanged", t_player.GetIdentity());
					}
					else
					{
						Error("[Advanced Banking] -> Cant find Permissions from: " + sender.GetPlainId());
					}
				}
				else
				{
					Error("[Advanced Banking] -> Internal Clan data can not be loaded! Please report this to the Dev Team!");
				}
			}
			else
			{
				Error("[Advanced Banking] -> Error cant load Internal Playerdata");
			}
		}
	}

	void ClanMemberLeave(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(type == CallType.Server)
		{	
			Param1<string> data;
			if(!ctx.Read(data)) return;
			KR_JsonDatabaseHandler senderdata = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());
			if(!senderdata) return;
			ClanDataBaseManager clandata = ClanDataBaseManager.LoadClanData(senderdata.GetClanID());
			if(clandata)
			{
				if(clandata.GetOwnersID() == data.param1)
				{
					//Handle leader leave
						bool wasDeleted = RemoteHandleLeaderLeave(clandata, sender.GetPlainId());

						if(wasDeleted)
							SendNotification("#AB_ClanDeleted", sender);
						else
							SendNotification("#AB_SomethingWentWrong", sender);
				}
				else
				{
					//Handle member leave.
					bool memberLeaved = RemoteHandleMemberLeave(clandata, sender.GetPlainId());
					senderdata.SetClan("NONE");

					if(memberLeaved)
							SendNotification("#AB_SucesfullyLeavedClan " + clandata.GetName(), sender);
						else
							SendNotification("#AB_SomethingWentWrong", sender);
				}
				GetRPCManager().SendRPC("KR_BANKING","UIQuitRequest", null, true, sender);
			}
			else
			{
				Error("Cant load clandata from player: " + sender.GetPlainId());
			}
		}
	}

	void ClanUpdate(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(type == CallType.Server)
		{	
			Param2<string, string> data;
			if(!ctx.Read(data)) return;
			KR_JsonDatabaseHandler senderdata = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());
			if(!senderdata) return;
			ClanDataBaseManager clandata = ClanDataBaseManager.LoadClanData(senderdata.GetClanID());
			if(clandata)
			{
				if(sender.GetPlainId() == clandata.GetOwnersID())
				{
					clandata.SetName(data.param1);
					clandata.SetPrefix(data.param2);
					clandata.SaveClanData(clandata);
					SendNotification("#AB_ClanNameUpdated " + clandata.GetName(), sender);
					SendNotification("#AB_ClanTagUpdated " 	+ clandata.GetName(), sender);
				}
				else
				{
					SendNotification("#AB_NotEnoughtPermissions", sender, true);
				}
			}
			else
			{
				Error("Cant load clan data");
			}
		}
	}

    void WitdrawMoneyFromBankAccount(PlayerIdentity identity, int Amount)
    {
        KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());
        if(playerdata)
        {
            int CorrectAmountToWitdraw;
			if(Amount == 0)
				CorrectAmountToWitdraw = playerdata.GetBankCredit();
			else
				CorrectAmountToWitdraw = Amount;
			

            if(CorrectAmountToWitdraw > playerdata.GetBankCredit())
                CorrectAmountToWitdraw = playerdata.GetBankCredit();
			
            int sillNeeded = AddCurrencyToPlayer(RemoteFindPlayer(identity.GetPlainId()), CorrectAmountToWitdraw);
			int finalAmount = CorrectAmountToWitdraw - sillNeeded; 
			playerdata.WitdrawMoney(finalAmount);
            GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param2< int, string >( playerdata.GetBankCredit(), playerdata.GetClanID() ), true, identity);


			if(m_krserverconfig.m_DiscordWebhook.m_LogWithdrawToDiscord)
				GetWebhookManager().PlayerLog(identity, "Withdrawed " + finalAmount.ToString() + " ₽ from his own Account.");
				//GetWebhookManager().POST("Advanced Banking", "Player: " + identity.GetPlainId() + " withdrawed " + finalAmount + " from own account.");

			if(m_krserverconfig.m_LoggingSettings.m_LogWithdraw)
				GetBankingLogManager().Log("Player: " + identity.GetPlainId() + " withdrawed " + finalAmount + " from own account.");
        }
    }

	void WitdrawMoneyFromClanBankAccount(PlayerIdentity identity, int Amount)
	{
		KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());
        if(playerdata)
        {
			ClanDataBaseManager clanDB = ClanDataBaseManager.LoadClanData(playerdata.GetClanID());
			if(clanDB)
			{
				if(Amount == 0)
					Amount = clanDB.GetBankCredit();
				
 				if(Amount <= clanDB.GetBankCredit())
				{
					ref ClanMemberObject user = clanDB.GetMemberByPlainId(identity.GetPlainId());
					if(!user || !user.GetPermission().m_CanWithdraw)
					{
						SendNotification("#AB_NotEnoughtPermissions", identity, true);
						return;
					}
					int stillNeeded = AddCurrencyToPlayer(RemoteFindPlayer(identity.GetPlainId()), Amount);
					int FinallyAmount = Amount - stillNeeded;
					clanDB.WitdrawMoney(FinallyAmount);
					clanDB.WriteLog(identity.GetName() + " withdrawed: " + FinallyAmount);
					GetRPCManager().SendRPC("KR_BANKING", "ClanSyncRespose", new Param1< ref ClanDataBaseManager >( clanDB ), true, identity);

					if(m_krserverconfig.m_DiscordWebhook.m_LogClanWithdrawToDiscord)
						GetWebhookManager().PlayerLog(identity, " Withdrawed " +  FinallyAmount.ToString() + " ₽ from Clan Account with id: " + clanDB.GetClanID());
						//GetWebhookManager().POST("Advanced Banking", "Player: " + identity.GetPlainId() + " withdrawed " + FinallyAmount + " from clan account.");

					if(m_krserverconfig.m_LoggingSettings.m_LogClanWithdraw)
						GetBankingLogManager().Log("Player: " + identity.GetPlainId() + " withdrawed " + FinallyAmount + " from clan account.");
				}
				else
				{
					SendNotification("#AB_NotEnoughtMoneyOnAccount", identity, true);
				}
			}
			else
			{
				Error("Cant Load Clan Data of Player: " + identity.GetName());
			}
        }
		else
		{
			Error("Cant Load Player Data of Player: " + identity.GetName());
		}
	}

    void DepositMoneyOnOwnBank(PlayerIdentity identity, int Amount)
    {
        KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());
        if(playerdata)
        {
			int MaxPlaceAbleAmount = GetMaxPlaceAbleAmmountForBank(playerdata);
			int SumToInsert = Amount;
			PlayerBase player = RemoteFindPlayer(identity.GetPlainId());
			int CurrencyOnPlayer = GetPlayerCurrencyAmount(player);
			if(!player) return;

			if(SumToInsert <= 0)
				SumToInsert = CurrencyOnPlayer;
			
			if(SumToInsert > MaxPlaceAbleAmount)
				SumToInsert = MaxPlaceAbleAmount;
			
			if(CurrencyOnPlayer >= SumToInsert)
			{
				playerdata.DepositMoney(SumToInsert);
				RemoveCurrencyFromPlayer(RemoteFindPlayer(identity.GetPlainId()), SumToInsert);
				GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", new Param2< int, string >( playerdata.GetBankCredit(), playerdata.GetClanID() ), true, identity);

				if(m_krserverconfig.m_DiscordWebhook.m_LogDepositToDiscord)
					GetWebhookManager().PlayerLog(identity, "Deposited " + SumToInsert.ToString() + " ₽ to his own Account.");
				
				if(m_krserverconfig.m_LoggingSettings.m_LogDepositOwnBank)
					GetBankingLogManager().Log("Player: " + identity.GetPlainId() + " Deposited " + SumToInsert + " on own account.");
			}
			else
			{
				SendNotification("#AB_NotEnoughtMoneyInInventory", identity, true);
			}
        }
		else
		{
			Error("Cant Load playerdata of player: " + identity.GetName() + " please report this to the dev team! @KR_Banking.");
		}
    }

	void DepositMoneyOnClanBank(PlayerIdentity identity, int Ammount)
    {
        KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());
        if(playerdata)
        {
			ClanDataBaseManager clanDB = ClanDataBaseManager.LoadClanData(playerdata.GetClanID());
			if(clanDB)
			{
				ref ClanMemberObject user = clanDB.GetMemberByPlainId(identity.GetPlainId());
				if(!user || !user.GetPermission().m_CanDeposit)
				{
					SendNotification("#AB_NotEnoughtPermissions", identity, true);
					return;
				}

				PlayerBase player = RemoteFindPlayer(identity.GetPlainId());
				if(!player) return;
				int MaxPlaceAbleAmount = GetMaxPlaceAbleAmmountForClanBank(clanDB);
				int SumToInsert = Ammount;
				int CurrencyOnPlayer = GetPlayerCurrencyAmount(player);
				
				if(SumToInsert == 0)
					SumToInsert = CurrencyOnPlayer;
				
				if(SumToInsert > MaxPlaceAbleAmount)
					SumToInsert = MaxPlaceAbleAmount;
				
				if( SumToInsert <= MaxPlaceAbleAmount)
				{
					if(CurrencyOnPlayer >= SumToInsert)
					{
						clanDB.DepositMoney(SumToInsert);
						clanDB.WriteLog(identity.GetName() + " deposited: " + SumToInsert);
						RemoveCurrencyFromPlayer(player, SumToInsert);
						GetRPCManager().SendRPC("KR_BANKING", "ClanSyncRespose", new Param1< ref ClanDataBaseManager >( clanDB ), true, identity);
						if(m_krserverconfig.m_DiscordWebhook.m_LogClanDepositToDiscord)
							GetWebhookManager().PlayerLog(identity, " Deposited " +  SumToInsert.ToString() + " ₽ to Clan Account with id: " + clanDB.GetClanID());
							//GetWebhookManager().POST("Advanced Banking", "Player: " + identity.GetPlainId() + " deposited " + SumToInsert + " on clan account.");

						if(m_krserverconfig.m_LoggingSettings.m_LogClanDeposit)
							GetBankingLogManager().Log("Player: " + identity.GetPlainId() + " deposited " + SumToInsert + " on clan account.");
					}
					else
					{
						SendNotification("#AB_NotEnoughtMoneyInInventory", identity, true);
					}
				}
				else
				{
					SendNotification("#AB_ClanMaxPlayerLimitReached", identity, true);
				}
			}
			else
			{
				Error("Cant Load Clan Data of Player: " + identity.GetName());
			}
        }
		else
		{
			Error("Cant Load Player Data of Player: " + identity.GetName());
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

	/* ======== CLAN SECTION ===========*/
	//!Creates a new clan & returns the clan after creating (Important: will return null if creation failed.)
	ref  ClanDataBaseManager RegisterNewClan(string ClanName, string ClanOwnersID)
	{
		//string ClanName, string ClanID, string ownersPlainID
		string ClanID = GenerateRandomClanID();
		ClanDataBaseManager clanDB = ClanDataBaseManager.CreateClan(ClanName, ClanID, ClanOwnersID);

		/*
		if(clanDB)
		{
			//string PlainID, string PlayerName, Permission
			PermissionObject perms = new PermissionObject();
			perms.GiveClanOwner();
			clanDB.AddMember("76561198796326626", "DevKrypton",perms);
		}
		*/
		return clanDB;
	}

	void AddClanMember(ref ClanDataBaseManager clan, ref PermissionObject permissions, string SteamID, string MemberName)
	{
		if(!clan || !permissions) return;

		if(m_krserverconfig.m_LoggingSettings.m_LogInviteMember)
			GetBankingLogManager().Log("Player: " + SteamID + " joined the Clan Banking Account: " + clan.GetName());
		
		clan.AddMember(SteamID, MemberName, permissions);
	}

	bool RemoteHandleLeaderLeave(ref ClanDataBaseManager clan, string LeaderID)
	{
		if(!clan || !LeaderID) return false;

		if(clan.GetOwnersID() == LeaderID)
		{
			//100 % sure its the leader.

			//Loop all Members & set clan id to NONE
			for(int i = 0; i < clan.GetClanMembers().Count(); i++)
			{
				string targetsPlainID = clan.GetClanMembers().Get(i).GetPlainID();
				KR_JsonDatabaseHandler currentClanMembersData = KR_JsonDatabaseHandler.LoadPlayerData(clan.GetClanMembers().Get(i).GetPlainID());
				if(currentClanMembersData)
				{
					currentClanMembersData.SetClan("NONE");
				}
				else
				{
					Error("Cant clean ClanID from: " + targetsPlainID);
				}
			}
			if(m_krserverconfig.m_LoggingSettings.m_LogDeleteClanAccount)
				GetBankingLogManager().Log("Player: " + LeaderID + " deleted clan: " + clan.GetName());
			
			return clan.DeleteClan();
		}
		return false;
	}

	//!returns true if remove process is done!
	bool RemoteHandleMemberLeave(ref ClanDataBaseManager clan, string MemberID)
	{
		if(!clan || !MemberID) return false;

		for(int i = 0; i < clan.GetClanMembers().Count(); i++)
		{
			if(clan.GetClanMembers().Get(i).GetPlainID() == MemberID)
			{
				//found
				clan.RemoveMember(i);
				return true;
			}
		}
		return false;
	}

	/*!Returns the permission from an Member @Params
		@PARAM 1 (ClanDataBaseManager ref clandata)
		@PARAM 2 (Plain ID Member to search)
	*/	
	ref PermissionObject GetMemberPermission(ref ClanDataBaseManager clan, string MemberID)
	{
		for(int i = 0; i < clan.GetClanMembers().Count(); i++)
		{
			if(clan.GetClanMembers().Get(i).GetPlainID() == MemberID)
			{
				return clan.GetClanMembers().Get(i).GetPermission();
			}
		}
		return null;
	}

	//!Generates a random ID to store & Load clan data!
	string GenerateRandomClanID()
	{
		string rndf = "";
		for(int i = 0; i < 16; i++)
			rndf += m_chars.GetRandomElement();
		return rndf;
	}

	int GetMaxPlaceAbleAmmountForClanBank(ClanDataBaseManager clandata)
	{
		int currentBankAmount = clandata.GetBankCredit();
		int maxConfigAmount = m_krserverconfig.MaxClanAccountLimit;

		int sum = maxConfigAmount - currentBankAmount;
		
		return sum;
	}

    int AddCurrencyToPlayer(PlayerBase player, int amountToAdd)
	{
		if(!player || amountToAdd <= 0)
			return 0;

		int BKamountStillNeeded = amountToAdd;
		int BKquantityNeeded;
		int BKquantityLeft;
		
		for(int i = 0; i < GetKR_BankingServerConfig().BankingCurrency.Count(); i++)
		{
			BKquantityNeeded = BKamountStillNeeded / GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue;
			if(BKquantityNeeded > 0)
			{
				BKquantityLeft = AddCurrencyToInventory(player, i, BKquantityNeeded);
				BKamountStillNeeded -= (BKquantityNeeded - BKquantityLeft) * GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue;
				
				if(BKamountStillNeeded == 0)
				{
					return 0;
				}
			}
		}
		return BKamountStillNeeded;
	}

    int RemoveCurrencyFromPlayer(PlayerBase player, int amountToRemove)
	{

        if(!player || amountToRemove <= 0)
			return 0;

		ItemBase item;
		int BKquantityNeeded;
		array<EntityAI> invItems = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, invItems);

		for(int i = GetKR_BankingServerConfig().BankingCurrency.Count() - 1; i >= 0; i--)
		{
			foreach(EntityAI invitem : invItems)
			{
				if(GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyName == invitem.GetType())
				{
					if(Class.CastTo(item, invitem))
					{
						BKquantityNeeded = amountToRemove / GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue;
						if(GetItemQuantityMax(item) == 0)
						{
							GetGame().ObjectDelete(item);
							if(BKquantityNeeded >= 1)
							{
								amountToRemove -= GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue;
							}
							else
							{
								return AddCurrencyToPlayer(player, GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue - amountToRemove);
							}
						}
						else
						{
							if(BKquantityNeeded >= GetItemQuantity(item))
							{
								amountToRemove -= GetItemQuantity(item) * GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue;
								GetGame().ObjectDelete(item);
							}
							else
							{
								SetItemQuantity(item, GetItemQuantity(item) - BKquantityNeeded);
								amountToRemove -= BKquantityNeeded * GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue;
								
								if(amountToRemove < GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue)
								{
									if(GetItemQuantity(item) == 1)
									{
										GetGame().ObjectDelete(item);
									}
									else
									{
										SetItemQuantity(item, GetItemQuantity(item) - 1);
									}
									return AddCurrencyToPlayer(player, GetKR_BankingServerConfig().BankingCurrency.Get(i).CurrencyValue - amountToRemove);
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
		if(!player || ammountToAdd <= 0)
			return 0;
		
		local int BKquantityLeft = ammountToAdd;
		local int BKaddableQuantity;
		local int BKcurrencyItemMaxQuantity;
		InventoryLocation il = new InventoryLocation();
		ref CurrencySettings currency = GetKR_BankingServerConfig().BankingCurrency.Get(CurrencyArrayIndex);
		array<EntityAI> invArray = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, invArray);
		ItemBase currencyItem;

		foreach(EntityAI item : invArray)
		{
			if(item.GetType() == currency.CurrencyName)
			{
				if(Class.CastTo(currencyItem, item))
				{
					BKaddableQuantity = GetItemQuantityMax(currencyItem) - GetItemQuantity(currencyItem);
					if(BKaddableQuantity > 0)
					{
						if(BKaddableQuantity < BKquantityLeft)
						{
							SetItemQuantity(currencyItem, GetItemQuantityMax(currencyItem));
							BKquantityLeft -= BKaddableQuantity;
						}
						else
						{
							SetItemQuantity(currencyItem, GetItemQuantity(currencyItem) + BKquantityLeft);
							BKquantityLeft = 0;
						}
					}
					
					if(BKquantityLeft == 0)
					{
						return 0;
					}
				}
			}
		}
		
		while(player.GetInventory().FindFirstFreeLocationForNewEntity(currency.CurrencyName, FindInventoryLocationType.CARGO, il))
		{
			if(Class.CastTo(currencyItem, player.GetHumanInventory().CreateInInventory(currency.CurrencyName)))
            {
				BKcurrencyItemMaxQuantity = GetItemQuantityMax(currencyItem);
				if(BKcurrencyItemMaxQuantity == 0)
				{
					SetItemQuantity(currencyItem, 0);
					BKquantityLeft -= 1;
				}
				else
				{
					if(BKquantityLeft <= BKcurrencyItemMaxQuantity)
					{
						SetItemQuantity(currencyItem, BKquantityLeft);
						BKquantityLeft = 0;
					}
					else
					{
						SetItemQuantity(currencyItem, BKcurrencyItemMaxQuantity);
						BKquantityLeft -= BKcurrencyItemMaxQuantity;
					}
				}
				
				if(BKquantityLeft == 0)
					return 0;
			}
			else
			{
				break;
			}
		}
		
		if(!player.GetHumanInventory().GetEntityInHands())
		{
			if(Class.CastTo(currencyItem, player.GetHumanInventory().CreateInInventory(currency.CurrencyName)))
			{
				BKcurrencyItemMaxQuantity = GetItemQuantityMax(currencyItem);
				if(BKcurrencyItemMaxQuantity == 0)
				{
					SetItemQuantity(currencyItem, 0);
					BKquantityLeft -= 1;
				}
				else
				{
					if(BKquantityLeft <= BKcurrencyItemMaxQuantity)
					{
						SetItemQuantity(currencyItem, BKquantityLeft);
						BKquantityLeft = 0;
					}
					else
					{
						SetItemQuantity(currencyItem, BKcurrencyItemMaxQuantity);
						BKquantityLeft -= BKcurrencyItemMaxQuantity;
					}
				}
				
				if(BKquantityLeft == 0)
					return 0;
			}
		}
		return BKquantityLeft;
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

    //!retrurns PlayerBase with steam64ID if player is connected to server! Duplicate of Garagemod now useable in SDK!
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

	//!returns players bank amount!
    int GetPlayersBankAmount(PlayerIdentity identity)
    {
        KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());
        if(playerdata)
        {
           return playerdata.GetBankCredit();
        }
        return -1;
    }

	//!Returns the currency Ammount in Players Invenory
    int GetPlayerCurrencyAmount(PlayerBase player)
	{
		int currencyInInventory = 0;
		
		array<EntityAI> itemsArray = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

		ItemBase currencyItem;
		foreach(EntityAI item : itemsArray)
		{
			if(Class.CastTo(currencyItem, item))
			{
				for (int i = 0; i < m_krserverconfig.BankingCurrency.Count(); i++)
				{
					if(currencyItem.GetType() == m_krserverconfig.BankingCurrency.Get(i).CurrencyName)
					{
						currencyInInventory += GetItemAmount(currencyItem) * m_krserverconfig.BankingCurrency.Get(i).CurrencyValue;
					}
				}
			}
		}
		return currencyInInventory;
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

	void SendNotification(string Message, PlayerIdentity Identity, bool IsError = false)
	{
		#ifdef NOTIFICATIONS
			if(IsError)
				NotificationSystem.SimpleNoticiation(" " + Message, "Banking", "KR_Banking/data/Logos/notificationbanking.edds", ARGB(240, 255, 0, 0), 5, Identity);
			else
			NotificationSystem.SimpleNoticiation(" " + Message, "Banking", "KR_Banking/data/Logos/notificationbanking.edds", ARGB(240, 255, 13, 55), 5, Identity);
		#else
			if(IsError)
				NotificationSystem.SendNotificationToPlayerExtended(RemoteFindPlayer(Identity.GetPlainId()), 5, "Banking ERROR", Message, "KR_Banking/data/Logos/notificationbanking.edds");
			else
				NotificationSystem.SendNotificationToPlayerExtended(RemoteFindPlayer(Identity.GetPlainId()), 5, "Banking", Message, "KR_Banking/data/Logos/notificationbanking.edds");
		#endif


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
			KR_BankingATM AdvATMS;
			if(Class.CastTo(AdvATMS, SpawnedATM))
			{
				if(m_krserverconfig.ATM.Get(i).GetCanRobbed())
					AdvATMS.m_Banking_CanBeRobbed = true;
			}
            Print("[Advanced Banking] -> Sucesfully spawned ATM: " + ObjectName + " on: " + tempPos);
        }
    }

	bool IsNegativ(int number)
	{
		if (number < 0)
			return true;
		return false;
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
};