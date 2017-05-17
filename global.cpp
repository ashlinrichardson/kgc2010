#include "global.h"
namespace myglut{

	#define CLUST_COMPARE_MIN 10

  int KNN_MAX;
	int KNN_USE;  // 100
	int NDESIRED;  // 16000
	int RAND_ITER_MAX;  // 8

	GLUT2d* myglut2d;
	GLUT3d* myglut3d;
	GLUT2d* myglut2d_img;

	GLvoid * source;

	clust_knn* myclust_knn;

	vector < SA<float> * > float_buffers;

  SA<int> * isBad;
  SA<int> * i_coord;
  SA<int> * j_coord;

	double avg_surprise;
	int beforelaststate; // save second most recent pick state.
	int number_of_classes, surprising_count;
  int _mouseLeft, _mouseRight, _lastmouseRight, threadcreated, ostopthread, PickThis, lastpick, laststate; // save most recent pick state
	int density_estimate, n_density_estimates, n_skip, select_distance_function;

	FILE * outfile;
}
