#pragma tabsize 0
#pragma semicolon 1

#include <cstrike>
#include <sourcemod>
#include <adminmenu>
#include <sdktools>

#undef REQUIRE_PLUGIN
#include <materialadmin>
#define REQUIRE_PLUGIN

#include <clientmod>
#include <clientmod/multicolors.inc>

#include "checkcheats/vars.inc"
#include "checkcheats/adminmenu.inc"
#include "checkcheats/functions.inc"
#include "checkcheats/menus.inc"

public Plugin myinfo = {
    name = "[Fork] CheckCheats",
    author = "Manifest (original xyligan & Nico Yazawa)",
    description = "Plugin for checking players for cheats",
    version = "4.0.0 [BETA]",
    url = "nevada-css.ru"
}

#define CHECKCHEATS_MAINMENU "CheckCheats_MainMenu"

public void OnPluginStart() 
{
    BuildPath(Path_SM, g_sLogPath, PLATFORM_MAX_PATH, "logs/check_cheats.log");

    LoadCvars();

    AutoExecConfig(true, "CheckCheats");
    LoadTranslations("checkcheats.phrases");

    CreateTimer(0.1, Timer_GiveOverlay, _, TIMER_REPEAT);
	
	AddCommandListener(Command_JoinTeam, "jointeam");

    if(LibraryExists("adminmenu")) {
        TopMenu hTopMenu;

        if((hTopMenu = GetAdminTopMenu()) != null) {
            OnAdminMenuReady(hTopMenu);
        }
    }
}

public void OnClientConnected(int iClient) 
{
    int iAdmin = CC_IsCheckedPlayer(iClient);
    if (iAdmin > 0)
        g_iActionPlayer[iAdmin] = 0;

    g_esPlayerInfo[iClient].ResetVerify(iClient);
}

public void OnLibraryRemoved(const char[] szName) 
{
    if(!strcmp(szName, "adminmenu"))
        g_hTopMenu = null;
}

public void OnClientDisconnect(int iClient) 
{
    int iClientChoose = -1;
    if (g_iActionPlayer[iClient] > 0)
    {
        iClientChoose = GetClientOfUserId(g_iActionPlayer[iClient]);
        if (CC_IsValidClient(iClientChoose))
        {
            CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "CheckLeaveAdmin" : "CheckLeaveAdmin ClientMod"));
            CC_PrintLog(iClient, iClientChoose, "CheckLeaveAdmin", "");

            g_esPlayerInfo[iClient].ResetVerify(iClientChoose);
            return;
        }
    }

    // iClientChoose == ADMIN WHO CHECKED IT
    if ((iClientChoose = CC_IsCheckedPlayer(iClient)) > 0)
    {
        if (STATUS_NOTCHECKING < g_esPlayerInfo[iClientChoose].iStatusCheck < STATUS_RESULT)
        {
            g_esPlayerInfo[iClient].ResetVerify(iClientChoose);

            CPrintToChat(iClientChoose, "%t", (!CM_IsClientModUser(iClientChoose) ? "CheckLeavePlayer" : "CheckLeavePlayer ClientMod"));
            CC_PrintLog(iClient, iClientChoose, "CheckPlayerLeave", "");

            if(g_bBanEnabled) 
                CC_BanClient(iClient, iClientChoose, true);
        }
    }
}

public Action Command_JoinTeam(int iClient, const char[] sCommand, int iArgs) 
{
    if (CC_IsCheckedPlayer(iClient) && g_esPlayerInfo[iClient].bBlockSpec) 
    {
		CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "BlockSpecText" : "BlockSpecText ClientMod"));
		
        return Plugin_Handled;
	}

	return Plugin_Continue;
}

public Action Timer_GiveOverlay(Handle hTimer) 
{
    int iAdmin = -1;
    for (int i = 1; i <= MaxClients; i++)
    {
        if (CC_IsValidClient(i))
        {
            if ((iAdmin = CC_IsCheckedPlayer(i)) > 0)
            {
                if (!g_esPlayerInfo[i].sDiscord[0])
                {
                    if (g_iWaitTime > 0 && g_esPlayerInfo[i].iWaitMessengerTime <= 0)
                    {
                        CC_PrintLog(iAdmin, i, "IgnoreEnterData", "");
                        CC_BanClient(i, iAdmin, false);
                    }
                    else
                    {
                        ChooseMessengerMenu(i);
                    }
                }

                GiveOverlay(i, g_sOverlayPath);
                Menu_PanelCheck(iAdmin);

                g_esPlayerInfo[i].iWaitMessengerTime--;
            }
        }
    }
}

public Action OnClientSayCommand(int iClient, const char[] sCommand, const char[] sNameDiscord) 
{
    if (!CC_IsValidClient(iClient))
        return Plugin_Continue;

    if (StrContains(sNameDiscord, "!contact ") != 0)
        return Plugin_Continue;

    if (g_esPlayerInfo[iClient].iStatusCheck == STATUS_WAITCOMMUNICATION)
    {
        strcopy(g_esPlayerInfo[iClient].sDiscord, 64, sNameDiscord[3]);

        int iAdmin = CC_IsCheckedPlayer(iClient);
        if (CC_IsValidClient(iAdmin))
        {
            CPrintToChat(iAdmin, "%t", (!CM_IsClientModUser(iAdmin) ? "PlayerSendedDiscordAdmin" : "PlayerSendedDiscordAdmin ClientMod"), iClient, sNameDiscord);
        }

        CPrintToChat(iClient, "%t", (!CM_IsClientModUser(iClient) ? "PlayerSendedDiscord" : "PlayerSendedDiscord ClientMod"));
        CC_PrintLog(iAdmin, iClient, "PlayerSendedDiscord", sNameDiscord);

        g_esPlayerInfo[iClient].iStatusCheck = STATUS_WAITCALL;

        return Plugin_Handled;
    }

	return Plugin_Continue;
}

public void MakeVerify(int iClient, int iClientChoose) 
{
    g_iActionPlayer[iClient] = GetClientOfUserId(iClientChoose);
    g_esPlayerInfo[iClientChoose].iWaitMessengerTime = g_iWaitTime;
    g_esPlayerInfo[iClientChoose].iStatusCheck = STATUS_WAITCOMMUNICATION;

    CC_PrintLog(iClient, iClientChoose, "CheckStart", "");
    CPrintToChat(iClientChoose, "%t", (!CM_IsClientModUser(iClientChoose) ? "CheckStartPlayerText" : "CheckStartPlayerText ClientMod"), iClient);
	
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			CPrintToChat(i, "%t", (!CM_IsClientModUser(i) ? "CheckStartAllText" : "CheckStartAllText ClientMod"), iClient, iClientChoose);
		}
	}
    

    if(g_sSoundPath[0]) 
        ClientCommand(iClientChoose, "playgamesound \"%s\"", g_sSoundPath);
}