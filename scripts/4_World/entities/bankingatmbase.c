class KR_BankingATM extends BuildingSuper
{
    bool m_Banking_CanBeRobbed = false;
    void KR_BankingATM()
    {
        RegisterNetSyncVariableBool("m_Banking_CanBeRobbed");
    }

    override void SetActions()
	{
        super.SetActions();
        AddAction(ActionOpenBankingMenu);
    }
}

//class KR_ATM: KR_BankingATM{};
//class KR_ATMRusty: KR_BankingATM{};