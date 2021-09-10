require "math"
require "string"

local seed = math.random(0)

local pIntr = sys.port("intr", "Event")

local pCtrl = sys.port("ctrl",
  "MemoryMaster", { depth = 0,
                    bandwidth = 1.0,
                    latency = 0.0,
                    fair = true,
                    packed = false,
                    seed = seed})
local pHost = sys.pass("host",
  "MemoryShared", { depth = 16,
                  bandwidthSink = 1.0,
                  bandwidthSource = 1.0,
                  latency = 64.0,
                  fair = true,
                  packed = true,
                  seed = seed})

local prepared = sys.condition()
local completed = sys.condition()
local finished = sys.condition()

sys.run(function ()
  sys.waitFor(4)
  sys.reset(false)
  print(" Lua: Begin")

  local irq = pIntr.ack("cycle")
  completed:set(true)
  print(" Lua: IRQ 0x" .. irq:toHex())

  finished:wait(true)
  print(" Lua: End")
  sys.waitFor(400)
  sys.stop()
end)


sys.run(function()
  sys.waitReset(false)

  val actyp = pCtrl.read(0x010, 4);
  val acver = pCtrl.read(0x014, 4);
  print(" Lua: Action is " .. actyp:toHex() .. ":" .. acver:toHex())

  for idx = 0,7 do
    pCtrl.write(0x100+0x10*idx, 4, 0x00010000*idx)
    pCtrl.write(0x104+0x10*idx, 4, 0x00000000)
    pCtrl.write(0x108+0x10*idx, 4, 0x00000100)
    pCtrl.write(0x10C+0x10*idx, 4, 0x0000003f)
  end
  pCtrl.write(0x008, 4, 0x00000000) -- Interrupt Index
  pCtrl.write(0x018, 4, 0x0000006c) -- Interrupt Handle Low
  pCtrl.write(0x018, 4, 0x00000000) -- Interrupt Handle High
  pCtrl.write(0x004, 4, 0x00000001) -- Interrupt Enable

  prepared:wait(true)
  pCtrl.write(0x000, 4, 0x00000001) -- Action Start
end)

sys.run(function()
  sys.waitReset(false)
  for idx = 0,0x3ff do
    pHost:write(0x00000100*idx, 256, bitv.str(string.rep(string.format("%04x", idx),256)))
  end
  prepared:set(true)
  completed:wait(true)
  for idx = 0,0x3ff do
    local data = pHost:read(0x00000100*idx, 256)
    print(string.format("%04x00 %s", idx, data:toStr()))
  end
  finished:set(true)
end)

