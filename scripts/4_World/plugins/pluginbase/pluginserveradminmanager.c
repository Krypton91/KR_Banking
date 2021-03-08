class BankingAdminManager extends PluginBase
{
    void BankingAdminManager()
    {
        Print("[Advanced Banking] -> Internal Admin Module starting....");
        Print("Try to load Admins from json!");
        Print(" Found: " + GetKR_AdminConfig().m_Admins.Count().ToString());
        foreach(AdminUsers admin: GetKR_AdminConfig().m_Admins)
        {
            Print("Found Admin with name: " + admin.m_PlainID);
        }
        Print("Admin Module loadet!");
        Print("============DEBUG END!! =============");

        GetRPCManager().AddRPC("KR_BANKING", "AdminDataRequest", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminRequestPlayerdata", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminRequestServerSettings", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("KR_BANKING", "AdminSafePlayerdata", 	this, SingleplayerExecutionType.Server);
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