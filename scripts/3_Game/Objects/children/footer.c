class BankingDiscordObject_Footer : BankingDiscordObject
{
	void BankingDiscordObject_Footer(string text)
	{
		m_fields.Set("text", text);
	}

	override string DefaultJSONConvert()
	{
		return ConvertToJsonCloseByObject("footer");
	}

	override protected bool CanSetKey(string key)
	{
		switch (key)
		{
			case "text":
				return true;
			case "icon_url":
				return true;
			case "proxy_icon_url":
				return true;
		}
		return false;
	}
};