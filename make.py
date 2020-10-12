import os
import sys

def err(m):
  print('Error: ' + str(m))
  sys.exit(1)

CC = 'g++'
CFLAGS = '-w -O4 misc.cpp'
UNAME = os.popen('uname').read().strip()

if UNAME == 'Linux':
  LFLAGS = '-lc -lm -lstdc++  -pthread -lGL -lglut -lGL'
elif UNAME == "Darwin":
  LFLAGS = '-lm -pthread -lstdc++ -framework Cocoa -framework GLUT -framework openGL'
else:
  err('Platform not supported: ' + str(UNAME))

cf = ['knn', 'glut', 'clust_knn', 'zpr', 'pick', 'global', 'console']
for f in cf:
  s = ' '.join([CC, CFLAGS, '-c', f + '.cpp'])
  print(s)

s = ' '.join([CC, CFLAGS, (' '.join([f + '.o' for f in cf])), '-o kgc.exe', LFLAGS])
print(s)

# g++ -w -O4 misc.cpp knn.o glut.o clust_knn.o zpr.o pick.o global.o console.o -o knn.exe -lm -pthread -lstdc++ -framework Cocoa -framework GLUT -framework openGL




