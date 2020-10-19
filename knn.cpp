// DO HCLUST BY DECREASING K AND CHOOSING INCLUSION DIAGRAM...
// what's the null class label? -1 ????
/* 20170517 verified that program hardly uses CPU when idle */
#include <cmath>
#include <float.h>
#include "misc.h"
#include "glut.h"
#include "pick.h"
#include "global.h"
#include "clust_knn.h"
#include "ansi_color.h"

using namespace myglut;

size_t N, NRow, NCol; // number of bands, rows, col's
int bi[3]; // active band indices

vector <FILE*> filehandles;
vector <GLUT2d *> GLUT2d_windows;
vector <GLUT3d *> GLUT3d_windows;
vector <clust_knn*> knn_clusterings;

void quit(){
  int i;
  for0(i, filehandles.size()) if(filehandles[i]) fclose(filehandles[i]);
  for0(i, float_buffers.size()) free(float_buffers[i]);
  exit(0);
}

void idle(){
  printf("idle\n");
  myglut2d_img->refresh();
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
  float dat, MAX, MIN;
  long int i, n;
  MAX = FLT_MIN;
  MIN = FLT_MAX;
  n = buf->size();

  for0(i, n){
    dat = buf->at(i);
    if(!(isinf(dat) || isnan(dat))){
      if(dat < MIN) MIN = dat;
      if(dat > MAX) MAX = dat;
    }
    // else (*isBad)[i] = 1;
  }
  printf(" MIN %e MAX %e\n", MIN, MAX);

  for0(i, buf->size()){
    dat = buf->at(i);
    buf->at(i) = (dat - MIN) / (MAX - MIN);
    dat = buf->at(i);
  }
}

int main(int argc, char *argv[]){
  srand(37);
  
  // default parameters
  const char *args[5] = {"kgc.exe\0", "data/rgb.bin\0", "33333\0", "230\0", "2\0"};

  if(argc < 5){
    str msg("kgc2010 [input binary file] [n_desired] [knn_use] [rand_iter_max]");
    cout << "Error: " << msg << endl;
    argv = (char **)(void **)args;
  }

  str fn(argv[1]);
  if(!exists(fn)) err("input file not found");

  str hfn(hdr_fn(fn));
  hread(hfn, NRow, NCol, N);
  printf("NRow %d NCol %d NBand %d\n", (int)NRow, (int)NCol, (int)N);

  /* make output folder, if it doesn't already exist */
  int a = system("mkdir -p output");

  NDESIRED = atoi(argv[2]);
  KNN_MAX = KNN_USE = atoi(argv[3]);
  printf("KNN_MAX %d KNN_USE %d\n", KNN_MAX, KNN_USE);
  RAND_ITER_MAX = atoi(argv[4]);

  register int i;
  int n = NRow * NCol;
  float * dd = bread(fn, NRow, NCol, N); // buffer data

  SA< SA< float > * > fbufs(N); // reshaped data
  for0(i, N){
    fbufs[i] = (SA<float> *) new SA<float>(NRow, NCol);
    float_buffers.push_back(fbufs[i]);
  }
  i_coord = new SA<int>(NRow * NCol);
  j_coord = new SA<int>(NRow * NCol);

  int kk;
  for0(kk, N){
    SA<float> * db = fbufs[kk];
    for0(i, n) (*db)[i] = dd[(kk * n) + i]; // buffer data band
    scaleband(fbufs[kk]); // scale band to [0, 1]
  }

  // substitute NAN in one band, or zero in all bands, with small random number..
  for0(kk, n){
    bool all_zero = true;
    for0(i, N){
      float d = (* (fbufs[i]) )[kk];
      if(isnan(d) || isinf(d)){
        float r = (rand() / RAND_MAX) / 111111.;
        (* (fbufs[i]) )[kk] = r;

      }
      else{
        if( d != 0.) all_zero = false;
      }

    }
    if(all_zero){
      for0(i, N){
        float r = (rand() / RAND_MAX) / 111111.;
        (* (fbufs[i]) )[kk] = r;
      }
    }
  }
  free(dd);

  if(false){
    int kk;
    // write data in ascii format
    FILE * afile = fopen("./ascii.txt", "wb");
    for0(kk, NRow * NCol){
      for0(i, N) fprintf(afile, "%7.7e,", fbufs[i]->at(kk));
      fprintf(afile, "\n");
    }
    fclose(afile);
  }

  //set default band vis.: 1, 2, 3
  for0(i, 3) bi[i] = i;

  glutInit(&argc,argv);

  // image display window
  GLUT2d img( NRow, NCol, "image");
  GLUT2d_windows.push_back(&img);

  img.setPos(0, 0);
  img.setRGB(fbufs[0], fbufs[1], fbufs[2], 0, 1, 2);
  myglut2d_img = &img;
  img.glbusy = img.isClassification = false;
  img.mark();

  // classification display window
  GLUT2d clasi(NRow, NCol, "class");
  GLUT2d_windows.push_back(&clasi);

  clasi.setRightOf(&img);
  clasi.setRGB(fbufs[0], fbufs[1], fbufs[2], 0, 1, 2);
  myglut2d = &clasi;
  clasi.isClassification = true;
  clasi.glbusy = false;
  clasi.mark();

  number_of_classes = 0;
  laststate = beforelaststate = -1;

  // init scatter plot
  GLUT3d scatter(NCol * 2, NCol * 2, N);
  scatter.setRGB(fbufs[0], fbufs[1], fbufs[2], 0, 1, 2);
  GLUT3d_windows.push_back(&scatter);
  scatter.setBelow(&img);
  scatter.mark();

  clust_knn myKNNclust(NRow, NCol);
  // myglut::
  myclust_knn = &myKNNclust;

  // init clustering
  // KMax = KNN_USE;
  float ratio = floor(((float)n) / ((float)NDESIRED));
  printf("floor(n / NDESIRED) = %f\n", ratio);
  myKNNclust.init(&scatter, &clasi, &float_buffers, ratio, false);

  myKNNclust.set_Rand_Iter_Max(RAND_ITER_MAX);
  knn_clusterings.push_back(&myKNNclust);

  scatter.set_clust(&myKNNclust);
  clasi.set_clust(&myKNNclust);

  glutMainLoop();
  quit();
  return 0;
}
