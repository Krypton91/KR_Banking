class LogSettings
{
    int m_LogMode;
    int m_LogDepositOwnBank;
    int m_LogWithdraw;
    int m_LogClanDeposit;
    int m_LogClanWithdraw;
    int m_LogCreateClanAccount;
    int m_LogDeleteClanAccount;
    int m_LogInviteMember;
    int m_LogUpdatePermission;
    
    void LogSettings(int LogMode, int LogDepositOwnBank, int LogWithdraw, int LogClanDeposit, int LogClanWithdraw, int LogCreateClanAccount, int LogDeleteClanAccount, int LogInviteMember, int LogUpdatePermission)
    {
        m_LogMode = LogMode;
        m_LogDepositOwnBank = LogDepositOwnBank;
        m_LogWithdraw = LogWithdraw;
        m_LogClanDeposit = LogClanDeposit;
        m_LogWithdraw = LogWithdraw;
        m_LogCreateClanAccount = LogCreateClanAccount;
        m_LogDeleteClanAccount = LogDeleteClanAccount;
        m_LogInviteMember = LogInviteMember;
        m_LogUpdatePermission = LogUpdatePermission;
    }
    
}