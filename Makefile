CC=g++
CFLAGS=-w -O4 

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
LFLAGS  = $(DEBUG) -lc -lm -lstdc++  -pthread -lGL -lglut -lGLU
endif

# OS X
ifeq ($(UNAME_S), Darwin)
LFLAGS =  $(DEBUG) -lm -pthread -lstdc++ -framework Cocoa -framework GLUT -framework openGL
endif

knn.exe: knn.o glut.o clust_knn.o zpr.o pick.o global.o console.o
	$(CC) $(CFLAGS) $(LFLAGS)  knn.o glut.o clust_knn.o zpr.o pick.o global.o console.o -o knn.exe

knn.o: knn.cpp glut.h clust_knn.h pick.h global.h
	$(CC) $(CFLAGS) -c knn.cpp

pick.o: pick.cpp pick.h glut.h global.h
	$(CC) $(CFLAGS) -c pick.cpp

global.o: global.cpp global.h glut.h
	$(CC) $(CFLAGS) -c global.cpp

glut.o: glut.cpp glut.h zpr.h clust_knn.h pick.h global.h console.h
	$(CC) $(CFLAGS) -c glut.cpp

clust_knn.o: clust_knn.cpp clust_knn.h clust_knn.h pick.h glut.h global.h
	$(CC) $(CFLAGS) -c clust_knn.cpp

zpr.o: zpr.cpp zpr.h pick.h glut.h global.h console.h
	$(CC) $(CFLAGS) -c zpr.cpp

console.o: console.cpp console.h glut.h global.h
	$(CC) $(CFLAGS) -c console.cpp
