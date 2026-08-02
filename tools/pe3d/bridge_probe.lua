local BRIDGE_ADDRESS = 0x0203CF64
local EXPECTED_MAGIC = 0x44334550

local frames = 0

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

    -- Mostrar datos aproximadamente una vez por segundo
    if frames % 60 == 0 then
        local major = emu:read16(BRIDGE_ADDRESS + 4)
        local minor = emu:read16(BRIDGE_ADDRESS + 6)
        local size = emu:read32(BRIDGE_ADDRESS + 8)
        local counter = emu:read32(BRIDGE_ADDRESS + 12)

        console:log(string.format(
            "PE3D OK | version=%d.%d | size=%d | counter=%d",
            major,
            minor,
            size,
            counter
        ))
    end
end

callbacks:add("frame", readBridge)

console:log("PE3D Bridge Probe cargado")
