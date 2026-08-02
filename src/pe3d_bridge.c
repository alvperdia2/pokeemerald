#include "global.h"
#include "pe3d_bridge.h"

EWRAM_DATA volatile struct PE3DBridge gPE3DBridge = {0};

void PE3DBridge_Init(void)
{
    gPE3DBridge.magic = PE3D_BRIDGE_MAGIC;
    gPE3DBridge.protocolMajor = PE3D_PROTOCOL_MAJOR;
    gPE3DBridge.protocolMinor = PE3D_PROTOCOL_MINOR;
    gPE3DBridge.structureSize = sizeof(struct PE3DBridge);
    gPE3DBridge.frameCounter = 0;
}

void PE3DBridge_Update(void)
{
    gPE3DBridge.frameCounter++;
}
