tesselation: tesselation.cpp tesselation.tes tesselation.frag
	g++ `pkg-config --cflags glfw3 glew` -o tesselation tesselation.cpp `pkg-config --static --libs glfw3 glew`

tesselationd: tesselation.cpp tesselation.tes tesselation.frag
	g++ -ggdb `pkg-config --cflags glfw3 glew` -o tesselationd tesselation.cpp `pkg-config --static --libs glfw3 glew`
