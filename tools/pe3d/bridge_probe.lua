local BRIDGE_ADDRESS = 0x0203CF64
local EXPECTED_MAGIC = 0x44334550

local EVENT_WRITE_SEQUENCE_OFFSET = 0x1C
local EVENT_CAPACITY_OFFSET = 0x21
local EVENTS_OFFSET = 0x24
local EVENT_SIZE = 16

local directionNames = {
    [0] = "NONE",
    [1] = "SOUTH",
    [2] = "NORTH",
    [3] = "WEST",
    [4] = "EAST"
}

local frames = 0
local lastEventSequence = nil

local function readS16(address)
    local value = emu:read16(address)

    if value >= 0x8000 then
        value = value - 0x10000
    end

    return value
end

local function readSnapshot()
    local status = emu:read8(BRIDGE_ADDRESS + 16)

    if status == 0 then
        console:log("PE3D | fuera del overworld")
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
        "SNAPSHOT | map=%d:%d | pos=(%d,%d) | elev=%d | facing=%s | movement=%s | flags=0x%02X",
        mapGroup,
        mapNum,
        playerX,
        playerY,
        elevation,
        directionNames[facing] or tostring(facing),
        directionNames[movement] or tostring(movement),
        avatarFlags
    ))
end

local function consumeEvents()
    local writeSequence =
        emu:read32(BRIDGE_ADDRESS + EVENT_WRITE_SEQUENCE_OFFSET)

    local capacity =
        emu:read8(BRIDGE_ADDRESS + EVENT_CAPACITY_OFFSET)

    -- Al cargar el script empezamos desde el estado actual,
    -- para no imprimir eventos antiguos de la cola.
    if lastEventSequence == nil then
        lastEventSequence = writeSequence
        console:log(string.format(
            "Cola inicializada | sequence=%d | capacity=%d",
            writeSequence,
            capacity
        ))
        return
    end

    local pending = writeSequence - lastEventSequence

    if pending <= 0 then
        return
    end

    if pending > capacity then
        console:log(string.format(
            "AVISO: se han sobrescrito %d eventos",
            pending - capacity
        ))

        lastEventSequence = writeSequence - capacity
        pending = capacity
    end

    while lastEventSequence < writeSequence do
        local sequence = lastEventSequence + 1
        local index = (sequence - 1) % capacity

        local address =
            BRIDGE_ADDRESS
            + EVENTS_OFFSET
            + index * EVENT_SIZE

        local storedSequence = emu:read32(address)

        -- El productor escribe sequence al final.
        if storedSequence ~= sequence then
            return
        end

        local eventFrame = emu:read32(address + 4)
        local eventType = emu:read8(address + 8)
        local mapGroup = emu:read8(address + 9)
        local mapNum = emu:read8(address + 10)
        local direction = emu:read8(address + 11)
        local x = readS16(address + 12)
        local y = readS16(address + 14)

        if eventType == 1 then
            console:log(string.format(
                "EVENT #%d | DESTINATION_CHANGED | frame=%d | map=%d:%d | pos=(%d,%d) | direction=%s",
                sequence,
                eventFrame,
                mapGroup,
                mapNum,
                x,
                y,
                directionNames[direction] or tostring(direction)
            ))
        else
            console:log(string.format(
                "EVENT #%d | type=%d",
                sequence,
                eventType
            ))
        end

        lastEventSequence = sequence
    end
end

local function update()
    local magic = emu:read32(BRIDGE_ADDRESS)

    if magic ~= EXPECTED_MAGIC then
        console:error(string.format(
            "Firma PE3D incorrecta: 0x%08X",
            magic
        ))
        return
    end

    -- Los eventos se comprueban en cada frame.
    consumeEvents()

    -- El snapshot solo se muestra una vez por segundo.
    frames = frames + 1

    if frames % 60 == 0 then
        readSnapshot()
    end
end

callbacks:add("frame", update)

console:log("PE3D Bridge Probe 1.2 cargado")
