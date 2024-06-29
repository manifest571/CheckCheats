TopMenu g_hTopMenu;

public void OnAdminMenuReady(Handle hTopMenu) 
{
    TopMenu pTopMenu = TopMenu.FromHandle(hTopMenu);

    if(pTopMenu == g_hTopMenu) 
        return;

    g_hTopMenu = pTopMenu;

    g_hTopMenu.AddCategory("checkcheats_admin_category", CheckCheatsMainMenu_Handler, "check_cheats", ADMFLAG_BAN);

    //if(hMyCategory != INVALID_TOPMENUOBJECT) {
    //    g_hTopMenu.AddItem(CHECKCHEATS_MAINMENU, CheckCheatsMainMenu_Handler, hMyCategory, "check_cheats", ADMFLAG_BAN, "");
    //}
}

public void CheckCheatsMainMenu_Handler(TopMenu hMenu, TopMenuAction action, TopMenuObject object_id, int iClient, char[] sBuffer, int maxlength) 
{
    char sMainMenuName[256];
    FormatEx(sMainMenuName, sizeof sMainMenuName, "%T", "AdminMenuCategoryName", iClient);

    switch(action) 
    {
        case TopMenuAction_DisplayOption: 
        {
            FormatEx(sBuffer, maxlength, sMainMenuName);
        }
        case TopMenuAction_SelectOption: 
        {
            MainMenu(iClient);
        }
    }
}

