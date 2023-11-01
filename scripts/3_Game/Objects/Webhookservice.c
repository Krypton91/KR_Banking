class Webhookservice
{
    bool    m_UseWebhook = false;
    string  m_WebhookURL = "Here the full URL!";
    bool    m_LogDepositToDiscord;
    bool    m_LogWithdrawToDiscord;
    bool    m_LogClanDepositToDiscord;
    bool    m_LogClanWithdrawToDiscord;
    bool    m_LogClanCreateToDiscord;
    bool    m_LogClanInviteMemberToDiscord;
    bool    m_LogClanRemoveMemberToDiscord;
    bool    m_LogClanUpdatePermissionToDiscord;
    bool    m_LogClanKickMemberToDiscord;
    bool    m_LogPlayerMoneyTransefer;
    bool    m_LogPlayerATMRobbery;
    bool    m_AdminLogUpdateServerConfig;
    bool    m_AdminLogUpdatePlayerData;
    bool    m_AdminLogUpdateClanData;
    bool    m_AdminLogUseMisc;
};