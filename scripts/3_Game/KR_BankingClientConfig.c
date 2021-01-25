class KR_BankingClientConfig
{
    float   MaxCurrency;
    float   InteractDelay;
    bool    isRobActiv;
    bool    isBankCardNeeded;
    protected bool ConfigRecvied;

    void KR_BankingClientConfig(float mxcur, float interdelay, bool rob, bool card)
    {
        MaxCurrency = mxcur;
        InteractDelay = interdelay;
        isRobActiv = rob;
        isBankCardNeeded = card;
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