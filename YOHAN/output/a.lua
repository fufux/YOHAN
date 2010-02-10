-- Lua script.
p=tetview:new()
p:load_plc("F:/Library/Documents/Dev/N7/Projet Long/YOHAN/YOHAN/output/09Car-tire-modelin.poly")
rnd=glvCreate(0, 0, 500, 500, "TetView")
p:plot(rnd)
glvWait()
