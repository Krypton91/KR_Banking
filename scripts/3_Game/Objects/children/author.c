class BankingDiscordObject_Author : BankingDiscordObject
{
	override string DefaultJSONConvert()
	{
		return ConvertToJsonCloseByObject("provider");
	}

	override protected bool CanSetKey(string key)
	{
		switch (key)
		{
			case "name":
				return true;
			case "url":
				return true;
			case "icon_url":
				return true;
			case "proxy_icon_url":
				return true;
		}
		return false;
	}
};