TopMenu g_hTopMenu;

void LoadAdminMenu()
{
    if (LibraryExists("adminmenu"))
	{
		TopMenu hTopMenu;
        if ((g_hTopMenu = GetAdminTopMenu()) != null)
        	OnAdminMenuReady(hTopMenu);
	}
}

public void OnAdminMenuReady(Handle hTopMenu) 
{
    if (hTopMenu)
    {
        TopMenu pTopMenu = TopMenu.FromHandle(hTopMenu);

        if(pTopMenu == g_hTopMenu) 
            return;

        g_hTopMenu = pTopMenu;

        TopMenuObject hMyCategory = g_hTopMenu.FindCategory(ADMINMENU_PLAYERCOMMANDS);

        if (hMyCategory != INVALID_TOPMENUOBJECT) {
            g_hTopMenu.AddItem("checkcheats_admin_category", CheckCheatsMainMenu_Handler, hMyCategory, "check_cheats", ADMFLAG_BAN);
        }
    }
}

public void CheckCheatsMainMenu_Handler(TopMenu hMenu, TopMenuAction action, TopMenuObject object_id, int iClient, char[] sBuffer, int maxlength) 
{
    char sMainMenuName[256];
    FormatEx(sMainMenuName, sizeof sMainMenuName, "%T", "AdminMenuCategoryName", iClient);

    switch(action) 
    {
        case TopMenuAction_DisplayOption: 
        {
            FormatEx(sBuffer, maxlength, "%s", sMainMenuName);
        }
        case TopMenuAction_SelectOption: 
        {
            //MainMenu(iClient);
			ChoosePlayerMenu(iClient);
        }
    }
}

