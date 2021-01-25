class KR_BankingATM extends BuildingSuper
{
    bool IsATM()
    {
        return true;
    }
    override void SetActions()
	{
        super.SetActions();
        AddAction(ActionOpenBankingMenu);
    }
}
//class KR_ATM: KR_BankingATM{};