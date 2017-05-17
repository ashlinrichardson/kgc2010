#pragma once
#ifndef HEADER_GLOBAL_H
#define HEADER_GLOBAL_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif


#include "SA.h"
#include <queue>
#include <vector>
#include <math.h>
#include "float.h"
#include "sas.cpp"
#include <pthread.h>
#include "clust_knn.h"

using namespace _SA;
using namespace std;

namespace myglut{
	
	extern int NDESIRED;// 16000
	extern int KNN_USE;// 100 
	extern int RAND_ITER_MAX;// 8
	
	class GLUT2d;
	class GLUT3d;
	class clust_knn;
	
	extern GLUT2d* myglut2d;
	extern GLUT2d* myglut2d_img;
	extern GLUT3d* myglut3d;
	extern clust_knn* myclust_knn;
	
	extern GLvoid * source;  // array to render 2d labelling 
	
	// thread vars
	extern int threadcreated;
	extern int ostopthread;
	
  // zpr variables
	extern int _mouseLeft;
	extern int _mouseRight;
	extern int _lastmouseRight;
	
  // buffer objects
	extern vector < SA<float> * > float_buffers;
	
	extern SA<int> * isBad;
  extern SA<int> * i_coord; 	
  extern SA<int> * j_coord;

  // picking variables	
	extern int PickThis;
	extern int lastpick;
	extern int laststate;
	extern int beforelaststate;
	extern int number_of_classes;
	extern double avg_surprise;
	extern int surprising_count;
	
  // density estimate variables
	extern int density_estimate;
	extern int n_density_estimates;
	extern int n_skip;
	extern int select_distance_function;

  // output file (binary format)	
	extern FILE * outfile;
	
}

#endif
