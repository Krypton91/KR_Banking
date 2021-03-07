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
    }

    void AdminDataRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Print("Sender: " + sender.GetName() + " requested admin data...");
        if(type == CallType.Server)
        {
            foreach(AdminUsers admin: GetKR_AdminConfig().m_Admins)
            {
                if(sender.GetPlainId() == admin.m_PlainID)
                {
                    GetRPCManager().SendRPC("KR_BANKING", "AdminDataResponse", new Param2< ref AdminPermissions, bool >( admin.m_permissions, true ), true, sender);
                    return;
                }
            }
            GetRPCManager().SendRPC("KR_BANKING", "AdminDataResponse", new Param2< ref AdminPermissions, bool >( null, false ), true, sender);
        }
    }
};