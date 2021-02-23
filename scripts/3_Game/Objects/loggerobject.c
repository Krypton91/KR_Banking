class LogSettings
{
    int m_LogMode;
    bool m_LogDepositOwnBank;
    bool m_LogWithdraw;
    bool m_LogClanDeposit;
    bool m_LogClanWithdraw;
    bool m_LogCreateClanAccount;
    bool m_LogDeleteClanAccount;
    bool m_LogInviteMember;
    bool m_LogUpdatePermission;
    
    void LogSettings(int LogMode = 1, bool LogDepositOwnBank = true, bool LogWithdraw = true, bool LogClanDeposit = true, bool LogClanWithdraw = true, bool LogCreateClanAccount = true, bool LogDeleteClanAccount = true, bool LogInviteMember = true, bool LogUpdatePermission = true)
    {
        m_LogMode = LogMode;
        m_LogDepositOwnBank = LogDepositOwnBank;
        m_LogWithdraw = LogWithdraw;
        m_LogClanDeposit = LogClanDeposit;
        m_LogClanWithdraw = LogClanWithdraw;
        m_LogCreateClanAccount = LogCreateClanAccount;
        m_LogDeleteClanAccount = LogDeleteClanAccount;
        m_LogInviteMember = LogInviteMember;
        m_LogUpdatePermission = LogUpdatePermission;
    }
}