local BRIDGE_ADDRESS = 0x0203CF64
local EXPECTED_MAGIC = 0x44334550

local directionNames = {
    [0] = "NONE",
    [1] = "SOUTH",
    [2] = "NORTH",
    [3] = "WEST",
    [4] = "EAST"
}

local frames = 0

local function readS16(address)
    local value = emu:read16(address)

    if value >= 0x8000 then
        value = value - 0x10000
    end

    return value
end

local function readBridge()
    local magic = emu:read32(BRIDGE_ADDRESS)

    if magic ~= EXPECTED_MAGIC then
        console:error(string.format(
            "Firma PE3D incorrecta: 0x%08X",
            magic
        ))
        return
    end

    frames = frames + 1

    if frames % 60 ~= 0 then
        return
    end

    local major = emu:read16(BRIDGE_ADDRESS + 4)
    local minor = emu:read16(BRIDGE_ADDRESS + 6)
    local size = emu:read32(BRIDGE_ADDRESS + 8)
    local counter = emu:read32(BRIDGE_ADDRESS + 12)

    local status = emu:read8(BRIDGE_ADDRESS + 16)

    if status == 0 then
        console:log(string.format(
            "PE3D %d.%d | size=%d | fuera del overworld | counter=%d",
            major,
            minor,
            size,
            counter
        ))
        return
    end

    local mapGroup = emu:read8(BRIDGE_ADDRESS + 17)
    local mapNum = emu:read8(BRIDGE_ADDRESS + 18)
    local elevation = emu:read8(BRIDGE_ADDRESS + 19)

    local playerX = readS16(BRIDGE_ADDRESS + 20)
    local playerY = readS16(BRIDGE_ADDRESS + 22)

    local facing = emu:read8(BRIDGE_ADDRESS + 24)
    local movement = emu:read8(BRIDGE_ADDRESS + 25)
    local avatarFlags = emu:read8(BRIDGE_ADDRESS + 26)

    console:log(string.format(
        "PE3D %d.%d | map=%d:%d | pos=(%d,%d) | elev=%d | facing=%s | movement=%s | flags=0x%02X | counter=%d",
        major,
        minor,
        mapGroup,
        mapNum,
        playerX,
        playerY,
        elevation,
        directionNames[facing] or tostring(facing),
        directionNames[movement] or tostring(movement),
        avatarFlags,
        counter
    ))
end

callbacks:add("frame", readBridge)

console:log("PE3D Bridge Probe 1.1 cargado")
