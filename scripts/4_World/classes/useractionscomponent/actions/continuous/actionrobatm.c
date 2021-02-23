class ActionRobATMCB : ActiondeployObjectCB
{
    override void CreateActionComponent()
	{
        if(GetGame().IsServer())
		    m_ActionData.m_ActionComponent = new CAContinuousTime(GetKR_BankingServerConfig().TimeInSecToRobATM);
        else
            m_ActionData.m_ActionComponent = new CAContinuousTime(GetBankingClientManager().GetClientSettings().TimeInSecToRobATM);
	}
};

class ActionRobATM: ActionContinuousBase
{
	int         m_ClientSideTry = 0;
	void ActionRobATM()
	{
		m_CallbackClass = ActionRobATMCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DISASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;		
		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
	}
	
	override string GetText()
	{
		return "Break ATM";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
        if ( !GetGame().IsMultiplayer() || GetGame().IsClient() )
		{
            //Action not allowed if player has broken legs
            if (player.m_BrokenLegState == eBrokenLegs.BROKEN_LEGS)
                return false;

            if(!item)
                return false;
            
            if(item.IsKindOf("Crowbar") || item.IsKindOf("SledgeHammer"))
            {
                KR_BankingATM AdvATM;
                if(Class.CastTo(AdvATM, target.GetObject()))
                {
                    return AdvATM.m_Banking_CanBeRobbed;
                }
            }
            return false;
        }
        return true;
	}

    override void OnStartServer( ActionData action_data )
    {
		vector ATMPos = action_data.m_Target.GetObject().GetPosition();
        string RobMessage = "";
        for(int i = 0; i < GetKR_BankingServerConfig().ATM.Count(); i++)
        {
            float distance = vector.Distance(GetKR_BankingServerConfig().ATM.Get(i).GetATMPosition(), ATMPos);
            if(distance < 2)
            {
                //Very sure its the correct ATM!
                RobMessage = ReplacePlaceHolder(GetKR_BankingServerConfig().ATM.Get(i).GetRobMessage(), action_data.m_Player);
            }
        }

        array<Man> onlinePlayers = new array<Man>;
        GetGame().GetPlayers(onlinePlayers);
        if(onlinePlayers.Count() < GetKR_BankingServerConfig().MinPlayersForRob)
        {
            GetBankingServerManager().SendNotification("Sorry but player population is to low!", action_data.m_Player.GetIdentity());
            return;
        }
        for(int n = 0; n < onlinePlayers.Count(); n++)
        {
            PlayerBase player = PlayerBase.Cast(onlinePlayers.Get(n));
            GetBankingServerManager().SendNotification(RobMessage, player.GetIdentity());
        }
    }

    string ReplacePlaceHolder(string robMessage, PlayerBase player)
    {
        local string TempRobmsg = robMessage;
        if(TempRobmsg.Contains("%PlayerName%"))
        {
            TempRobmsg.Replace("%PlayerName%", player.GetIdentity().GetName());
        }
        if(TempRobmsg.Contains("%id%"))
        {
            TempRobmsg.Replace("%id%", player.GetIdentity().GetPlainId());
        }
        return TempRobmsg;
    }

	override void OnFinishProgressServer( ActionData action_data )
	{	
        array<Man> onlinePlayers = new array<Man>;
        GetGame().GetPlayers(onlinePlayers);
        if(onlinePlayers.Count() < GetKR_BankingServerConfig().MinPlayersForRob)
        {
            GetBankingServerManager().SendNotification("Sorry but player population is to low!", action_data.m_Player.GetIdentity());
            return;
        }
        
        int Money = Math.RandomInt(GetKR_BankingServerConfig().MinMoneyForRob, GetKR_BankingServerConfig().MaxMoneyForRob);
        GetBankingServerManager().AddCurrencyToPlayer(action_data.m_Player, Money);
        GetBankingServerManager().SendNotification("You sucessfully robbbed this atm you received: " + Money + " money!", action_data.m_Player.GetIdentity());

        KR_BankingATM AdvATM;
        if(Class.CastTo(AdvATM, action_data.m_Target.GetObject()))
        {
            AdvATM.m_Banking_CanBeRobbed = false;
            AdvATM.SetSynchDirty();
        }

        EntityAI item_in_Hands = EntityAI.Cast(action_data.m_Target.GetObject());
        if(item_in_Hands)
        {
            float max = item_in_Hands.GetMaxHealth("","");
			item_in_Hands.SetHealth( "", "", max * GameConstants.DAMAGE_RUINED_VALUE );
        }
	}

    override bool ActionConditionContinue( ActionData action_data )
	{	
		 KR_BankingATM AdvATM;
        if(Class.CastTo(AdvATM, action_data.m_Target.GetObject()))
        {
            return AdvATM.m_Banking_CanBeRobbed;
        }
        return false;
	}

    override void OnStartClient( ActionData action_data )
    {
        m_ClientSideTry++;
        if(m_ClientSideTry > 2)
        {
            KR_BankingATM AdvATM;
            if(Class.CastTo(AdvATM, action_data.m_Target.GetObject()))
            {
                AdvATM.m_Banking_CanBeRobbed = false;
            }
            GetBankingClientManager().SendNotification("You are now banned from Robbing this ATM because spamming!");
        }
    }

    override void OnEndClient( ActionData action_data )
	{
        m_ClientSideTry = 0;
    }
}