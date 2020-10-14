#pragma once // later: restore and min-heap
#ifndef HEADER_CLUST_KNN_H
#define HEADER_CLUST_KNN_H
#include "SA.h"
#include <queue>
#include "sas.cpp"
#include "glut.h"
#include "pick.h"
#include "global.h"
#include <pthread.h>

using namespace std;
using namespace _SA;

namespace myglut{
  class GLUTWindow;
  class GLUT3d;
  class GLUT2d;
  class clust_knn;

  void distance_calculation();


  class clust_knn{
    public:
    int N, K, KMax, Rand_Iter_Max, nj; // nj = number of points used
    vector < SA<float> * > * float_buffers; // float data

    SA<int> origIndex;
    SA<float> dat; //the sampled data set (a subset of the full data set).
    SA<int> nnIndex; //listing the indices of the nearest neighbors of each of the data points.
    SA<float> nnD; //the listing of distances between each point and it's K-nearest neighbors.
    vector< vector<unsigned int> > classmembers;

    SAS<float> ** D_j; 

    SA<float> dE;
    SA<int> knn_indices;
    SA<int> badData;
    vector<int> knn_J_indices;
    int nkci; // next knn class index;
    int n_knn_centres, NRow, NCol;

    clust_knn(int _NRow, int _NCol);

    void init(GLUT3d * _my3d,
    GLUT2d * _my2d,
    vector < SA<float> * > * _float_buffers,
    int nskip, //calculate nj from N and nskip.
    int _KMax);

    void init(GLUT3d * _my3d,
    GLUT2d * _my2d,
    vector < SA<float> * > * _float_buffers,
    int nskip, //calculate nj from N and nskip.
    int _KMax, bool re_init);

    int getK(){
      return K;
    }

    void reinit(int nskip, int _KMax);
    float densityEstimate(int j, int K);

    int classf(int j,
    SA<int> * highestdensityneighborindex,
    SA<float> * highestdensityneighbordensity);

    void knn_clustering();
    int get_n_knn_centres();
    int get_n_knn_elements(int centre_index);
    float get_centre_coord(int centre_index, int elem_ind, int coord);
    float get_centre_coord( int centre_index, int coord);
    float distance(int i, int j);
    float euclidean_distance(int i, int j);
    float T3_distance(int i, int j);
    float normalized_euclidean_distance(int i, int j);

    void set_Rand_Iter_Max(int rim){
      Rand_Iter_Max = rim;
    }

    int get_Rand_Iter_Max(){
      return Rand_Iter_Max;
    }
  };
}
#endif
