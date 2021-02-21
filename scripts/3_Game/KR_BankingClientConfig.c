class KR_BankingClientConfig
{
    int     MaxCurrency;
    float   InteractDelay;
    bool    isRobActive;
    bool    isBankCardNeeded;
    ref array<ref CurrencySettings> BankingCurrency;
    protected bool ConfigRecvied;
    int     CostsToCreateClan;

    void KR_BankingClientConfig(int mxcur, float interdelay, bool rob, bool card, ref array<ref CurrencySettings> ServersCurrency, int coststocreateclan )
    {
        MaxCurrency = mxcur;
        InteractDelay = interdelay;
        isRobActive = rob;
        isBankCardNeeded = card;
        BankingCurrency = ServersCurrency;
        CostsToCreateClan = coststocreateclan;
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