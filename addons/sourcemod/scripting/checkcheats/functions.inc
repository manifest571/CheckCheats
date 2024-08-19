void CC_BanClient(int iClient, int iAdmin, bool bOffline, char[] szReason = "") 
{
    if(CanTestFeatures() && GetFeatureStatus(FeatureType_Native, "MABanPlayer") == FeatureStatus_Available) {
        if(bOffline) {
            char sSteamID[256], sIP[256], sName[256];

            GetClientAuthId(iClient, AuthId_Steam2, sSteamID, sizeof sSteamID);
            GetClientIP(iClient, sIP, sizeof sIP);
            GetClientName(iClient, sName, sizeof sName);

            MAOffBanPlayer(iAdmin, MA_BAN_STEAM, sSteamID, sIP, sName, g_iBanTime, szReason);
        } 
        else 
            MABanPlayer(iAdmin, iClient, MA_BAN_STEAM, g_iBanTime, szReason);
    }
}

void LoadCvars()
{
    g_cWaitMessengerTime = CreateConVar("checkcheats_wait_time", "10", "Время в минутах отведённое на ожидание данных игрока для связи. [0 - без лимита]");
    g_iWaitTime = g_cWaitMessengerTime.IntValue * 60;
    g_cWaitMessengerTime.AddChangeHook(OnConVarChanged);

    g_cBanEnabled = CreateConVar("checkcheats_ban_enabled", "1", "Бан игроков при наличии читов/выходе с сервера. [0 - Выключить | 1 - Включить]");
    g_bBanEnabled = g_cBanEnabled.BoolValue;
    g_cBanEnabled.AddChangeHook(OnConVarChanged);

    g_cBanReason = CreateConVar("checkcheats_ban_reason", "Отказ пройти проверку на читы", "Причина бана игроков при отказе от проверки.");
    g_cBanReason.GetString(g_sBanReason, sizeof(g_sBanReason));
    g_cBanReason.AddChangeHook(OnConVarChanged);
	
	g_cBanCheats = CreateConVar("checkcheats_ban_cheat_reason", "Читы обнаружены", "Причина бана игроков при нахождении читов");
    g_cBanCheats.GetString(g_sBanCheatsReason, sizeof(g_sBanCheatsReason));
    g_cBanCheats.AddChangeHook(OnConVarChanged);

    g_cHideAdmins = CreateConVar("checkcheats_admins_hide", "1", "Скрытие администраторов в списке на проверку. [0 - Выключить | 1 - Включить]");
    g_bHideAdmins = g_cHideAdmins.BoolValue;
    g_cHideAdmins.AddChangeHook(OnConVarChanged);

    g_cSoundPath = CreateConVar("checkcheats_sound_path", "check_cheats/man.mp3", "Путь к звуку, который будет проигрываться у игрока во время вызова на проверку. [пустое поле отключает функцию]");
    g_cSoundPath.GetString(g_sSoundPath, sizeof(g_sSoundPath));
    g_cSoundPath.AddChangeHook(OnConVarChanged);

    g_cBanTime = CreateConVar("checkcheats_ban_time", "0", "Время бана игроков в минутах.");
    g_iBanTime = g_cBanTime.IntValue;
    g_cBanTime.AddChangeHook(OnConVarChanged);
    
    g_cOverlayPath = CreateConVar("checkcheats_overlay_path", "overlay_cheats/ban_cheats", "Путь к оверлею, который будет отображаться у игрока во время проверки. [без папки materials]");
    g_cOverlayPath.GetString(g_sOverlayPath, sizeof(g_sOverlayPath));
    g_cOverlayPath.AddChangeHook(OnConVarChanged);
}

public void OnConVarChanged(ConVar convar, const char[] oldValue, const char[] newValue) 
{
    if (convar == g_cWaitMessengerTime)
        g_iWaitTime = g_cWaitMessengerTime.IntValue * 60;
    else if (convar == g_cBanEnabled)
        g_bBanEnabled = g_cBanEnabled.BoolValue;
    else if (convar == g_cBanReason)
        g_cBanReason.GetString(g_sBanReason, sizeof(g_sBanReason));
	else if (convar == g_cBanCheats)
        g_cBanCheats.GetString(g_sBanCheatsReason, sizeof(g_sBanCheatsReason));
    else if (convar == g_cHideAdmins)
        g_bHideAdmins = g_cHideAdmins.BoolValue;
    else if (convar == g_cSoundPath)
    {
        g_cSoundPath.GetString(g_sSoundPath, sizeof(g_sSoundPath));
        if (g_sSoundPath[0])
        {
            char szBuffer[PLATFORM_MAX_PATH];
            FormatEx(szBuffer, sizeof(szBuffer), "sound/%s", g_sSoundPath);
            if (FileExists(szBuffer))
                PrecacheSound(g_sSoundPath);
        }
    }
    else if (convar == g_cBanTime)
        g_iBanTime = convar.IntValue;
    else if (convar == g_cOverlayPath)
    {
        g_cOverlayPath.GetString(g_sOverlayPath, sizeof(g_sOverlayPath));
        if (g_sOverlayPath[0])
        {
            char szBuffer[PLATFORM_MAX_PATH];
            FormatEx(szBuffer, sizeof(szBuffer), "materials/%s.vmt", g_sOverlayPath);
            if (FileExists(szBuffer))
            {
                PrecacheModel(szBuffer);
                AddFileToDownloadsTable(szBuffer);

                FormatEx(szBuffer, sizeof(szBuffer), "materials/%s.vtf", g_sOverlayPath);
                if (FileExists(szBuffer))
                    AddFileToDownloadsTable(szBuffer);
            }
        }
    }
}

char[] GetStatus(int iClient, int iStatus) 
{
    char szStatus[128];
    switch (iStatus) 
    {
		case STATUS_WAITCOMMUNICATION: 
        {
			Format(szStatus, sizeof(szStatus), "%T", "Status_WaitDiscord", iClient);
		}

		case STATUS_WAITCALL: 
        {
			Format(szStatus, sizeof(szStatus), "%T", "Status_WaitCall", iClient);
		}

		case STATUS_CHECKING: 
        {
			Format(szStatus, sizeof(szStatus), "%T", "Status_Checking", iClient);
		}
		
        case STATUS_RESULT: 
        {
			Format(szStatus, sizeof(szStatus), "%T", "Status_Result", iClient);
		}
	}

	return szStatus;
}

bool CC_IsValidClient(int iClient) 
{
    if(iClient > 0 && iClient <= MaxClients && IsClientInGame(iClient) && IsClientConnected(iClient) && !IsFakeClient(iClient)) 
        return true;

    return true;
}

int CC_IsCheckedPlayer(int iClient) 
{
    if(iClient > 0 && iClient <= MaxClients && IsClientInGame(iClient) && !IsFakeClient(iClient)) 
    {
        if (STATUS_NOTCHECKING < g_esPlayerInfo[iClient].iStatusCheck <= STATUS_RESULT)
        {
            int iUserID = GetClientUserId(iClient);
            for(int i = 1; i <= MaxClients; i++) 
            {
                if(IsClientInGame(i)) 
                {
                    if(g_iActionPlayer[i] == iUserID) 
                        return i;
                }
            }
        }
    }

    return 0;
}

void CC_PrintLog(int iClient, int iClientChoose, const char[] sLog, const char[] sMessenger) 
{
    char sUserSteamID[MAX_AUTHID_LENGTH], sAdminSteamID[MAX_AUTHID_LENGTH];

    if (iClient < 1)
        sAdminSteamID = "INVALID_STEAM";
    else 
        GetClientAuthId(iClient, AuthId_Steam2, sAdminSteamID, sizeof(sAdminSteamID));

    if (iClientChoose < 1)
        sUserSteamID = "INVALID_STEAM";
    else
        GetClientAuthId(iClientChoose, AuthId_Steam2, sUserSteamID, sizeof(sUserSteamID));

    if (!strcmp(sLog, "CheckStart"))
        LogToFileEx(g_sLogPath, "[CheckCheats] Администратор %N (%s) вызвал на проверку игрока %N (%s)", iClient, sAdminSteamID, iClientChoose, sUserSteamID);
    else if (!strcmp(sLog, "CheckPlayerLeave"))
        LogToFileEx(g_sLogPath, "[CheckCheats] Игрок %N (%s) покинул сервер, проверка автоматически отменена!", iClientChoose, sUserSteamID);
    else if (!strcmp(sLog, "CheckLeaveAdmin"))
        LogToFileEx(g_sLogPath, "[CheckCheats] Администратор %N (%s) покинул сервер, проверка автоматически отменена!", iClient, sAdminSteamID);
    else if (!strcmp(sLog, "PlayerSendedDiscord"))
        LogToFileEx(g_sLogPath, "[CheckCheats] Игрок %N (%s) успешно ввёл свой Discord: %s", iClientChoose, sUserSteamID, sMessenger);
    else if (!strcmp(sLog, "PlayerToSpec"))
        LogToFileEx(g_sLogPath, "[CheckCheats] Администратор %N (%s) переместил игрока %N (%s) в наблюдатели!", iClient, sAdminSteamID, iClientChoose, sUserSteamID);
    else if (!strcmp(sLog, "PlayerBlockSpec"))
        LogToFileEx(g_sLogPath, "[CheckCheats] Администратор %N (%s) заблокировал игроку %N (%s) переход в другие команды!", iClient, sAdminSteamID, iClientChoose, sUserSteamID);
    else if (!strcmp(sLog, "PlayerCheatsNotFound")) 
        LogToFileEx(g_sLogPath, "[CheckCheats] Администратор %N (%s) завершил проверку игрока %N (%s). Результат: Читы не обнаружены!", iClient, sAdminSteamID, iClientChoose, sUserSteamID);
    else if (!strcmp(sLog, "PlayerCheatsFound")) 
        LogToFileEx(g_sLogPath, "[CheckCheats] Администратор %N (%s) завершил проверку игрока %N (%s). Результат: Читы обнаружены!", iClient, sAdminSteamID, iClientChoose, sUserSteamID);
    else if (!strcmp(sLog, "IgnoreEnterData")) 
        LogToFileEx(g_sLogPath, "[CheckCheats] Игрок %N (%s) был забанен по причине игнорирования ввода данных для проверки!", iClientChoose, sUserSteamID);
}

void GiveOverlay(int iClient, char[] sPath) 
{
    if(!IsClientInGame(iClient)) 
        return;
    
	ClientCommand(iClient, "r_screenoverlay \"%s\"", sPath);
}
