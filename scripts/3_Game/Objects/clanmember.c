class ClanMemberObject
{
    protected string m_plainID;
    protected string m_playername;
    ref PermissionObject m_permissions;

    void ClanMemberObject(string PlainID, string PlayerName, PermissionObject Permission)
    {
        m_plainID = PlainID;
        m_playername = PlayerName;
        SetPermission(Permission);
    }

    void SetPermission(ref PermissionObject Permissions)
    {
        m_permissions = Permissions;
    }

    ref PermissionObject GetPermission()
    {
        return m_permissions;
    } 

    string GetPlainID()
    {
        return m_plainID;
    }

    string GetPlayerName()
    {
        return m_playername;
    }
}