all:
	mkdir linux_x86_dbg
	g++ -std=c++0x $(wildcard *.cpp) -lSOIL -I /usr/include -L ../Libs/linux -I ../Includes -lGL -lGLU -lglut -lfmodex -o linux_x86_dbg/Platformer
debug:
	@echo $(wildcard *.cpp)
