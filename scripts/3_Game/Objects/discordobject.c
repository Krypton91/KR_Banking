class BankingDiscordObject
{
	protected ref map<string, string> m_fields;

	void BankingDiscordObject()
	{
		m_fields = new map<string, string>;
	}
	
	string DefaultJSONConvert()
	{
		return ConvertToJson();
	}

	protected string ConvertToJson()
	{
		string jsonData = "";
		array<string> keys =  m_fields.GetKeyArray();
		
		for (int keyIndex = 0; keyIndex < keys.Count(); keyIndex++)
		{
			jsonData += FormatField(keys.Get(keyIndex));
		}
		jsonData += ConvertSpecialFields();
		return jsonData;
	}

	protected string ConvertToJsonCloseByObject(string fieldName)
	{
		string jsonData = "\"" + fieldName + "\":{";
		
		foreach (string key : m_fields.GetKeyArray())
		{
			jsonData += FormatField(key);
		}
		jsonData += ConvertSpecialFields() + "},";
		return jsonData;
	}

	protected string ConvertToJsonCloseByArray()
	{
		string jsonData = "[";
		
		foreach (string key : m_fields.GetKeyArray())
		{
			jsonData += FormatField(key);
		}
		jsonData += ConvertSpecialFields() + "]";
		return jsonData;
	}
	protected string ConvertSpecialFields() {return "";}

	protected string FormatField(string key)
	{
		return BankingDiscordHelper.GetJSONLineString(key, m_fields.Get(key));
	}
	
	protected bool CanSetKey(string key)
	{
		return true;
	}
	
	void SetField(string field, string value)
	{
		if (CanSetKey(field))
		{
			m_fields.Set(field, value);
		}
	}
	
	void RemoveField(string field)
	{
		if (m_fields.Contains(field))
		{
			m_fields.Remove(field);
		}
	}
};