class Webhookservice
{
    bool    m_UseWebhook;
    string  m_WebhookURL;
    bool    m_LogDepositToDiscord;
    bool    m_LogWithdrawToDiscord;
    bool    m_LogClanDepositToDiscord;
    bool    m_LogClanWithdrawToDiscord;
    
    void Webhookservice(bool UseWebhook = false, string WebhookURL = "HERE THE URL", bool LogDepositToDiscord = false, bool LogWithdrawToDiscord = false, bool LogClanDepositToDiscord = false, bool LogClanWithdrawToDiscord = false)
    {
        m_UseWebhook = UseWebhook;
        m_WebhookURL = WebhookURL;
        m_LogDepositToDiscord = LogDepositToDiscord;
        m_LogWithdrawToDiscord = LogWithdrawToDiscord;
        m_LogClanDepositToDiscord = LogClanDepositToDiscord;
        m_LogClanWithdrawToDiscord = LogClanWithdrawToDiscord;
    }
};