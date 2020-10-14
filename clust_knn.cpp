#include "clust_knn.h"
using namespace myglut;

clust_knn::clust_knn(int _NRow, int _NCol){
  NRow = _NRow;
  NCol = _NCol;
  select_distance_function = 0;
}

void clust_knn::reinit(int _nskip, int _KMax){
  printf("%sclust_knn::reinit %snskip %s%d %sKNN_USE %s%d%s\n", KMAG, KGRN, KRED, _nskip, KGRN, KRED, _KMax, KNRM);
  GLUT3d * _my3d = myglut3d;
  GLUT2d * _my2d = myglut2d;
  clust_knn::init(_my3d, _my2d, float_buffers, _nskip, _KMax, true);
}

float clust_knn::euclidean_distance(int i, int j){
  int m;
  float d, tmp; d = 0;
  for0(m, N){
    tmp = dat.at(j, m) - dat.at(i, m);
    d += tmp * tmp;
  }
  return d;
}

float clust_knn::distance(int i, int j){
  if((*isBad)[i] == 1 || (*isBad)[j] == 1){
    printf("Error: NAN\n"); //exit(1);
    return NAN;
  }
  return euclidean_distance(i, j);
}

void clust_knn::init(GLUT3d * _my3d, GLUT2d * _my2d, vector < SA<float> * > * _float_buffers, int nskip, int _KMax){
  init(_my3d, _my2d, _float_buffers, nskip, _KMax, false);
}

//calculate nj from N and nskip
void clust_knn::init(GLUT3d * _my3d, GLUT2d * _my2d, vector < SA<float> * > * _float_buffers, int nskip, int _KMax, bool re_init){

  KNN_USE = _KMax;

  printf("KNN_MAX %d KNN_USE %d KMax %d _KMax %d\n", KNN_MAX, KNN_USE, KMax, _KMax);
  if(!re_init){
    K = KMax;
    KMax = _KMax;
    D_j = NULL;
  }

  myglut3d = _my3d;
  myglut2d = _my2d;
  myglut::n_skip = nskip;
  float_buffers = _float_buffers;

  // allocate
  int n = (float_buffers->at(0))->size();
  N = float_buffers->size();
  nj = n / nskip; // fixed for this invocation of the program

  int h, i, j, k, m;
  if(!re_init){
    
    D_j = (SAS<float> **)(void *)malloc(sizeof(SAS<float> *) * nj);	  
    for0(i, nj) D_j[i] = new SAS<float>(nj); // dmat row (could be more ragged)

    // only allocate memory the first time
    dE.init(nj);
    dat.init(nj, N);
    badData.init(nj);
    nnD.init(nj, KMax);
    origIndex.init(nj);
    nnIndex.init(nj, KMax);

    printf("%sReducing data...%s\n", KGRN, KNRM);

    // decimate: use 1/nskip of data
    k = h = 0;
    for0(i, NRow){
      for0(j, NCol){
        if(k >= (nj - 1)){
          break;
        }
        if((h % nskip) == 0){
          origIndex.at(k) = h;
          for0(m, N){
            dat.at(k, m) = (float_buffers->at(m))->at(i, j);
            badData.at(i, j) = isBad->at(i, j);
            (*i_coord)[m] = i;
            (*j_coord)[m] = j;
          }
          k++;
        }
        h++;
      }
    }
  }

 if(!re_init){
	 distance_calculation();
	 /*
  printf("%sCalculating distances...%s\n", KGRN, KNRM);

  // distances with respect to a given point
  // SAS <float> D(nj);
  SAS<float> * D; 
  float d, tmp, x, y, z, X, Y, Z;
  int ix, iy, iz, ind;
  int dispfact = nj / 20;
  for0(j, nj){
    D = D_j[j];
    D->reset();

    if((j % dispfact) == 0){
      printf("%s\n(%s%d%s/%s%d%s)%s<==>%s(%s%d%s/%s100%s)%s", KGRN, KRED, j + 1, KMAG, KRED, nj, KGRN, KMAG, KGRN, KRED, (int)(100. * ((float)(j + 1)) / ((float)nj)), KMAG, KRED, KGRN, KNRM);
    }

    for0(i, nj){
      d = distance(i, j);
      D->f(i) = d;
    }

    D->Sort();
  }
  */

 }

 printf("calculating density..\n");

  // j, nj
for0(j, nj){
    int ind;
    SAS<float> * D = D_j[j];
     for0(i, KMax){
      ind = D->index(i);
      nnIndex.at(j, i) = ind;
      nnD.at(j, i) = D->f(i);
    }
  }
  // j, nj
}
// function


void myglut::distance_calculation(){
  printf("%sCalculating distances...%s\n", KGRN, KNRM);

  int nj = myclust_knn->nj;
  // distances with respect to a given point
  // SAS <float> D(nj);
  SAS<float> * D;
  float d, tmp, x, y, z, X, Y, Z;
  int ix, iy, iz, ind, i, j, k, m;
  int dispfact = nj / 20;
  for0(j, nj){
    D = myclust_knn->D_j[j];
    D->reset();

    if((j % dispfact) == 0){
      printf("%s\n(%s%d%s/%s%d%s)%s<==>%s(%s%d%s/%s100%s)%s", KGRN, KRED, j + 1, KMAG, KRED, nj, KGRN, KMAG, KGRN, KRED, (int)(100. * ((float)(j + 1)) / ((float)nj)), KMAG, KRED, KGRN, KNRM);
    }

    for0(i, nj){
      d = myclust_knn->distance(i, j);
      D->f(i) = d;
    }

    D->Sort();
  }
}

float clust_knn::densityEstimate(int j, int K){
  int i;
  float sumd = 0.;
  float d = 0.;
  for0(i, K){
    d = nnD.at(j, i);
    sumd += d;
  }
  return - sumd;
}

int clust_knn::classf( int j, SA<int> * highestdensityneighborindex, SA<float> * highestdensityneighbordensity){

  if(knn_indices.at(j) >= 0) return knn_indices.at(j); // return pre-existing label

  if(highestdensityneighbordensity->at(j) <= dE.at(j)){
    knn_J_indices.push_back(j);
    knn_indices.at(j) = nkci;
    return(nkci++);
  }
  else{
    int nextJ = highestdensityneighborindex->at(j); // move towards peak
    int recursiveclass = classf(nextJ, highestdensityneighborindex, highestdensityneighbordensity);
    return recursiveclass;
  }
}

void clust_knn::knn_clustering(){
  printf("start knn clustering...\n");

  int K = KNN_USE;
  if(K > KMax){
    printf("K %d KMax %d\n", K, KMax);
    printf("Error: you have selected K>K_Max.\n");
    exit(1);
  }

  int j,i;
  for0(j, nj){
    dE[j] = densityEstimate(j, KMax);
  }

  knn_indices.init(nj); //these are the labels for the clustering.

  SA<int> hdni(nj); //highest density neighbor index.
  SA<float> hdnd(nj); //highest density neighbor density.
  SAS <float> D(KMax); //list the densities of the neighbors of the current point.

  int ni; //neighbor index.
  int chdni; //current highest density neighbor index.

  for0(j, nj){
    D.reset();
    for0(i, KMax){
      ni = nnIndex.at(j,i);
      D.f(i) = dE[ni];
    }
    D.Sort(); // increasing order
    chdni = D.index(K - 1);
    chdni = nnIndex.at(j, chdni);
    hdni[j] = chdni;
    hdnd[j] = D.f(K - 1);
  }

  knn_J_indices.clear();
  nkci = 0;

  for0(j, nj) knn_indices[j] = -1;
  for0(j, nj) knn_indices[j] = classf(j, &(hdni), &(hdnd));

  n_knn_centres = nkci;
  number_of_classes = n_knn_centres;
  printf("Number of classes: %d\n", nkci);

  myglut3d->lock = true;

  int N = float_buffers->size();
  myglut3d->class_centres.clear();

  for0(i, knn_J_indices.size()){
    int myind = knn_J_indices.at(i);
    for0(j, N) myglut3d->class_centres.push_back(dat.at(myind, j));
  }

  classmembers.clear();
  for0(i, n_knn_centres){
    vector<unsigned int> a;
    classmembers.push_back(a);
  }
  for0(j, nj) classmembers[knn_indices[j]].push_back(j);

  myglut3d->lock = false;
  myglut2d->unlock();
  myglut2d->mark();
  myglut3d->mark();
}

int clust_knn::get_n_knn_centres(){
  // the number of classes.
  return n_knn_centres;
}

int clust_knn::get_n_knn_elements(int centre_index){
  // number of dat elements for given class.
  return (classmembers[centre_index]).size();
}

float clust_knn::get_centre_coord(int centre_index, int elem_ind, int coord){
  //get the i'th coordinate of a given N-dimensional dat element.
  return dat.at((classmembers[centre_index])[elem_ind], coord);
}

//get the i'th coordinate of the representative element of the cluster...
float clust_knn::get_centre_coord( int centre_index, int coord){
  return dat.at( knn_J_indices.at(centre_index), coord);
}
