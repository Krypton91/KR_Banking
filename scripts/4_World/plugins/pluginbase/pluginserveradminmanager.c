class BankingAdminManager extends PluginBase
{
    void BankingAdminManager()
    {
        if (!GetGame() || !GetGame().IsServer())
            return;

        Print("[Advanced Banking] -> Internal Admin Module starting...");

        if (GetKR_AdminConfig() && GetKR_AdminConfig().m_Admins)
            Print("[Advanced Banking] -> Admins found: " + GetKR_AdminConfig().m_Admins.Count().ToString());
        else
            Print("[Advanced Banking] -> Admin config or admin list is missing.");

        Init();
    }

    void Init()
    {
        GetRPCManager().AddRPC("KR_BANKING", "AdminDataRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminRequestPlayerdata", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminRequestServerSettings", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminSafePlayerdata", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminTeleportToPosition", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminDeleteATM", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminInsertATM", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminUpdateServerConfig", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminReloadConfig", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminRequestClanDataWithID", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminJoinClan", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminDeleteClan", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminRequestMoneyDrop", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminResetAtmRobs", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminUpdateClanData", this, SingleplayerExecutionType.Server);
    }

    // ---------------------------------------------------------------------
    // Validation and permission helpers
    // ---------------------------------------------------------------------

    protected bool IsValidServerRequest(CallType type, PlayerIdentity sender)
    {
        if (type != CallType.Server)
            return false;

        if (!sender)
            return false;

        return true;
    }

    protected void SendError(PlayerIdentity sender, string message)
    {
        if (sender)
            GetBankingServerManager().SendNotification(message, sender, true);
    }

    protected bool IsAdminNameValid(AdminUsers admin, PlayerIdentity sender)
    {
        if (!admin || !admin.m_permissions || !sender)
            return false;

        if (admin.m_permissions.m_CanIgnoreNamePermission)
            return true;

        if (admin.m_Name == sender.GetName())
            return true;

        SendError(sender, "Action can only be executed with name: " + admin.m_Name);
        return false;
    }

    protected AdminUsers GetValidatedAdmin(PlayerIdentity sender)
    {
        if (!sender)
            return null;

        AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
        if (!admin || !admin.m_permissions)
        {
            SendError(sender, "You do not have permission to use the banking admin tools.");
            return null;
        }

        if (!IsAdminNameValid(admin, sender))
            return null;

        return admin;
    }

    protected bool RequirePlayersBankDataPermission(PlayerIdentity sender)
    {
        AdminUsers admin = GetValidatedAdmin(sender);
        if (!admin)
            return false;

        if (!admin.m_permissions.m_CanUsePlayersBankData)
        {
            SendError(sender, "Permission denied: m_CanUsePlayersBankData is required.");
            return false;
        }

        return true;
    }

    protected bool RequireServerConfigPermission(PlayerIdentity sender)
    {
        AdminUsers admin = GetValidatedAdmin(sender);
        if (!admin)
            return false;

        if (!admin.m_permissions.m_CanUseServerConfig)
        {
            SendError(sender, "Permission denied: m_CanUseServerConfig is required.");
            return false;
        }

        return true;
    }

    protected bool RequirePlayersClansPermission(PlayerIdentity sender)
    {
        AdminUsers admin = GetValidatedAdmin(sender);
        if (!admin)
            return false;

        // Preserve the original server-config gate, but enforce the clan flag as a real permission.
        if (!admin.m_permissions.m_CanUseServerConfig)
        {
            SendError(sender, "Permission denied: m_CanUseServerConfig is required.");
            return false;
        }

        if (!admin.m_permissions.m_CanUsePlayersClans)
        {
            SendError(sender, "Permission denied: m_CanUsePlayersClans is required.");
            return false;
        }

        return true;
    }

    protected bool RequireMiscServerPermission(PlayerIdentity sender)
    {
        AdminUsers admin = GetValidatedAdmin(sender);
        if (!admin)
            return false;

        // Preserve the original server-config gate, but enforce the misc flag as a real permission.
        if (!admin.m_permissions.m_CanUseServerConfig)
        {
            SendError(sender, "Permission denied: m_CanUseServerConfig is required.");
            return false;
        }

        if (!admin.m_permissions.m_CanUseMiscServer)
        {
            SendError(sender, "Permission denied: m_CanUseMiscServer is required.");
            return false;
        }

        return true;
    }

    // ---------------------------------------------------------------------
    // Admin data and player banking
    // ---------------------------------------------------------------------

    void AdminDataRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param2<AdminPermissions, bool> adminDataResponse;
        AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
        if (!admin || !admin.m_permissions)
        {
            adminDataResponse = new Param2<AdminPermissions, bool>(null, false);
            GetRPCManager().SendRPC("KR_BANKING", "AdminDataResponse", adminDataResponse, true, sender);
            return;
        }

        if (!IsAdminNameValid(admin, sender))
        {
            adminDataResponse = new Param2<AdminPermissions, bool>(null, false);
            GetRPCManager().SendRPC("KR_BANKING", "AdminDataResponse", adminDataResponse, true, sender);
            return;
        }

        adminDataResponse = new Param2<AdminPermissions, bool>(admin.m_permissions, true);
        GetRPCManager().SendRPC("KR_BANKING", "AdminDataResponse", adminDataResponse, true, sender);
    }

    void AdminRequestPlayerdata(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param1<string> data;
        if (!ctx.Read(data) || !data || data.param1 == "")
        {
            SendError(sender, "Invalid player-data request.");
            return;
        }

        if (!RequirePlayersBankDataPermission(sender))
            return;

        string targetPlainId = data.param1;
        KR_JsonDatabaseHandler targetData = KR_JsonDatabaseHandler.LoadPlayerData(targetPlainId, "", true);
        if (!targetData)
        {
            SendError(sender, "Unable to load player data for: " + targetPlainId);
            return;
        }

        Param5<int, int, string, string, string> playerDataResponse;
        playerDataResponse = new Param5<int, int, string, string, string>(targetData.GetBankCredit(), targetData.GetBonusAmount(), targetData.GetName(), targetData.GetSteamID(), targetData.GetClanID());
        GetRPCManager().SendRPC("KR_BANKING", "AdminPlayerDataResponse", playerDataResponse, true, sender);
    }

    void AdminSafePlayerdata(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param3<string, int, int> data;
        if (!ctx.Read(data) || !data || data.param1 == "")
        {
            SendError(sender, "Invalid player-data update request.");
            return;
        }

        if (!RequirePlayersBankDataPermission(sender))
            return;

        string targetPlainId = data.param1;

        // Search-only prevents an invalid ID from creating a new player JSON file.
        KR_JsonDatabaseHandler targetData = KR_JsonDatabaseHandler.LoadPlayerData(targetPlainId, "", true);
        if (!targetData)
        {
            SendError(sender, "Unable to load player data for: " + targetPlainId);
            return;
        }

        targetData.UpdateCurrency(data.param2, false);
        targetData.UpdateBonus(data.param3, true);
        GetBankingServerManager().SendNotification("Successfully updated player data for: " + targetData.GetName(), sender);

        if (GetKR_BankingServerConfig().m_DiscordWebhook.m_AdminLogUpdatePlayerData)
        {
            GetWebhookManager().AdminLog(sender, " Admin updated player data for: " + targetData.GetName() + " with ID: " + targetPlainId);
        }
    }

    // ---------------------------------------------------------------------
    // Server configuration and ATM management
    // ---------------------------------------------------------------------

    void AdminRequestServerSettings(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        if (!RequireServerConfigPermission(sender))
            return;

        Param1<KR_BankingConfigManager> serverConfigResponse;
        serverConfigResponse = new Param1<KR_BankingConfigManager>(GetKR_BankingServerConfig());
        GetRPCManager().SendRPC("KR_BANKING", "AdminServerConfigResponse", serverConfigResponse, true, sender);
    }

    void AdminReloadConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        if (!RequireServerConfigPermission(sender))
            return;

        GetBankingServerManager().InternalConfigReload();

        array<Man> allPlayers = new array<Man>;
        GetGame().GetPlayers(allPlayers);

        PlayerBase player;
        foreach (Man currentMan : allPlayers)
        {
            if (!Class.CastTo(player, currentMan))
                continue;

            PlayerIdentity identity = player.GetIdentity();
            if (identity)
                GetBankingServerManager().SendConfigToClient(identity);
        }

        GetBankingServerManager().SendNotification("Config reloaded and sent to all connected clients.", sender);
    }

    void AdminDeleteATM(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param1<int> data;
        if (!ctx.Read(data) || !data)
        {
            SendError(sender, "Invalid ATM delete request.");
            return;
        }

        if (!RequireServerConfigPermission(sender))
            return;

        int atmIndex = data.param1;
        if (!GetKR_BankingServerConfig() || !GetKR_BankingServerConfig().ATM)
        {
            SendError(sender, "ATM configuration is not available.");
            return;
        }

        if (atmIndex < 0 || atmIndex >= GetKR_BankingServerConfig().ATM.Count())
        {
            SendError(sender, "Invalid ATM index: " + atmIndex.ToString());
            return;
        }

        ATMPosition currentATM = GetKR_BankingServerConfig().ATM.Get(atmIndex);
        if (!currentATM)
        {
            SendError(sender, "Unable to find the configured ATM.");
            return;
        }

        array<Object> excludedObjects = new array<Object>;
        array<Object> nearbyObjects = new array<Object>;

        GetGame().IsBoxColliding(currentATM.GetATMPosition(), currentATM.GetATMDirectory(), Vector(3, 5, 9), excludedObjects, nearbyObjects);

        KR_BankingATM atm;
        foreach (Object nearbyObject : nearbyObjects)
        {
            if (!Class.CastTo(atm, nearbyObject))
                continue;

            if (GetKR_BankingServerConfig().m_DiscordWebhook.m_AdminLogUpdateServerConfig)
            {
                GetWebhookManager().AdminLog(sender, " Deleted ATM: " + atm.GetType() + " at position: " + atm.GetPosition().ToString());
            }

            GetGame().ObjectDelete(atm);
            GetKR_BankingServerConfig().RemoveATMWithIndex(atmIndex);
            GetBankingServerManager().SendNotification("ATM deleted.", sender);
            return;
        }

        SendError(sender, "The ATM exists in the config, but no KR_BankingATM object was found at its position.");
    }

    void AdminInsertATM(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param4<string, bool, vector, vector> data;
        if (!ctx.Read(data) || !data || data.param1 == "")
        {
            SendError(sender, "Invalid ATM insert request.");
            return;
        }

        if (!RequireServerConfigPermission(sender))
            return;

        if (!GetKR_BankingServerConfig() || !GetKR_BankingServerConfig().ATM)
        {
            SendError(sender, "ATM configuration is not available.");
            return;
        }

        // Keep the original order because the ATM may read its config during initialization.
        // If spawning or casting fails, the newly appended config entry is rolled back.
        int insertedConfigIndex = GetKR_BankingServerConfig().ATM.Count();
        GetKR_BankingServerConfig().AddATM(data.param1, data.param2, data.param3, data.param4);

        EntityAI spawnedATM = EntityAI.Cast(GetGame().CreateObject(data.param1, data.param3));
        if (!spawnedATM)
        {
            if (GetKR_BankingServerConfig().ATM.Count() > insertedConfigIndex)
                GetKR_BankingServerConfig().RemoveATMWithIndex(insertedConfigIndex);

            SendError(sender, "Unable to spawn ATM class: " + data.param1);
            return;
        }

        spawnedATM.SetPosition(data.param3);
        spawnedATM.SetOrientation(data.param4);

        KR_BankingATM bankingATM;
        if (!Class.CastTo(bankingATM, spawnedATM))
        {
            GetGame().ObjectDelete(spawnedATM);

            if (GetKR_BankingServerConfig().ATM.Count() > insertedConfigIndex)
                GetKR_BankingServerConfig().RemoveATMWithIndex(insertedConfigIndex);

            SendError(sender, "Spawned object is not derived from KR_BankingATM: " + data.param1);
            return;
        }

        bankingATM.m_Banking_CanBeRobbed = data.param2;
        bankingATM.SetSynchDirty();

        if (GetKR_BankingServerConfig().m_DiscordWebhook.m_AdminLogUpdateServerConfig)
        {
            GetWebhookManager().AdminLog(sender, " Inserted ATM " + data.param1 + " at position: " + spawnedATM.GetPosition().ToString());
        }

        GetBankingServerManager().SendNotification("ATM inserted.", sender);
    }

    void AdminUpdateServerConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        // ref is valid here because it is used inside a template argument.
        Param1<ref KR_BankingConfigManager> data;
        if (!ctx.Read(data) || !data || !data.param1)
        {
            SendError(sender, "Invalid server-config update request.");
            return;
        }

        if (!RequireServerConfigPermission(sender))
            return;

        GetKR_BankingServerConfig().SetConfig(data.param1);

        if (GetKR_BankingServerConfig().m_DiscordWebhook.m_AdminLogUpdateServerConfig)
            GetWebhookManager().AdminLog(sender, " Updated server config.");

        GetBankingServerManager().SendNotification("Server config updated.", sender);
    }

    void AdminTeleportToPosition(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param1<vector> data;
        if (!ctx.Read(data) || !data)
        {
            SendError(sender, "Invalid teleport request.");
            return;
        }

        if (!RequireServerConfigPermission(sender))
            return;

        PlayerBase player = GetBankingServerManager().RemoteFindPlayer(sender.GetPlainId());
        if (!player)
        {
            SendError(sender, "Unable to find your player entity for teleport.");
            return;
        }

        player.SetPosition(data.param1);

        if (GetKR_BankingServerConfig().m_DiscordWebhook.m_AdminLogUpdateServerConfig)
        {
            GetWebhookManager().AdminLog(sender, " Admin teleported to ATM position: " + player.GetPosition().ToString());
        }
    }

    // ---------------------------------------------------------------------
    // Miscellaneous server actions
    // ---------------------------------------------------------------------

    void AdminRequestMoneyDrop(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param1<int> data;
        if (!ctx.Read(data) || !data)
        {
            SendError(sender, "Invalid money-drop request.");
            return;
        }

        if (!RequireMiscServerPermission(sender))
            return;

        int amount = data.param1;
        if (amount <= 0)
        {
            SendError(sender, "Money-drop amount must be greater than zero.");
            return;
        }

        array<Man> allOnlinePlayers = new array<Man>;
        GetGame().GetPlayers(allOnlinePlayers);

        int recipientCount = 0;
        PlayerBase onlinePlayer;

        foreach (Man playerMan : allOnlinePlayers)
        {
            if (!Class.CastTo(onlinePlayer, playerMan))
                continue;

            PlayerIdentity identity = onlinePlayer.GetIdentity();
            if (!identity)
                continue;

            KR_JsonDatabaseHandler playerData = KR_JsonDatabaseHandler.LoadPlayerData(identity.GetPlainId(), identity.GetName());

            if (!playerData)
                continue;

            playerData.DepositMoney(amount);
            GetRPCManager().SendRPC("KR_BANKING", "MoneyDropRecvied", null, true, identity);
            GetBankingServerManager().SendNotification("You received a money drop (" + amount.ToString() + ") from admin: " + sender.GetName(), identity);
            recipientCount++;
        }

        GetBankingServerManager().SendNotification("Money drop sent to " + recipientCount.ToString() + " player(s).", sender);

        if (GetKR_BankingServerConfig().m_DiscordWebhook.m_AdminLogUseMisc)
        {
            GetWebhookManager().AdminLog(sender, " Created a money drop with amount: " + amount.ToString() + " for " + recipientCount.ToString() + " player(s).");
        }
    }

    void AdminResetAtmRobs(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        // The original RPC had no admin or permission validation.
        if (!RequireMiscServerPermission(sender))
            return;

        if (!GetKR_BankingServerConfig() || !GetKR_BankingServerConfig().ATM)
        {
            SendError(sender, "ATM configuration is not available.");
            return;
        }

        int resetCounter = 0;

        for (int i = 0; i < GetKR_BankingServerConfig().ATM.Count(); i++)
        {
            ATMPosition configuredATM = GetKR_BankingServerConfig().ATM.Get(i);
            if (!configuredATM || !configuredATM.GetCanRobbed())
                continue;

            array<Object> excludedObjects = new array<Object>;
            array<Object> nearbyObjects = new array<Object>;

            // A zero orientation (0 0 0) is valid, so vectors must not be used as booleans here.
            if (!GetGame().IsBoxColliding(configuredATM.GetATMPosition(), configuredATM.GetATMDirectory(), Vector(4, 4, 4), excludedObjects,nearbyObjects))
            {
                continue;
            }

            KR_BankingATM atmObject;
            foreach (Object nearbyObject : nearbyObjects)
            {
                if (!Class.CastTo(atmObject, nearbyObject))
                    continue;

                atmObject.SetATMIsRobbed(false);
                atmObject.m_Banking_CanBeRobbed = true;
                atmObject.SetSynchDirty();
                resetCounter++;
                break;
            }
        }

        GetBankingServerManager().SendNotification(resetCounter.ToString() + " ATM(s) have been reset.", sender);

        if (GetKR_BankingServerConfig().m_DiscordWebhook.m_AdminLogUseMisc)
        {
            GetWebhookManager().AdminLog(sender, " Reset all robbed ATMs (" + resetCounter.ToString() + ").");
        }
    }

    // ---------------------------------------------------------------------
    // Clan administration
    // ---------------------------------------------------------------------

    void AdminRequestClanDataWithID(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param1<string> data;
        if (!ctx.Read(data) || !data || data.param1 == "")
        {
            SendError(sender, "Invalid clan search request.");
            return;
        }

        if (!RequirePlayersClansPermission(sender))
            return;

        string term = data.param1;
        ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(term);

        // If the term is not a clan ID, try it as a player Plain ID.
        if (!clanData)
        {
            KR_JsonDatabaseHandler targetPlayer = KR_JsonDatabaseHandler.LoadPlayerData(term, "", true);
            if (targetPlayer)
            {
                string targetClanId = targetPlayer.GetClanID();
                if (targetClanId != "" && targetClanId != "NONE")
                    clanData = ClanDataBaseManager.LoadClanData(targetClanId);
            }
        }

        // Finally, search all clan JSON files by case-insensitive name or tag.
        if (!clanData)
            clanData = NHFAB_FindClanByName(term);

        if (!clanData)
        {
            SendError(sender, "Clan not found: no match for this name, tag, clan ID or player ID.");
            return;
        }

        // Keep the original RPC spelling for client compatibility.
        Param1<ClanDataBaseManager> clanDataResponse;
        clanDataResponse = new Param1<ClanDataBaseManager>(clanData);
        GetRPCManager().SendRPC("KR_BANKING", "AdminClanDataReponse", clanDataResponse, true, sender);
    }

    ClanDataBaseManager NHFAB_FindClanByName(string term)
    {
        if (term == "")
            return null;

        string loweredTerm = term;
        loweredTerm.ToLower();

        string pattern = "$profile:KR_BANKING/ClanDataBase/*.json";
        string fileName;
        FileAttr fileAttr;
        FindFileHandle handle = FindFile(pattern, fileName, fileAttr, FindFileFlags.DIRECTORIES);

        if (!handle)
            return null;

        ClanDataBaseManager foundClan = NHFAB_MatchClanFile(fileName, loweredTerm);

        while (!foundClan && FindNextFile(handle, fileName, fileAttr))
            foundClan = NHFAB_MatchClanFile(fileName, loweredTerm);

        CloseFindFile(handle);
        return foundClan;
    }

    ClanDataBaseManager NHFAB_MatchClanFile(string fileName, string loweredTerm)
    {
        if (fileName == "" || loweredTerm == "")
            return null;

        string clanId = fileName;
        clanId.Replace(".json", "");

        if (clanId == "")
            return null;

        ClanDataBaseManager clan = ClanDataBaseManager.LoadClanData(clanId);
        if (!clan)
            return null;

        string clanName = clan.GetName();
        string clanTag = clan.GetClanTag();
        clanName.ToLower();
        clanTag.ToLower();

        if (clanName.Contains(loweredTerm) || clanTag.Contains(loweredTerm))
            return clan;

        return null;
    }

    void AdminJoinClan(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param1<string> data;
        if (!ctx.Read(data) || !data || data.param1 == "")
        {
            SendError(sender, "Invalid join-clan request.");
            return;
        }

        if (!RequirePlayersClansPermission(sender))
            return;

        ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(data.param1);
        if (!clanData)
        {
            SendError(sender, "Unable to find clan: " + data.param1);
            return;
        }

        KR_JsonDatabaseHandler adminData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());

        if (!adminData)
        {
            SendError(sender, "Unable to load your player data.");
            return;
        }

        adminData.SetClan(clanData.GetClanID());

        PermissionObject permissions = new PermissionObject();
        permissions.GiveClanOwner();
        GetBankingServerManager().AddClanMember(clanData, permissions, sender.GetPlainId(), sender.GetName());

        GetBankingServerManager().SendNotification("Successfully joined clan.", sender);

        if (GetKR_BankingServerConfig().m_DiscordWebhook.m_AdminLogUpdateClanData)
            GetWebhookManager().AdminLog(sender, " Admin joined clan: " + clanData.GetClanID());
    }

    void AdminDeleteClan(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param1<string> data;
        if (!ctx.Read(data) || !data || data.param1 == "")
        {
            SendError(sender, "Invalid delete-clan request.");
            return;
        }

        if (!RequirePlayersClansPermission(sender))
            return;

        ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(data.param1);
        if (!clanData)
        {
            SendError(sender, "Unable to find clan: " + data.param1);
            return;
        }

        if (clanData.GetClanMembers())
        {
            for (int i = 0; i < clanData.GetClanMembers().Count(); i++)
            {
                string plainId = clanData.GetClanMembers().Get(i).GetPlainID();
                if (plainId == "")
                    continue;

                KR_JsonDatabaseHandler memberData = KR_JsonDatabaseHandler.LoadPlayerData(plainId, "", true);
                if (memberData)
                    memberData.SetClan("NONE");
            }
        }

        if (GetKR_BankingServerConfig().m_DiscordWebhook.m_AdminLogUpdateClanData)
            GetWebhookManager().AdminLog(sender, " Admin deleted clan: " + clanData.GetClanID());

        clanData.DeleteClan();
        GetBankingServerManager().SendNotification("Successfully deleted clan.", sender);
    }

    void AdminUpdateClanData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!IsValidServerRequest(type, sender))
            return;

        Param4<string, string, string, int> data;
        if (!ctx.Read(data) || !data || data.param1 == "")
        {
            SendError(sender, "Invalid clan-data update request.");
            return;
        }

        // The original RPC had no admin or permission validation.
        if (!RequirePlayersClansPermission(sender))
            return;

        if (data.param4 < 0)
        {
            SendError(sender, "Clan balance cannot be negative.");
            return;
        }

        ClanDataBaseManager editedClan = ClanDataBaseManager.LoadClanData(data.param1);
        if (!editedClan)
        {
            SendError(sender, "Unable to load clan data for: " + data.param1);
            Error("Unable to load clan data (AdminMode): " + data.param1);
            return;
        }

        editedClan.SetName(data.param2);
        editedClan.SetMoney(data.param4);

        // SetPrefix is kept last because the original implementation uses it as the JSON-safe save step.
        editedClan.SetPrefix(data.param3);

        GetBankingServerManager().SendNotification("Successfully updated clan data.", sender);

        if (GetKR_BankingServerConfig().m_DiscordWebhook.m_AdminLogUpdateClanData)
            GetWebhookManager().AdminLog(sender, " Admin updated clan data for: " + editedClan.GetClanID());
    }

    // Returns the matching admin object or null when the Plain ID is not configured.
    // Do not use ref in a method return type or for local variables.
    AdminUsers GetAdminByPlainId(string plainId)
    {
        if (plainId == "")
            return null;

        if (!GetKR_AdminConfig() || !GetKR_AdminConfig().m_Admins)
            return null;

        foreach (AdminUsers admin : GetKR_AdminConfig().m_Admins)
        {
            if (admin && plainId == admin.m_PlainID)
                return admin;
        }

        return null;
    }
};
