#ifndef GUARD_PE3D_BRIDGE_H
#define GUARD_PE3D_BRIDGE_H

#include "global.h"

#define PE3D_BRIDGE_MAGIC          0x44334550
#define PE3D_PROTOCOL_MAJOR        1
#define PE3D_PROTOCOL_MINOR        0

struct PE3DBridge
{
    u32 magic;
    u16 protocolMajor;
    u16 protocolMinor;
    u32 structureSize;
    u32 frameCounter;
};

extern volatile struct PE3DBridge gPE3DBridge;

void PE3DBridge_Init(void);
void PE3DBridge_Update(void);

#endif
