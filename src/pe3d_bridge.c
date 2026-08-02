#include "global.h"
#include "field_player_avatar.h"
#include "main.h"
#include "overworld.h"
#include "pe3d_bridge.h"
#include "fieldmap.h"

EWRAM_DATA volatile struct PE3DBridge gPE3DBridge = {0};

static void PE3DBridge_ClearOverworldState(void)
{
    gPE3DBridge.status = 0;
    gPE3DBridge.mapGroup = 0xFF;
    gPE3DBridge.mapNum = 0xFF;
    gPE3DBridge.elevation = 0;
    gPE3DBridge.playerX = 0;
    gPE3DBridge.playerY = 0;
    gPE3DBridge.facingDirection = 0;
    gPE3DBridge.movementDirection = 0;
    gPE3DBridge.avatarFlags = 0;
    gPE3DBridge.reserved = 0;
}

void PE3DBridge_Init(void)
{
    gPE3DBridge.magic = PE3D_BRIDGE_MAGIC;
    gPE3DBridge.protocolMajor = PE3D_PROTOCOL_MAJOR;
    gPE3DBridge.protocolMinor = PE3D_PROTOCOL_MINOR;
    gPE3DBridge.structureSize = sizeof(struct PE3DBridge);
    gPE3DBridge.frameCounter = 0;

    PE3DBridge_ClearOverworldState();
}

void PE3DBridge_Update(void)
{
    s16 x;
    s16 y;

    gPE3DBridge.frameCounter++;

    /*
     * El status se escribe al final. Mientras actualizamos los demás
     * campos, el snapshot se considera temporalmente no válido.
     */
    gPE3DBridge.status = 0;

    if (gMain.callback1 != CB1_Overworld
     || gMain.callback2 != CB2_Overworld)
    {
        PE3DBridge_ClearOverworldState();
        return;
    }

    PlayerGetDestCoords(&x, &y);

    gPE3DBridge.mapGroup =
        (u8)gSaveBlock1Ptr->location.mapGroup;

    gPE3DBridge.mapNum =
        (u8)gSaveBlock1Ptr->location.mapNum;

    /*
     * PlayerGetDestCoords devuelve coordenadas internas que incluyen
     * el borde virtual del mapa.
     */
    gPE3DBridge.playerX = x - MAP_OFFSET;
    gPE3DBridge.playerY = y - MAP_OFFSET;

    gPE3DBridge.elevation = PlayerGetElevation();
    gPE3DBridge.facingDirection = GetPlayerFacingDirection();
    gPE3DBridge.movementDirection = GetPlayerMovementDirection();
    gPE3DBridge.avatarFlags = GetPlayerAvatarFlags();
    gPE3DBridge.reserved = 0;

    /* Publicamos el snapshot cuando todos los campos están escritos. */
    gPE3DBridge.status = PE3D_STATUS_OVERWORLD;
}
