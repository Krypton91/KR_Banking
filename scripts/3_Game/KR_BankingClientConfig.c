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
    }

    bool hasConfigfromServer()
    {
        return ConfigRecvied;
    }

    void UpdatehasConfig(bool state)
    {
        ConfigRecvied = state;
    }
}