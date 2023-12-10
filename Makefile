CFLAGS = -std=c++17 -O2

LDFLAGS = -lglfw -ldl -lX11  -lXi -lGL 

OGLTest:main.cpp
	g++ $(CFLAGS) -o OGLTest main.cpp glad.c $(LDFLAGS)

.PHONY: test clean

test: OGLTest
	./OGLTest

clean:
	rm -f OGLTest