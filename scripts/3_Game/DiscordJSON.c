class BankingDiscordJSON
{
	ref BankingDiscordObject_Body m_body;
	ref BankingDiscordObject_Footer m_footer;
	ref BankingDiscordObject_Image m_image;
	ref BankingDiscordObject_Thumbnail m_thumbnail;
	ref BankingDiscordObject_Video m_video;
	ref BankingDiscordObject_Provider m_provider;
	ref BankingDiscordObject_Author m_author;

	string GetJSON()
	{
		string jsonData = "{";
		
		jsonData += GetModuleJSON(m_body);
		jsonData += GetModuleJSON(m_footer);
		jsonData += GetModuleJSON(m_image);
		jsonData += GetModuleJSON(m_thumbnail);
		jsonData += GetModuleJSON(m_video);
		jsonData += GetModuleJSON(m_provider);
		jsonData += GetModuleJSON(m_author);
		jsonData += "}";
		jsonData = BankingDiscordHelper.RemoveExtraCommasJSON(jsonData);
		return jsonData;
	}
	
	string GetModuleJSON(ref BankingDiscordObject module)
	{
		if (module)
		{
			return module.DefaultJSONConvert();
		}
		return "";
	}
};