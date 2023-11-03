class KR_BankingClientConfig
{
    int     MaxCurrency;
    float   InteractDelay;
    bool    isRobActive;
    bool    isBankCardNeeded;
    bool    IsClanAccountActive;
    ref array<ref CurrencySettings> BankingCurrency;
    protected bool ConfigRecvied;
    int     CostsToCreateClan;
    int     MaxClanAccountLimit;
    int     TimeInSecToRobATM;
    int     ServersMaxCurrency;
    protected bool m_MaxCurrencyUpdated;

    void KR_BankingClientConfig(int mxcur, float interdelay, bool rob, bool card, ref array<ref CurrencySettings> ServersCurrency, int coststocreateclan, int MaxClanAmount, bool clanAcc)
    {
        MaxCurrency = mxcur;
        InteractDelay = interdelay;
        isRobActive = rob;
        isBankCardNeeded = card;
        BankingCurrency = ServersCurrency;
        CostsToCreateClan = coststocreateclan;
        MaxClanAccountLimit = MaxClanAmount;
        IsClanAccountActive = clanAcc;
        ServersMaxCurrency = mxcur;
    }

    bool hasConfigfromServer()
    {
        return ConfigRecvied;
    }

    void UpdatehasConfig(bool state)
    {
        ConfigRecvied = state;
    }

    protected bool NeedsMaxCurrencyAnUpdate(int Limit)
    {
        if(!m_MaxCurrencyUpdated)
            return true;
        int NewMax = MaxCurrency + Limit;
        if(NewMax != ServersMaxCurrency)
            return true;
        
        return false;
    }

    void IncreaseMaxLimit(int NewLimit)
    {

        if(NewLimit == 0 || NewLimit < MaxCurrency || !NeedsMaxCurrencyAnUpdate(NewLimit)) return;
        MaxCurrency += NewLimit;
        m_MaxCurrencyUpdated = true;
    }
}