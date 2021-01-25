modded class PluginManager
{	

	//=================================
	// Init
	//=================================
	void Init()
	{	
		super.Init();
		//----------------------------------------------------------------------
		// Register modules
		//----------------------------------------------------------------------
		//				Module Class Name 						Client	Server
		//----------------------------------------------------------------------
		if(GetGame().IsServer() && !GetGame().IsClient() && GetGame().IsMultiplayer())
		{
			RegisterPlugin( "PluginKRBankingManagerServer",			false, 	true );
		}
		else
		{
			RegisterPlugin( "PluginKrBankingClientManager", 		true, 	false );
		}
	}
}
