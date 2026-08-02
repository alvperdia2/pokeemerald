#ifndef GUARD_PE3D_BRIDGE_H
#define GUARD_PE3D_BRIDGE_H

#include "global.h"

#define PE3D_BRIDGE_MAGIC           0x44334550
#define PE3D_PROTOCOL_MAJOR         1
#define PE3D_PROTOCOL_MINOR         2

#define PE3D_STATUS_OVERWORLD       (1 << 0)

#define PE3D_EVENT_CAPACITY         16

enum PE3DEventType
{
    PE3D_EVENT_NONE = 0,
    PE3D_EVENT_PLAYER_DESTINATION_CHANGED = 1,
};

struct PE3DEvent
{
    u32 sequence;
    u32 frame;

    u8 type;
    u8 mapGroup;
    u8 mapNum;
    u8 direction;

    s16 x;
    s16 y;
};

struct PE3DBridge
{
    u32 magic;
    u16 protocolMajor;
    u16 protocolMinor;
    u32 structureSize;
    u32 frameCounter;

    u8 status;
    u8 mapGroup;
    u8 mapNum;
    u8 elevation;

    s16 playerX;
    s16 playerY;

    u8 facingDirection;
    u8 movementDirection;
    u8 avatarFlags;
    u8 reserved;

    u32 eventWriteSequence;
    u8 eventWriteIndex;
    u8 eventCapacity;
    u16 eventReserved;

    struct PE3DEvent events[PE3D_EVENT_CAPACITY];
};

extern volatile struct PE3DBridge gPE3DBridge;

void PE3DBridge_Init(void);
void PE3DBridge_Update(void);

#endif
