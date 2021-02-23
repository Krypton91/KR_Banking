class DiscordObject_Body : DiscordObject
{
	private int m_color;
	private ref array<ref DiscordObject_Fields> m_otherFields;

	void DiscordObject_Body(int color = -1)
	{
		m_color = color;
		m_otherFields = new array<ref DiscordObject_Fields>;
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
			return DiscordHelper.GetJSONLineOthers("color", m_color.ToString());
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
		m_otherFields.Insert(new DiscordObject_Fields(name, value, inline));
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