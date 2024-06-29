public void MainMenu(int iClient) {
    char sBuffer[256];

    Menu hMenu = new Menu(MainMenu_Handler);

    hMenu.SetTitle("%T", "MainMenuTitle", iClient);

    FormatEx(sBuffer, sizeof(sBuffer), "%T", "MainMenuFirstItem", iClient);
    hMenu.AddItem("0", sBuffer);

    hMenu.ExitBackButton = true;
    hMenu.ExitButton = true;

    hMenu.Display(iClient, MENU_TIME_FOREVER);
}

public int MainMenu_Handler(Menu hMenu, MenuAction mAction, int iClient, int iSlot) 
{
    switch(mAction) 
    {
        case MenuAction_End: delete hMenu;
        case MenuAction_Cancel: {
            if(iSlot == MenuCancel_ExitBack) 
                RedisplayAdminMenu(g_hTopMenu, iClient);
        }
        case MenuAction_Select: {
            char sInfo[2];

			GetMenuItem(hMenu, iSlot, sInfo, sizeof sInfo);

            switch(sInfo[0]) {
                case '0': ChoosePlayerMenu(iClient);
            }
        } 
    }
}

public void ChoosePlayerMenu(int iClient) 
{
    int iTarget = -1;
    if((iTarget = GetClientOfUserId(g_iActionPlayer[iClient])) > 0) 
    {
        CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "PlayerAlreadyCheck" : "PlayerAlreadyCheck ClientMod"), iClient, iTarget);
        return;
    }

    Menu hMenu = new Menu(ChoosePlayerMenu_Handler);
    hMenu.SetTitle("%T", "ChoosePlayerMenuTitle", iClient);

    char sID[16], szName[MAX_NAME_LENGTH];

    bool bCheck = false;
    int iUserFlags = 0;
    for (int i = 1; i <= MaxClients; i++)
    {
        if (CC_IsValidClient(i))
        {
            iUserFlags = GetUserFlagBits(i);
            if (iUserFlags & ADMFLAG_ROOT)
                continue;

            if (!iUserFlags || (!g_bHideAdmins && (iUserFlags & ADMFLAG_BAN)))
            {
                IntToString(GetClientUserId(i), sID, sizeof(sID));
                GetClientName(i, szName, MAX_NAME_LENGTH);

                bCheck = (CC_IsCheckedPlayer(i) > 0);
                if (bCheck)
                    Format(szName, MAX_NAME_LENGTH, "%s %T", szName, "ChoosePlayerOnVerification", iClient);

                hMenu.AddItem(sID, szName, (!bCheck ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED));
            }
        }
    }

    if (hMenu.ItemCount <= 0)
    {
        FormatEx(szName, MAX_NAME_LENGTH, "%T", "PlayersNotFound", iClient);
        hMenu.AddItem(NULL_STRING, szName, ITEMDRAW_DISABLED);
    }

    hMenu.ExitBackButton = true;
    hMenu.ExitButton = true;

    hMenu.Display(iClient, MENU_TIME_FOREVER);
}

public int ChoosePlayerMenu_Handler(Menu hMenu, MenuAction mAction, int iClient, int iSlot) 
{
    switch (mAction) 
    {
        case MenuAction_End: delete hMenu;
        case MenuAction_Cancel: 
        {
            if(iSlot == MenuCancel_ExitBack) 
                MainMenu(iClient);
        }
        case MenuAction_Select: 
        {
            char sInfo[256];
            hMenu.GetItem(iSlot, sInfo, sizeof(sInfo));

            int iClientChoose = GetClientOfUserId(StringToInt(sInfo));
            if (CC_IsValidClient(iClientChoose)) 
            {
                g_iActionPlayer[iClient] = StringToInt(sInfo);
                MakeVerify(iClient, iClientChoose);
            } 
            else CPrintToChat(iClient, "%T", (!CM_IsClientModUser(iClient) ? "CheckPlayerLeft" : "CheckPlayerLeft ClientMod"), iClient, iClientChoose);  
        }
    }
}

public void ChooseMessengerMenu(int iClient) 
{
    Menu hMenu = new Menu(ChooseMessengerMenu_Handler);
    hMenu.SetTitle("%T\n ", "ChooseMessengerMenuTitle", iClient);

    hMenu.AddItem("ds", "Discord");

    char sBuffer[256];
    FormatEx(sBuffer, sizeof(sBuffer), "%T", "RefusalToCheck", iClient);
    hMenu.AddItem("refusal", sBuffer);

    hMenu.ExitBackButton = true;
    hMenu.ExitButton = true;

    hMenu.Display(iClient, MENU_TIME_FOREVER);
}

public int ChooseMessengerMenu_Handler(Menu hMenu, MenuAction mAction, int iClient, int iSlot) 
{
    char sInfo[256];
    
    switch(mAction) 
    {
        case MenuAction_End: {
            delete hMenu;
        }
        case MenuAction_Select: {
            if(g_esPlayerInfo[iClient].iStatusCheck == STATUS_NOTCHECKING) 
            {
                CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "InteractionErrorPlayer" : "InteractionErrorPlayer ClientMod"));
                return 0;
            }

            hMenu.GetItem(iSlot, sInfo, sizeof(sInfo));

            switch (sInfo[0])
            {
                case 'd': {
                    g_iMessenger[iClient] = 1;
                    CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "PlayerNotifyWriteData" : "PlayerNotifyWriteData ClientMod"));
                }
                case 'r': {
                    if(g_bBanEnabled) 
                        CC_BanClient(iClient, 0, false);
                }
            }
        }
    }

    return 0;
}

public void Menu_PanelCheck(int iClient) 
{
	int iClientChoose = GetClientOfUserId(g_iActionPlayer[iClient]);
    if (CC_IsValidClient(iClientChoose))
    {
        char szBuffer[128];

        Menu hMenu = new Menu(Menu_PanelCheck_Handler);
        hMenu.SetTitle("%T", "CheckMenuTitle", iClient, iClientChoose, GetStatus(iClient, g_esPlayerInfo[iClientChoose].iStatusCheck));

        switch (g_esPlayerInfo[iClientChoose].iStatusCheck)
        {
            case STATUS_WAITCOMMUNICATION:
            {
                FormatEx(szBuffer, sizeof(szBuffer), "%T", "CheckMenuNotifyItem", iClient);
                hMenu.AddItem("Notif", szBuffer);

                FormatEx(szBuffer, sizeof(szBuffer), "%T", "EndCheckPlayer", iClient);
                hMenu.AddItem("GoodResult", szBuffer);
            }
            case STATUS_WAITCALL:
            {
                FormatEx(szBuffer, sizeof(szBuffer), "%T", "CheckMenuFirstItem", iClient, "Discord", g_esPlayerInfo[iClientChoose].sDiscord);
                AddMenuItem(hMenu, "Status", szBuffer);

                FormatEx(szBuffer, sizeof(szBuffer), "%T", "EndCheckPlayer", iClient);
                hMenu.AddItem("GoodResult", szBuffer);
            }
            case STATUS_CHECKING:
            {
                FormatEx(szBuffer, sizeof(szBuffer), "%T", "CheckEnded", iClient);
                AddMenuItem(hMenu, "Status", szBuffer);

                FormatEx(szBuffer, sizeof(szBuffer), "%T", "EndCheckPlayer", iClient);
                hMenu.AddItem("GoodResult", szBuffer);
            }
            case STATUS_RESULT:
            {
                FormatEx(szBuffer, sizeof(szBuffer), "%T", "GoodResult", iClient);
                AddMenuItem(hMenu, "GoodResult", szBuffer);

                FormatEx(szBuffer, sizeof(szBuffer), "%T", "BadResult", iClient);
                hMenu.AddItem("BadResult", szBuffer);
            }
        }

        if (!g_esPlayerInfo[iClientChoose].bBlockSpec) 
        {
            int iTeam = GetClientTeam(iClientChoose);

            FormatEx(szBuffer, sizeof(szBuffer), "%T", (iTeam != CS_TEAM_SPECTATOR ? "PlayerToSpec" : "PlayerBlockSpec"), iClient);
            hMenu.AddItem((iTeam != CS_TEAM_SPECTATOR ? "ToSpec" : "BlockSpec"), szBuffer);
        }

        hMenu.ExitBackButton = true;
        hMenu.ExitButton = true;

        hMenu.Display(iClient, MENU_TIME_FOREVER);
    }
}

public int Menu_PanelCheck_Handler(Menu hMenu, MenuAction mAction, int iClient, int iSlot) 
{
    switch (mAction) 
    {
        case MenuAction_End: 
        {
            delete hMenu;
        }
        case MenuAction_Select: 
        {
            char sInfo[128];
            hMenu.GetItem(iSlot, sInfo, sizeof(sInfo));

            if (g_iActionPlayer[iClient] > 0)
            {
                int iClientChoose = GetClientOfUserId(g_iActionPlayer[iClient]);
                if (!CC_IsValidClient(iClientChoose)) 
                {
                    CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "InteractionErrorAdmin" : "InteractionErrorAdmin ClientMod"));

                    return;
                }

                if (!strcmp(sInfo, "ToSpec"))
                {
                    ChangeClientTeam(iClientChoose, CS_TEAM_SPECTATOR);
                    CC_PrintLog(iClient, iClientChoose, "PlayerToSpec", "");
                    CPrintToChat(iClientChoose, "%t", (!CM_IsClientModUser(iClientChoose) ? "PlayerToSpecText" : "PlayerToSpecText ClientMod"));
                    CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "PlayerToSpecAdmin" : "PlayerToSpecAdmin ClientMod"));
                }
                else if (!strcmp(sInfo, "Notif"))
                {
                    CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "PlayerNotifyAdmin" : "PlayerNotifyAdmin ClientMod"), iClientChoose);
                    CPrintToChat(iClientChoose, "%t", (!CM_IsClientModUser(iClientChoose) ? "PlayerNotifyWriteData" : "PlayerNotifyWriteData ClientMod"));
                } 
                else if (!strcmp(sInfo, "BlockSpec"))
                {
                    g_esPlayerInfo[iClientChoose].bBlockSpec = true;

                    CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "PlayerBlockSpecAdmin" : "PlayerBlockSpecAdmin ClientMod"), iClientChoose);
                    CPrintToChat(iClientChoose, "%t", (!CM_IsClientModUser(iClientChoose) ? "PlayerBlockSpecText" : "PlayerBlockSpecText ClientMod"));
                    CC_PrintLog(iClient, iClientChoose, "PlayerBlockSpec", "");
                }
                else if (!strcmp(sInfo, "Status"))
                {
                    g_esPlayerInfo[iClientChoose].iStatusCheck++;
                }
                else if (!strcmp(sInfo, "GoodResult"))
                {
                    g_iActionPlayer[iClient] = 0;

                    g_esPlayerInfo[iClientChoose].ResetVerify(iClientChoose);

                    CC_PrintLog(iClient, iClientChoose, "PlayerCheatsNotFound", "");
                    CPrintToChat(iClientChoose, "%t", (!CM_IsClientModUser(iClientChoose) ? "PlayerCheatsNotFound" : "PlayerCheatsNotFound ClientMod"));
                    CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "PlayerCheatsNotFoundAdmin" : "PlayerCheatsNotFoundAdmin ClientMod"));
                }
                else if (!strcmp(sInfo, "BadResult"))
                {
                    g_iActionPlayer[iClient] = 0;

                    g_esPlayerInfo[iClientChoose].ResetVerify(iClientChoose);

                    CC_PrintLog(iClient, iClientChoose, "PlayerCheatsFound", "");
                    CPrintToChat(iClientChoose, "%t", (!CM_IsClientModUser(iClientChoose) ? "PlayerCheatsFound" : "PlayerCheatsFound ClientMod"));
                    CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "PlayerCheatsFoundAdmin" : "PlayerCheatsFoundAdmin ClientMod"));

                    if(g_bBanEnabled) 
                        CC_BanClient(iClientChoose, iClient, false);
                }
            }
        }
    }
}