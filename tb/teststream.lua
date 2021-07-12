require "math"
require "string"
require "coroutine"

local seed = math.random(0)

local stmSrc = dfaccto.port("stmSrc",
  "StreamSource", { depth = 0,
                    bufferBandwidth = 1.0,
                    bufferFair = false,
                    bufferPacked = false,
                    latency = 0.0,
                    sourceBandwidth = 1.0,
                    sourceFair = false,
                    sourcePacked = false,
                    seed = seed})
local stmBuf = dfaccto.loop("stmBufIn", "stmBufOut",
  "StreamPass", { depth = 0,
                  sinkBandwidth = 1.0,
                  latency = 0.0,
                  sourceBandwidth = 1.0,
                  sourceFair = false,
                  sourcePacked = false,
                  seed = seed})
local stmSnk = dfaccto.port("stmSnk",
  "StreamSink", { depth = 0,
                  sinkBandwidth = 1.0,
                  latency = 0.0,
                  bufferBandwidth = 1.0,
                  bufferFair = false,
                  bufferPacked = false,
                  seed = seed})

local countA = 5
local doneA = dfaccto.condition(false)

local countB = 10
local doneB = dfaccto.condition(false)

dfaccto.run(function ()
  dfaccto.waitFor(4)
  dfaccto.reset(false)
  print(" Lua: Begin")

  doneA:wait(true)
  doneB:wait(true)

  print(" Lua: End")
  dfaccto.waitFor(4)
  dfaccto.stop()
end)


dfaccto.run(function()
  dfaccto.waitReset(false)
  local unit = stmSrc:dataBytes()
  for idx = 0,countA do
    local size = math.random(unit * 2, unit * 32)
    local data = string.rep("A", size)
    stmSrc:write(0, data, true)
    stmSrc:flush()
    print(" Lua: A Sending packet #"..size)
  end
end)

dfaccto.run(function()
  dfaccto.waitReset(false)
  local unit = stmSrc:dataBytes()
  for idx = 0,countB do
    local size = math.random(unit * 2, unit * 32)
    local data = string.rep("B", size)
    stmSrc:write(1, data, true)
    coroutine.yield()
    print(" Lua: B Sending packet #"..size)
  end
end)

dfaccto.run(function()
  dfaccto.waitReset(false)
  for idx = 0,countA do
    local data = stmSnk:read(0, "packet")
    print(" Lua: A Received packet #"..#data)
  end
  doneA:set(true)
end)

dfaccto.run(function()
  dfaccto.waitReset(false)
  for idx = 0,countB do
    local data = stmSnk:read(1, "packet")
    print(" Lua: B Received packet #"..#data)
  end
  doneB:set(true)
end)

