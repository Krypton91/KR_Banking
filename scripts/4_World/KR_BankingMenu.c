class KR_BankingMenu extends UIScriptedMenu
{
    protected bool              m_IsBankingMenuInitialized = false;
    protected bool              m_IsBankingMenuOpen = false;
    protected Widget            m_OwnBankAccountTab;
    protected Widget            m_ClanBankAccountTab;
    protected ButtonWidget      m_CloseUiBtn;
    protected ButtonWidget      m_BankAccBtn;
    protected ButtonWidget      m_ClanAccBtn;
    protected ButtonWidget      m_WithdrawOwnAccBtn;
    protected ButtonWidget      m_DepositOwnAccBtn;
    protected EditBoxWidget     m_OwnAccInputBox;
    protected TextWidget        m_OwnedCurrencyLabel;
    protected TextWidget        m_OnPlayerCurrencyLabel;

    void KR_BankingMenu()
    {
       
    }

    override Widget Init()
    {
        if(!m_IsBankingMenuInitialized)
        {
            layoutRoot                      = GetGame().GetWorkspace().CreateWidgets("KR_Banking/GUI/layouts/BankingMenu.layout");
            m_OwnBankAccountTab             = Widget.Cast(layoutRoot.FindAnyWidget("PanelBankAcc"));
            m_ClanBankAccountTab            = Widget.Cast(layoutRoot.FindAnyWidget("PanelClanAcc"));
            m_CloseUiBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CloseInvi"));
            m_BankAccBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabBank"));
            m_ClanAccBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabClanBank"));
            m_WithdrawOwnAccBtn             = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnWitdraw"));
            m_DepositOwnAccBtn              = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnDeposit"));
            m_OwnAccInputBox                = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxWidget0"));
            m_OwnedCurrencyLabel            = TextWidget.Cast(layoutRoot.FindAnyWidget("BankAmountValueText"));
            m_OnPlayerCurrencyLabel         = TextWidget.Cast(layoutRoot.FindAnyWidget("TextCashOnPlayer"));

            m_IsBankingMenuInitialized = true;
        }

        return layoutRoot;
    }

    override bool OnClick(Widget w, int x, int y, int button) 
    {
        switch(w)
        {
            case m_CloseUiBtn:
                GetGame().GetUIManager().Back();
                break;
            case m_BankAccBtn:
                SwitchTab(1);
                break;
            case m_ClanAccBtn:
                SwitchTab(2);
                break;
        }

        return super.OnClick(w, x, y, button);
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
                m_OwnBankAccountTab.Show(true);
                m_ClanBankAccountTab.Show(false);
                break;
            case 2:
                m_OwnBankAccountTab.Show(false);
                m_ClanBankAccountTab.Show(true);
                break;
        }
    }

    void UpdatePlayersTab()
    {
        m_OwnedCurrencyLabel.SetText(" " + GetBankingClientManager().GetBankCredits());
    }

    override void OnShow()
	{
        UpdatePlayersTab();
        
		super.OnShow();

		PPEffects.SetBlurMenu(0.5);

		GetGame().GetInput().ChangeGameFocus(1);

		SetFocus( layoutRoot );

		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);

        GetGame().GetUIManager().ShowUICursor( true );
	}

    bool IsBankingMenuOpen()
    {
        return m_IsBankingMenuOpen;
    }
    void SetIsBankingMenuOpen(bool visible)
    {
        m_IsBankingMenuOpen = visible;
    }

    override void OnHide()
	{
		super.OnHide();

		PPEffects.SetBlurMenu(0);

		GetGame().GetInput().ResetGameFocus();

		GetGame().GetMission().PlayerControlEnable(false);

        GetGame().GetUIManager().ShowUICursor( false );
        SetIsBankingMenuOpen( false );
		Close();
	}
}