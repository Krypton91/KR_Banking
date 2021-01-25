class ActionOpenBankingMenu: ActionInteractBase
{
	
	void ActionOpenBankingMenu()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_HUDCursorIcon = CursorIcons.CloseHood;
	}

	override string GetText()
	{
		return "Open Banking";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionTarget = new CCTObject(10);
		m_ConditionItem = new CCINone;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (GetGame().IsServer())
            return true;
		if(GetBankingClientManager().WaitForServerResponse())
			return false;
		Object target_object = target.GetObject();
		KR_BankingATM atmbase;
		vector playersPos = player.GetPosition();
		vector targetPos = target_object.GetPosition();
		float distancetobank = vector.Distance(playersPos, targetPos);
		if(Class.CastTo(atmbase, target_object))
		{
			if(distancetobank && distancetobank < 1.5)
				return true;
		}
		return false;
	}
	
	override void OnStartClient( ActionData action_data )
	{
		GetBankingClientManager().OpenBankingMenu();
	}
}