tessellation: tessellation.cpp tessellation.tes tessellation.frag
	g++ `pkg-config --cflags glfw3 glew` -o tessellation tessellation.cpp `pkg-config --static --libs glfw3 glew`

tessellationd: tessellation.cpp tessellation.tes tessellation.frag
	g++ -ggdb `pkg-config --cflags glfw3 glew` -o tessellationd tessellation.cpp `pkg-config --static --libs glfw3 glew`
