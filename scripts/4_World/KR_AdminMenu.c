class KR_AdminMenu extends UIScriptedMenu
{
    protected bool                      m_IsAdminMenuInitialized = false;
    protected bool                      m_IsAdminMenuOpened;
    protected float                     m_UICooldownTimer;
    protected float                     m_UIUpdateTimer;

    protected Widget                    m_AdminMenu;
    protected Widget                    m_ATMspotlol;
    protected Widget                    m_ServerLogSetting;
    protected Widget                    m_GeneralSetting;
    

    protected Widget                    m_ServerGeneralSettings;
    protected Widget                    m_ServerPlayerManager;
    protected Widget                    m_ServerClanManager;



    protected TextWidget                m_PlayerName;
    protected TextWidget                m_SteamIDBox;
    protected TextWidget                m_ClanIdBox;

    protected EditBoxWidget             m_AtmAmount;
    protected EditBoxWidget             m_Bonus;

    protected EditBoxWidget             m_StarterEdit;
    protected EditBoxWidget             m_MaxATMEdit;
    protected EditBoxWidget             m_ClanCreateCostsEdit;
    protected EditBoxWidget             m_AddPlayerCosts;
    protected EditBoxWidget             m_MaxClanStorageEdit;
    protected EditBoxWidget             m_MaxClanPlayerEdit;
    protected EditBoxWidget             m_SearchWithSteamIDBox;
    protected EditBoxWidget             m_MinTransferEdit;
    protected EditBoxWidget             m_TransferFeesEdit;
    protected EditBoxWidget             m_MinPlayerForRobEdit;
    protected EditBoxWidget             m_MinMoneyForRobEdit;
    protected EditBoxWidget             m_MaxMoneyForRobEdit;
    protected EditBoxWidget             m_SecondsForRobEdit;
    protected EditBoxWidget             m_PaycheckValueEdit;
    protected EditBoxWidget             m_PayCheckMinPlayersEdit;
    protected EditBoxWidget             m_WebHookUrlEdit;
    protected EditBoxWidget             m_SearchPlayerInListBox;


    protected CheckBoxWidget            m_CanBeRobbedCheck;
    protected CheckBoxWidget            m_UseWebhookCheck;
    protected CheckBoxWidget            m_LogDepositCheck;
    protected CheckBoxWidget            m_LogWithdrawCheck;
    protected CheckBoxWidget            m_LogClanDepositCheck;
    protected CheckBoxWidget            m_LogClanWithdrawCheck;

    protected CheckBoxWidget            m_ClanAccountsActiveCheck;
    protected CheckBoxWidget            m_ATMRobActiveCheck;
    protected CheckBoxWidget            m_RobMessageActiveCheck;
    protected CheckBoxWidget            m_PaycheckMessageActiveCheck;
    protected CheckBoxWidget            m_CanAddToFullAccCheck;
    protected CheckBoxWidget            m_NeedBankCardToOpenCheck;


    protected ButtonWidget              m_ServerSettingsButton;
    protected ButtonWidget              m_PlayerManagerButton;
    protected ButtonWidget              m_ClanManagerButton;
    protected ButtonWidget              m_BtnSavePlayerData;
    protected ButtonWidget              m_ReloadButton;
    protected ButtonWidget              m_CloseButton;
    protected ButtonWidget              m_SearchOfflineID;
    protected ButtonWidget              m_OpenInClanManagerButton;


    protected TextListboxWidget         m_PlayersList;
    protected TextListboxWidget         m_SearchBarPlayers;
    protected TextListboxWidget         m_ATMspots;

    protected int                       m_LastSelectedPlayerIndex;
    protected string                    m_LastTargetsSteamID;

    override Widget Init ()
    {

        if(!m_IsAdminMenuInitialized)
        {
            layoutRoot                      = GetGame().GetWorkspace().CreateWidgets("KR_Banking/GUI/layouts/BankingAdminMenu.layout");

            m_GeneralSetting                =  Widget.Cast(layoutRoot.FindAnyWidget("GroupBoxGernal"));
            m_ATMspotlol                    =  Widget.Cast(layoutRoot.FindAnyWidget("GroupBoxATM"));
            m_ServerLogSetting              =  Widget.Cast(layoutRoot.FindAnyWidget("GroupBoxLogs"));
            m_ServerGeneralSettings         =  Widget.Cast(layoutRoot.FindAnyWidget("PanelTabServerSettings"));
            m_ServerPlayerManager           =  Widget.Cast(layoutRoot.FindAnyWidget("PanelTabPlayerManager"));
            m_ServerClanManager             =  Widget.Cast(layoutRoot.FindAnyWidget("PanelWidgetClanManager"));


            m_PlayerName                    =  TextWidget.Cast(layoutRoot.FindAnyWidget("PlayerName"));
            m_SteamIDBox                    =  TextWidget.Cast(layoutRoot.FindAnyWidget("SteamIDBox"));
            m_ClanIdBox                     =  TextWidget.Cast(layoutRoot.FindAnyWidget("ClanIDBox"));


            m_StarterEdit                   =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Starter"));
            m_MaxATMEdit                    =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Max"));
            m_ClanCreateCostsEdit           =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("ClanCosts"));
            m_AddPlayerCosts                =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("AddCosts"));
            m_MaxClanStorageEdit            =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("MaxClanMoney"));
            m_MaxClanPlayerEdit             =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("MaxClanMember"));
            m_MinTransferEdit               =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("MinTransfer"));
            m_TransferFeesEdit              =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("TransferFees"));
            m_MinPlayerForRobEdit           =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("MinPlayerRob"));
            m_MinMoneyForRobEdit            =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("MinMoneyRob"));
            m_MaxMoneyForRobEdit            =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("MaxMoneyRob"));
            m_SecondsForRobEdit             =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("SecondsRob"));
            m_PaycheckValueEdit             =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("PaycheckValue"));
            m_PayCheckMinPlayersEdit        =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("PaycheckMinPlayers"));
            m_WebHookUrlEdit                =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("WebhookLink"));
            m_AtmAmount                     =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("BankOnAtm"));
            m_Bonus                         =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Bonus"));
            m_SearchWithSteamIDBox          =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("SearchOfflineBox"));
            m_SearchPlayerInListBox         =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxPlayersFind"));
        




            m_ServerSettingsButton          =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnServerS"));
            m_PlayerManagerButton           =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnPlManager"));
            m_ClanManagerButton             =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnClManager"));
            m_BtnSavePlayerData             =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnSavePlayer"));
            m_ReloadButton                  =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnClManager4"));
            m_CloseButton                   =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget0"));
            m_SearchOfflineID               =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnSearchOfflineID"));
            m_OpenInClanManagerButton       =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("OpenInClanManagerBtn"));



            m_CanBeRobbedCheck              =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CanbeRobbed"));
            m_UseWebhookCheck               =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("UseWebhook"));
            m_LogDepositCheck               =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("Log1"));
            m_LogWithdrawCheck              =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("Log2"));
            m_LogClanDepositCheck           =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("Log3"));
            m_LogClanWithdrawCheck          =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("Log4"));

            m_ClanAccountsActiveCheck       =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("ClanActive"));
            m_ATMRobActiveCheck             =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("AtmRobActive"));
            m_RobMessageActiveCheck         =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("RobMessageActive"));
            m_PaycheckMessageActiveCheck    =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("PaycheckMessageActive"));
            m_CanAddToFullAccCheck          =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CanAddToFullAccount"));
            m_NeedBankCardToOpenCheck       =  CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("NeedBankCardToOpen"));


            m_PlayersList                   =  TextListboxWidget.Cast(layoutRoot.FindAnyWidget("TextListboxPlayers"));
            m_SearchBarPlayers              =  TextListboxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxPlayersFind"));
            m_ATMspots                      =  TextListboxWidget.Cast(layoutRoot.FindAnyWidget("AtmSpotList"));


            m_IsAdminMenuInitialized = true;
        }

        return layoutRoot;
    }

     override void Update(float timeslice)
    {
        super.Update(timeslice);

        if(m_UICooldownTimer > 0)
            m_UICooldownTimer -= timeslice;
        
        if(m_UIUpdateTimer >= 0.05)
        {
            if(m_LastSelectedPlayerIndex != m_PlayersList.GetSelectedRow())
            {
                m_LastSelectedPlayerIndex = m_PlayersList.GetSelectedRow();
                GetBankingClientAdminManager().RequestPlayerdata(m_LastSelectedPlayerIndex);
            }
            m_UIUpdateTimer = 0;
        }
        else
        {
            m_UIUpdateTimer += timeslice;
        }
    }

    //Gets triggert from PluginAdminClient, after RPC response from remote! :)
    void UpdatePlayerCard(int AmountOnBank, int BonusAmountOnBank, string PlayersName, string TargetsPlainID, string ClanID)
    {
        m_PlayerName.SetText(PlayersName);
        m_SteamIDBox.SetText(TargetsPlainID);
        m_LastTargetsSteamID = TargetsPlainID;
        m_ClanIdBox.SetText(ClanID);
        m_AtmAmount.SetText(AmountOnBank.ToString());
        m_Bonus.SetText(BonusAmountOnBank.ToString());
    }

    override bool OnChange(Widget w, int x, int y, bool finished)
	{
        switch(w)
        {
            case m_SearchPlayerInListBox:
                SearchPlayer();
                break;
        }
        return super.OnChange(w, x, y, finished);
    }
    override bool OnClick(Widget w, int x, int y, int button) 
    {
        switch(w)
        {
            case m_CloseButton:
                CloseAdminMenu();
                break;
            case m_PlayerManagerButton:
                SwitchTab(2);
                break;
            case m_ServerSettingsButton:
                SwitchTab(1);
                break;
            case m_ClanManagerButton:
                SwitchTab(3);
                break;
            case m_BtnSavePlayerData:
                HandlePlayerDataSave();
                break;
            case m_ReloadButton:
                //handleConfigReload(); NOT IMPLEMENTED YET
                break;
            case m_OpenInClanManagerButton:
               // handleShowPlayerClan(); NOT IMPLEMENTED YET
                SwitchTab(3);
                break;
            case m_SearchOfflineID:
                GetBankingClientAdminManager().RequestPlayerdataWithId(m_SearchWithSteamIDBox.GetText());
                break;

        }

        return super.OnClick(w, x, y, button);
    }

    void SearchPlayer()
    {
        string searchTerm = m_SearchPlayerInListBox.GetText();
        if(searchTerm == ""){
            InvokePlayerList();
        }
        searchTerm.ToLower();
        m_PlayersList.ClearItems();

        for(int i = 0; i < GetBankingClientManager().GetOnlinePlayers().Count(); i++)
        {
            string trgname = GetBankingClientManager().GetOnlinePlayers().Get(i).name;
            trgname.ToLower();
            if(trgname.Contains(searchTerm))
            {
                //Add 
                m_PlayersList.AddItem(" " + GetBankingClientManager().GetOnlinePlayers().Get(i).name, NULL, 0);
            }
        }
    }

    void HandlePlayerDataSave()
    {
        int newBankAmount = m_AtmAmount.GetText().ToInt();
        int newBankBonus =  m_Bonus.GetText().ToInt();
        //Todo add nullchecks.
        GetBankingClientAdminManager().RequestSavePlayerData(m_LastTargetsSteamID, newBankAmount, newBankBonus);
    }

    void CloseAdminMenu()
    {
        GetGame().GetUIManager().Back();
    }

    void SwitchTab(int TabIndex)
    {
        int lastTab;
        if(lastTab == TabIndex)
            return;
        lastTab = TabIndex;
        switch(TabIndex)
        {
            case 1:
                m_ServerGeneralSettings.Show(true);
                m_ServerPlayerManager.Show(false);
                m_ServerClanManager.Show(false);
                break;
            case 2:
                m_ServerPlayerManager.Show(true);
                m_ServerClanManager.Show(false);
                m_ServerGeneralSettings.Show(false);
                break;
            case 3:
                m_ServerClanManager.Show(true);
                m_ServerPlayerManager.Show(false);
                m_ServerGeneralSettings.Show(false);
                break;
        }
    }

    void InvokePlayerList()
    {
        m_PlayersList.ClearItems();
        for(int i = 0; i < GetBankingClientManager().GetOnlinePlayers().Count(); i++)
        {
            m_PlayersList.AddItem(" " + GetBankingClientManager().GetOnlinePlayers().Get(i).name, NULL, 0);
        }
    }

    bool IsAdminMenuOpen()
    {
        return m_IsAdminMenuOpened;
    }

    void SetIsAdminMenuOpened(bool visiblestate)
    {
        m_IsAdminMenuOpened= visiblestate;
    }

    override void OnShow()
	{
        GetBankingClientManager().RequestOnlinePlayers(true);

		super.OnShow();

		PPEffects.SetBlurMenu(0.5);

		GetGame().GetInput().ChangeGameFocus(1);

		SetFocus( layoutRoot );

		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);

        GetGame().GetUIManager().ShowUICursor( true );
	}

    override void OnHide()
	{
		super.OnHide();

		PPEffects.SetBlurMenu(0);

		GetGame().GetInput().ResetGameFocus();

		GetGame().GetMission().PlayerControlEnable(false);

        GetGame().GetUIManager().ShowUICursor( false );

        SetIsAdminMenuOpened( false );

		Close();
	}
};