g++ -w -O4 misc.cpp -c knn.cpp &
g++ -w -O4 misc.cpp -c glut.cpp &
g++ -w -O4 misc.cpp -c clust_knn.cpp &
g++ -w -O4 misc.cpp -c zpr.cpp &
g++ -w -O4 misc.cpp -c pick.cpp &
g++ -w -O4 misc.cpp -c global.cpp &
g++ -w -O4 misc.cpp -c console.cpp &
wait
g++ -w -O4 misc.cpp knn.o glut.o clust_knn.o zpr.o pick.o global.o console.o -o kgc.exe -lm -pthread -lstdc++ -framework Cocoa -framework GLUT -framework openGL
