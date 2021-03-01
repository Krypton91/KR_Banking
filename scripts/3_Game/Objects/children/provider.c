class BankingDiscordObject_Provider : BankingDiscordObject
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
		}
		return false;
	}
};