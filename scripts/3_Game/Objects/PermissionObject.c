class PermissionObject
{
    bool m_CanWithdraw;
    bool m_CanDeposit;
    bool m_CanInvite;
    bool m_CanKick;
    bool m_CanEdit;

    void PermissionObject(bool CanWithdraw = false, bool CanDeposit = false, bool CanInvite = false, bool CanKick = false, bool CanEdit = false)
    {
        m_CanWithdraw = CanWithdraw;
        m_CanDeposit = CanDeposit;
        m_CanInvite = CanInvite;
        m_CanKick = CanKick;
        m_CanEdit = CanEdit;
    }

    bool HasAnyPermission()
    {
        if(m_CanWithdraw || m_CanDeposit || m_CanInvite || m_CanKick || m_CanEdit)
            return true;
        return false;
    }

    void GiveClanOwner()
    {
        m_CanWithdraw = true;
        m_CanDeposit = true;
        m_CanInvite = true;
        m_CanKick = true;
        m_CanEdit = true;
    }
}