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
}