class KR_BankingMenu extends UIScriptedMenu
{
    protected bool                      m_IsBankingMenuInitialized = false;
    protected bool                      m_IsBankingMenuOpen = false;

    protected float                     m_UIUpdateTimer = 0;
    protected float                     m_UICooldownTimer = 0;
    protected int                       m_LastMemberIndexClanList = -1;

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
    protected Widget                    m_LeaveConfirm;
    protected Widget                    m_OwnersPanel;

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
    protected ButtonWidget              m_RobATMBtn;
    protected ButtonWidget              m_BtnLeaveClan;
    protected ButtonWidget              m_BtnLeaveYes;
    protected ButtonWidget              m_BtnLeaveNo;
    protected ButtonWidget              m_BtnOwnerSafeBtn;
    protected ButtonWidget              m_BtnYesCreate;

    protected EditBoxWidget             m_OwnAccInputBox;
    protected EditBoxWidget             m_ClanAccInputBox;
    protected EditBoxWidget             m_TransferInputBox;
    protected EditBoxWidget             m_EditBoxClanName;
    protected EditBoxWidget             m_EditBoxClanTag;
    protected EditBoxWidget             m_EditBoxClanNameOwner;
    protected EditBoxWidget             m_EditBoxClanTagOwner;

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

    protected int                       m_LastPlayerIndexTransfer = -1;
    protected string                    m_PendingTransferTargetID = "";
    protected string                    m_PendingTransferTargetName = "";
    protected int                       m_PendingTransferAmount = 0;

    protected vector                    m_OpenMenuPosition;
    protected PlayerBase                m_Player;

    void KR_BankingMenu()
    {
        if (!GetGame())
            return;

        m_Player = PlayerBase.Cast(GetGame().GetPlayer());
        if (m_Player)
            m_OpenMenuPosition = m_Player.GetPosition();
    }

    void ~KR_BankingMenu()
    {
        if (layoutRoot)
        {
            layoutRoot.Unlink();
            layoutRoot = null;
        }

        m_IsBankingMenuInitialized = false;
    }

    override Widget Init()
    {
        if (m_IsBankingMenuInitialized)
            return layoutRoot;

        if (!GetGame() || !GetGame().GetWorkspace())
            return null;

        layoutRoot = GetGame().GetWorkspace().CreateWidgets("KR_Banking/GUI/layouts/BankingMenu.layout");
        if (!layoutRoot)
        {
            Error("[Advanced Banking] -> BankingMenu.layout could not be loaded.");
            return null;
        }

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
        m_LeaveConfirm                  = Widget.Cast(layoutRoot.FindAnyWidget("PanelLeaveConfirm"));
        m_OwnersPanel                   = Widget.Cast(layoutRoot.FindAnyWidget("PanelOwner"));

        m_CloseUiBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CloseInvi"));
        m_BankAccBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabBank"));
        m_ClanAccBtn                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabClanBank"));
        m_WithdrawOwnAccBtn             = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnWithdraw"));
        m_WithdrawClan                  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnWithdrawClan"));
        m_DepositOwnAccBtn              = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnDeposit"));
        m_DepositClanAccbtn             = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnDepositClan"));
        m_TransferBtn                   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnTabTransfer"));
        m_YesConfirmBtn                 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget3"));
        m_NoConfirmBtn                  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget2"));
        m_BtnClanSettings               = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnClanSettings"));
        m_BtnKick                       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonKick"));
        m_BtnAdd                        = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonEdit0"));
        m_BtnBack                       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonBack"));
        m_BtnSave                       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget0"));
        m_BtnSendTransfer               = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnSend"));
        m_BtnFinallyCreate              = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidgetCreate"));
        m_BtnYesCreate                  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWidget1"));
        m_BtnLeaveClan                  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonLeave"));
        m_BtnLeaveYes                   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonLeaveYes"));
        m_BtnLeaveNo                    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonLeaveNo"));
        m_BtnOwnerSafeBtn               = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnSave"));

        m_OwnAccInputBox                = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxWidget0"));
        m_TransferInputBox              = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxWidget1"));
        m_ClanAccInputBox               = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("InputBoxClan"));
        m_EditBoxClanName               = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxWidgetName"));
        m_EditBoxClanTag                = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxWidgetTag"));
        m_EditBoxClanNameOwner          = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("txtBoxClanName"));
        m_EditBoxClanTagOwner           = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("txtBoxClanTag"));

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
        m_ClanLogs                      = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("TextListboxLogs"));

        m_CheckBoxWithdraw              = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckBoxWithdraw"));
        m_CheckBoxDeposit               = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckBoxDeposit"));
        m_CheckBoxKick                  = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckBoxKick"));
        m_CheckBoxAdd                   = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckBoxInvite"));
        m_CheckBoxPermissions           = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("CheckBoxPermission"));

        m_OwnBankAccountProgressbar     = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("ProgressBarWidget0"));
        m_ClanBankAccountProgressbar    = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("ProgressBarWidgetClan"));

        // The supplied class contains no layout widget name for m_RobATMBtn.
        // It remains optional and every access is guarded.

        m_IsBankingMenuInitialized = true;
        return layoutRoot;
    }

    protected bool TryStartInteractionCooldown()
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager)
            return false;

        KR_BankingClientConfig settings = manager.GetClientSettings();
        if (!settings)
            return false;

        if (m_UICooldownTimer > 0)
        {
            manager.SendNotification("#AB_NotThatFast");
            return false;
        }

        m_UICooldownTimer = settings.InteractDelay;
        if (m_UICooldownTimer < 0)
            m_UICooldownTimer = 0;

        return true;
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        switch (w)
        {
            case m_CloseUiBtn:
                CloseBankingMenu();
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
                if (TryStartInteractionCooldown())
                    HandleWithdrawMoneyFromBank(BankType.OWNBANK);
                break;

            case m_DepositOwnAccBtn:
                if (TryStartInteractionCooldown())
                    HandleDepositMoney(BankType.OWNBANK);
                break;

            case m_YesConfirmBtn:
                HandleTransferConfirm();
                break;

            case m_NoConfirmBtn:
                HandleTransferCancel();
                break;

            case m_BtnSendTransfer:
                HandleTransferStart();
                break;

            case m_BtnFinallyCreate:
                HandleClanCreate();
                break;

            case m_DepositClanAccbtn:
                if (TryStartInteractionCooldown())
                    HandleDepositMoney(BankType.CLANBANK);
                break;

            case m_WithdrawClan:
                if (TryStartInteractionCooldown())
                    HandleWithdrawMoneyFromBank(BankType.CLANBANK);
                break;

            case m_BtnClanSettings:
                OpenClanSettings();
                break;

            case m_BtnBack:
                if (m_ClanSettings)
                    m_ClanSettings.Show(false);
                if (m_ClanBankAccountTab)
                    m_ClanBankAccountTab.Show(true);
                break;

            case m_BtnAdd:
                HandleAddMemberToClan();
                break;

            case m_BtnKick:
                if (m_ListboxMember)
                    HandleRemoveMemberFromClan(m_ListboxMember.GetSelectedRow());
                break;

            case m_BtnSave:
                if (m_ListboxMember)
                    HandleEditPermission(m_ListboxMember.GetSelectedRow());
                break;

            case m_BtnLeaveClan:
                HandleLeaveClan();
                break;

            case m_BtnLeaveNo:
                HandleLeaveNo();
                break;

            case m_BtnLeaveYes:
                HandleLeaveYes();
                break;

            case m_BtnOwnerSafeBtn:
                HandleEditClan();
                break;

            case m_BtnYesCreate:
                SpawnClanCreatePopup();
                break;
        }

        return super.OnClick(w, x, y, button);
    }

    bool IsStringNumberOnly(string inputText, EditBoxWidget contextBox = null)
    {
        string sanitizedText = "";

        for (int i = 0; i < inputText.Length(); i++)
        {
            string currentCharacter = inputText.Get(i);
            if (IsCharNumberOnly(currentCharacter))
                sanitizedText = sanitizedText + currentCharacter;
        }

        if (sanitizedText == inputText)
            return true;

        if (contextBox)
            contextBox.SetText(sanitizedText);

        return false;
    }

    bool IsCharNumberOnly(string input)
    {
        if (input == "0" || input == "1" || input == "2" || input == "3" || input == "4")
            return true;

        if (input == "5" || input == "6" || input == "7" || input == "8" || input == "9")
            return true;

        return false;
    }

    protected void SetPermissionCheckboxes(PermissionObject permissions)
    {
        bool canWithdraw = false;
        bool canDeposit = false;
        bool canInvite = false;
        bool canKick = false;
        bool canEdit = false;

        if (permissions)
        {
            canWithdraw = permissions.m_CanWithdraw;
            canDeposit = permissions.m_CanDeposit;
            canInvite = permissions.m_CanInvite;
            canKick = permissions.m_CanKick;
            canEdit = permissions.m_CanEdit;
        }

        if (m_CheckBoxWithdraw)
            m_CheckBoxWithdraw.SetChecked(canWithdraw);
        if (m_CheckBoxDeposit)
            m_CheckBoxDeposit.SetChecked(canDeposit);
        if (m_CheckBoxAdd)
            m_CheckBoxAdd.SetChecked(canInvite);
        if (m_CheckBoxKick)
            m_CheckBoxKick.SetChecked(canKick);
        if (m_CheckBoxPermissions)
            m_CheckBoxPermissions.SetChecked(canEdit);
    }

    protected void ApplyClanFeatureVisibility(bool isClanAccountActive)
    {
        if (m_ClanAccBtn)
            m_ClanAccBtn.Show(isClanAccountActive);

        if (isClanAccountActive)
            return;

        bool clanPageWasVisible = false;

        if (m_ClanBankAccountTab && m_ClanBankAccountTab.IsVisible())
            clanPageWasVisible = true;
        if (m_PanelNewClan && m_PanelNewClan.IsVisible())
            clanPageWasVisible = true;
        if (m_ClanSettings && m_ClanSettings.IsVisible())
            clanPageWasVisible = true;

        if (m_ClanBankAccountTab)
            m_ClanBankAccountTab.Show(false);
        if (m_PanelNewClan)
            m_PanelNewClan.Show(false);
        if (m_ClanSettings)
            m_ClanSettings.Show(false);
        if (m_PanelName)
            m_PanelName.Show(false);
        if (m_LeaveConfirm)
            m_LeaveConfirm.Show(false);

        if (clanPageWasVisible && m_OwnBankAccountTab)
            m_OwnBankAccountTab.Show(true);
    }

    void OpenClanSettings()
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager)
            return;

        ClanDataBaseManager clanData = manager.GetClientsClanData();
        PermissionObject permissions = manager.GetClanPermission();
        if (!clanData || !permissions)
        {
            Error("[Advanced Banking] -> Cannot open clan settings: clan data or permissions missing.");
            return;
        }

        manager.RequestOnlinePlayers();

        if (m_ClanBankAccountTab)
            m_ClanBankAccountTab.Show(false);
        if (m_ClanSettings)
            m_ClanSettings.Show(true);

        if (m_PanelPerms)
            m_PanelPerms.Show(permissions.m_CanEdit);

        if (m_CheckBoxWithdraw)
            m_CheckBoxWithdraw.Show(permissions.m_CanEdit && permissions.m_CanWithdraw);
        if (m_CheckBoxDeposit)
            m_CheckBoxDeposit.Show(permissions.m_CanEdit && permissions.m_CanDeposit);
        if (m_CheckBoxAdd)
            m_CheckBoxAdd.Show(permissions.m_CanEdit && permissions.m_CanInvite);
        if (m_CheckBoxKick)
            m_CheckBoxKick.Show(permissions.m_CanEdit && permissions.m_CanKick);
        if (m_CheckBoxPermissions)
            m_CheckBoxPermissions.Show(permissions.m_CanEdit);

        if (m_BtnKick)
            m_BtnKick.Show(permissions.m_CanKick);
        if (m_BtnAdd)
            m_BtnAdd.Show(permissions.m_CanInvite);

        LoadClanMemberList();
        m_LastMemberIndexClanList = -1;
        SetPermissionCheckboxes(null);

        bool isOwner = clanData.GetOwnersID() == manager.GetSteamID();
        if (m_OwnersPanel)
            m_OwnersPanel.Show(isOwner);

        if (isOwner)
        {
            if (m_EditBoxClanTagOwner)
                m_EditBoxClanTagOwner.SetText(clanData.GetClanTag());
            if (m_EditBoxClanNameOwner)
                m_EditBoxClanNameOwner.SetText(clanData.GetName());
        }
    }

    void HandleLeaveClan()
    {
        if (m_LeaveConfirm)
            m_LeaveConfirm.Show(true);
    }

    void HandleLeaveYes()
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (manager)
            manager.RequestRemoteLeaveClan();

        if (m_LeaveConfirm)
            m_LeaveConfirm.Show(false);
    }

    void HandleLeaveNo()
    {
        if (m_LeaveConfirm)
            m_LeaveConfirm.Show(false);
    }

    void HandleEditClan()
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager || !m_EditBoxClanNameOwner || !m_EditBoxClanTagOwner)
            return;

        string clanName = m_EditBoxClanNameOwner.GetText();
        string clanTag = m_EditBoxClanTagOwner.GetText();

        if (clanTag.Length() > 8)
        {
            manager.SendNotification("#AB_ClanTagIsToLong");
            return;
        }

        if (!IsValidClanNameAndTag(clanName, clanTag))
        {
            manager.SendNotification("#AB_ClanTagOrNameIsNotValid");
            return;
        }

        manager.RequestRemoteEditClan(clanName, clanTag);
    }

    void HandleAddMemberToClan()
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager || !m_ListboxPlayers)
            return;

        int rowIndex = m_ListboxPlayers.GetSelectedRow();
        array<ref bankingplayerlistobj> onlinePlayers = manager.GetOnlinePlayers();

        if (!onlinePlayers || rowIndex < 0 || rowIndex >= onlinePlayers.Count())
        {
            manager.SendNotification("#AB_NoPlayerSelected");
            return;
        }

        bankingplayerlistobj member = onlinePlayers.Get(rowIndex);
        if (!member || member.plainid == "")
        {
            manager.SendNotification("#AB_NoPlayerSelected");
            return;
        }

        manager.AddMemberToClan(member.plainid);
    }

    void HandleEditPermission(int rowIndex)
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager)
            return;

        ClanDataBaseManager clanData = manager.GetClientsClanData();
        if (!clanData || !clanData.GetClanMembers())
            return;

        if (rowIndex < 0 || rowIndex >= clanData.GetClanMembers().Count())
        {
            manager.SendNotification("#AB_NoPlayerSelected");
            return;
        }

        ClanMemberObject member = clanData.GetClanMembers().Get(rowIndex);
        if (!member)
            return;

        string steamID = member.GetPlainID();
        if (steamID == "")
            return;

        PermissionObject ownPermissions = manager.GetClanPermission();
        PermissionObject currentPermissions = member.GetPermission();
        if (!ownPermissions || !ownPermissions.m_CanEdit || !currentPermissions)
            return;

        bool canWithdraw = currentPermissions.m_CanWithdraw;
        bool canDeposit = currentPermissions.m_CanDeposit;
        bool canInvite = currentPermissions.m_CanInvite;
        bool canKick = currentPermissions.m_CanKick;
        bool canEdit = currentPermissions.m_CanEdit;

        if (ownPermissions.m_CanWithdraw && m_CheckBoxWithdraw)
            canWithdraw = m_CheckBoxWithdraw.IsChecked();
        if (ownPermissions.m_CanDeposit && m_CheckBoxDeposit)
            canDeposit = m_CheckBoxDeposit.IsChecked();
        if (ownPermissions.m_CanInvite && m_CheckBoxAdd)
            canInvite = m_CheckBoxAdd.IsChecked();
        if (ownPermissions.m_CanKick && m_CheckBoxKick)
            canKick = m_CheckBoxKick.IsChecked();
        if (m_CheckBoxPermissions)
            canEdit = m_CheckBoxPermissions.IsChecked();

        PermissionObject newPermissions;
        newPermissions = new PermissionObject();
        newPermissions.m_CanWithdraw = canWithdraw;
        newPermissions.m_CanDeposit = canDeposit;
        newPermissions.m_CanInvite = canInvite;
        newPermissions.m_CanKick = canKick;
        newPermissions.m_CanEdit = canEdit;

        manager.RequestEditPermission(newPermissions, steamID);
    }

    void HandleRemoveMemberFromClan(int rowIndex)
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager)
            return;

        ClanDataBaseManager clanData = manager.GetClientsClanData();
        if (!clanData || !clanData.GetClanMembers())
            return;

        if (rowIndex < 0 || rowIndex >= clanData.GetClanMembers().Count())
        {
            manager.SendNotification("#AB_NoPlayerSelected");
            return;
        }

        ClanMemberObject member = clanData.GetClanMembers().Get(rowIndex);
        if (!member || member.GetPlainID() == "")
            return;

        manager.RemoveMember(member.GetPlainID());
    }

    void LoadClanMemberList()
    {
        if (!m_ListboxMember)
            return;

        m_ListboxMember.ClearItems();
        m_LastMemberIndexClanList = -1;

        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager)
            return;

        ClanDataBaseManager clanData = manager.GetClientsClanData();
        if (!clanData || !clanData.GetClanMembers())
            return;

        for (int i = 0; i < clanData.GetClanMembers().Count(); i++)
        {
            ClanMemberObject member = clanData.GetClanMembers().Get(i);
            string memberName = "";

            if (member)
                memberName = member.GetPlayerName();

            m_ListboxMember.AddItem(" " + memberName, null, 0);
        }
    }

    void RefreshClanUI()
    {
        LoadClanLogs();

        if (!m_ClanSettings || !m_ClanSettings.IsVisible())
            return;

        LoadClanMemberList();
        SetPermissionCheckboxes(null);
    }

    override void Update(float timeslice)
    {
        super.Update(timeslice);

        if (m_UICooldownTimer > 0)
        {
            m_UICooldownTimer -= timeslice;
            if (m_UICooldownTimer < 0)
                m_UICooldownTimer = 0;
        }

        m_UIUpdateTimer += timeslice;
        if (m_UIUpdateTimer < 0.05)
            return;

        m_UIUpdateTimer = 0;

        if (!m_Player && GetGame())
        {
            m_Player = PlayerBase.Cast(GetGame().GetPlayer());
            if (m_Player)
                m_OpenMenuPosition = m_Player.GetPosition();
        }

        if (m_Player)
        {
            float distanceToATM = vector.Distance(m_Player.GetPosition(), m_OpenMenuPosition);
            if (distanceToATM > 1.5)
            {
                CloseBankingMenu();
                SetIsBankingMenuOpen(false);
                return;
            }
        }

        if (m_OwnAccInputBox)
            IsStringNumberOnly(m_OwnAccInputBox.GetText(), m_OwnAccInputBox);
        if (m_ClanAccInputBox)
            IsStringNumberOnly(m_ClanAccInputBox.GetText(), m_ClanAccInputBox);
        if (m_TransferInputBox)
            IsStringNumberOnly(m_TransferInputBox.GetText(), m_TransferInputBox);

        UpdateUI();
        UpdateUIClanData();

        if (m_TransferPlayerList)
            m_LastPlayerIndexTransfer = m_TransferPlayerList.GetSelectedRow();
        else
            m_LastPlayerIndexTransfer = -1;
    }

    void HandleWithdrawMoneyFromBank(int mode)
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager)
            return;

        int parsedMoney = 0;

        if (mode == BankType.OWNBANK)
        {
            if (!m_OwnAccInputBox)
                return;

            parsedMoney = m_OwnAccInputBox.GetText().ToInt();
        }
        else if (mode == BankType.CLANBANK)
        {
            if (!m_ClanAccInputBox)
                return;

            parsedMoney = m_ClanAccInputBox.GetText().ToInt();
        }
        else
        {
            return;
        }

        manager.RequestRemoteToWithdraw(parsedMoney, mode);
    }

    void HandleDepositMoney(int mode)
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager)
            return;

        int parsedMoney = 0;

        if (mode == BankType.OWNBANK)
        {
            if (!m_OwnAccInputBox)
                return;

            parsedMoney = m_OwnAccInputBox.GetText().ToInt();
        }
        else if (mode == BankType.CLANBANK)
        {
            if (!m_ClanAccInputBox)
                return;

            parsedMoney = m_ClanAccInputBox.GetText().ToInt();
        }
        else
        {
            return;
        }

        manager.RequestRemoteToDeposit(parsedMoney, mode);
    }

    protected void ClearPendingTransfer()
    {
        m_PendingTransferTargetID = "";
        m_PendingTransferTargetName = "";
        m_PendingTransferAmount = 0;
    }

    void HandleTransferStart()
    {
        ClearPendingTransfer();

        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager || !m_TransferPlayerList || !m_TransferInputBox)
            return;

        array<ref bankingplayerlistobj> onlinePlayers = manager.GetOnlinePlayers();
        int rowIndex = m_TransferPlayerList.GetSelectedRow();

        if (!onlinePlayers || rowIndex < 0 || rowIndex >= onlinePlayers.Count())
        {
            manager.SendNotification("#AB_NoPlayerSelected");
            return;
        }

        bankingplayerlistobj targetPlayer = onlinePlayers.Get(rowIndex);
        int transferAmount = m_TransferInputBox.GetText().ToInt();

        if (!targetPlayer || targetPlayer.plainid == "")
        {
            manager.SendNotification("#AB_NoPlayerSelected");
            return;
        }

        if (targetPlayer.plainid == manager.GetSteamID())
        {
            manager.SendNotification("#AB_PlayerNotAvaibleForTransfers");
            return;
        }

        if (transferAmount <= 0)
        {
            manager.SendNotification("Invalid transfer amount.");
            return;
        }

        m_PendingTransferTargetID = targetPlayer.plainid;
        m_PendingTransferTargetName = targetPlayer.name;
        m_PendingTransferAmount = transferAmount;

        string confirmationBody = "#AB_TransferConfirm " + transferAmount.ToString() + " to " + m_PendingTransferTargetName + " ?";
        CreateYesNoMessage("Transfer Check", confirmationBody);
    }

    void HandleTransferConfirm()
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager)
            return;

        if (m_PendingTransferTargetID == "" || m_PendingTransferAmount <= 0)
        {
            HandleTransferCancel();
            return;
        }

        if (!TryStartInteractionCooldown())
            return;

        manager.RequestRemoteForTransfer(m_PendingTransferTargetID, m_PendingTransferAmount);

        if (m_YesNoMessage)
            m_YesNoMessage.Show(false);

        ClearPendingTransfer();
    }

    void SpawnClanCreatePopup()
    {
        if (m_PanelName)
            m_PanelName.Show(true);
    }

    void HideNewClanPopup()
    {
        if (m_PanelName)
            m_PanelName.Show(false);
    }

    void HandleTransferCancel()
    {
        if (m_YesNoMessage)
            m_YesNoMessage.Show(false);

        ClearPendingTransfer();
    }

    void HandleClanCreate()
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager || !m_EditBoxClanName || !m_EditBoxClanTag)
            return;

        string clanName = m_EditBoxClanName.GetText();
        string clanTag = m_EditBoxClanTag.GetText();

        if (clanName == "Clan Name")
            clanName = "";
        if (clanTag == "Clan Tag")
            clanTag = "";

        if (clanTag.Length() > 8)
        {
            manager.SendNotification("#AB_ClanTagIsToLong");
            return;
        }

        if (!IsValidClanNameAndTag(clanName, clanTag))
        {
            manager.SendNotification("#AB_ClanTagOrNameIsNotValid");
            return;
        }

        if (!TryStartInteractionCooldown())
            return;

        manager.RequestRemoteClanCreate(clanName, clanTag);
        HideNewClanPopup();

        if (m_PanelNewClan)
            m_PanelNewClan.Show(false);
    }

    bool IsValidClanTag(string clanTag)
    {
        if (clanTag == "" || clanTag == "Clan Tag")
            return false;

        return clanTag.Length() <= 8;
    }

    bool IsValidClanNameAndTag(string clanName, string clanTag)
    {
        if (clanName == "" || clanName == "Clan Name")
            return false;

        if (clanName.Length() > 32)
            return false;

        return IsValidClanTag(clanTag);
    }

    void SwitchTab(int tabIndex)
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();

        if (tabIndex == 1)
        {
            if (m_OwnBankAccountTab)
                m_OwnBankAccountTab.Show(true);
            if (m_ClanBankAccountTab)
                m_ClanBankAccountTab.Show(false);
            if (m_TransferTab)
                m_TransferTab.Show(false);
            if (m_PanelNewClan)
                m_PanelNewClan.Show(false);
            if (m_ClanSettings)
                m_ClanSettings.Show(false);
        }
        else if (tabIndex == 2)
        {
            if (!manager || !manager.GetClientSettings() || !manager.GetClientSettings().IsClanAccountActive)
                return;

            DrawCorrectMenu();

            if (m_OwnBankAccountTab)
                m_OwnBankAccountTab.Show(false);
            if (m_TransferTab)
                m_TransferTab.Show(false);
            if (m_ClanSettings)
                m_ClanSettings.Show(false);
        }
        else if (tabIndex == 3)
        {
            if (manager)
                manager.RequestOnlinePlayers();

            if (m_TransferTab)
                m_TransferTab.Show(true);
            if (m_OwnBankAccountTab)
                m_OwnBankAccountTab.Show(false);
            if (m_ClanBankAccountTab)
                m_ClanBankAccountTab.Show(false);
            if (m_PanelNewClan)
                m_PanelNewClan.Show(false);
            if (m_ClanSettings)
                m_ClanSettings.Show(false);
        }
        else
        {
            return;
        }

        if (tabIndex != 3)
            HandleTransferCancel();

        if (m_LeaveConfirm)
            m_LeaveConfirm.Show(false);
        if (m_PanelName)
            m_PanelName.Show(false);
    }

    protected float ClampProgress(float progress)
    {
        if (progress < 0)
            return 0;

        if (progress > 100)
            return 100;

        return progress;
    }

    void UpdateUI()
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager || !manager.GetClientSettings())
            return;

        KR_BankingClientConfig settings = manager.GetClientSettings();

        if (m_RobATMBtn)
            m_RobATMBtn.Show(settings.isRobActive);

        ApplyClanFeatureVisibility(settings.IsClanAccountActive);

        int bankCredits = manager.GetBankCredits();
        int playerCash = manager.GetPlayerCurrencyAmount();

        if (m_OwnedCurrencyLabel)
            m_OwnedCurrencyLabel.SetText(" " + bankCredits.ToString());

        if (m_OnPlayerCurrencyLabel)
            m_OnPlayerCurrencyLabel.SetText(" " + playerCash.ToString());

        if (m_OnPlayerCurrencyLabel2)
            m_OnPlayerCurrencyLabel2.SetText(" " + bankCredits.ToString());

        int maxOnBank = settings.MaxCurrency;
        int currentThousands = bankCredits / 1000;

        if (m_CurrentProgressMinTxt)
            m_CurrentProgressMinTxt.SetText(currentThousands.ToString() + "K");

        if (maxOnBank <= 0)
        {
            if (m_OwnBankAccountProgressbar)
                m_OwnBankAccountProgressbar.SetCurrent(0);

            if (m_ProgressText)
                m_ProgressText.SetText("--");

            if (m_CurrentMaxProgressTxt)
                m_CurrentMaxProgressTxt.SetText("--");
            
            return;
        }

        float bankCreditsFloat = bankCredits;
        float maxOnBankFloat = maxOnBank;
        float progress = (bankCreditsFloat / maxOnBankFloat) * 100.0;
        progress = ClampProgress(progress);

        int progressValue = progress;
        int maxThousands = maxOnBank / 1000;

        if (m_OwnBankAccountProgressbar)
            m_OwnBankAccountProgressbar.SetCurrent(progress);
        if (m_ProgressText)
            m_ProgressText.SetText(progressValue.ToString() + "%");
        if (m_CurrentMaxProgressTxt)
            m_CurrentMaxProgressTxt.SetText(maxThousands.ToString() + "K");
    }

    void DrawCorrectMenu()
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager || !manager.GetClientSettings())
            return;

        KR_BankingClientConfig settings = manager.GetClientSettings();
        if (!settings.IsClanAccountActive)
        {
            if (m_ClanBankAccountTab)
                m_ClanBankAccountTab.Show(false);
            if (m_PanelNewClan)
                m_PanelNewClan.Show(false);
            return;
        }

        if (manager.hasClan())
        {
            manager.RequestClanData();

            if (m_ClanBankAccountTab)
                m_ClanBankAccountTab.Show(true);
            if (m_PanelNewClan)
                m_PanelNewClan.Show(false);

            LoadClanLogs();
            return;
        }

        int priceToCreateClan = settings.CostsToCreateClan;
        int playerCash = manager.GetPlayerCurrencyAmount();

        if (m_PriceToCreate)
        {
            m_PriceToCreate.Show(priceToCreateClan > 0);
            m_PriceToCreate.SetText("#AB_Price " + priceToCreateClan.ToString());

            if (priceToCreateClan > 0)
            {
                if (playerCash >= priceToCreateClan)
                    m_PriceToCreate.SetColor(ARGB(255, 0, 255, 0));
                else
                    m_PriceToCreate.SetColor(ARGB(255, 255, 0, 0));
            }
        }

        if (m_PanelNewClan)
            m_PanelNewClan.Show(true);
        if (m_ClanBankAccountTab)
            m_ClanBankAccountTab.Show(false);
    }

    void UpdateUIClanData()
    {
        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager)
            return;

        ClanDataBaseManager clanData = manager.GetClientsClanData();
        if (!clanData)
            return;

        int clanCredits = clanData.GetBankCredit();
        int playerCash = manager.GetPlayerCurrencyAmount();

        if (m_ClanAmount)
            m_ClanAmount.SetText(" " + clanCredits.ToString());
        if (m_CashOnPlayer)
            m_CashOnPlayer.SetText(" " + playerCash.ToString());

        int maxClanCredits = 0;
        if (manager.GetClientSettings())
            maxClanCredits = manager.GetClientSettings().MaxClanAccountLimit;

        if (maxClanCredits <= 0)
        {
            if (m_ClanBankAccountProgressbar)
                m_ClanBankAccountProgressbar.SetCurrent(0);
            if (m_ProgressTextClan)
                m_ProgressTextClan.SetText("--");
        }
        else
        {
            float clanCreditsFloat = clanCredits;
            float maxClanCreditsFloat = maxClanCredits;
            float clanProgress = (clanCreditsFloat / maxClanCreditsFloat) * 100.0;
            clanProgress = ClampProgress(clanProgress);

            int clanProgressValue = clanProgress;

            if (m_ClanBankAccountProgressbar)
                m_ClanBankAccountProgressbar.SetCurrent(clanProgress);
            if (m_ProgressTextClan)
                m_ProgressTextClan.SetText(clanProgressValue.ToString() + "%");
        }

        if (!m_ListboxMember || !m_ClanSettings || !m_ClanSettings.IsVisible())
            return;

        PermissionObject ownPermissions = manager.GetClanPermission();
        if (!ownPermissions || !ownPermissions.m_CanEdit)
            return;

        if (!clanData.GetClanMembers())
            return;

        int currentRowIndex = m_ListboxMember.GetSelectedRow();
        if (m_LastMemberIndexClanList == currentRowIndex)
            return;

        m_LastMemberIndexClanList = currentRowIndex;

        if (currentRowIndex < 0 || currentRowIndex >= clanData.GetClanMembers().Count())
        {
            SetPermissionCheckboxes(null);
            return;
        }

        ClanMemberObject selectedMember = clanData.GetClanMembers().Get(currentRowIndex);
        if (!selectedMember)
        {
            SetPermissionCheckboxes(null);
            return;
        }

        SetPermissionCheckboxes(selectedMember.GetPermission());
    }

    void InvokePlayerList()
    {
        if (m_TransferPlayerList)
            m_TransferPlayerList.ClearItems();
        if (m_ListboxPlayers)
            m_ListboxPlayers.ClearItems();

        m_LastPlayerIndexTransfer = -1;

        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager)
            return;

        array<ref bankingplayerlistobj> onlinePlayers = manager.GetOnlinePlayers();
        if (!onlinePlayers)
            return;

        for (int i = 0; i < onlinePlayers.Count(); i++)
        {
            bankingplayerlistobj playerEntry = onlinePlayers.Get(i);
            string playerName = "";

            if (playerEntry)
                playerName = playerEntry.name;

            if (m_TransferPlayerList)
                m_TransferPlayerList.AddItem(" " + playerName, null, 0);
            if (m_ListboxPlayers)
                m_ListboxPlayers.AddItem(" " + playerName, null, 0);
        }
    }

    void LoadClanLogs()
    {
        if (!m_ClanLogs)
            return;

        m_ClanLogs.ClearItems();

        PluginKrBankingClientManager manager = GetBankingClientManager();
        if (!manager)
        {
            m_ClanLogs.AddItem("NO LOGS FOUND!!", null, 0);
            return;
        }

        ClanDataBaseManager clanData = manager.GetClientsClanData();
        if (!clanData || !clanData.GetClanLogs() || clanData.GetClanLogs().Count() == 0)
        {
            m_ClanLogs.AddItem("NO LOGS FOUND!!", null, 0);
            return;
        }

        for (int i = 0; i < clanData.GetClanLogs().Count(); i++)
            m_ClanLogs.AddItem(clanData.GetClanLogs().Get(i), null, 0);
    }

    void CreateYesNoMessage(string headline, string bodyMessage)
    {
        if (m_YesNoMsgHeadline)
            m_YesNoMsgHeadline.SetText(" " + headline);
        if (m_YesNoMsgBody)
            m_YesNoMsgBody.SetText(" " + bodyMessage);
        if (m_YesNoMessage)
            m_YesNoMessage.Show(true);
    }

    override void OnShow()
    {
        super.OnShow();

        SetIsBankingMenuOpen(true);

        if (GetGame())
        {
            m_Player = PlayerBase.Cast(GetGame().GetPlayer());
            if (m_Player)
                m_OpenMenuPosition = m_Player.GetPosition();

            if (GetGame().GetMission())
                GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
        }

        if (layoutRoot)
            SetFocus(layoutRoot);

        UpdateUI();
        PPEffects.SetBlurMenu(0.5);
    }

    bool IsBankingMenuOpen()
    {
        return m_IsBankingMenuOpen;
    }

    void ClearPlayerInputBox()
    {
        if (m_OwnAccInputBox)
            m_OwnAccInputBox.SetText("");
    }

    void SetIsBankingMenuOpen(bool visible)
    {
        m_IsBankingMenuOpen = visible;
    }

    void CloseBankingMenu()
    {
        if (GetGame() && GetGame().GetUIManager())
            GetGame().GetUIManager().Back();
    }

    override void OnHide()
    {
        super.OnHide();

        PPEffects.SetBlurMenu(0);

        if (GetGame() && GetGame().GetMission())
            GetGame().GetMission().PlayerControlEnable(false);

        SetIsBankingMenuOpen(false);
        HandleTransferCancel();
        m_UICooldownTimer = 0;
    }
};
