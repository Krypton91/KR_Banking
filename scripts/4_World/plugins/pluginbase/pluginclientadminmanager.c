class BankingClientAdminManager extends PluginBase
{
    protected ref KR_AdminMenu                    m_AdminMenu;
    protected ref AdminPermissions                m_ClientPermissions;

    void BankingClientAdminManager()
    {
        GetRPCManager().AddRPC("KR_BANKING", "AdminDataResponse", this, SingleplayerExecutionType.Client);
    }

    void ~BankingClientAdminManager()
    {
        IsBankingAdminDataRecived = false;
        IsBankingAdmin = false;
    }

    //======= ADMIN RPC Handle =========
    void AdminDataResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Print("Admin Data from Remote recived!");
        Param2<ref AdminPermissions, bool> data;
        if(!ctx.Read(data)) return;
        if(type == CallType.Client)
        {
            m_ClientPermissions = data.param1;
            IsBankingAdminDataRecived = true;
            IsBankingAdmin = data.param2;//We set this variable from remote.
        }
    }

    void OpenBankingAdmin()
    {
        if(!IsBankingAdminDataRecived)
        {
            GetBankingClientManager().SendNotification("Please wait for server response....");
            GetRPCManager().SendRPC("KR_BANKING", "AdminDataRequest", null, true);
            return;
        }
        else
        {
            //Open the Menu... 
            if(GetGame().GetUIManager().GetMenu() == NULL && m_AdminMenu == null)
            {
                m_AdminMenu = KR_AdminMenu.Cast(GetGame().GetUIManager().EnterScriptedMenu(KR_BANKING_ADMIN_MENU, null));
                m_AdminMenu.SetIsAdminMenuOpened(true);
            }
            else
            {
                if(GetGame().GetUIManager().GetMenu() == NULL && !m_AdminMenu.IsAdminMenuOpen())
                {
                    GetGame().GetUIManager().ShowScriptedMenu(m_AdminMenu, NULL);
                    m_AdminMenu.SetIsAdminMenuOpened(true);
                }
            }
        }
    }

    ref AdminPermissions GetAdminPermissions()
    {
        return m_ClientPermissions;
    }
};
BankingClientAdminManager GetBankingClientAdminManager()
{
	return BankingClientAdminManager.Cast(GetPluginManager().GetPluginByType(BankingClientAdminManager));
};