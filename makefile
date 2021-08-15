IN = main.cpp
OUT = Program
CC = g++
LINK = -lGL -lGLU -lglut -lm
target: 
	$(CC) -o $(OUT) $(IN) $(LINK) 
