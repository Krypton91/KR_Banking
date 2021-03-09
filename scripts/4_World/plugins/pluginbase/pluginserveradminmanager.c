class BankingAdminManager extends PluginBase
{
    void BankingAdminManager()
    {
        if(GetGame() && GetGame().IsServer())
        {
            Print("[Advanced Banking] -> Internal Admin Module starting....");
            Print(" Found: " + GetKR_AdminConfig().m_Admins.Count().ToString());
            Init();
        }
    }

    void Init()
    {
        Print("[Advanced Banking] -> Register RPCs");
        GetRPCManager().AddRPC("KR_BANKING", "AdminDataRequest", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminRequestPlayerdata", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminRequestServerSettings", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminSafePlayerdata", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminTeleportToPosition", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminDeleteATM", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminInsertATM", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminUpdateServerConfig", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminReloadConfig", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminRequestClanDataWithID", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminJoinClan", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminDeleteClan", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminRequestMoneyDrop", this, SingleplayerExecutionType.Server);
        Print("[Advanced Banking] -> RPCs Registered!");
    }

    void AdminDataRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            foreach(AdminUsers admin: GetKR_AdminConfig().m_Admins)
            {
                if(sender.GetPlainId() == admin.m_PlainID)
                {
                    if(!admin.m_permissions.m_CanIgnoreNamePermission)
                    {
                        if(admin.m_Name != sender.GetName())
                        {
                            GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                            return;
                        }
                    }
                    GetRPCManager().SendRPC("KR_BANKING", "AdminDataResponse", new Param2< ref AdminPermissions, bool >( admin.m_permissions, true ), true, sender);
                    return;
                }
            }
            //GetRPCManager().SendRPC("KR_BANKING", "AdminDataResponse", new Param2< ref AdminPermissions, bool >( null, false ), true, sender);
        }
    }

    void AdminRequestPlayerdata(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<string> data;
            if(!ctx.Read(data)) return;

            string targetsPlainId = data.param1;
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUsePlayersBankData)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                KR_JsonDatabaseHandler targetdata = KR_JsonDatabaseHandler.LoadPlayerData(targetsPlainId, "", true);//true because search only otherwise it will create a new json lol!
                if(targetdata)
                {
                    GetRPCManager().SendRPC("KR_BANKING", "AdminPlayerDataResponse", new Param5< int, int, string, string, string >( targetdata.GetBankCredit(), targetdata.GetBonusAmount(), targetdata.GetName(), targetdata.GetSteamID(), targetdata.GetClanID()), true, sender);
                }
                else
                {
                    GetBankingServerManager().SendNotification("Error while loading playerdata from: " + targetsPlainId, sender, true);
                }
            }
            else
            {
                GetBankingServerManager().SendNotification("You dont have enought permission to do that!", sender, true);
            }
        }
    }

    void AdminSafePlayerdata(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param3<string, int, int> data;
            if(!ctx.Read(data)) return;
            string targetsPlainId = data.param1;
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUsePlayersBankData)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                KR_JsonDatabaseHandler targetdata = KR_JsonDatabaseHandler.LoadPlayerData(targetsPlainId, "");
                if(targetdata)
                {
                    targetdata.UpdateCurrency(data.param2, false);
                    targetdata.UpdateBonus(data.param3, true);
                    GetBankingServerManager().SendNotification("Sucesfully Update Playerdata from: " + targetdata.GetName(), sender, true);
                    //GetRPCManager().SendRPC("KR_BANKING", "AdminPlayerDataResponse", new Param5< int, int, string, string, string >( targetdata.GetBankCredit(), targetdata.GetBonusAmount(), targetdata.GetName(), targetdata.GetSteamID(), targetdata.GetClanID()), true, sender);
                }
                else
                {
                    GetBankingServerManager().SendNotification("Error while loading playerdata from: " + targetsPlainId, sender, true);
                }
            }
            else
            {
                GetBankingServerManager().SendNotification("You dont have enought permission to do that!", sender, true);
            }
        }
    }

    void AdminRequestServerSettings(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                GetRPCManager().SendRPC("KR_BANKING", "AdminServerConfigResponse", new Param1< ref KR_BankingConfigManager >( GetKR_BankingServerConfig() ), true, sender);
            }
            else
            {
                GetBankingServerManager().SendNotification("You dont have enought permission to do that! Permission needed: m_CanUseServerConfig", sender, true);
            }
        }
    }

    void AdminReloadConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {

            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                GetBankingServerManager().InternalConfigReload();
                array<Man> allPlayers = new array<Man>;
                GetGame().GetPlayers(allPlayers);
                PlayerBase player;
                foreach(Man curman : allPlayers)
                {
                    if(Class.CastTo(player, curman))
                    {
                        GetBankingServerManager().SendConfigToClient(player.GetIdentity());
                    }
                }
                GetBankingServerManager().SendNotification("Config reloadet & sended to every connected client!", sender);
            }
        }
    }

    void AdminDeleteATM(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<int> data;
            if(!ctx.Read(data)) return;
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }

                ref ATMPosition CurrentATM;
                CurrentATM = GetKR_BankingServerConfig().ATM.Get(data.param1);
                if(CurrentATM)
                {
                    array<Object> excluded_objects = new array<Object>;
                    array<Object> nearby_objects = new array<Object>;
                    KR_BankingATM atm;
                    GetGame().IsBoxColliding(CurrentATM.GetATMPosition() , CurrentATM.GetATMDirectory(), Vector(3, 5, 9), excluded_objects, nearby_objects);
                    if(nearby_objects.Count() > 0)
                    {
                        foreach(Object nerbyobj : nearby_objects)
                        {

                            if(Class.CastTo(atm, nerbyobj))
                            {
                                //Found an ATM on this position!
                                GetGame().ObjectDelete(atm);
                                GetKR_BankingServerConfig().RemoveATMWithIndex(data.param1);
                                GetBankingServerManager().SendNotification("ATM Deleted!", sender, true);
                                //Send him an RPC back with new Config data
                                //GetRPCManager().SendRPC("KR_BANKING", "AdminServerConfigResponse", new Param1< ref KR_BankingConfigManager >( GetKR_BankingServerConfig() ), true, sender);
                                return;
                            }
                        }
                    }
                    else
                    {
                        GetBankingServerManager().SendNotification("ATM On Map Already deleted failed to delete ATM!", sender, true);
                    }
                }
                else
                {
                    GetBankingServerManager().SendNotification("Error cant find this Atm!", sender, true);
                }
            }
        }
    }

    void AdminInsertATM(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param4<string, bool, vector, vector> data;
            if(!ctx.Read(data)) return;
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                GetKR_BankingServerConfig().AddATM(data.param1, data.param2, data.param3, data.param4);

                EntityAI SpawnedATM = EntityAI.Cast(GetGame().CreateObject(data.param1, data.param3));
                SpawnedATM.SetPosition(data.param3);
                SpawnedATM.SetOrientation(data.param4);
                KR_BankingATM AdvATMS;
                if(Class.CastTo(AdvATMS, SpawnedATM))
                {
                    AdvATMS.m_Banking_CanBeRobbed = data.param2;
                    Print("[Advanced Banking] -> ATM Was an Correct ATM!");
                }
            }
        }
    }

    void AdminUpdateServerConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<ref KR_BankingConfigManager> data;
            if(!ctx.Read(data)) return;
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                GetKR_BankingServerConfig().SetConfig(data.param1);
            }
        }
    }

    void AdminRequestMoneyDrop(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<int> data;
            if(!ctx.Read(data)) return;
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanUseMiscServer)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                
                array<Man> allOnlinePlayers = new array<Man>;
                GetGame().GetPlayers(allOnlinePlayers);

                PlayerBase ntarplayer;
                foreach(Man man : allOnlinePlayers)
                {
                    if(Class.CastTo(ntarplayer, man))
                    {
                        KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(ntarplayer.GetIdentity().GetPlainId(), ntarplayer.GetIdentity().GetName());
                        if(playerdata)
                        {
                            playerdata.DepositMoney(data.param1);
                            GetRPCManager().SendRPC("KR_BANKING", "MoneyDropRecvied", null, true, ntarplayer.GetIdentity());
                            GetBankingServerManager().SendNotification("You reviced an Money Drop (" + data.param1 + ") from Admin: " + sender.GetName(), ntarplayer.GetIdentity(), true);
                        }
                    }
                }
            }
        }
    }

    void AdminTeleportToPosition(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<vector> data;
            if(!ctx.Read(data)) return;
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }

                PlayerBase player = GetBankingServerManager().RemoteFindPlayer(sender.GetPlainId());
                if(player)
                {
                    player.SetPosition(data.param1);
                }
            }
        }
    }

    void AdminRequestClanDataWithID(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<string> data;
            if(!ctx.Read(data)) return;
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanUsePlayersClans)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                KR_JsonDatabaseHandler targetPlayer = KR_JsonDatabaseHandler.LoadPlayerData(data.param1); 
                if(!targetPlayer) return;
                ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(targetPlayer.GetClanID());
                if(clanData)
                {
                    GetRPCManager().SendRPC("KR_BANKING", "AdminClanDataReponse", new Param1< ref ClanDataBaseManager >( clanData ), true, sender);
                }
            }
        }
    }

    void AdminJoinClan(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<string> data;
            if(!ctx.Read(data)) return;

            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanUsePlayersClans)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }

                ClanDataBaseManager clandata = ClanDataBaseManager.LoadClanData(data.param1);
				if(clandata)
				{
                    KR_JsonDatabaseHandler adminsData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());
                    if(adminsData)
                    {
                        adminsData.SetClan(data.param1);
                        PermissionObject perms = new PermissionObject();
                        perms.GiveClanOwner();
                        GetBankingServerManager().AddClanMember(clandata, perms, sender.GetPlainId(), sender.GetName());
                        GetBankingServerManager().SendNotification("Sucesfully joined clan!", sender);
                    }
                }
            }
        }
    }

    void AdminDeleteClan(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<string> data;
            if(!ctx.Read(data)) return;

            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanUsePlayersClans)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }

                ClanDataBaseManager clandata = ClanDataBaseManager.LoadClanData(data.param1);
				if(clandata)
				{
                    for(int i = 0; i < clandata.GetClanMembers().Count(); i++)
                    {
                        string plainId = clandata.GetClanMembers().Get(i).GetPlainID();
                        if(plainId)
                        {
                            KR_JsonDatabaseHandler membersData = KR_JsonDatabaseHandler.LoadPlayerData(plainId);
                            if(membersData)
                            {
                                membersData.SetClan("NONE");
                            }
                        }
                    }
                    clandata.DeleteClan();
                    GetBankingServerManager().SendNotification("Sucesfully deleted clan!", sender);
                }
            }
        }
    }

    //!This returns the Admin Object of a user! if a user is no admin it returns null.
    ref AdminUsers GetAdminByPlainId(string PlainId)
    {
        foreach(AdminUsers admin: GetKR_AdminConfig().m_Admins)
        {
            if(PlainId == PlainId)
            {
                return admin;
            }
        }
        return null;
    }
};