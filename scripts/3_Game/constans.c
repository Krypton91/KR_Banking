static const int KR_BANKING_MENU = 86721531;
static const int KR_BANKING_ADMIN_MENU = 32236412;
const protected static string m_BankingProfileDIR = "$profile:";
const protected static string m_BankingConfigDIR = "KR_BANKING";
const protected static string m_BankingConfigPath = m_BankingProfileDIR + m_BankingConfigDIR + "/" + "server-config.json";
bool IsBankingAdminDataRecived = false; //This sure client has this data already from remote!
bool IsBankingAdmin = false;//This goes to true if a player has admin on the bank!