class PluginBankingLogManagerServer extends PluginBase
{
    protected string m_BankingLogDir = m_BankingProfileDIR + "KR_BANKING/ServerLogs/";
    FileHandle m_LogFileHandler;
    int year, month, day, hour, minute, second;
	string sYear, sMonth, sDay, sHour, sMinute, sSecond, currentTime;
    string m_CurrentBankingLogFileName = "";

    void PluginBankingLogManagerServer()
    {
        Print("[Advanced Banking] -> LogService starting....");
        Print("[Advanced Banking] -> LogMode is: " + GetCurrentLogMode().ToString());

        if(GetCurrentLogMode() == 1)
        {
            if(m_CurrentBankingLogFileName == "")
            {
                m_CurrentBankingLogFileName =  GetTimeStamp2() + ".log";
                if(!FileExist(m_BankingLogDir))
                    MakeDirectory(m_BankingLogDir);
                
                if(FileExist(m_BankingLogDir + m_CurrentBankingLogFileName))
                {
                    DeleteFile(m_BankingLogDir + m_CurrentBankingLogFileName);
                }

                CreateLogFile();
            }
        }
        else
        {
            Log("LogService Inited!");
        }
    }

    void CreateLogFile()
    {
        m_LogFileHandler = OpenFile(m_BankingLogDir + "/" + m_CurrentBankingLogFileName, FileMode.WRITE);
        if(m_LogFileHandler != 0)
        {
            Log("Succesfully started BankingLogger");
        }
        else
        {
            Error("ERROR UNABLE TO CREATE LOGFILE!!");
        }
    }

    void Log(string Message)
    {
        if(GetCurrentLogMode() == 1)
        {
            FPrintln(m_LogFileHandler, GetTimeStamp() + " | " + Message);
        }
        else if(GetCurrentLogMode() == 2)
        {
            GetGame().AdminLog(GetTimeStamp() + " | " + "[Advanced Banking]-> " + Message);
        }
        else if(GetCurrentLogMode() == 3)
        {
            Print(GetTimeStamp() + " | " + "[Advanced Banking]-> " + Message);
        }
        else if(GetCurrentLogMode() == 0)
        {
            
        }
        else
        {
            Error(GetCurrentLogMode().ToString() + " is not a valid LogMode! Please read install Guide on steam!");
        }
    }

    protected int GetCurrentLogMode()
    {
        return GetKR_BankingServerConfig().m_LoggingSettings.m_LogMode;
    }

    protected string GetTimeStamp()
    {
            // Setting Time Variables for new file name
			GetYearMonthDay(year, month, day);
			GetHourMinuteSecond(hour, minute, second);
			
			// Setting String for Time Variables for new file name
			sYear = year.ToString();
			
			sMonth = month.ToString();
			if (sMonth.Length() == 1)
			{
				sMonth = "0" + sMonth;
			}
			
			sDay = day.ToString();
			if (sDay.Length() == 1)
			{
				sDay = "0" + sDay;
			}
			
			sHour = hour.ToString();
			if (sHour.Length() == 1)
			{
				sHour = "0" + sHour;
			}
			
			sMinute = minute.ToString();
			if (sMinute.Length() == 1)
			{
				sMinute = "0" + sMinute;
			}
			
			sSecond = second.ToString();
			if (sSecond.Length() == 1)
			{
				sSecond = "0" + sSecond;
			}
            currentTime = "[Date: " + sDay + "." + sMonth + "." + sYear + " time: " + sHour + ":" + sMinute + ":" + sSecond + "]";
			//currentTime =  sYear + "-" + sMonth + "-" + sDay + "_" + sHour + "-" + sMinute + "-" + sSecond;
            return currentTime;

    }

    protected string GetTimeStamp2()
    {
            // Setting Time Variables for new file name
			GetYearMonthDay(year, month, day);
			GetHourMinuteSecond(hour, minute, second);
			
			// Setting String for Time Variables for new file name
			sYear = year.ToString();
			
			sMonth = month.ToString();
			if (sMonth.Length() == 1)
			{
				sMonth = "0" + sMonth;
			}
			
			sDay = day.ToString();
			if (sDay.Length() == 1)
			{
				sDay = "0" + sDay;
			}
			
			sHour = hour.ToString();
			if (sHour.Length() == 1)
			{
				sHour = "0" + sHour;
			}
			
			sMinute = minute.ToString();
			if (sMinute.Length() == 1)
			{
				sMinute = "0" + sMinute;
			}
			
			sSecond = second.ToString();
			if (sSecond.Length() == 1)
			{
				sSecond = "0" + sSecond;
			}
            //currentTime = sDay + "." + sMonth + "." + sYear + " | " + sHour + ":" + sMinute + ":" + sSecond;
			currentTime =  sYear + "-" + sMonth + "-" + sDay + "_" + sHour + "-" + sMinute + "-" + sSecond;
            return currentTime;

    }
};

/* Global Plugin Getter */ 
PluginBankingLogManagerServer GetBankingLogManager()
{
    if(GetGame().IsServer() && GetGame().IsMultiplayer())
    {
        return PluginBankingLogManagerServer.Cast(GetPluginManager().GetPluginByType(PluginBankingLogManagerServer));
    }
    return NULL;
};