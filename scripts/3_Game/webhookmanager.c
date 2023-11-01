class WebhookManager
{
	ref map<string, string> m_alias;
	ref BankingDiscordJSON data;
	ref BankingDiscordObject_Body dataBody;
	ref BankingDiscordObject_Body admindataBody;
	ref BankingDiscordObject_Thumbnail dataThumb;

	void WebhookManager()
	{
		if(GetKR_BankingServerConfig().m_DiscordWebhook.m_UseWebhook)
		{
			m_alias = new map<string, string>;
			data = new BankingDiscordJSON();
			dataBody = new BankingDiscordObject_Body(59135);
			admindataBody = new BankingDiscordObject_Body(8395804);
			dataThumb = new BankingDiscordObject_Thumbnail(64, 64);
			m_alias.Set("Advanced Banking", GetKR_BankingServerConfig().m_DiscordWebhook.m_WebhookURL);
		}
	}
	
	bool POST(string alias, string message)
	{
		if(!GetKR_BankingServerConfig().m_DiscordWebhook.m_UseWebhook)
		{
			return false;
		}
		dataBody.SetField("title", "Log Entry");
		dataBody.SetField("description", message);
		dataThumb.SetField("url", "https://media.discordapp.net/attachments/757674333480157184/813195126674554951/brand-logo-maker-with-abstract-graphics-1519a.png?width=675&height=675");
		data.m_body = dataBody;
		data.m_thumbnail = dataThumb;
		
		RestCallback cbx1 = new WebhookRestCB;
		RestApi clCore = GetRestApi();

		if (m_alias.Contains(alias))
		{
			if (!clCore)
			{
				clCore = CreateRestApi();
			}
			RestContext ctx = clCore.GetRestContext(m_alias.Get(alias));
			ctx.SetHeader("application/json");
			ctx.POST(cbx1, "", WrapData(data));
			return true;
		}
		return false;
	}

	void PlayerLog(PlayerIdentity identity, string action)
	{
		if(!GetKR_BankingServerConfig().m_DiscordWebhook.m_UseWebhook)
			return;

		dataBody.SetField("title", "Log Entry");
		string Message = "**Player:** [" + identity.GetName() + "](http://steamcommunity.com/profiles/" + identity.GetPlainId() + ") | Steam64: " + identity.GetPlainId() + "\\n\\n__**Action: **__ \\n" + action;
		dataBody.SetField("description", Message);
		dataThumb.SetField("url", "https://media.discordapp.net/attachments/757674333480157184/813195126674554951/brand-logo-maker-with-abstract-graphics-1519a.png?width=675&height=675");
		data.m_body = dataBody;
		data.m_thumbnail = dataThumb;
		
		RestCallback cbx1 = new WebhookRestCB;
		RestApi clCore = GetRestApi();

		if (m_alias.Contains("Advanced Banking"))
		{
			if (!clCore)
			{
				clCore = CreateRestApi();
			}
			
			RestContext ctx = clCore.GetRestContext(m_alias.Get("Advanced Banking"));
			ctx.SetHeader("application/json");
			ctx.POST(cbx1, "", WrapData(data));
		}
	}

	void AdminLog(PlayerIdentity identity, string action)
	{
		if(!GetKR_BankingServerConfig().m_DiscordWebhook.m_UseWebhook)
			return;

		admindataBody.SetField("title", "**ADMIN LOG**");
		string Message = "**Admin:** [" + identity.GetName() + "](http://steamcommunity.com/profiles/" + identity.GetPlainId() + ") | Steam64: " + identity.GetPlainId() + "\\n\\n__**Admin activity: **__ \\n" + action;
		admindataBody.SetField("description", Message);
		dataThumb.SetField("url", "https://media.discordapp.net/attachments/757674333480157184/813195126674554951/brand-logo-maker-with-abstract-graphics-1519a.png?width=675&height=675");
		data.m_body = admindataBody;
		data.m_thumbnail = dataThumb;
		
		RestCallback cbx1 = new WebhookRestCB;
		RestApi clCore = GetRestApi();

		if (m_alias.Contains("Advanced Banking"))
		{
			if (!clCore)
			{
				clCore = CreateRestApi();
			}
			RestContext ctx = clCore.GetRestContext(m_alias.Get("Advanced Banking"));
			ctx.SetHeader("application/json");
			ctx.POST(cbx1, "", WrapData(data));
		}
	}

	private string WrapData(BankingDiscordJSON webhookData)
	{
		string json = "{\"embeds\":[";
		
		json += webhookData.GetJSON();
		json += "]}";
		return json;
	}
};

class WebhookRestCB : RestCallback
{
	override void OnError(int errorCode) {};
	override void OnTimeout() {};
	override void OnSuccess(string data, int dataSize) {};
};

static ref WebhookManager g_WebhookManager;
static WebhookManager GetWebhookManager()
{
	if (!g_WebhookManager)
	{
		g_WebhookManager = new WebhookManager;
	}
	return g_WebhookManager;
}