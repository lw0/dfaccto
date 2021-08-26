
valA = sys.port("valA", "Wire")
valB = sys.port("valB", "Wire")
valC = sys.port("valC", "Wire")
evtA = sys.port("evtA", "Event")
evtB = sys.port("evtB", "Event")
evtC = sys.port("evtC", "Event")

finished = sys.condition()

print("\n<bitv testcase>")

vec1 = bitv.str("Heute ist ein schöner Tag!")
print(vec1:toHex())
vec1:set(32, 80, 0x55555555)
print(vec1:toHex())
vec2 = vec1:slice(8, 48)
print(vec2:toBin())
print(vec2:toHex())
print(vec2:toStr())

print(bitv.int(50, 20):toHex())

print("</bitv testcase>\n")


sys.run(function ()
  sys.waitFor(4)
  sys.reset(false)
  print(" Lua: Begin")

  finished:wait(true)

  print(" Lua: End")
  sys.waitFor(4)
  sys.stop()
end)

sys.run(function()
  sys.waitReset(false)

  evtA:stb("assert", 6)
  sys.waitFor(7)
  evtB:stb("cycle", 4)
  evtA:stb("release")

  local val = evtC:ack("cycle")
  print(" Lua: evtC sent " .. val:toHex())
  finished:set(true)
end)

sys.run(function ()
  sys.waitReset(false)
  valA:setData(bitv.int(5, 8))
  valB:setData(9)

  print(" Lua: valC is " .. valC:data():toHex())
  while true do
    valC:waitChange()
    print(" Lua: valC is " .. valC:data():toHex())
  end
end)
