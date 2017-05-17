#include "global.h"
namespace myglut{

	#define CLUST_COMPARE_MIN 10

	int NDESIRED;  // 16000
	int KNN_USE;  // 100
	int RAND_ITER_MAX;  // 8

	GLUT2d* myglut2d;
	GLUT2d* myglut2d_img;
	GLUT3d* myglut3d;

	clust_knn* myclust_knn;
	GLvoid * source;

	vector < SA<float> * > float_buffers;

  SA<int> * isBad;
  SA<int> * i_coord;
  SA<int> * j_coord;


  int _mouseLeft;
	int _mouseRight;
	int _lastmouseRight;

	int threadcreated;
	int ostopthread;

	int PickThis;
	int lastpick;
	int laststate; // save most recent pick state
	int beforelaststate; // save second most recent pick state.
	int number_of_classes;
	double avg_surprise;
	int surprising_count;

	int density_estimate;
	int n_density_estimates;
	int n_skip;
	int select_distance_function;

	FILE * outfile;
}
