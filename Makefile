CC=clang
CCFLAGS=-Wall -pedantic -g
LDFLAGS=-lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lGLEW -lGLU -L/usr/local/lib -lcglm -lm
INCLUDE=-I/usr/local/include

all:
	${CC} ${CCFLAGS} ${LDFLAGS} ./src/*.c -o renderer
