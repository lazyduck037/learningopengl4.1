BIN = lesson2
CC = g++
PROJECT_ROOT =  $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
FLAGS = -std=c++11 -DAPPLE -Wall -pedantic -Wextra -Werror
INC = -I ../lib/include -I/sw/include -I/usr/local/include
LIB_PATH = ../lib/osx/
LOC_LIB = 
DYN_LIB = -lGLEW -lglfw
FRAMEWORKS = -framework Cocoa -framework OpenGL -framework IOKit
SRC = ${PROJECT_ROOT}/lesson2.cpp ${PROJECT_ROOT}/utils/utils.cpp

all:
	${CC} ${FLAGS} ${FRAMEWORKS} -o ${BIN} ${SRC} ${INC} ${LOC_LIB} ${DYN_LIB}