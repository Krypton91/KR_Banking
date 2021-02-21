class KR_BankingMenu extends UIScriptedMenu
{
    protected bool              m_IsBankingMenuInitialized = false;
    protected bool              m_IsBankingMenuOpen = false;
    protected Widget            m_OwnBankAccountTab;
    protected Widget            m_ClanBankAccountTab;
    protected Widget            m_YesNoMessage;
    protected Widget            m_TransferTab;
    protected ButtonWidget      m_CloseUiBtn;
    protected ButtonWidget      m_BankAccBtn;
    protected ButtonWidget      m_ClanAccBtn;
    protected ButtonWidget      m_WithdrawOwnAccBtn;
    protected ButtonWidget      m_DepositOwnAccBtn;
    protected ButtonWidget      m_TransferBtn;
    protected ButtonWidget      m_YesConfirmBtn;
    protected ButtonWidget      m_NoConfirmBtn;
    protected ButtonWidget      m_BtnClanSettings;
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
            m_TransferTab                   = Widget.Cast(layoutRoot.FindAnyWidget("PDABodyTransfer"));
            m_YesNoMessage                  = Widget.Cast(layoutRoot.FindAnyWidget("PanelYesNoMsg"));
            m_CloseUiBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CloseInvi"));
            m_BankAccBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabBank"));
            m_ClanAccBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabClanBank"));
            m_WithdrawOwnAccBtn             = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnWitdraw"));
            m_DepositOwnAccBtn              = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnDeposit"));
            m_TransferBtn                   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabTransfer"));
            m_YesConfirmBtn                 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget2"));
            m_NoConfirmBtn                  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget3"));
            m_BtnClanSettings               = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnClanSettings"));
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
            case m_TransferBtn:
                SwitchTab(3);
                break;
            case m_WithdrawOwnAccBtn:
                HandleWitdrawMoneyFromBank(1);
                break;
            case m_DepositOwnAccBtn:
                HandleDepositMoney(1);
                break;
            case m_YesConfirmBtn:
                HandleTransferConfirm();
                break;
            case m_NoConfirmBtn:
                HandleTransferCancel();
                break;
        }

        return super.OnClick(w, x, y, button);
    }


    void HandleWitdrawMoneyFromBank(int mode)
    {
        int parsedMoney = m_OwnAccInputBox.GetText().ToInt();
        if(parsedMoney)
            GetBankingClientManager().RequestRemoteToWitdraw(parsedMoney, mode);
    }
    
    void HandleDepositMoney(int mode)
    {
        int parsedMoney = m_OwnAccInputBox.GetText().ToInt();
        if(parsedMoney)
            GetBankingClientManager().RequestRemoteToDeposit(parsedMoney, mode);
    }

    void HandleTransferConfirm()
    {

    }

    void HandleTransferCancel()
    {
        m_YesNoMessage.Show(false);
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
                m_TransferTab.Show(false);
                break;
            case 2:
                m_ClanBankAccountTab.Show(true);
                m_OwnBankAccountTab.Show(false);
                m_TransferTab.Show(false);
                break;
            case 3:
                m_TransferTab.Show(true);
                m_OwnBankAccountTab.Show(false);
                m_ClanBankAccountTab.Show(false);
                break;
        }
    }

    void UpdateUI()//If you add UI stuff what needs to be updated from remote you can just add this here!
    {
        m_OwnedCurrencyLabel.SetText(" " + GetBankingClientManager().GetBankCredits());
        m_OnPlayerCurrencyLabel.SetText(" " + GetBankingClientManager().GetPlayerCurrencyAmount().ToString());
    }

    override void OnShow()
	{
        UpdateUI();

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

    void ClearPlayerInputBox()
    {
        m_OwnAccInputBox.SetText("");
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
};