all:
	mkdir -p linux_x86
	g++ -std=c++0x $(wildcard *.cpp) -lSOIL -I /usr/include -L ../Libs/linux -I ../Includes -lGL -lGLU -lglut -o linux_x86/Platformer

debug:
	mkdir -p linux_x86_dbg
	g++ -g -std=c++0x $(wildcard *.cpp) -lSOIL -I /usr/include -L ../Libs/linux -I ../Includes -lGL -lGLU -lglut -o linux_x86_dbg/Platformer

clean: 
	rm -rf linux_x86

clean_debug:
	rm -rf linux_x86_dbg
