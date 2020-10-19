#include "clust_knn.h"
using namespace myglut;

clust_knn::clust_knn(int _NRow, int _NCol){
  NRow = _NRow;
  NCol = _NCol;
  select_distance_function = 0;
}

void clust_knn::reinit(int _nskip){
  printf("%sclust_knn::reinit %snskip %s%d %sKNN_USE %s%d%s\n", KMAG, KGRN, KRED, _nskip, KGRN, KRED, KNN_USE, KNRM);
   GLUT3d * _my3d = myglut3d;
 GLUT2d * _my2d = myglut2d;
 clust_knn::init(_my3d, _my2d, float_buffers, _nskip, true);
  
  // clust_knn::init(myglut3d, myglut2d, float_buffers, _nskip, true);
}

float clust_knn::euclidean_distance(int i, int j){
  int m;
	float d, tmp;

  d = 0.;
  for0(m, N){
    tmp = dat.at(j, m) - dat.at(i, m);
    d += tmp * tmp;
  }

  if(isnan(d) || isinf(d) || isnan(-d) || isinf(-d)){
    printf("euclidean_distance bad: i %d j %d\n", i, j);
    for0(m, N) printf("\tdjm %f dim %f\n", dat.at(j, m), dat.at(i, m));
    exit(1);
  }
  return d;
}

float clust_knn::distance(int i, int j){
  	// there were other distance functions in here.. put them back?
	return euclidean_distance(i, j);
}

void clust_knn::init(GLUT3d * _my3d, GLUT2d * _my2d, vector < SA<float> * > * _float_buffers, int nskip){
  init(_my3d, _my2d, _float_buffers, nskip, false);
}

//calculate nj from N and nskip
void clust_knn::init(GLUT3d * _my3d, GLUT2d * _my2d, vector < SA<float> * > * _float_buffers, int nskip, bool re_init){
  printf("\tre_init %d\n", (int)re_init);

  if(!re_init) D_j = NULL;
  
  myglut3d = _my3d;
  myglut2d = _my2d;
  n_skip = nskip;
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
    nnD.init(nj, KNN_MAX);
    origIndex.init(nj);
    nnIndex.init(nj, KNN_MAX);

    printf("%sReducing data...%s\n", KGRN, KNRM);

    // decimate: use 1/nskip of data
    k = h = 0;
    for0(i, NRow){
      for0(j, NCol){
        if(k >= (nj - 1)) break;

        if((h % nskip) == 0){
          origIndex.at(k) = h;
          for0(m, N){
            dat.at(k, m) = float_buffers->at(m)->at(i, j);
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

  printf("calculating sorted truncated distance matrix..\n");
  if(!re_init) distance_calculation();

  for0(j, nj){
    int ind;
    SAS<float> * D = D_j[j];
    for0(i, KNN_MAX){
      ind = D->index(i);
      nnIndex.at(j, i) = ind;
      nnD.at(j, i) = D->f(i);
    }
  }
  printf("end init..\n");
}

void distance_calculation(){
	// run distance calculation in parallel
  parfor(0, myclust_knn->nj, &distance_calculation);
}

void distance_calculation(size_t j){
  SAS<float> * D; // distances w.r.t. a given point
  float d, tmp, x, y, z, X, Y, Z;
  int ix, iy, iz, ind, i, k, m, nj;
  nj = myclust_knn->nj;
  
  int dispfact = nj / 20;
  D = myclust_knn->D_j[j];
  D->reset();

  if(j % dispfact == 0){
    printf("%s\n(%s%d%s/%s%d%s)%s<==>%s(%s%d%s/%s100%s)%s", KGRN, KRED, (int)(j + 1), KMAG, KRED, nj, KGRN, KMAG, KGRN, KRED, (int)(100. * ((float)(j + 1)) / ((float)nj)), KMAG, KRED, KGRN, KNRM);
  }

  for0(i, nj){
    d = myclust_knn->distance(i, j);
    D->f(i) = d;
    if(isnan(d) || isnan(-d) || isinf(d) || isinf(-d)){
      printf("bad data\n");
      exit(1);
    }
  }
  D->Sort();
}

float clust_knn::densityEstimate(int j){
  int K = KNN_USE;
  float sumd, d;
  int i;

  d = sumd = 0.;
  for0(i, K){
    d = nnD.at(j, i);
    sumd += d;
  }
  d = - sumd;

  if(isnan(d) || isinf(d) || isnan(-d) || isinf(-d)){
    sumd = 0.;
    printf("bad densityEstimate j %d K %d\n", j, K);
    for0(i, K){
      d = nnD.at(j, i);
      sumd += d;
      printf("\td %f\n", d);
    }
    exit(1);
  }

  return d;
}

int clust_knn::classf( int j, SA<int> * highestdensityneighborindex, SA<float> * highestdensityneighbordensity){

  int debug = false;
  if(debug) printf("classf j %d\n", j);
  if(knn_indices.at(j) >= 0){
    if(debug) printf("\talready classed\n");
    return knn_indices.at(j); // return pre-existing label
  }

  if(highestdensityneighbordensity->at(j) <= dE.at(j)){
    knn_J_indices.push_back(j);
    knn_indices.at(j) = nkci;
    if(debug) printf("\tat top: j %d nkci %d\n", j, nkci);
    return(nkci++);
  }
  else{
    int nextJ = highestdensityneighborindex->at(j); // move towards peak
    if(debug) printf("\t move to peak: hdnd %f nextJ %d\n", (float) highestdensityneighbordensity->at(nextJ), nextJ);
    int recursiveclass = classf(nextJ, highestdensityneighborindex, highestdensityneighbordensity);
    return recursiveclass;
  }
}

void clust_knn::knn_clustering(){
  printf("start knn clustering...\n");

  if(KNN_USE > KNN_MAX){
    printf("K %d KMax %d\n", KNN_USE, KNN_MAX);
    printf("Error: you have selected K>K_Max.\n");
    exit(1);
  }

  int j,i;
  for0(j, nj) dE[j] = densityEstimate(j);

  if(false){
	  // debug printout for density estimates
    for0(j, nj) printf("%f ", dE[j]);
    printf("\n");
  }

  knn_indices.init(nj); //these are the labels for the clustering.

  SA<int> hdni(nj); //highest density neighbor index.
  SA<float> hdnd(nj); //highest density neighbor density.
  SAS <float> D(KNN_USE); //list the densities of the neighbors of the current point.

  int ni; //neighbor index.
  int chdni; //current highest density neighbor index.

  // sorted list of density estimates of neighbours
  for0(j, nj){
    D.reset();
    for0(i, KNN_USE){
      ni = nnIndex.at(j,i);
      D.f(i) = dE[ni];
    }
    D.Sort(); // increasing order
    chdni = D.index(KNN_USE - 1);
    chdni = nnIndex.at(j, chdni);
    hdni[j] = chdni;
    hdnd[j] = D.f(KNN_USE - 1);
  }

  knn_J_indices.clear();
  nkci = 0;

  // run the classification
  for0(j, nj) knn_indices[j] = -1;
  for0(j, nj) knn_indices[j] = classf(j, &(hdni), &(hdnd));

  // how many centres?
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

  // calculate class membership
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
  return n_knn_centres; // number of classes
}

int clust_knn::get_n_knn_elements(int centre_index){
  return (classmembers[centre_index]).size(); // number of dat elements for a given class
}

float clust_knn::get_centre_coord(int centre_index, int elem_ind, int coord){
  return dat.at((classmembers[centre_index])[elem_ind], coord); // i'th coordinate of an N-dimensional dat element
}

float clust_knn::get_centre_coord( int centre_index, int coord){
  return dat.at(knn_J_indices.at(centre_index), coord); // i'th coordinate of "representative" element of cluster ("hill top")
}
