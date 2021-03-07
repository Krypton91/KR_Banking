class KR_AdminMenu extends UIScriptedMenu
{
    protected bool                      m_IsAdminMenuInitialized = false;
    protected bool                      m_IsAdminMenuOpened;

    protected Widget                    m_AdminMenu;
    protected Widget                    m_ATMspots;
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


    protected ButtonWidget              m_ServerSettingsButton;
    protected ButtonWidget              m_PlayerManagerButton;
    protected ButtonWidget              m_ClanManagerButton;
    protected ButtonWidget              m_SaveButton;
    protected ButtonWidget              m_ReloadButton;
    protected ButtonWidget              m_CloseButton;
    protected ButtonWidget              m_SearchOfflineID;


    protected TextListboxWidget         m_PlayersList;
    protected TextListboxWidget         m_SearchBarPlayers;


    override Widget Init ()
    {

        if(!m_IsAdminMenuInitialized)
        {
            layoutRoot                      = GetGame().GetWorkspace().CreateWidgets("KR_Banking/GUI/layouts/BankingAdminMenu.layout");

            m_GeneralSetting                =  Widget.Cast(layoutRoot.FindAnyWidget("GroupBoxGernal"));
            m_ATMspots                      =  Widget.Cast(layoutRoot.FindAnyWidget("GroupBoxATM"));
            m_ServerLogSetting              =  Widget.Cast(layoutRoot.FindAnyWidget("GroupBoxLogs"));
            m_ServerGeneralSettings         =  Widget.Cast(layoutRoot.FindAnyWidget("PanelTabServerSettings"));
            m_ServerPlayerManager           =  Widget.Cast(layoutRoot.FindAnyWidget("PanelTabPlayerManager"));
            m_ServerClanManager             =  Widget.Cast(layoutRoot.FindAnyWidget("PanelWidgetClanManager"));


            m_PlayerName                    =  TextWidget.Cast(layoutRoot.FindAnyWidget("PlayerName"));
            m_SteamIDBox                    =  TextWidget.Cast(layoutRoot.FindAnyWidget("SteamIDBox"));
            m_ClanIdBox                     =  TextWidget.Cast(layoutRoot.FindAnyWidget("ClanIDBox"));

            m_AtmAmount                     =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("BankOnAtm"));
            m_Bonus                         =  EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Bonus"));




            m_ServerSettingsButton          =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnServerS"));
            m_PlayerManagerButton           =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnPlManager"));
            m_ClanManagerButton             =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnClManager"));
            m_SaveButton                    =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnClManager3"));
            m_ReloadButton                  =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnClManager4"));
            m_CloseButton                   =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget0"));
            m_SearchOfflineID               =  ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnSearchOfflineID"));

            m_PlayersList                   =  TextListboxWidget.Cast(layoutRoot.FindAnyWidget("TextListboxPlayers"));
            m_SearchBarPlayers              =  TextListboxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxPlayersFind"));:


            m_IsAdminMenuInitialized = true;
        }

        return layoutRoot;
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
            case m_SaveButton:
                //HandleSave();
                break;
            case m_ReloadButton:
                //handleConfigReload();
                break;
        }

        return super.OnClick(w, x, y, button);
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


    void UpdatePlayerCard(int AmountOnBank, int BonusAmountOnBank, string TargetsPlainID, string ClanID)
    {
        
    }    

    
       
};