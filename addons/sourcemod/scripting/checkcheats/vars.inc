#define MAX_AUTHID_LENGTH       64

enum {
    STATUS_NOTCHECKING = -1,
    STATUS_WAITCOMMUNICATION = 0,
    STATUS_WAITCALL = 1,
    STATUS_CHECKING = 2,
    STATUS_RESULT = 3
}

enum struct esPlayer 
{
    bool bBlockSpec;

    int iStatusCheck;
    int iWaitMessengerTime;

    char sDiscord[64];

    void ResetVerify(int iClient)
    {
        this.bBlockSpec = false;
        this.iStatusCheck = STATUS_NOTCHECKING;
        this.iWaitMessengerTime = 0;
        this.sDiscord[0] = 0;

        GiveOverlay(iClient, "");
    }
}

ConVar  g_cHideAdmins, 
        g_cSoundPath, 
        g_cBanTime, 
        g_cOverlayPath, 
        g_cBanReason,
		g_cBanCheats,
        g_cBanEnabled, 
        g_cWaitMessengerTime;

bool    g_bHideAdmins,
        g_bBanEnabled;

int     g_iActionPlayer[MAXPLAYERS+1];
        g_iBanTime, 
        g_iMessenger[MAXPLAYERS+1], 
        g_iWaitTime;

char    g_sLogPath[PLATFORM_MAX_PATH],
        g_sSoundPath[PLATFORM_MAX_PATH], 
        g_sOverlayPath[PLATFORM_MAX_PATH], 
        g_sBanReason[128],
		g_sBanCheatsReason[128];

esPlayer g_esPlayerInfo[MAXPLAYERS+1];
