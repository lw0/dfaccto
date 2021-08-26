require "math"
require "string"

local seed = math.random(0)

local stmSrc = sys.port("stmSrc",
  "StreamSource", { depth = 0,
                    bandwidth = 1.0,
                    latency = 0.0,
                    fair = true,
                    packed = false,
                    seed = seed})
local stmBuf = sys.pass("stmBufIn", "stmBufOut",
  "StreamPass", { depth = 16,
                  bandwidthSink = 1.0,
                  bandwidthSource = 1.0,
                  latency = 64.0,
                  fair = true,
                  packed = true,
                  seed = seed})
local stmSnk = sys.port("stmSnk",
  "StreamSink", { depth = 0,
                  bandwidth = 1.0,
                  latency = 0.0,
                  seed = seed})

local idA = 2
local countA = 2
local sizeA = 16
local doneA = sys.condition(false)

local idB = 3
local countB = 3
local sizeB = 16
local doneB = sys.condition(false)

sys.run(function ()
  sys.waitFor(4)
  sys.reset(false)
  print(" Lua: Begin")

  doneA:wait(true)
  doneB:wait(true)

  print(" Lua: End")
  sys.waitFor(400)
  sys.stop()
end)


sys.run(function()
  sys.waitReset(false)
  local unit = stmSrc:dataBytes()
  local idx
  for idx = 1,countA do
    local size = math.random(1, unit * sizeA)
    local data = string.rep("A", size)
    stmSrc:write(idA, bitv.str(data), true)
    print(" Lua("..sys.ticks()..") A Sending packet #"..(size*8))
    sys.waitFor(math.random(1, 2*size//unit))
  end
end)

sys.run(function()
  sys.waitReset(false)
  local unit = stmSrc:dataBytes()
  local idx
  for idx = 1,countB do
    local size = math.random(1, unit * sizeB)
    local data = string.rep("B", size)
    stmSrc:write(idB, bitv.str(data), true)
    print(" Lua("..sys.ticks()..") B Sending packet #"..(size*8))
    sys.waitFor(math.random(1, 2*size//unit))
  end
end)

sys.run(function()
  sys.waitReset(false)
  local cnt = 0
  while true do
    local data, last, id = stmSnk:read(idA)
    if data:bits() > 0 then
      print(" Lua("..sys.ticks()..") A Received packet ", last, data:bits(), data:toHex())
      if last then
        cnt = cnt + 1
        if cnt >= countA then break end
      end
    end
    sys.next()
  end
  doneA:set(true)
end)

sys.run(function()
  sys.waitReset(false)
  local cnt = 0
  while true do
    local data, last, id = stmSnk:read(idB)
    if data:bits() > 0 then
      print(" Lua("..sys.ticks()..") B Received packet ", last, data:bits(), data:toHex())
      if last then
        cnt = cnt + 1
        if cnt >= countB then break end
      end
    end
    sys.next()
  end
  doneB:set(true)
end)

