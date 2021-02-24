class KR_BankingATM extends BuildingSuper
{
    bool m_Banking_CanBeRobbed = false;
    protected bool m_IsRobbed;
    void KR_BankingATM()
    {
        RegisterNetSyncVariableBool("m_Banking_CanBeRobbed");
        RegisterNetSyncVariableBool("m_IsRobbed");
    }

    bool IsATMRobbed()
    {
        return m_IsRobbed;
    }

    void SetATMIsRobbed(bool state)
    {
        m_IsRobbed = state;
    }

    override void SetActions()
	{
        super.SetActions();
        AddAction(ActionOpenBankingMenu);
    }
}

//class KR_ATM: KR_BankingATM{};
//class KR_ATMRusty: KR_BankingATM{};