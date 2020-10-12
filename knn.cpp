/* 20170517 verified that program hardly uses CPU when idle */

#include <cmath>
#include <float.h>
#include "glut.h"
#include "pick.h"
#include "global.h"
#include "clust_knn.h"
#include "ansi_color.h"
#include "misc.h"

using namespace myglut;

size_t  N; // number of bands
size_t NRow; // number of rows
size_t NCol; // number of cols
int bi[3]; // active band indices

vector <FILE*> filehandles;
vector <GLUT2d *> GLUT2d_windows;
vector <GLUT3d *> GLUT3d_windows;
vector <clust_knn*> knn_clusterings;

void quit(){
  int i;
  for(i = 0; i < filehandles.size(); i++){
    if(filehandles[i]){
      fclose(filehandles[i]);
    }
  }
  for(i = 0; i < float_buffers.size(); i++){
    free(float_buffers[i]);
  }
  exit(0);
}

void idle(){
  (myglut2d_img)->refresh();
  myglut2d->refresh();
  if(number_of_classes == 0){
    myglut3d->runclust();
    if(!myglut2d->glbusy){
      myglut2d->recalc_classes();
      myglut2d->draw_classes();
    }
  }
}

void scaleband(SA<float> * buf){
  float dat;
  long int i;
  float MAX = FLT_MIN;
  float MIN = FLT_MAX;
  long int n = buf->size();

  for(i = 0; i < n; i++){
    dat = buf->at(i);
    if(!(isinf(dat) || isnan(dat))){
      if(dat < MIN){
        MIN = dat;
      }
      if(dat > MAX){
        MAX = dat;
      }
    }
    else{
      (*isBad)[i] = 1;
    }
  }
  printf(" MIN %e MAX %e\n", MIN, MAX);

  for(i = 0; i < buf->size(); i++){
    dat = buf->at(i);
    buf->at(i) = (dat - MIN) / (MAX - MIN);
    dat = buf->at(i);
  }
}

int main(int argc, char *argv[]){
  printf("start\n");
  if(argc < 5){
    err(str("kgc2010 [input binary file] [n_desired] [knn_use] [rand_iter_max]"));
  }
  str fn(argv[1]);
  if(!exists(fn)) err("input file not found");

  str hfn(hdr_fn(fn));

  hread(hfn, NRow, NCol, N);
  /*
  if(argc < 9){
    printf("%s%sdensity estimation%s & mode finding %sfor %snon-parametric %scolor-segmentation%s\n", KRED, KINV, KNRM, KMAG, KGRN, KCYN, KNRM);
    printf("by %sashlin richardson,%s august 16, 2009%s\n\n", KINV, KNRM, KNRM);
    printf("knn.cpp:\n\tusage:\tknn.exe [nrow] [ncol] [n_desired] [knn_use] [rand_iter_max] [band_1] [band_2] [band_3] ... \n");
    printf("n_desired - desired # of points to use to cluster (sampling based approach) e.g., 10000\n");
    printf("knn_use - k-nbhd size for density estimation (eg. 100).\n");
    printf("rand_iter_max - clusters are determined via sampled pixels (of which there's approx. n_desired) so it's necessary to assign remaining pixels.. this parameter is # of random iterations (per final cluster) used to assign a given un-labeled pixel in image to a cluster.\n");
    printf("Results are stored in output/\n");
    quit();
  }
*/

  /* make output folder, if it doesn't already exist */
  system("mkdir -p output");

  //NRow = atoi(argv[1]);
  //NCol = atoi(argv[2]);
  NDESIRED = atoi(argv[2]); //3]);
  KNN_MAX = KNN_USE = atoi(argv[3]); //4]);
  RAND_ITER_MAX = atoi(argv[4]);//5]);

  int n = NRow*NCol;

  printf("open data files\n");
  register int i;

  // buffer data
  float * dd = bread(fn, NRow, NCol, N);

  //reshape
  printf("fbufs\n");
  SA< SA< float > * > fbufs(N);
  for(i = 0; i < N; i++){
    fbufs[i] = (SA<float> *) new SA<float>(NRow, NCol);
    float_buffers.push_back(fbufs[i]);
  }
  isBad = new SA<int>(NRow * NCol);
  i_coord = new SA<int>(NRow * NCol);
  j_coord = new SA<int>(NRow * NCol);

  int kk;
  for(kk = 0; kk < N; kk++){
    SA<float> * db = fbufs[kk];

    // buffer data band
    for(i = 0; i < n; i++){
      (*db)[i] = dd[(kk * n) + i];  
    }
    // scale band into [0, 1]
    scaleband(fbufs[kk]);
  }

  free(dd);

  if(false){
    int kk;
    // write data in ascii format
    FILE * afile = fopen("./ascii.txt", "wb");
    for(kk = 0; kk < NRow*NCol; kk++){
      for(i = 0; i < N; i++){
        fprintf(afile, "%7.7e,", fbufs[i]->at(kk));
      }
      fprintf(afile, "\n");
    }
    fclose(afile);
  }

  //set default band vis.: 1, 2, 3
  for(i = 0; i < 3; i++){
    bi[i] = i;
  }

  printf("glutInit\n");
  glutInit(&argc,argv);

  // image display window
  GLUT2d img( NRow, NCol, "image");
  GLUT2d_windows.push_back(&img);

  img.setPos(0, 0);
  img.setRGB(fbufs[0], fbufs[1], fbufs[2], 0, 1, 2);
  img.mark();
  myglut2d_img = &img;
  img.glbusy = false;

  // classification display window
  GLUT2d clasi(NRow, NCol, "class");
  GLUT2d_windows.push_back(&clasi);

  clasi.setRightOf(&img);
  clasi.setRGB(fbufs[0], fbufs[1], fbufs[2], 0, 1, 2);
  clasi.mark();
  clasi.enableClassification();
  clasi.glbusy = false;

  number_of_classes = 0;
  laststate = beforelaststate = -1;

  // init scatter plot
  GLUT3d scatter(NCol * 2, NCol * 2, N);
  scatter.setRGB(fbufs[0], fbufs[1], fbufs[2], 0, 1, 2);
  GLUT3d_windows.push_back(&scatter);
  scatter.setBelow(&img);
  scatter.mark();

  clust_knn myKNNclust(NRow, NCol);
  myglut::myclust_knn = &myKNNclust;

  // init clustering
  myKNNclust.init(&scatter, &clasi, &float_buffers, floor(((float)n)/((float)NDESIRED)), KNN_USE);

  myKNNclust.set_Rand_Iter_Max(RAND_ITER_MAX);
  knn_clusterings.push_back(&myKNNclust);

  scatter.set_clust( &myKNNclust);
  clasi.set_clust(&myKNNclust);
  clasi.enableClassification();

  img.mark();
  img.refresh();
  img.isClassification = false;

  printf("Start GLUT main loop\n");
  glutMainLoop();

  // after exit glut loop
  quit();
  return 0;
}
