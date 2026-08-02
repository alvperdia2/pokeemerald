#include "global.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "main.h"
#include "overworld.h"
#include "pe3d_bridge.h"

EWRAM_DATA volatile struct PE3DBridge gPE3DBridge = {0};

static EWRAM_DATA bool8 sHasLastDestination = FALSE;
static EWRAM_DATA u8 sLastMapGroup = 0;
static EWRAM_DATA u8 sLastMapNum = 0;
static EWRAM_DATA s16 sLastX = 0;
static EWRAM_DATA s16 sLastY = 0;

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

static void PE3DBridge_PublishDestinationChanged(
    u8 mapGroup,
    u8 mapNum,
    s16 x,
    s16 y,
    u8 direction)
{
    u8 index = gPE3DBridge.eventWriteIndex;
    u32 sequence = gPE3DBridge.eventWriteSequence + 1;
    volatile struct PE3DEvent *event =
        &gPE3DBridge.events[index];

    /*
     * La secuencia se escribe al final para indicar que
     * el evento ya está completamente publicado.
     */
    event->sequence = 0;

    event->frame = gPE3DBridge.frameCounter;
    event->type = PE3D_EVENT_PLAYER_DESTINATION_CHANGED;
    event->mapGroup = mapGroup;
    event->mapNum = mapNum;
    event->direction = direction;
    event->x = x;
    event->y = y;

    event->sequence = sequence;

    gPE3DBridge.eventWriteSequence = sequence;

    index++;

    if (index >= PE3D_EVENT_CAPACITY)
        index = 0;

    gPE3DBridge.eventWriteIndex = index;
}

void PE3DBridge_Init(void)
{
    u8 i;

    gPE3DBridge.magic = PE3D_BRIDGE_MAGIC;
    gPE3DBridge.protocolMajor = PE3D_PROTOCOL_MAJOR;
    gPE3DBridge.protocolMinor = PE3D_PROTOCOL_MINOR;
    gPE3DBridge.structureSize = sizeof(struct PE3DBridge);
    gPE3DBridge.frameCounter = 0;

    gPE3DBridge.eventWriteSequence = 0;
    gPE3DBridge.eventWriteIndex = 0;
    gPE3DBridge.eventCapacity = PE3D_EVENT_CAPACITY;
    gPE3DBridge.eventReserved = 0;

    for (i = 0; i < PE3D_EVENT_CAPACITY; i++)
        gPE3DBridge.events[i].sequence = 0;

    sHasLastDestination = FALSE;

    PE3DBridge_ClearOverworldState();
}

void PE3DBridge_Update(void)
{
    s16 x;
    s16 y;
    u8 mapGroup;
    u8 mapNum;
    u8 facingDirection;

    gPE3DBridge.frameCounter++;
    gPE3DBridge.status = 0;

    if (gMain.callback1 != CB1_Overworld
     || gMain.callback2 != CB2_Overworld)
    {
        PE3DBridge_ClearOverworldState();
        sHasLastDestination = FALSE;
        return;
    }

    PlayerGetDestCoords(&x, &y);

    x -= MAP_OFFSET;
    y -= MAP_OFFSET;

    mapGroup = (u8)gSaveBlock1Ptr->location.mapGroup;
    mapNum = (u8)gSaveBlock1Ptr->location.mapNum;
    facingDirection = GetPlayerFacingDirection();

    gPE3DBridge.mapGroup = mapGroup;
    gPE3DBridge.mapNum = mapNum;
    gPE3DBridge.playerX = x;
    gPE3DBridge.playerY = y;
    gPE3DBridge.elevation = PlayerGetElevation();
    gPE3DBridge.facingDirection = facingDirection;
    gPE3DBridge.movementDirection =
        GetPlayerMovementDirection();
    gPE3DBridge.avatarFlags = GetPlayerAvatarFlags();
    gPE3DBridge.reserved = 0;

    if (sHasLastDestination
     && (mapGroup != sLastMapGroup
      || mapNum != sLastMapNum
      || x != sLastX
      || y != sLastY))
    {
        PE3DBridge_PublishDestinationChanged(
            mapGroup,
            mapNum,
            x,
            y,
            facingDirection);
    }

    sLastMapGroup = mapGroup;
    sLastMapNum = mapNum;
    sLastX = x;
    sLastY = y;
    sHasLastDestination = TRUE;

    gPE3DBridge.status = PE3D_STATUS_OVERWORLD;
}
