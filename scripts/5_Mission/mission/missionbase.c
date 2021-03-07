modded class MissionBase 
{
    override UIScriptedMenu CreateScriptedMenu (int id) 
    {
        UIScriptedMenu menu = NULL;
        menu = super.CreateScriptedMenu(id);
        if (!menu) 
        {
            switch (id) 
            {
                case KR_BANKING_MENU:
                    menu = new KR_BankingMenu;
                    break;
                case KR_BANKING_ADMIN_MENU:
                    menu = new KR_AdminMenu;
                    break;
            }
            if (menu)
            {
                menu.SetID(id);
            }
        }
        return menu;
    }
};