# Mac OSX and Linux supported by this Makefile
CC=g++
CFLAGS=-w -O4 misc.cpp

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
LFLAGS  = -lc -lm -lstdc++  -pthread -lGL -lglut -lGLU
endif

# OS X
ifeq ($(UNAME_S), Darwin)
LFLAGS =  -lm -pthread -lstdc++ -framework Cocoa -framework GLUT -framework openGL
endif

knn.exe: knn.o glut.o clust_knn.o zpr.o pick.o global.o console.o
	$(CC) $(CFLAGS) knn.o glut.o clust_knn.o zpr.o pick.o global.o console.o -o knn.exe $(LFLAGS)

global.o: global.cpp global.h glut.h
	$(CC) $(CFLAGS) -c global.cpp

pick.o: pick.cpp pick.h glut.h global.h
	$(CC) $(CFLAGS) -c pick.cpp

console.o: console.cpp console.h glut.h global.h
	$(CC) $(CFLAGS) -c console.cpp

knn.o: knn.cpp glut.h clust_knn.h pick.h global.h
	$(CC) $(CFLAGS) -c knn.cpp

zpr.o: zpr.cpp zpr.h pick.h glut.h global.h console.h
	$(CC) $(CFLAGS) -c zpr.cpp

glut.o: glut.cpp glut.h zpr.h clust_knn.h pick.h global.h console.h
	$(CC) $(CFLAGS) -c glut.cpp

clust_knn.o: clust_knn.cpp clust_knn.h clust_knn.h pick.h glut.h global.h
	$(CC) $(CFLAGS) -c clust_knn.cpp

