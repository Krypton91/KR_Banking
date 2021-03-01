class BankingDiscordObject_Body : BankingDiscordObject
{
	private int m_color;
	private ref array<ref BankingDiscordObject_Fields> m_otherFields;

	void BankingDiscordObject_Body(int color = -1)
	{
		m_color = color;
		m_otherFields = new array<ref BankingDiscordObject_Fields>;
	}
	
	override protected string ConvertSpecialFields()
	{
		string addedFieldsToJson = "";
		
		addedFieldsToJson += GetColorField();
		addedFieldsToJson += GetOtherFields();
		return addedFieldsToJson;
	}
	
	private string GetColorField()
	{
		if (m_color > -1)
		{
			return BankingDiscordHelper.GetJSONLineOthers("color", m_color.ToString());
		}
		return "";
	}
	
	private string GetOtherFields()
	{
		string dataFields = "";

		if (m_otherFields.Count() > 0)
		{
			dataFields += "[";
			foreach (auto otherField : m_otherFields)
			{
				dataFields += otherField.DefaultJSONConvert();
			}
			dataFields += "],";
		}
		return dataFields;
	}

	private void AddNewField(string name, string value, bool inline = false)
	{
		m_otherFields.Insert(new BankingDiscordObject_Fields(name, value, inline));
	}

	override protected bool CanSetKey(string key)
	{
		switch (key)
		{
			case "title":
				return true;
			case "description":
				return true;
			case "url":
				return true;
			case "timestamp":
				return true;
		}
		return false;
	}
};