-- Lua script.
p=tetview:new()
p:load_mesh("F:/Library/Documents/Dev/N7/Projet Long/Propre/YOHAN/YOHAN/YOHAN/output/09Car-tire-modelout.ele")
rnd=glvCreate(0, 0, 500, 500, "TetView")
p:plot(rnd)
glvWait()
