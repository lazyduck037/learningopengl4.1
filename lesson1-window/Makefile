BIN = lesson1
CC = g++
FLAGS = -DAPPLE -std=c++11 -DAPPLE -Wall -pedantic -Wextra -Werror
INC = -I ../lib/include -I/sw/include -I/usr/local/include
LIB_PATH = ../lib/osx/
LOC_LIB = 
DYN_LIB = -lGLEW -lglfw
FRAMEWORKS = -framework Cocoa -framework OpenGL -framework IOKit
SRC = /Users/vitcon/workspace/learningopengl4.1/lesson1-window/lesson1.cpp

all:
	${CC} ${FLAGS} ${FRAMEWORKS} -o ${BIN} ${SRC} ${INC} ${LOC_LIB} ${DYN_LIB}