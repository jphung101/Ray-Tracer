CC = g++
ifeq ($(shell uname 2>/dev/null), Darwin)
CFLAGS = -g -DGL_GLEXT_PROTOTYPES -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED -DOSX -Wno-deprecated-register -Wno-deprecated-declarations -Wno-shift-op-parentheses
INCFLAGS = -I./glm-0.9.7.1 -I/opt/local/include -I./include/
LDFLAGS = -framework GLUT -framework OpenGL -L/opt/local/lib \
  -L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
  -lGL -lGLU -lm -lstdc++ -lfreeimage
else
CFLAGS = -g -DGL_GLEXT_PROTOTYPES 
INCFLAGS = -I./glm-0.9.7.1 -I./include/ -I/usr/X11R6/include -I/sw/include \
		-I/usr/sww/include -I/usr/sww/pkg/Mesa/include
LDFLAGS = -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L./lib/nix/ \
		-L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lGLEW -lglut -lGLU -lGL -lX11 -lfreeimage
endif

RM = /bin/rm -f 
all: raytrace
raytrace: main.o Image.o Transform.o readfile.o Scene.o Object.o 
	$(CC) $(CFLAGS) -o raytrace Image.o main.o Transform.o readfile.o Scene.o $(INCFLAGS) $(LDFLAGS) 
main.o: main.cpp Object.h Image.h Transform.h readfile.h Scene.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c main.cpp
Image.o: Image.cpp Image.h Scene.h FreeImage.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Image.cpp
Transform.o: Transform.cpp Transform.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c Transform.cpp  
readfile.o: readfile.cpp readfile.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c readfile.cpp
Scene.o: Scene.cpp Scene.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Scene.cpp
Object.o: Object.cpp Object.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Object.cpp
clean: 
	$(RM) *.o raytrace *.png


 
