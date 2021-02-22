class KR_BankingMenu extends UIScriptedMenu
{
    protected bool                      m_IsBankingMenuInitialized = false;
    protected bool                      m_IsBankingMenuOpen = false;
    bool                                m_IsClanAccountFresh = false;

    protected float                     m_UIUpdateTimer = 0;
    protected float                     m_UICooldownTimer = 0;

    protected Widget                    m_OwnBankAccountTab;
    protected Widget                    m_ClanBankAccountTab;
    protected Widget                    m_YesNoMessage;
    protected Widget                    m_TransferTab;
    protected Widget                    m_ClanSettings;
    protected Widget                    m_PanelAdmin;
    protected Widget                    m_PanelPerms;
    protected Widget                    m_PanelNewClan;
    protected Widget                    m_PanelNewClanPrice;
    protected Widget                    m_PanelName;                

    protected ButtonWidget              m_CloseUiBtn;
    protected ButtonWidget              m_BankAccBtn;
    protected ButtonWidget              m_ClanAccBtn;
    protected ButtonWidget              m_WithdrawOwnAccBtn;
    protected ButtonWidget              m_WithdrawClan;
    protected ButtonWidget              m_DepositOwnAccBtn;
    protected ButtonWidget              m_DepositClanAccbtn;
    protected ButtonWidget              m_TransferBtn;
    protected ButtonWidget              m_YesConfirmBtn;
    protected ButtonWidget              m_NoConfirmBtn;
    protected ButtonWidget              m_BtnClanSettings;
    protected ButtonWidget              m_BtnKick;
    protected ButtonWidget              m_BtnAdd;
    protected ButtonWidget              m_BtnEdit;
    protected ButtonWidget              m_BtnBack;
    protected ButtonWidget              m_BtnSave;
    protected ButtonWidget              m_BtnSendTransfer;
    protected ButtonWidget              m_BtnFinallyCreate;
    protected ButtonWidget              m_BtnYesCreate;
    protected ButtonWidget              m_RobATMBtn;

    protected EditBoxWidget             m_OwnAccInputBox;
    protected EditBoxWidget             m_ClanAccInputBox;
    protected EditBoxWidget             m_TransferInputBox;
    protected EditBoxWidget             m_EditBoxClanName;
    protected EditBoxWidget             m_EditBoxClanTag;

    protected TextWidget                m_OwnedCurrencyLabel;
    protected TextWidget                m_OnPlayerCurrencyLabel;
    protected TextWidget                m_PriceToCreate;
    protected TextWidget                m_OnPlayerCurrencyLabel2;
    protected TextWidget                m_CurrentValueOnBank;
    protected TextWidget                m_CurrentMaxOnBank;
    protected TextWidget                m_ProgressText;
    protected TextWidget                m_CurrentProgressMinTxt;
    protected TextWidget                m_CurrentMaxProgressTxt;
    protected TextWidget                m_ProgressTextClan;
    protected TextWidget                m_ClanAmount;
    protected TextWidget                m_CashOnPlayer;

    protected TextListboxWidget         m_ListboxPlayers;
    protected TextListboxWidget         m_ListboxMember;
    protected TextListboxWidget         m_TransferPlayerList;
    protected TextListboxWidget         m_ClanLogs;

    protected MultilineTextWidget       m_YesNoMsgHeadline;
    protected MultilineTextWidget       m_YesNoMsgBody;

    protected CheckBoxWidget            m_CheckBoxWithdraw;
    protected CheckBoxWidget            m_CheckBoxDeposit;
    protected CheckBoxWidget            m_CheckBoxKick;
    protected CheckBoxWidget            m_CheckBoxAdd;
    protected CheckBoxWidget            m_CheckBoxPermissions;

    protected ProgressBarWidget         m_OwnBankAccountProgressbar;
    protected ProgressBarWidget         m_ClanBankAccountProgressbar;


    protected ref bankingplayerlistobj  m_target


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
            m_PanelName                     = Widget.Cast(layoutRoot.FindAnyWidget("PanelWidgetName"));

            m_CloseUiBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CloseInvi"));
            m_BankAccBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabBank"));
            m_ClanAccBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabClanBank"));
            m_WithdrawOwnAccBtn             = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnWitdraw"));
            m_WithdrawClan                  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnWitdrawClan"));
            m_DepositOwnAccBtn              = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnDeposit"));
            m_DepositClanAccbtn             = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnDepositClan"));
            m_TransferBtn                   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabTransfer"));
            m_YesConfirmBtn                 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget3"));
            m_NoConfirmBtn                  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget2"));
            m_BtnClanSettings               = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnClanSettings"));
            m_BtnKick                       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonKick"));
            m_BtnAdd                        = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonEdit0"));
            m_BtnEdit                       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonEdit"));
            m_BtnBack                       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonBack"));
            m_BtnSave                       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget0"));
            m_BtnSendTransfer               = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnSend"));
            m_BtnFinallyCreate              = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidgetCreate"));
            m_BtnYesCreate                  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget1"));
            m_RobATMBtn                     = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnRob"));

            m_OwnAccInputBox                = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxWidget0"));
            m_TransferInputBox              = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxWidget1"));
            m_ClanAccInputBox               = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("InputBoxClan"));
            m_EditBoxClanName               = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxWidgetName"));
            m_EditBoxClanTag                = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxWidgetTag"));


            m_OwnedCurrencyLabel            = TextWidget.Cast(layoutRoot.FindAnyWidget("BankAmountValueText"));
            m_OnPlayerCurrencyLabel         = TextWidget.Cast(layoutRoot.FindAnyWidget("TextCashOnPlayer"));
            m_PriceToCreate                 = TextWidget.Cast(layoutRoot.FindAnyWidget("TextWidgetPrice"));
            m_OnPlayerCurrencyLabel2        = TextWidget.Cast(layoutRoot.FindAnyWidget("BankAmountValueText0"));
            m_ProgressText                  = TextWidget.Cast(layoutRoot.FindAnyWidget("TextWidgetPercentage"));
            m_CurrentProgressMinTxt         = TextWidget.Cast(layoutRoot.FindAnyWidget("TextWidgetCurrent"));
            m_CurrentMaxProgressTxt         = TextWidget.Cast(layoutRoot.FindAnyWidget("TextWidgetMax"));
            m_ProgressTextClan              = TextWidget.Cast(layoutRoot.FindAnyWidget("TextWidgetPercentage0"));
            m_CashOnPlayer                  = TextWidget.Cast(layoutRoot.FindAnyWidget("TextCashOnPlayer0"));
            m_ClanAmount                    = TextWidget.Cast(layoutRoot.FindAnyWidget("BankClanAmmount"));

            m_YesNoMsgHeadline              = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("TextHeadline0"));
            m_YesNoMsgBody                  = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("TextMessage0"));

            m_ListboxMember                 = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("TextListboxMember"));
            m_ListboxPlayers                = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("TextListboxPlayers"));
            m_TransferPlayerList            = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("TextListboxWidget0"));
            m_ClanLogs                      = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("TextListboxWidgetLogs"));

            m_CheckBoxWithdraw              = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckBoxWithdraw"));
            m_CheckBoxDeposit               = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckBoxDeposit")); 
            m_CheckBoxKick                  = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckBoxKick"));
            m_CheckBoxAdd                   = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckBoxInvite"));
            m_CheckBoxPermissions           = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckBoxPermission"));

            m_OwnBankAccountProgressbar     = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("ProgressBarWidget0"));
            m_ClanBankAccountProgressbar    = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("ProgressBarWidgetClan"));


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
                if(m_UICooldownTimer > 0)
                {
                    //not that fast Todo: send Notify...
                    GetBankingClientManager().SendNotification("Not that fast bro!");
                    break;
                }

                m_UICooldownTimer = GetBankingClientManager().GetClientSettings().InteractDelay;
                HandleWitdrawMoneyFromBank(1);
                break;
            case m_DepositOwnAccBtn:
                if(m_UICooldownTimer > 0)
                {
                    //not that fast Todo: send Notify...
                    GetBankingClientManager().SendNotification("Not that fast bro!");
                    break;
                }

                m_UICooldownTimer = GetBankingClientManager().GetClientSettings().InteractDelay;
                HandleDepositMoney(1);
                break;
            case m_YesConfirmBtn:
                if(m_UICooldownTimer > 0)
                {
                    //not that fast Todo: send Notify...
                    GetBankingClientManager().SendNotification("Not that fast bro!");
                    break;
                }
                m_UICooldownTimer = GetBankingClientManager().GetClientSettings().InteractDelay;
                HandleTransferConfirm();
                break;
            case m_NoConfirmBtn:
                HandleTransferCancel();
                break;
            case m_BtnSendTransfer:
                if(m_UICooldownTimer > 0)
                {
                    //not that fast Todo: send Notify...
                    GetBankingClientManager().SendNotification("Not that fast bro!");
                    break;
                }
                m_UICooldownTimer = GetBankingClientManager().GetClientSettings().InteractDelay;

                int rowIndex = m_TransferPlayerList.GetSelectedRow();
                m_target = GetBankingClientManager().GetOnlinePlayers().Get(rowIndex);
                CreateYesNoMessage("Transfer Check","Are you sure you want to transfer " + m_TransferInputBox.GetText() + " to: " + m_target.name + " ?");
                break;
            case m_BtnYesCreate:
                int PriceToCreateClan = GetBankingClientManager().GetClientSettings().CostsToCreateClan;
                if(PriceToCreateClan < GetBankingClientManager().GetPlayerCurrencyAmount());
                    SpawnClanCreatePopup();
                break;
            case m_BtnFinallyCreate:
                if(m_UICooldownTimer > 0)
                {
                    //not that fast Todo: send Notify...
                    break;
                }
                m_UICooldownTimer = GetBankingClientManager().GetClientSettings().InteractDelay;

                HandleClanCreate();
                break;
            case m_DepositClanAccbtn:
                if(m_UICooldownTimer > 0)
                {
                    //not that fast Todo: send Notify...
                    GetBankingClientManager().SendNotification("Not that fast bro!");
                    break;
                }
                m_UICooldownTimer = GetBankingClientManager().GetClientSettings().InteractDelay;

                HandleDepositMoney(2);
                break;
            case m_WithdrawClan:
                if(m_UICooldownTimer > 0)
                {
                    //not that fast Todo: send Notify...
                    GetBankingClientManager().SendNotification("Not that fast bro!");
                    break;
                }
                m_UICooldownTimer = GetBankingClientManager().GetClientSettings().InteractDelay;

                HandleWitdrawMoneyFromBank(2);
                break;
            case m_BtnClanSettings:
                OpenClanSettings();
                break;
            case m_BtnBack:
                m_ClanSettings.Show(false);
                m_ClanBankAccountTab.Show(true);
                break;
            case m_BtnAdd:
                HandleAddMemberToClan();
                break;
            case m_BtnKick:
                HandleRemoveMemberFromClan(m_ListboxMember.GetSelectedRow());
                break;
            case m_BtnSave:
                HandleEditPermission(m_ListboxMember.GetSelectedRow());
                break;
        }
        return super.OnClick(w, x, y, button);
    }
    
    void OpenClanSettings()
    {
        GetBankingClientManager().RequestOnlinePlayers();

        m_ClanBankAccountTab.Show(false);
        m_ClanSettings.Show(true);

        PermissionObject perms = new PermissionObject();
        perms = GetBankingClientManager().GetClanPermission();
        if(perms)
        {
            if(perms.m_CanEdit)
            {
                m_PanelPerms.Show(true);
                if(perms.m_CanWithdraw)
                    m_CheckBoxWithdraw.Show(true);
                if(perms.m_CanDeposit)
                    m_CheckBoxDeposit.Show(true);
                if(perms.m_CanInvite)
                    m_CheckBoxAdd.Show(true);
                if(perms.m_CanKick)
                    m_CheckBoxKick.Show(true);
                if(perms.m_CanEdit)
                    m_CheckBoxPermissions.Show(true);
            }
            else
            {
                m_PanelPerms.Show(false);
            }

            if(perms.m_CanKick)
            {
                m_BtnKick.Show(true);
            }
            else
            {
                m_BtnKick.Show(false);
            }

            if(perms.m_CanEdit)
            {
               m_BtnEdit.Show(true);
            }
            else
            {
                m_BtnEdit.Show(false);
            }

            if(perms.m_CanInvite)
            {
                m_BtnAdd.Show(true);
            }
            else
            {
                m_BtnAdd.Show(false);
            }
            LoadClanMemberList();
            
        }
        else
        {
            Error("Permission error cant find clients permission.");
        }
    }

    void HandleAddMemberToClan()
    {
        int rowIndex = m_ListboxPlayers.GetSelectedRow();

        if(rowIndex == -1)
        {
            GetBankingClientManager().SendNotification("No Player Selected in List!");
            return;
        }

        ref bankingplayerlistobj member;
        if(!GetBankingClientManager().GetOnlinePlayers()) return;
        member = GetBankingClientManager().GetOnlinePlayers().Get(rowIndex);

        GetBankingClientManager().AddMemberToClan(member.plainid);
    }

    void HandleEditPermission(int rowIndex)
    {
        if(rowIndex == -1)
        {
            GetBankingClientManager().SendNotification("No Player Selected in List!");
            return;
        }

        if(!GetBankingClientManager().GetClientsClanData().GetClanMembers()) return;
        string SteamID = GetBankingClientManager().GetClientsClanData().GetClanMembers().Get(rowIndex).GetPlainID();
        PermissionObject newPerms = PermissionObject(m_CheckBoxWithdraw.IsChecked(), m_CheckBoxDeposit.IsChecked(), m_CheckBoxAdd.IsChecked(), m_CheckBoxKick.IsChecked(), m_CheckBoxPermissions.IsChecked());
        GetBankingClientManager().RequestEditPermission(newPerms, SteamID);

    }

    void HandleRemoveMemberFromClan(int rowIndex)
    {
        if(rowIndex == -1)
        {
            GetBankingClientManager().SendNotification("No Player Selected in List!");
            return;
        }
        
        ref bankingplayerlistobj member;
        if(!GetBankingClientManager().GetOnlinePlayers()) return;
        member = GetBankingClientManager().GetOnlinePlayers().Get(rowIndex);
        GetBankingClientManager().RemoveMember(member.plainid);
    }

    void LoadClanMemberList()
    {
        m_ListboxMember.ClearItems();
        for(int i = 0; i < GetBankingClientManager().GetClientsClanData().GetClanMembers().Count(); i++)
        {
            m_ListboxMember.AddItem(" " + GetBankingClientManager().GetClientsClanData().GetClanMembers().Get(i).GetPlayerName(), NULL, 0);
        }
    }

    override void Update(float timeslice)
    {
        super.Update(timeslice);

        if(m_UICooldownTimer > 0)
            m_UICooldownTimer -= timeslice;
        
        if(m_UIUpdateTimer >= 0.05)
        {
            //Update all in UI
            UpdateUI();
            UpdateUIClanData();
            m_UIUpdateTimer = 0;
        }
        else
        {
            m_UIUpdateTimer += timeslice;
        }
    }

    void HandleWitdrawMoneyFromBank(int mode)
    {
        int parsedMoney;
        if(mode == 1)
        {
            parsedMoney = m_OwnAccInputBox.GetText().ToInt();
        }
        else if(mode == 2)
        {
            parsedMoney = m_ClanAccInputBox.GetText().ToInt();
        }
        if(parsedMoney)
            GetBankingClientManager().RequestRemoteToWitdraw(parsedMoney, mode);
    }
    
    void HandleDepositMoney(int mode)
    {
        int parsedMoney;
        if(mode == 1)
        {
            parsedMoney = m_OwnAccInputBox.GetText().ToInt();
        }
        else if(mode == 2)
        {
            parsedMoney = m_ClanAccInputBox.GetText().ToInt();
        }
        if(parsedMoney)
            GetBankingClientManager().RequestRemoteToDeposit(parsedMoney, mode);
    }

    void HandleTransferConfirm()
    {
        GetBankingClientManager().RequestRemoteForTransfer(m_target, m_TransferInputBox.GetText().ToInt());
        m_YesNoMessage.Show(false);
    }

    void SpawnClanCreatePopup()
    {
        m_PanelName.Show(true);
    }

    void HideNewClanPopup()
    {
        m_PanelName.Show(false);
    }

    void HandleTransferCancel()
    {
        m_YesNoMessage.Show(false);
    }

    void HandleClanCreate()
    {
        string ClansName    = m_EditBoxClanName.GetText();
        string ClanTag      = m_EditBoxClanTag.GetText();
        GetBankingClientManager().RequestRemoteClanCreate(ClansName, ClanTag);
        HideNewClanPopup();
        m_PanelNewClan.Show(false);
        m_IsClanAccountFresh = true;
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
                m_PanelNewClan.Show(false);
                m_ClanSettings.Show(false);
                break;
            case 2:
                DrawCorrectMenu();
                m_OwnBankAccountTab.Show(false);
                m_TransferTab.Show(false);
                m_ClanSettings.Show(false);
                break;
            case 3:
                GetBankingClientManager().RequestOnlinePlayers();
                m_TransferTab.Show(true);
                m_OwnBankAccountTab.Show(false);
                m_ClanBankAccountTab.Show(false);
                m_PanelNewClan.Show(false);
                m_ClanSettings.Show(false);
                break;
        }
    }

    void UpdateUI()//If you add UI stuff what needs to be updated from remote you can just add this here!
    {
        if(!GetBankingClientManager().GetClientSettings().isRobActive)
            m_RobATMBtn.Show(false);
        if(!GetBankingClientManager().GetClientSettings().IsClanAccountActive)
            m_ClanAccBtn.Show(false);
        m_OwnedCurrencyLabel.SetText(" " + GetBankingClientManager().GetBankCredits());
        m_OnPlayerCurrencyLabel.SetText(" " + GetBankingClientManager().GetPlayerCurrencyAmount().ToString());
        m_OnPlayerCurrencyLabel2.SetText(" " + GetBankingClientManager().GetPlayerCurrencyAmount().ToString());

        /* Set progressbar */
        int CurrentAmountOnBank = GetBankingClientManager().GetBankCredits();
        int MaxOnBank = GetBankingClientManager().GetClientSettings().MaxCurrency;
    	int progValue = 100 * CurrentAmountOnBank / MaxOnBank;
        float parsedfloat = progValue.ToString().ToFloat();
        m_OwnBankAccountProgressbar.SetCurrent(parsedfloat);
        m_ProgressText.SetText(progValue.ToString() + "%");
        m_CurrentProgressMinTxt.SetText((CurrentAmountOnBank / 1000).ToString() + "K");
        m_CurrentMaxProgressTxt.SetText((MaxOnBank / 1000).ToString() + "K");
    }

    void DrawCorrectMenu()
    {
        if(GetBankingClientManager().hasClan())
        {
            GetBankingClientManager().RequestClanData();
            m_ClanBankAccountTab.Show(true);
            m_PanelNewClan.Show(false);
        } 
        else
        {
            int PriceToCreateClan = GetBankingClientManager().GetClientSettings().CostsToCreateClan;
            if(PriceToCreateClan == 0)
            {
                m_PriceToCreate.Show(false);
            }
            else
            {
                if(PriceToCreateClan < GetBankingClientManager().GetPlayerCurrencyAmount())
                {
                    //Green
                    m_PriceToCreate.SetColor(ARGB(255, 0, 255, 0));
                }
                else
                {   
                    //red
                    m_PriceToCreate.SetColor(ARGB(255, 255, 0, 0));
                }
            }
            m_PriceToCreate.SetText("Price: " + PriceToCreateClan.ToString());
            m_PanelNewClan.Show(true);
            m_ClanBankAccountTab.Show(false);
        }
    }

    void UpdateUIClanData()
    {
        if(!GetBankingClientManager().GetClientsClanData()) return;
        if(GetBankingClientManager().GetClientsClanData() &&  GetBankingClientManager().GetClientsClanData().GetBankCredit())
            m_ClanAmount.SetText(" " + GetBankingClientManager().GetClientsClanData().GetBankCredit());
        else
            m_ClanAmount.SetText(" 0");
        m_CashOnPlayer.SetText(" " + GetBankingClientManager().GetPlayerCurrencyAmount());

        int progValue = 100 * GetBankingClientManager().GetClientsClanData().GetBankCredit() / GetBankingClientManager().GetClientSettings().MaxClanAccountLimit;
        float parsedfloat = progValue.ToString().ToFloat();
        m_ClanBankAccountProgressbar.SetCurrent(parsedfloat);
        m_ProgressTextClan.SetText(parsedfloat.ToString() + "%");
    }

    //!This Updates the Playerlists from remote!
    void InvokePlayerList()
    {
        m_TransferPlayerList.ClearItems();
        m_ListboxPlayers.ClearItems();
        for(int i = 0; i < GetBankingClientManager().GetOnlinePlayers().Count(); i++)
        {
            m_TransferPlayerList.AddItem(" " + GetBankingClientManager().GetOnlinePlayers().Get(i).name, NULL, 0);
            m_ListboxPlayers.AddItem(" " + GetBankingClientManager().GetOnlinePlayers().Get(i).name, NULL, 0);
        }
    }

    //!Creates a yes no Message.
    void CreateYesNoMessage(string Headline, string BodyMessage)
    {
        m_YesNoMsgHeadline.SetText(" " + Headline);
        m_YesNoMsgBody.SetText(" " + BodyMessage);
        m_YesNoMessage.Show(true);
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