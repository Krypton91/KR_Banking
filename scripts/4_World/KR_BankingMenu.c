class KR_BankingMenu extends UIScriptedMenu
{
    protected bool                      m_IsBankingMenuInitialized = false;
    protected bool                      m_IsBankingMenuOpen = false;
    protected bool                      m_IsYesNoVisible = false;
    protected Widget                    m_OwnBankAccountTab;
    protected Widget                    m_ClanBankAccountTab;
    protected Widget                    m_YesNoMessage;
    protected Widget                    m_TransferTab;
    protected Widget                    m_ClanSettings;
    protected Widget                    m_PanelAdmin;
    protected Widget                    m_PanelPerms;
    protected Widget                    m_PanelNewClan;
    protected Widget                    m_PanelNewClanPrice;

    protected ButtonWidget              m_CloseUiBtn;
    protected ButtonWidget              m_BankAccBtn;
    protected ButtonWidget              m_ClanAccBtn;
    protected ButtonWidget              m_WithdrawOwnAccBtn;
    protected ButtonWidget              m_DepositOwnAccBtn;
    protected ButtonWidget              m_TransferBtn;
    protected ButtonWidget              m_YesConfirmBtn;
    protected ButtonWidget              m_NoConfirmBtn;
    protected ButtonWidget              m_BtnClanSettings;
    protected ButtonWidget              m_BtnKick;
    protected ButtonWidget              m_BtnAdd;
    protected ButtonWidget              m_BtnEdit;
    protected ButtonWidget              m_BtnBack;
    protected ButtonWidget              m_BtnSave;

    protected EditBoxWidget             m_OwnAccInputBox;
    protected TextWidget                m_OwnedCurrencyLabel;
    protected TextWidget                m_OnPlayerCurrencyLabel;
    protected TextWidget                m_PriceToCreate;

    protected TextListboxWidget         m_ListboxPlayers;
    protected TextListboxWidget         m_ListboxMember;
    protected TextListboxWidget         m_TransferPlayerList;

    protected MultilineTextWidget       m_YesNoMsgHeadline;
    protected MultilineTextWidget       m_YesNoMsgBody;

    protected CheckBoxWidget            m_CheckBoxWithdraw;
    protected CheckBoxWidget            m_CheckBoxDeposit;
    protected CheckBoxWidget            m_CheckBoxKick;
    protected CheckBoxWidget            m_CheckBoxAdd;
    protected CheckBoxWidget            m_CheckBoxPermissions;


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
            m_ClanSettings                  = Widget.Cast(layoutRoot.FindAnyWidget("PanelClanSettings"));
            m_PanelAdmin                    = Widget.Cast(layoutRoot.FindAnyWidget("PanelAdmin"));
            m_PanelPerms                    = Widget.Cast(layoutRoot.FindAnyWidget("PanelPerms"));
            m_PanelNewClan                  = Widget.Cast(layoutRoot.FindAnyWidget("PanelNewClan"));
            m_PanelNewClanPrice             = Widget.Cast(layoutRoot.FindAnyWidget("PanelNewClanPrice"));

            m_CloseUiBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CloseInvi"));
            m_BankAccBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabBank"));
            m_ClanAccBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabClanBank"));
            m_WithdrawOwnAccBtn             = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnWitdraw"));
            m_DepositOwnAccBtn              = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnDeposit"));
            m_TransferBtn                   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabTransfer"));
            m_YesConfirmBtn                 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget2"));
            m_NoConfirmBtn                  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget3"));
            m_BtnClanSettings               = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnClanSettings"));
            m_BtnKick                       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonKick"));
            m_BtnAdd                        = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonEdit0"));
            m_BtnEdit                       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonEdit"));
            m_BtnBack                       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonBack"));
            m_BtnSave                       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget0"));

            m_OwnAccInputBox                = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxWidget0"));
            m_OwnedCurrencyLabel            = TextWidget.Cast(layoutRoot.FindAnyWidget("BankAmountValueText"));
            m_OnPlayerCurrencyLabel         = TextWidget.Cast(layoutRoot.FindAnyWidget("TextCashOnPlayer"));
            m_PriceToCreate                 = TextWidget.Cast(layoutRoot.FindAnyWidget("TextWidgetPrice"));

            m_YesNoMsgHeadline              = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("TextHeadline0"));
            m_YesNoMsgBody                  = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("TextMessage0"));

            m_ListboxMember                 = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("TextListboxMember"));
            m_ListboxPlayers                = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("TextListboxPlayers"));
            m_TransferPlayerList            = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("TextListboxWidget0"));

            m_CheckBoxWithdraw              = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckboxWithdraw"));
            m_CheckBoxDeposit               = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckboxDeposit"));                  
            m_CheckBoxKick                  = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckboxKick"));
            m_CheckBoxAdd                   = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckboxInvite"));
            m_CheckBoxPermissions           = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckboxPermission"));


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
        GetBankingClientManager().RequestRemoteForTransfer("", 1);
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
                GetBankingClientManager().RequestOnlinePlayers();
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

    void InvokePlayerList()
    {
        for(int i = 0; i < GetBankingClientManager().GetOnlinePlayers().Count(); i++)
        {
            m_TransferPlayerList.AddItem(" " + GetBankingClientManager().GetOnlinePlayers().Get(i).name, NULL, 0);
            m_ListboxPlayers.AddItem(" " + GetBankingClientManager().GetOnlinePlayers().Get(i).name, NULL, 0);
        }
    }

    //!Creates a yes no Message.
    void CreateYesNoMessage(string Headline, string BodyMessage)
    {
        if(!m_IsYesNoVisible)
        {
            m_YesNoMsgHeadline.SetText(" " + Headline);
            m_YesNoMsgBody.SetText(" " + BodyMessage);
            m_YesNoMessage.Show(true);
            m_IsYesNoVisible = true;
        }
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