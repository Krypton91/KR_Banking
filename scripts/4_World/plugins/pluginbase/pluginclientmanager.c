class PluginKrBankingClientManager extends PluginBase
{
    protected ref KR_BankingMenu    m_BankingMenu;
    protected int                   m_PlayersCurrency;
    protected bool                  m_IsFirstRequest = true;
    protected bool                  m_IsWaitingForServersResponse;
    void PluginKrBankingClientManager()
    {
        Init();
    }

    void Init()
    {
        GetRPCManager().AddRPC("KR_BANKING","PlayerDataResponse", this, SingleplayerExecutionType.Client);
    }

    void PlayerDataResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            Param1<int> data;
            if ( !ctx.Read( data ) ) return;
            m_PlayersCurrency = data.param1;

            if(m_IsFirstRequest)
            {
                m_IsFirstRequest = false;
                m_IsWaitingForServersResponse = false;
                OpenBankingMenu();
            }
        }
    }
    
    void OpenBankingMenu()
    {
        if(m_IsWaitingForServersResponse) return;
        if(m_IsFirstRequest)
        {
            GetRPCManager().SendRPC("KR_BANKING", "PlayerDataRequest", null, true);
            m_IsWaitingForServersResponse = true;
            return;//wait for servers response!
        }
        if(GetGame().GetUIManager().GetMenu() == NULL && m_BankingMenu == null)
        {
            m_BankingMenu = KR_BankingMenu.Cast(GetGame().GetUIManager().EnterScriptedMenu(KR_BANKING_MENU, null));
            m_BankingMenu.SetIsBankingMenuOpen(false);
        }
        else
        {
            if(GetGame().GetUIManager().GetMenu() == NULL && !m_BankingMenu.IsBankingMenuOpen())
            {
                GetGame().GetUIManager().ShowScriptedMenu (m_BankingMenu, NULL);
                m_BankingMenu.SetIsBankingMenuOpen(true);
            }
        }
    }

    int GetBankCredits()
    {
        return m_PlayersCurrency;
    }

    bool WaitForServerResponse()
    {
        return m_IsWaitingForServersResponse;
    }
};
PluginKrBankingClientManager GetBankingClientManager()
{
	return PluginKrBankingClientManager.Cast(GetPluginManager().GetPluginByType(PluginKrBankingClientManager));
}