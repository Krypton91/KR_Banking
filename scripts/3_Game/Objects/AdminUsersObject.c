class AdminUsers
{
    string m_PlainID;
    string m_Name;
    ref AdminPermissions m_permissions;

    void AdminUsers(string PlainID = "", string Name = "", ref AdminPermissions permissions = null)
    {
        m_PlainID = PlainID;
        m_Name = Name;
        m_permissions = permissions;
    }
};