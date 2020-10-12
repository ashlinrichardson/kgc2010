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
  s = ' '.join([CC, CFLAGS, '-c', x + '.cpp'])
  f.write((s + ' &\n').encode()) # multithreaded compile

f.write('wait\n'.encode()) # join threads before link step

s = ' '.join([CC, CFLAGS, (' '.join([f + '.o' for f in cf])), '-o kgc.exe', LFLAGS]) + '\n'
f.write(s.encode())
f.close()

a = os.system("chmod 777 make.sh")
