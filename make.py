#!/usr/bin/env python3
import os

def err(m):
  import sys
  print('Error: ' + str(m)); sys.exit(1)

CC = 'g++'
CFLAGS = '-w -O4 misc.cpp'
UNAME = os.popen('uname').read().strip()

LFLAGS = '-lm -pthread -lstdc++ '

if UNAME == 'Linux':
  LFLAGS += '-lc -lGL -lglut -lGL'
elif UNAME == "Darwin":
  LFLAGS += '-framework Cocoa -framework GLUT -framework openGL'
else:
  err('Platform not supported: ' + str(UNAME))

f = open('make.sh', 'wb')
cf = ['knn', 'glut', 'clust_knn', 'zpr', 'pick', 'global', 'console']
for x in cf:
  s = ' '.join([CC, CFLAGS, '-c', x + '.cpp']) + ' &'
  f.write((s + '\n').encode()) # multithreaded compile
  print(s.strip())
  
s = 'wait'
print(s)
f.write((s + '\n').encode()) # join threads before link step

s = ' '.join([CC, CFLAGS, (' '.join([f + '.o' for f in cf])), '-o kgc.exe', LFLAGS])
f.write((s + '\n').encode())
f.close()
print(s.strip())

a = os.system('chmod 777 make.sh')
a = os.system('./make.sh')
