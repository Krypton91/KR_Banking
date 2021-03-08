class BankingClientAdminManager extends PluginBase
{
    protected ref KR_AdminMenu                    m_AdminMenu;
    protected ref AdminPermissions                m_ClientPermissions;

    void BankingClientAdminManager()
    {
        GetRPCManager().AddRPC("KR_BANKING", "AdminDataResponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("KR_BANKING", "AdminPlayerDataResponse", this, SingleplayerExecutionType.Client);
    }

    void ~BankingClientAdminManager()
    {
        IsBankingAdminDataRecived = false;
        IsBankingAdmin = false;
    }

    //======= ADMIN RPC Handle =========
    void AdminDataResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param2<ref AdminPermissions, bool> data;
        if(!ctx.Read(data)) return;
        if(type == CallType.Client)
        {
            m_ClientPermissions = data.param1;
            IsBankingAdminDataRecived = true;
            IsBankingAdmin = data.param2;//We set this variable from remote.
            OpenBankingAdmin();
        }
    }

    void AdminPlayerDataResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param5<int, int, string, string, string> data;
        if(!ctx.Read(data)) return;
        if(type == CallType.Client)
        {
            if(m_AdminMenu)
                m_AdminMenu.UpdatePlayerCard(data.param1, data.param2, data.param3, data.param4, data.param5);
        }
    }

    void UpdatePlayerlist()
    {
        if(m_AdminMenu)
            m_AdminMenu.InvokePlayerList();
    }

    void OpenBankingAdmin()
    {
        if(!IsBankingAdminDataRecived)
        {
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

    void RequestSavePlayerData(string PlainID, int newBankAmount, int newBonusAmount)
    {
        GetRPCManager().SendRPC("KR_BANKING", "AdminSafePlayerdata", new Param3<string, int, int>(PlainID, newBankAmount, newBonusAmount), true);
    }

    void RequestPlayerdata(int PlayerArrayIndex)
    {
        string trgstid = GetBankingClientManager().GetOnlinePlayers().Get(PlayerArrayIndex).plainid;
        if(trgstid)
            GetRPCManager().SendRPC("KR_BANKING", "AdminRequestPlayerdata", new Param1<string>(trgstid), true);
    }

    void RequestPlayerdataWithId(string PlainID)
    {
        if(PlainID)
            GetRPCManager().SendRPC("KR_BANKING", "AdminRequestPlayerdata", new Param1<string>(PlainID), true);
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