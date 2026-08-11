class PluginKRBankingManagerServer extends PluginBase
{
    protected ref KR_BankingConfigManager m_krserverconfig;
    protected ref array<ref bankingplayerlistobj> m_BankingPlayers;
    protected ref TStringArray m_chars;

    void PluginKRBankingManagerServer()
    {
        if (!GetGame() || !GetGame().IsServer() || !GetGame().IsMultiplayer())
            return;

        m_krserverconfig = GetKR_BankingServerConfig();
        if (!m_krserverconfig)
        {
            Error("[Advanced Banking] -> Server config could not be loaded.");
            return;
        }

        Init();
    }

    void Init()
    {
        m_BankingPlayers = new array<ref bankingplayerlistobj>;
        m_chars = {
            "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
            "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
            "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
            "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
            "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
        };

        RegisterServersideRPCs();
        SpawnATMs();
        InitPayCheck();

        #ifdef Trader
        Print("[Advanced Banking] -> Trader is running on the server.");
        #endif
    }

    protected void RegisterServersideRPCs()
    {
        GetRPCManager().AddRPC("KR_BANKING", "PlayerDataRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "ServerConfigRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "WithdrawRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "DepositRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "PlayerListRequst", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "TransferRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "ClanCreateRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "ClanSyncRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "ClanAddMember", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "ClanRemoveMember", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "ClanUpdateMember", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "ClanMemberLeave", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "ClanUpdate", this, SingleplayerExecutionType.Server);
    }

    // ---------------------------------------------------------------------
    // Common validation and synchronization
    // ---------------------------------------------------------------------

    protected bool IsValidServerRequest(CallType type, PlayerIdentity sender)
    {
        if (type != CallType.Server)
            return false;

        if (!sender)
            return false;

        return true;
    }

    protected bool HasClanID(string clanID)
    {
        return clanID != "" && clanID != "NONE";
    }

    protected bool IsClanNameAndTagValid(string clanName, string clanTag)
    {
        if (clanName == "" || clanTag == "")
            return false;

        if (clanName.Length() > 32 || clanTag.Length() > 8)
            return false;

        return true;
    }

    protected void SendClanDataToClient(ClanDataBaseManager clanData, PlayerIdentity identity)
    {
        if (!clanData || !identity)
            return;

        Param1<ClanDataBaseManager> clanResponse;
        clanResponse = new Param1<ClanDataBaseManager>(clanData);

        GetRPCManager().SendRPC("KR_BANKING", "ClanSyncRespose", clanResponse, true, identity);
    }

    protected void CloseBankingUI(PlayerIdentity identity)
    {
        if (!identity)
            return;

        GetRPCManager().SendRPC("KR_BANKING", "UIQuitRequest", null, true, identity);
    }

    // Reloading the data before sending is intentional. It integrates the
    // resync behavior from NHFAB_BankManagerFix.c.
    void ResyncPlayerBank(PlayerIdentity identity)
    {
        if (!identity)
            return;

        KR_JsonDatabaseHandler playerData = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());

        if (!playerData)
            return;

        Param3<int, string, int> response = new Param3<int, string, int>( playerData.GetBankCredit(), playerData.GetClanID(), playerData.GetBonusAmount());

        GetRPCManager().SendRPC("KR_BANKING", "PlayerDataResponse", response, true, identity);
    }

    // ---------------------------------------------------------------------
    // Paycheck
    // ---------------------------------------------------------------------

    protected void InitPayCheck()
    {
        if (!m_krserverconfig)
            return;

        int tickTime = m_krserverconfig.GetCorrectPayCheckTime();
        if (tickTime > 0 && m_krserverconfig.PayCheckTickTime != -1)
        {
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.TickPayCheck, tickTime, true);
        }
    }

    void TickPayCheck()
    {
        if (!m_krserverconfig)
            return;

        array<Man> onlinePlayers = new array<Man>;
        GetGame().GetPlayers(onlinePlayers);

        if (onlinePlayers.Count() < m_krserverconfig.MinPlayersForPayCheck)
            return;

        foreach (Man man : onlinePlayers)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (!player)
                continue;

            PlayerIdentity identity = player.GetIdentity();
            if (!identity)
                continue;

            #ifdef Trader
            if (player.IsInSafeZone() && !m_krserverconfig.CanAddPayCheckInSafezone)
            {
                Print("[Advanced Banking] -> Paycheck skipped for player in safe zone: " + identity.GetPlainId());
                continue;
            }
            #endif

            KR_JsonDatabaseHandler playerData = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());

            if (!playerData)
                continue;

            int paycheckAmount = m_krserverconfig.PayCheckValue;
            if (paycheckAmount <= 0)
                continue;

            int paycheckBonus = playerData.GetPayCheckBonus();
            if (paycheckBonus != 0)
            {
                paycheckAmount += (m_krserverconfig.PayCheckValue * paycheckBonus) / 100;
            }

            if (paycheckAmount <= 0)
                continue;

            if (!m_krserverconfig.CanAddToFullAcc)
            {
                int maxPlaceableAmount = GetMaxPlaceAbleAmmountForBank(playerData);
                if (paycheckAmount > maxPlaceableAmount)
                {
                    if (m_krserverconfig.IsPayCheckMessageActive)
                        SendNotification("#AB_BankAlreadyFullPayCheckError", identity, true);

                    continue;
                }
            }

            playerData.DepositMoney(paycheckAmount);
            ResyncPlayerBank(identity);

            if (m_krserverconfig.IsPayCheckMessageActive)
                SendNotification(ReplacedPlaceHolderWithValue(m_krserverconfig.PayCheckMessage), identity);
        }
    }

    string ReplacedPlaceHolderWithValue(string placeholder)
    {
        if (placeholder.Contains("%Amount%"))
            placeholder.Replace("%Amount%", m_krserverconfig.PayCheckValue.ToString());

        return placeholder;
    }

    // ---------------------------------------------------------------------
    // General RPCs
    // ---------------------------------------------------------------------

    void PlayerDataRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        ResyncPlayerBank(sender);
    }

    void ServerConfigRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        SendConfigToClient(sender);
    }

    void InternalConfigReload()
    {
        KR_BankingConfigManager reloadedConfig = GetKR_BankingServerConfig();
        if (reloadedConfig)
            m_krserverconfig = reloadedConfig;
    }

    void SendConfigToClient(PlayerIdentity identity)
    {
        if (!identity || !m_krserverconfig)
            return;

		//ewwww
        KR_BankingClientConfig clientSettings = new KR_BankingClientConfig(m_krserverconfig.maxCurrency, m_krserverconfig.MenuDelay, m_krserverconfig.IsRobEventActive, m_krserverconfig.NeedsBankCardToOpenMenu, m_krserverconfig.BankingCurrency, m_krserverconfig.CostsToCreateClan, m_krserverconfig.MaxClanAccountLimit, m_krserverconfig.IsClanAccountActive);

        clientSettings.TimeInSecToRobATM = m_krserverconfig.TimeInSecToRobATM;

        Param2<KR_BankingClientConfig, string> configResponse;
        configResponse = new Param2<KR_BankingClientConfig, string>(clientSettings, identity.GetPlainId());

        GetRPCManager().SendRPC("KR_BANKING", "ServerConfigResponse", configResponse, true, identity);
    }

    void DepositRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param2<int, int> data;
        if (!ctx.Read(data))
            return;

        if (data.param1 < 0)
            return;

        if (data.param2 == BankType.OWNBANK)
            DepositMoneyOnOwnBank(sender, data.param1);
        else
            DepositMoneyOnClanBank(sender, data.param1);
    }

    void WithdrawRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param2<int, int> data;
        if (!ctx.Read(data))
            return;

        if (data.param1 < 0)
            return;

        if (data.param2 == BankType.OWNBANK)
            WithdrawMoneyFromBankAccount(sender, data.param1);
        else
            WithdrawMoneyFromClanBankAccount(sender, data.param1);
    }

    void TransferRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param2<string, int> data;
        if (!ctx.Read(data))
            return;

        string targetPlainID = data.param1;
        int transferAmount = data.param2;

        if (targetPlainID == "" || transferAmount <= 0)
        {
            SendNotification("#AB_PlayerNotAvaibleForTransfers", sender, true);
            return;
        }

        if (transferAmount < m_krserverconfig.MinAmountToTransfer)
        {
            SendNotification("#AB_ErrorMinAmountNotReachedTransfer " + m_krserverconfig.MinAmountToTransfer, sender, true);
            return;
        }

        if (sender.GetPlainId() == targetPlainID)
        {
            SendNotification("#AB_PlayerNotAvaibleForTransfers", sender, true);
            return;
        }

        PlayerBase targetPlayer = RemoteFindPlayer(targetPlainID);
        if (!targetPlayer || !targetPlayer.GetIdentity())
        {
            SendNotification("#AB_PlayerNotAvaibleForTransfers", sender, true);
            return;
        }

        float transferAmountAsFloat = transferAmount;
        int currentFee = Math.Floor((transferAmountAsFloat / 100.0) * m_krserverconfig.TransferfeesInProcent);

        if (currentFee < 0)
            currentFee = 0;

        int amountToTransfer = transferAmount - currentFee;
        if (amountToTransfer <= 0)
        {
            SendNotification("#AB_PlayerNotAvaibleForTransfers", sender, true);
            return;
        }

        KR_JsonDatabaseHandler senderData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());

        if (!senderData)
            return;

        if (senderData.GetBankCredit() < transferAmount)
        {
            SendNotification("You dont have enough money to do this!", sender, true);
            return;
        }

        PlayerIdentity targetIdentity = targetPlayer.GetIdentity();
        KR_JsonDatabaseHandler targetData = KR_JsonDatabaseHandler.LoadPlayerData(targetIdentity.GetPlainId(), targetIdentity.GetName());

        if (!targetData)
        {
            SendNotification("#AB_PlayerHasNoBankAccount", sender, true);
            return;
        }

        int targetMaxPlaceableAmount = GetMaxPlaceAbleAmmountForBank(targetData);
        if (amountToTransfer > targetMaxPlaceableAmount)
        {
            SendNotification("#AB_PlayerNotAvaibleForTransfers", sender, true);
            return;
        }

        senderData.WithdrawMoney(transferAmount);
        targetData.DepositMoney(amountToTransfer);

        SendNotification("#AB_TransferSucess " + transferAmount + " #AB_ToPlayer " + targetIdentity.GetName(), sender);
        SendNotification("#AB_TransferRecived " + amountToTransfer + " #AB_FromPlayer " + sender.GetName(), targetIdentity);

        ResyncPlayerBank(sender);
        ResyncPlayerBank(targetIdentity);

        if (m_krserverconfig.m_DiscordWebhook.m_LogPlayerMoneyTransefer)
        {
            GetWebhookManager().PlayerLog(sender, " transfered " + transferAmount + " to reciver: " + targetIdentity.GetName() + "<" + targetIdentity.GetPlainId() + ">");
        }
    }

    void PlayerListRequst(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        UpdatePlayerList();
        Param1<array<ref bankingplayerlistobj>> playerListResponse;
        playerListResponse = new Param1<array<ref bankingplayerlistobj>>(m_BankingPlayers);

        GetRPCManager().SendRPC("KR_BANKING", "PlayeristResponse", playerListResponse, true, sender);
    }

    // ---------------------------------------------------------------------
    // Clan RPCs
    // ---------------------------------------------------------------------

    void ClanCreateRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param2<string, string> data;
        if (!ctx.Read(data))
            return;

        string clanName = data.param1;
        string clanTag = data.param2;

        if (!m_krserverconfig.IsClanAccountActive)
        {
            SendNotification("#AB_ClanTagOrNameIsNotValid", sender, true);
            return;
        }

        if (!IsClanNameAndTagValid(clanName, clanTag))
        {
            SendNotification("#AB_ClanTagOrNameIsNotValid", sender, true);
            return;
        }

        KR_JsonDatabaseHandler playerData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());

        if (!playerData)
            return;

        if (HasClanID(playerData.GetClanID()))
        {
            ClanDataBaseManager currentClan = ClanDataBaseManager.LoadClanData(playerData.GetClanID());
            if (currentClan)
            {
                SendNotification("#AB_ErrorPlayerHasAlreadyAnClan", sender, true);
                return;
            }

            playerData.SetClan("NONE");
        }

        PlayerBase player = RemoteFindPlayer(sender.GetPlainId());
        if (!player)
            return;

        int currencyOnPlayer = GetPlayerCurrencyAmount(player);
        if (currencyOnPlayer < m_krserverconfig.CostsToCreateClan)
        {
            SendNotification("#AB_YouDontHave " + m_krserverconfig.CostsToCreateClan + " #AB_InYourInventory", sender, true);
            return;
        }

        ClanDataBaseManager usersNewClan = RegisterNewClan(clanName, sender.GetPlainId());
        if (!usersNewClan)
        {
            Error("[Advanced Banking] -> Failed to create clan.");
            return;
        }

        PermissionObject permissions = new PermissionObject();
        permissions.GiveClanOwner();

        usersNewClan.SetPrefix(clanTag);
        AddClanMember(usersNewClan, permissions, sender.GetPlainId(), sender.GetName());

        string clanID = usersNewClan.GetClanID();
        if (clanID == "")
        {
            usersNewClan.DeleteClan();
            Error("[Advanced Banking] -> Created clan returned an empty clan ID.");
            return;
        }

        RemoveCurrencyFromPlayer(player, m_krserverconfig.CostsToCreateClan);
        playerData.SetClan(clanID);

        CloseBankingUI(sender);
        ResyncPlayerBank(sender);
        SendClanDataToClient(usersNewClan, sender);
        SendNotification("#AB_SucesfullyCreatedClan " + usersNewClan.GetName(), sender);

        if (m_krserverconfig.m_LoggingSettings.m_LogCreateClanAccount)
        {
            GetBankingLogManager().Log("Player: " + sender.GetPlainId() + " Created Clan: " + usersNewClan.GetName());
        }

        if (m_krserverconfig.m_DiscordWebhook.m_LogClanCreateToDiscord)
        {
            GetWebhookManager().PlayerLog(sender, " Created an clan account with id: " + usersNewClan.GetClanID());
        }
    }

    void ClanSyncRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        KR_JsonDatabaseHandler playerData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());

        if (!playerData)
            return;

        string clanID = playerData.GetClanID();
        if (!HasClanID(clanID))
            return;

        ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(clanID);
        if (clanData)
        {
            SendClanDataToClient(clanData, sender);
            return;
        }

        playerData.SetClan("NONE");
        ResyncPlayerBank(sender);
        CloseBankingUI(sender);
        SendNotification("Your clan no longer exists. You have been removed from it.", sender, true);
    }

    void ClanAddMember(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param1<string> data;
        if (!ctx.Read(data))
            return;

        string targetPlainID = data.param1;
        if (targetPlainID == "" || targetPlainID == sender.GetPlainId())
        {
            SendNotification("#AB_ErrorPlayerHasAlreadyAnClan", sender, true);
            return;
        }

        PlayerBase targetPlayer = RemoteFindPlayer(targetPlainID);
        if (!targetPlayer || !targetPlayer.GetIdentity())
            return;

        PlayerIdentity targetIdentity = targetPlayer.GetIdentity();
        KR_JsonDatabaseHandler targetData = KR_JsonDatabaseHandler.LoadPlayerData(targetIdentity.GetPlainId(), targetIdentity.GetName());
        KR_JsonDatabaseHandler senderData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());

        if (!targetData || !senderData)
        {
            Error("[Advanced Banking] -> Could not load internal player data for clan invite.");
            return;
        }

        if (HasClanID(targetData.GetClanID()))
        {
            SendNotification("#AB_ErrorPlayerHasAlreadyAnClan", sender, true);
            return;
        }

        if (!HasClanID(senderData.GetClanID()))
        {
            SendNotification("#AB_NotEnoughtPermissions", sender, true);
            return;
        }

        ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(senderData.GetClanID());
        if (!clanData)
        {
            Error("[Advanced Banking] -> Cannot load clan data with ID: " + senderData.GetClanID());
            return;
        }

        ClanMemberObject senderMember = clanData.GetMemberByPlainId(sender.GetPlainId());
        if (!senderMember || !senderMember.GetPermission() || !senderMember.GetPermission().m_CanInvite)
        {
            SendNotification("#AB_NotEnoughtPermissions", sender, true);
            return;
        }

        if (m_krserverconfig.MaxPlayersInClan != -1 && clanData.GetMemberCount() >= m_krserverconfig.MaxPlayersInClan)
        {
            SendNotification("#AB_ClanMaxPlayerLimitReached", sender, true);
            return;
        }

        PlayerBase senderPlayer = RemoteFindPlayer(sender.GetPlainId());
        if (!senderPlayer)
            return;

        int currencyOnPlayer = GetPlayerCurrencyAmount(senderPlayer);
        if (currencyOnPlayer < m_krserverconfig.CostsToInviteAnPlayer)
        {
            SendNotification("#AB_YouDontHave: " + m_krserverconfig.CostsToInviteAnPlayer.ToString() + " #AB_InYourInventory", sender, true);
            return;
        }

        RemoveCurrencyFromPlayer(senderPlayer, m_krserverconfig.CostsToInviteAnPlayer);
        targetData.SetClan(clanData.GetClanID());

        PermissionObject permissions = new PermissionObject();
        AddClanMember(clanData, permissions, targetPlainID, targetData.GetName());

        Print("[Advanced Banking] -> Player: " + sender.GetPlainId() + " invited player: " + targetPlainID + " to clan: " + clanData.GetClanID());
        SendClanDataToClient(clanData, sender);
        SendClanDataToClient(clanData, targetIdentity);
        ResyncPlayerBank(targetIdentity);

        SendNotification("#AB_YouGotAnInviteFromClan " + clanData.GetName() + " #AB_FromPlayer " + sender.GetName(), targetIdentity);
    }

    void ClanRemoveMember(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param1<string> data;
        if (!ctx.Read(data))
            return;

        string targetPlainID = data.param1;
        if (targetPlainID == "")
            return;

        if (sender.GetPlainId() == targetPlainID)
        {
            SendNotification("#AB_CanNotKickSelf", sender, true);
            return;
        }

        KR_JsonDatabaseHandler targetData = KR_JsonDatabaseHandler.LoadPlayerData(targetPlainID, "", true);
        KR_JsonDatabaseHandler senderData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());

        if (!targetData || !senderData)
            return;

        if (!HasClanID(senderData.GetClanID()) || targetData.GetClanID() != senderData.GetClanID())
        {
            SendNotification("#AB_ThisPlayerIsNotInYourClan", sender, true);
            return;
        }

        ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(senderData.GetClanID());
        if (!clanData)
            return;

        ClanMemberObject senderMember = clanData.GetMemberByPlainId(sender.GetPlainId());
        if (!senderMember || !senderMember.GetPermission() || !senderMember.GetPermission().m_CanKick)
        {
            SendNotification("#AB_NotEnoughtPermissions", sender, true);
            return;
        }

        if (targetPlainID == clanData.GetOwnersID())
        {
            SendNotification("#AB_CanNotKickOwner", sender, true);
            return;
        }

        bool memberFound = false;
        for (int i = 0; i < clanData.GetClanMembers().Count(); i++)
        {
            ClanMemberObject member = clanData.GetClanMembers().Get(i);
            if (member && member.GetPlainID() == targetPlainID)
            {
                clanData.RemoveMember(i);
                memberFound = true;
                break;
            }
        }

        if (!memberFound)
        {
            SendNotification("#AB_ThisPlayerIsNotInYourClan", sender, true);
            return;
        }

        targetData.SetClan("NONE");
        SendClanDataToClient(clanData, sender);
        Print("[Advanced Banking] -> Player: " + sender.GetPlainId() + " kicked player: " + targetPlainID + " from clan: " + clanData.GetClanID());
        SendNotification("#AB_SucessKicked " + targetPlainID, sender);

        PlayerBase targetPlayer = RemoteFindPlayer(targetPlainID);
        if (targetPlayer && targetPlayer.GetIdentity())
        {
            PlayerIdentity targetIdentity = targetPlayer.GetIdentity();
            CloseBankingUI(targetIdentity);
            ResyncPlayerBank(targetIdentity);
            SendNotification("#AB_YouHaveBeenKicked " + clanData.GetName() + " #AB_FromPlayer " + sender.GetName(), targetIdentity);
        }

        if (m_krserverconfig.m_DiscordWebhook.m_LogClanKickMemberToDiscord)
        {
            GetWebhookManager().PlayerLog(sender, " kicked member: " + targetPlainID + " from clan: " + clanData.GetName());
        }
    }

    void ClanUpdateMember(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param2<PermissionObject, string> data;
        if (!ctx.Read(data))
            return;

        PermissionObject requestedPermissions = data.param1;
        string targetPlainID = data.param2;
        if (!requestedPermissions || targetPlainID == "")
            return;

        KR_JsonDatabaseHandler targetData = KR_JsonDatabaseHandler.LoadPlayerData(targetPlainID, "", true);
        KR_JsonDatabaseHandler senderData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());

        if (!targetData || !senderData)
            return;

        if (!HasClanID(senderData.GetClanID()) || targetData.GetClanID() != senderData.GetClanID())
        {
            SendNotification("#AB_ThisPlayerIsNotInYourClan", sender, true);
            return;
        }

        ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(senderData.GetClanID());
        if (!clanData)
            return;

        PermissionObject senderPermissions = GetMemberPermission(clanData, sender.GetPlainId());
        if (!senderPermissions || !senderPermissions.m_CanEdit)
        {
            SendNotification("#AB_NotEnoughtPermissions", sender, true);
            return;
        }

        if (targetPlainID == clanData.GetOwnersID())
        {
            SendNotification("#AB_TryToModifyClanOwnersPermission", sender, true);
            return;
        }

        bool memberFound = false;
        for (int i = 0; i < clanData.GetClanMembers().Count(); i++)
        {
            ClanMemberObject member = clanData.GetClanMembers().Get(i);
            if (member && member.GetPlainID() == targetPlainID)
            {
                member.SetPermission(requestedPermissions);
                clanData.SaveClanData(clanData);
                memberFound = true;
                break;
            }
        }

        if (!memberFound)
        {
            SendNotification("#AB_ThisPlayerIsNotInYourClan", sender, true);
            return;
        }

        SendClanDataToClient(clanData, sender);
        SendNotification("#AB_PermissionsUpdated " + targetPlainID, sender);

        if (m_krserverconfig.m_LoggingSettings.m_LogUpdatePermission)
        {
            GetBankingLogManager().Log("Player: " + targetPlainID + " PermissionsUpdated! from clan: " + clanData.GetName());
        }

        if (m_krserverconfig.m_DiscordWebhook.m_LogClanUpdatePermissionToDiscord)
        {
            GetWebhookManager().PlayerLog(sender, " updated Permissions for member: " + targetPlainID + " from clan: " + clanData.GetName());
        }

        PlayerBase targetPlayer = RemoteFindPlayer(targetPlainID);
        if (targetPlayer && targetPlayer.GetIdentity())
        {
            PlayerIdentity targetIdentity = targetPlayer.GetIdentity();
            SendClanDataToClient(clanData, targetIdentity);
            ResyncPlayerBank(targetIdentity);
            SendNotification("#AB_PermissionsHasBeenChanged", targetIdentity);
        }
    }

    void ClanMemberLeave(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        // Read the original payload for RPC compatibility, but never trust it
        // to decide whether the sender is the clan owner.
        Param1<string> data;
        if (!ctx.Read(data))
            return;

        KR_JsonDatabaseHandler senderData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());

        if (!senderData || !HasClanID(senderData.GetClanID()))
            return;

        ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(senderData.GetClanID());
        if (!clanData)
        {
            senderData.SetClan("NONE");
            ResyncPlayerBank(sender);
            CloseBankingUI(sender);
            return;
        }

        if (clanData.GetOwnersID() == sender.GetPlainId())
        {
            bool wasDeleted = RemoteHandleLeaderLeave(clanData, sender.GetPlainId());
            if (wasDeleted)
            {
                senderData.SetClan("NONE");
                SendNotification("#AB_ClanDeleted", sender);
            }
            else
            {
                SendNotification("#AB_SomethingWentWrong", sender, true);
            }
        }
        else
        {
            bool memberLeft = RemoteHandleMemberLeave(clanData, sender.GetPlainId());
            if (memberLeft)
            {
                senderData.SetClan("NONE");
                SendNotification("#AB_SucesfullyLeavedClan " + clanData.GetName(), sender);
            }
            else
            {
                SendNotification("#AB_SomethingWentWrong", sender, true);
            }
        }

        CloseBankingUI(sender);
        ResyncPlayerBank(sender);
    }

    void ClanUpdate(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param2<string, string> data;
        if (!ctx.Read(data))
            return;

        if (!IsClanNameAndTagValid(data.param1, data.param2))
        {
            SendNotification("#AB_ClanTagOrNameIsNotValid", sender, true);
            return;
        }

        KR_JsonDatabaseHandler senderData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());

        if (!senderData || !HasClanID(senderData.GetClanID()))
            return;

        ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(senderData.GetClanID());
        if (!clanData)
        {
            Error("[Advanced Banking] -> Cannot load clan data for update.");
            return;
        }

        if (sender.GetPlainId() != clanData.GetOwnersID())
        {
            SendNotification("#AB_NotEnoughtPermissions", sender, true);
            return;
        }

        clanData.SetName(data.param1);
        clanData.SetPrefix(data.param2);
        clanData.SaveClanData(clanData);

        SendNotification("#AB_ClanNameUpdated " + clanData.GetName(), sender);
        SendNotification("#AB_ClanTagUpdated " + clanData.GetClanTag(), sender);
        SendClanDataToClient(clanData, sender);
    }

    // ---------------------------------------------------------------------
    // Own-bank and clan-bank operations
    // ---------------------------------------------------------------------

    void WithdrawMoneyFromBankAccount(PlayerIdentity identity, int amount)
    {
        if (!identity || amount < 0)
            return;

        PlayerBase player = RemoteFindPlayer(identity.GetPlainId());
        if (!player)
            return;

        KR_JsonDatabaseHandler playerData = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());

        if (!playerData)
            return;

        int amountToWithdraw = amount;
        if (amountToWithdraw == 0)
            amountToWithdraw = playerData.GetBankCredit();

        if (amountToWithdraw > playerData.GetBankCredit())
            amountToWithdraw = playerData.GetBankCredit();

        if (amountToWithdraw <= 0)
        {
            ResyncPlayerBank(identity);
            return;
        }

        int stillNeeded = AddCurrencyToPlayer(player, amountToWithdraw);
        int finalAmount = amountToWithdraw - stillNeeded;
        if (finalAmount <= 0)
        {
            ResyncPlayerBank(identity);
            return;
        }

        playerData.WithdrawMoney(finalAmount);
        ResyncPlayerBank(identity);

        if (m_krserverconfig.m_DiscordWebhook.m_LogWithdrawToDiscord)
        {
            GetWebhookManager().PlayerLog(identity, "Withdrawed " + finalAmount.ToString() + " ₽ from his own Account.");
        }

        if (m_krserverconfig.m_LoggingSettings.m_LogWithdraw)
        {
            GetBankingLogManager().Log("Player: " + identity.GetPlainId() + " withdrawed " + finalAmount + " from own account.");
        }
    }

    void WithdrawMoneyFromClanBankAccount(PlayerIdentity identity, int amount)
    {
        if (!identity || amount < 0)
            return;

        PlayerBase player = RemoteFindPlayer(identity.GetPlainId());
        if (!player)
            return;

        KR_JsonDatabaseHandler playerData = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());

        if (!playerData || !HasClanID(playerData.GetClanID()))
            return;

        ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(playerData.GetClanID());
        if (!clanData)
        {
            playerData.SetClan("NONE");
            ResyncPlayerBank(identity);
            CloseBankingUI(identity);
            Error("[Advanced Banking] -> Cannot load clan data of player: " + identity.GetName());
            return;
        }

        ClanMemberObject member = clanData.GetMemberByPlainId(identity.GetPlainId());
        if (!member || !member.GetPermission() || !member.GetPermission().m_CanWithdraw)
        {
            SendNotification("#AB_NotEnoughtPermissions", identity, true);
            return;
        }

        int amountToWithdraw = amount;
        if (amountToWithdraw == 0)
            amountToWithdraw = clanData.GetBankCredit();

        if (amountToWithdraw > clanData.GetBankCredit())
        {
            SendNotification("#AB_NotEnoughtMoneyOnAccount", identity, true);
            return;
        }

        if (amountToWithdraw <= 0)
            return;

        int stillNeeded = AddCurrencyToPlayer(player, amountToWithdraw);
        int finalAmount = amountToWithdraw - stillNeeded;
        if (finalAmount <= 0)
            return;

        clanData.WithdrawMoney(finalAmount);
        clanData.WriteLog(identity.GetName() + " withdrawed: " + finalAmount);
        SendClanDataToClient(clanData, identity);

        if (m_krserverconfig.m_DiscordWebhook.m_LogClanWithdrawToDiscord)
        {
            GetWebhookManager().PlayerLog(identity, " Withdrawed " + finalAmount.ToString() + " ₽ from Clan Account with id: " + clanData.GetClanID());
        }

        if (m_krserverconfig.m_LoggingSettings.m_LogClanWithdraw)
        {
            GetBankingLogManager().Log("Player: " + identity.GetPlainId() + " withdrawed " + finalAmount + " from clan account.");
        }
    }

    void DepositMoneyOnOwnBank(PlayerIdentity identity, int amount)
    {
        if (!identity || amount < 0)
            return;

        PlayerBase player = RemoteFindPlayer(identity.GetPlainId());
        if (!player)
            return;

        KR_JsonDatabaseHandler playerData = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());

        if (!playerData)
        {
            Error("[Advanced Banking] -> Cannot load player data of: " + identity.GetName() + ". Please report this to the banking development team.");
            return;
        }

        int currencyOnPlayer = GetPlayerCurrencyAmount(player);
        int amountToDeposit = amount;
        if (amountToDeposit == 0)
            amountToDeposit = currencyOnPlayer;

        int maxPlaceableAmount = GetMaxPlaceAbleAmmountForBank(playerData);
        if (amountToDeposit > maxPlaceableAmount)
            amountToDeposit = maxPlaceableAmount;

        if (amountToDeposit <= 0)
        {
            SendNotification("#AB_BankAlreadyFullPayCheckError", identity, true);
            ResyncPlayerBank(identity);
            return;
        }

        if (currencyOnPlayer < amountToDeposit)
        {
            SendNotification("#AB_NotEnoughtMoneyInInventory", identity, true);
            ResyncPlayerBank(identity);
            return;
        }

        RemoveCurrencyFromPlayer(player, amountToDeposit);
        playerData.DepositMoney(amountToDeposit);
        ResyncPlayerBank(identity);

        if (m_krserverconfig.m_DiscordWebhook.m_LogDepositToDiscord)
        {
            GetWebhookManager().PlayerLog(identity, "Deposited " + amountToDeposit.ToString() + " ₽ to his own Account.");
        }

        if (m_krserverconfig.m_LoggingSettings.m_LogDepositOwnBank)
        {
            GetBankingLogManager().Log("Player: " + identity.GetPlainId() + " Deposited " + amountToDeposit + " on own account.");
        }
    }

    void DepositMoneyOnClanBank(PlayerIdentity identity, int amount)
    {
        if (!identity || amount < 0)
            return;

        PlayerBase player = RemoteFindPlayer(identity.GetPlainId());
        if (!player)
            return;

        KR_JsonDatabaseHandler playerData = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());

        if (!playerData || !HasClanID(playerData.GetClanID()))
            return;

        ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(playerData.GetClanID());
        if (!clanData)
        {
            playerData.SetClan("NONE");
            ResyncPlayerBank(identity);
            CloseBankingUI(identity);
            Error("[Advanced Banking] -> Cannot load clan data of player: " + identity.GetName());
            return;
        }

        ClanMemberObject member = clanData.GetMemberByPlainId(identity.GetPlainId());
        if (!member || !member.GetPermission() || !member.GetPermission().m_CanDeposit)
        {
            SendNotification("#AB_NotEnoughtPermissions", identity, true);
            return;
        }

        int currencyOnPlayer = GetPlayerCurrencyAmount(player);
        int amountToDeposit = amount;
        if (amountToDeposit == 0)
            amountToDeposit = currencyOnPlayer;

        int maxPlaceableAmount = GetMaxPlaceAbleAmmountForClanBank(clanData);
        if (amountToDeposit > maxPlaceableAmount)
            amountToDeposit = maxPlaceableAmount;

        if (amountToDeposit <= 0)
        {
            SendNotification("Clan bank account limit reached.", identity, true);
            return;
        }

        if (currencyOnPlayer < amountToDeposit)
        {
            SendNotification("#AB_NotEnoughtMoneyInInventory", identity, true);
            return;
        }

        RemoveCurrencyFromPlayer(player, amountToDeposit);
        clanData.DepositMoney(amountToDeposit);
        clanData.WriteLog(identity.GetName() + " deposited: " + amountToDeposit);

        SendClanDataToClient(clanData, identity);
        ResyncPlayerBank(identity);

        if (m_krserverconfig.m_DiscordWebhook.m_LogClanDepositToDiscord)
        {
            GetWebhookManager().PlayerLog(identity, " Deposited " + amountToDeposit.ToString() + " ₽ to Clan Account with id: " + clanData.GetClanID());
        }

        if (m_krserverconfig.m_LoggingSettings.m_LogClanDeposit)
        {
            GetBankingLogManager().Log("Player: " + identity.GetPlainId() + " deposited " + amountToDeposit + " on clan account.");
        }
    }

    // ---------------------------------------------------------------------
    // Player and clan data helpers
    // ---------------------------------------------------------------------

    void UpdatePlayerList()
    {
        if (!m_BankingPlayers)
            m_BankingPlayers = new array<ref bankingplayerlistobj>;
        else
            m_BankingPlayers.Clear();

        array<Man> onlinePlayers = new array<Man>;
        GetGame().GetPlayers(onlinePlayers);

        foreach (Man man : onlinePlayers)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (!player || !player.GetIdentity())
                continue;

            bankingplayerlistobj playerListEntry = new bankingplayerlistobj(player.GetIdentity().GetPlainId(), player.GetIdentity().GetName());
            m_BankingPlayers.Insert(playerListEntry);
        }
    }

    int GetMaxPlaceAbleAmmountForBank(KR_JsonDatabaseHandler playerData)
    {
        if (!playerData || !m_krserverconfig)
            return 0;

        int maxPlaceableAmount = m_krserverconfig.maxCurrency - playerData.GetBankCredit();
        maxPlaceableAmount += playerData.GetBonusAmount();

        if (maxPlaceableAmount < 0)
            return 0;

        return maxPlaceableAmount;
    }

    ClanDataBaseManager RegisterNewClan(string clanName, string clanOwnersID)
    {
        if (clanName == "" || clanOwnersID == "")
            return null;

        string clanID = GenerateRandomClanID();
        if (clanID == "")
            return null;

        return ClanDataBaseManager.CreateClan(clanName, clanID, clanOwnersID);
    }

    void AddClanMember(ClanDataBaseManager clanData, PermissionObject permissions, string steamID,string memberName)
    {
        if (!clanData || !permissions || steamID == "")
            return;

        if (m_krserverconfig.m_LoggingSettings.m_LogInviteMember)
        {
            GetBankingLogManager().Log("Player: " + steamID + " joined the Clan Banking Account: " + clanData.GetName());
        }

        clanData.AddMember(steamID, memberName, permissions);
    }

    bool RemoteHandleLeaderLeave(ClanDataBaseManager clanData, string leaderID)
    {
        if (!clanData || leaderID == "" || clanData.GetOwnersID() != leaderID)
            return false;

        string clanName = clanData.GetName();
        array<string> memberIDs = new array<string>;

        for (int i = 0; i < clanData.GetClanMembers().Count(); i++)
        {
            ClanMemberObject member = clanData.GetClanMembers().Get(i);
            if (!member || member.GetPlainID() == "")
                continue;

            memberIDs.Insert(member.GetPlainID());
        }

        // Delete the clan first. Player records are only changed after the
        // destructive operation succeeded, avoiding a half-deleted clan.
        if (!clanData.DeleteClan())
            return false;

        foreach (string memberID : memberIDs)
        {
            KR_JsonDatabaseHandler memberData = KR_JsonDatabaseHandler.LoadPlayerData(memberID, "", true);

            if (memberData)
                memberData.SetClan("NONE");
            else
                Error("[Advanced Banking] -> Cannot clear ClanID from: " + memberID);

            PlayerBase memberPlayer = RemoteFindPlayer(memberID);
            if (!memberPlayer || !memberPlayer.GetIdentity())
                continue;

            CloseBankingUI(memberPlayer.GetIdentity());
            ResyncPlayerBank(memberPlayer.GetIdentity());
        }

        if (m_krserverconfig.m_LoggingSettings.m_LogDeleteClanAccount)
        {
            GetBankingLogManager().Log("Player: " + leaderID + " deleted clan: " + clanName);
        }

        return true;
    }

    bool RemoteHandleMemberLeave(ClanDataBaseManager clanData, string memberID)
    {
        if (!clanData || memberID == "")
            return false;

        for (int i = 0; i < clanData.GetClanMembers().Count(); i++)
        {
            ClanMemberObject member = clanData.GetClanMembers().Get(i);
            if (member && member.GetPlainID() == memberID)
            {
                clanData.RemoveMember(i);
                return true;
            }
        }

        return false;
    }

    PermissionObject GetMemberPermission(ClanDataBaseManager clanData, string memberID)
    {
        if (!clanData || memberID == "")
            return null;

        ClanMemberObject member = clanData.GetMemberByPlainId(memberID);
        if (!member)
            return null;

        return member.GetPermission();
    }

    string GenerateRandomClanID()
    {
        if (!m_chars || m_chars.Count() == 0)
            return "";

        string randomID = "";
        for (int i = 0; i < 16; i++)
            randomID += m_chars.GetRandomElement();

        return randomID;
    }

    int GetMaxPlaceAbleAmmountForClanBank(ClanDataBaseManager clanData)
    {
        if (!clanData || !m_krserverconfig)
            return 0;

        int currentBankAmount = clanData.GetBankCredit();

        // MaxClanAccountLimit <= 0 means unlimited. The signed-int headroom
        // prevents an overflow while preserving the modder fix's intent.
        if (m_krserverconfig.MaxClanAccountLimit <= 0)
        {
            if (currentBankAmount >= 2000000000)
                return 0;

            return 2000000000 - currentBankAmount;
        }

        int maxPlaceableAmount = m_krserverconfig.MaxClanAccountLimit - currentBankAmount;
        if (maxPlaceableAmount < 0)
            return 0;

        return maxPlaceableAmount;
    }

    // ---------------------------------------------------------------------
    // Currency inventory helpers
    // ---------------------------------------------------------------------

    int AddCurrencyToPlayer(PlayerBase player, int amountToAdd)
    {
        if (amountToAdd <= 0)
            return 0;

        if (!player || !m_krserverconfig || !m_krserverconfig.BankingCurrency)
            return amountToAdd;

        int amountStillNeeded = amountToAdd;

        for (int i = 0; i < m_krserverconfig.BankingCurrency.Count(); i++)
        {
            CurrencySettings currency = m_krserverconfig.BankingCurrency.Get(i);
            if (!currency || currency.CurrencyValue <= 0)
                continue;

            int quantityNeeded = amountStillNeeded / currency.CurrencyValue;
            if (quantityNeeded <= 0)
                continue;

            int quantityLeft = AddCurrencyToInventory(player, i, quantityNeeded);
            amountStillNeeded -= (quantityNeeded - quantityLeft) * currency.CurrencyValue;

            if (amountStillNeeded <= 0)
                return 0;
        }

        return amountStillNeeded;
    }

    int RemoveCurrencyFromPlayer(PlayerBase player, int amountToRemove)
    {
        if (amountToRemove <= 0)
            return 0;

        if (!player || !m_krserverconfig || !m_krserverconfig.BankingCurrency)
            return amountToRemove;

        array<EntityAI> inventoryItems = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, inventoryItems);

        for (int currencyIndex = m_krserverconfig.BankingCurrency.Count() - 1; currencyIndex >= 0; currencyIndex--)
        {
            if (amountToRemove <= 0)
                return 0;

            CurrencySettings currency = m_krserverconfig.BankingCurrency.Get(currencyIndex);
            if (!currency || currency.CurrencyValue <= 0)
                continue;

            foreach (EntityAI inventoryEntity : inventoryItems)
            {
                if (amountToRemove <= 0)
                    return 0;

                if (!inventoryEntity || inventoryEntity.GetType() != currency.CurrencyName)
                    continue;

                ItemBase item;
                if (!Class.CastTo(item, inventoryEntity))
                    continue;

                int quantityNeeded = amountToRemove / currency.CurrencyValue;
                int itemQuantity = GetItemQuantity(item);
                int itemMaxQuantity = GetItemQuantityMax(item);

                if (itemMaxQuantity == 0)
                {
                    GetGame().ObjectDelete(item);

                    if (quantityNeeded >= 1)
                    {
                        amountToRemove -= currency.CurrencyValue;
                    }
                    else
                    {
                        return AddCurrencyToPlayer(player, currency.CurrencyValue - amountToRemove);
                    }
                }
                else if (quantityNeeded >= itemQuantity)
                {
                    amountToRemove -= itemQuantity * currency.CurrencyValue;
                    GetGame().ObjectDelete(item);
                }
                else
                {
                    SetItemQuantity(item, itemQuantity - quantityNeeded);
                    amountToRemove -= quantityNeeded * currency.CurrencyValue;

                    if (amountToRemove > 0 && amountToRemove < currency.CurrencyValue)
                    {
                        int remainingItemQuantity = GetItemQuantity(item);
                        if (remainingItemQuantity <= 1)
                            GetGame().ObjectDelete(item);
                        else
                            SetItemQuantity(item, remainingItemQuantity - 1);

                        return AddCurrencyToPlayer(player, currency.CurrencyValue - amountToRemove);
                    }
                }
            }
        }

        if (amountToRemove < 0)
            return 0;

        return amountToRemove;
    }

    int AddCurrencyToInventory(PlayerBase player, int currencyArrayIndex, int amountToAdd)
    {
        if (amountToAdd <= 0)
            return 0;

        if (!player || !m_krserverconfig || !m_krserverconfig.BankingCurrency)
            return amountToAdd;

        if (currencyArrayIndex < 0 || currencyArrayIndex >= m_krserverconfig.BankingCurrency.Count())
        {
            return amountToAdd;
        }

        CurrencySettings currency = m_krserverconfig.BankingCurrency.Get(currencyArrayIndex);
        if (!currency || currency.CurrencyName == "")
            return amountToAdd;

        int quantityLeft = amountToAdd;
        array<EntityAI> inventoryItems = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, inventoryItems);

        foreach (EntityAI inventoryEntity : inventoryItems)
        {
            if (quantityLeft <= 0)
                return 0;

            if (!inventoryEntity || inventoryEntity.GetType() != currency.CurrencyName)
                continue;

            ItemBase currencyItem;
            if (!Class.CastTo(currencyItem, inventoryEntity))
                continue;

            int addableQuantity = GetItemQuantityMax(currencyItem) - GetItemQuantity(currencyItem);
            if (addableQuantity <= 0)
                continue;

            if (addableQuantity < quantityLeft)
            {
                SetItemQuantity(currencyItem, GetItemQuantityMax(currencyItem));
                quantityLeft -= addableQuantity;
            }
            else
            {
                SetItemQuantity(currencyItem, GetItemQuantity(currencyItem) + quantityLeft);
                quantityLeft = 0;
            }
        }

        InventoryLocation inventoryLocation = new InventoryLocation();
        while (quantityLeft > 0 && player.GetInventory().FindFirstFreeLocationForNewEntity(currency.CurrencyName, FindInventoryLocationType.CARGO, inventoryLocation ))
        {
            ItemBase createdCurrencyItem;
            if (!Class.CastTo(createdCurrencyItem, player.GetHumanInventory().CreateInInventory(currency.CurrencyName)))
            {
                break;
            }

            int maxQuantity = GetItemQuantityMax(createdCurrencyItem);
            if (maxQuantity == 0)
            {
                SetItemQuantity(createdCurrencyItem, 0);
                quantityLeft--;
            }
            else if (quantityLeft <= maxQuantity)
            {
                SetItemQuantity(createdCurrencyItem, quantityLeft);
                quantityLeft = 0;
            }
            else
            {
                SetItemQuantity(createdCurrencyItem, maxQuantity);
                quantityLeft -= maxQuantity;
            }
        }

        if (quantityLeft > 0 && !player.GetHumanInventory().GetEntityInHands())
        {
            ItemBase handCurrencyItem;
            if (Class.CastTo(handCurrencyItem, player.GetHumanInventory().CreateInInventory(currency.CurrencyName)))
            {
                int handItemMaxQuantity = GetItemQuantityMax(handCurrencyItem);
                if (handItemMaxQuantity == 0)
                {
                    SetItemQuantity(handCurrencyItem, 0);
                    quantityLeft--;
                }
                else if (quantityLeft <= handItemMaxQuantity)
                {
                    SetItemQuantity(handCurrencyItem, quantityLeft);
                    quantityLeft = 0;
                }
                else
                {
                    SetItemQuantity(handCurrencyItem, handItemMaxQuantity);
                    quantityLeft -= handItemMaxQuantity;
                }
            }
        }

        if (quantityLeft < 0)
            return 0;

        return quantityLeft;
    }

    // ---------------------------------------------------------------------
    // Quantity helpers
    // ---------------------------------------------------------------------

    void SetItemQuantity(ItemBase item, int quantity)
    {
        if (!item || quantity < 0)
            return;

        if (item.IsMagazine())
        {
            Magazine magazine = Magazine.Cast(item);
            if (magazine)
                magazine.ServerSetAmmoCount(quantity);

            return;
        }

        item.SetQuantity(quantity);
    }

    int GetItemMaxQuantity(string itemClassname)
    {
        if (itemClassname == "")
            return 0;

        TStringArray searchPaths = new TStringArray;
        searchPaths.Insert(CFG_MAGAZINESPATH + " " + itemClassname + " count");
        searchPaths.Insert(CFG_VEHICLESPATH + " " + itemClassname + " varQuantityMax");

        foreach (string path : searchPaths)
        {
            if (GetGame().ConfigIsExisting(path))
                return g_Game.ConfigGetInt(path);
        }

        return 0;
    }

    int GetItemQuantity(ItemBase item)
    {
        if (!item)
            return 0;

        if (item.IsMagazine())
        {
            Magazine magazine = Magazine.Cast(item);
            if (magazine)
                return magazine.GetAmmoCount();
        }

        return item.GetQuantity();
    }

    int GetItemQuantityMax(ItemBase item)
    {
        if (!item)
            return 0;

        if (item.IsMagazine())
        {
            Magazine magazine = Magazine.Cast(item);
            if (magazine)
                return magazine.GetAmmoMax();
        }

        return item.GetQuantityMax();
    }

    // ---------------------------------------------------------------------
    // Player, notification, and ATM helpers
    // ---------------------------------------------------------------------

    PlayerBase RemoteFindPlayer(string steam64ID)
    {
        if (steam64ID == "" || !GetGame())
            return null;

        array<Man> onlinePlayers = new array<Man>;
        GetGame().GetPlayers(onlinePlayers);

        foreach (Man man : onlinePlayers)
        {
            if (!man || !man.GetIdentity())
                continue;

            if (man.GetIdentity().GetPlainId() == steam64ID)
                return PlayerBase.Cast(man);
        }

        return null;
    }

    int GetPlayersBankAmount(PlayerIdentity identity)
    {
        if (!identity)
            return -1;

        KR_JsonDatabaseHandler playerData = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(),identity.GetName());

        if (!playerData)
            return -1;

        return playerData.GetBankCredit();
    }

    int GetPlayerCurrencyAmount(PlayerBase player)
    {
        if (!player || !m_krserverconfig || !m_krserverconfig.BankingCurrency)
            return 0;

        int currencyInInventory = 0;
        array<EntityAI> inventoryItems = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, inventoryItems);

        foreach (EntityAI inventoryEntity : inventoryItems)
        {
            ItemBase currencyItem;
            if (!inventoryEntity || !Class.CastTo(currencyItem, inventoryEntity))
                continue;

            for (int i = 0; i < m_krserverconfig.BankingCurrency.Count(); i++)
            {
                CurrencySettings currency = m_krserverconfig.BankingCurrency.Get(i);
                if (!currency || currency.CurrencyValue <= 0)
                    continue;

                if (currencyItem.GetType() == currency.CurrencyName)
                {
                    currencyInInventory += GetItemAmount(currencyItem) * currency.CurrencyValue;
                    break;
                }
            }
        }

        return currencyInInventory;
    }

    int GetItemAmount(ItemBase item)
    {
        if (!item)
            return 0;

        if (item.IsMagazine())
        {
            Magazine magazine = Magazine.Cast(item);
            if (magazine)
                return magazine.GetAmmoCount();

            return 0;
        }

        return QuantityConversions.GetItemQuantity(item);
    }

    void SendNotification(string message, PlayerIdentity identity, bool isError = false)
    {
        if (!identity)
            return;

        #ifdef NOTIFICATIONS
        if (isError)
        {
            NotificationSystem.SimpleNoticiation(" " + message, "Banking", "KR_Banking/data/Logos/notificationbanking.edds", ARGB(240, 255, 0, 0), 5,identity);
        }
        else
        {
            NotificationSystem.SimpleNoticiation(" " + message, "Banking", "KR_Banking/data/Logos/notificationbanking.edds", ARGB(240, 255, 13, 55), 5, identity);
        }
        #else
        PlayerBase player = RemoteFindPlayer(identity.GetPlainId());
        if (!player)
            return;

        if (isError)
        {
            NotificationSystem.SendNotificationToPlayerExtended(player, 5, "Banking ERROR", message, "KR_Banking/data/Logos/notificationbanking.edds");
        }
        else
        {
            NotificationSystem.SendNotificationToPlayerExtended(player, 5, "Banking", message, "KR_Banking/data/Logos/notificationbanking.edds");
        }
        #endif
    }

    protected void SpawnATMs()
    {
        if (!m_krserverconfig || !m_krserverconfig.ATM)
            return;

        for (int i = 0; i < m_krserverconfig.ATM.Count(); i++)
        {
            ATMPosition atmConfig = m_krserverconfig.ATM.Get(i);
            if (!atmConfig || atmConfig.ATMSName == "")
                continue;

            vector position = atmConfig.GetATMPosition();
            vector orientation = atmConfig.GetATMDirectory();

            EntityAI spawnedATM = EntityAI.Cast(GetGame().CreateObject(atmConfig.ATMSName, position));

            if (!spawnedATM)
            {
                Error("[Advanced Banking] -> Failed to spawn ATM: " + atmConfig.ATMSName + " at: " + position);
                continue;
            }

            spawnedATM.SetPosition(position);
            spawnedATM.SetOrientation(orientation);

            KR_BankingATM bankingATM;
            if (Class.CastTo(bankingATM, spawnedATM))
                bankingATM.m_Banking_CanBeRobbed = atmConfig.GetCanRobbed();

            Print("[Advanced Banking] -> Successfully spawned ATM: " + atmConfig.ATMSName + " at: " + position);
        }
    }

    bool IsNegativ(int number)
    {
        return number < 0;
    }
};

/* Global Plugin Getter */
PluginKRBankingManagerServer GetBankingServerManager()
{
    if (!GetGame() || !GetGame().IsServer() || !GetGame().IsMultiplayer())
        return null;

    return PluginKRBankingManagerServer.Cast(GetPluginManager().GetPluginByType(PluginKRBankingManagerServer));
}
