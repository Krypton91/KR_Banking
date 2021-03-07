modded class MissionGameplay
{
    override void OnKeyRelease(int key)
	{
		super.OnKeyRelease(key);
		if ( key == KeyCode.KC_ESCAPE )
		{	
			GetBankingClientManager().CloseBankingMenu();
		}
	}

	override void OnUpdate(float timeslice) 
    {
        super.OnUpdate(timeslice);

		if (GetUApi() && !m_UIManager.IsMenuOpen(MENU_CHAT_INPUT))
        {
			Input input = GetGame().GetInput();
			if (input && input.LocalPress("UAAdvancedBanking", false))
            {
				if(IsBankingAdmin && IsBankingAdminDataRecived)
				{
                	GetBankingClientAdminManager().OpenBankingAdmin();
				}
				else if(!IsBankingAdminDataRecived)
				{
					GetRPCManager().SendRPC("KR_BANKING", "AdminDataRequest", null, true);
					GetBankingClientManager().SendNotification("Please wait for server response....");
				}
            }
		}
	}
}