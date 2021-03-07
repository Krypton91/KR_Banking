class AdminPermissions
{
    bool m_CanUseServerConfig;
    bool m_CanUsePlayersBankData;
    bool m_CanUsePlayersClans;
    bool m_CanUseMiscServer;
    bool m_CanAddAdmins;
    bool m_CanIgnoreNamePermission = false;

    void AdminPermissions(bool CanUseServerConfig = false, bool CanUsePlayersBankData = false, bool CanUsePlayersClans = false, bool CanUseMiscServer = false, bool CanAddAdmins = false)
    {
        m_CanUseServerConfig = CanUseServerConfig;
        m_CanUsePlayersBankData = CanUsePlayersBankData;
        m_CanUsePlayersClans = CanUsePlayersClans;
        m_CanUseMiscServer = CanUseMiscServer;
        m_CanAddAdmins = CanAddAdmins;
    }

    void GiveAllPermissions()
    {
        m_CanUseServerConfig = true;
        m_CanUsePlayersBankData = true;
        m_CanUsePlayersClans = true;
        m_CanUseMiscServer = true;
        m_CanAddAdmins = true;
        m_CanIgnoreNamePermission = true;
    }

    bool IsSuperAdmin()
    {
        if(m_CanUseServerConfig && m_CanUsePlayersBankData && m_CanUsePlayersClans && m_CanUseMiscServer)
            return true;
        
        return false;
    }
};