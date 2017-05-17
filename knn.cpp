#include <cmath>
#include <float.h>
#include "glut.h"
#include "pick.h"
#include "global.h"
#include "clust_knn.h"
#include "ansi_color.h"

using namespace myglut;

int N;  // number of bands
int NRow;  // number of rows
int NCol;  // number of cols
int bi[3];  // active band indices

vector <FILE*> filehandles;
vector <GLUT2d *> GLUT2d_windows;
vector <GLUT3d *> GLUT3d_windows;
vector <clust_knn*> knn_clusterings;

void quit(){
	int i;
	for(i=0; i<filehandles.size(); i++){
		if( filehandles[i]) fclose( filehandles[i]);
	}
	for(i=0; i<float_buffers.size(); i++){
		free(float_buffers[i]);
	}
	exit(0);
}

void idle(){
	(myglut2d_img)->refresh();
	myglut2d->refresh();
	if ( number_of_classes==0){
		myglut3d->runclust();
		if( !myglut2d->glbusy){
		myglut2d->recalc_classes();
		myglut2d->draw_classes();
		}
	}
}

void scaleband( SA<float> * buf){
	long int n = buf->size();
	float MAX = FLT_MIN;
	float MIN = FLT_MAX;
	float dat;
	long int i;
	for(i=0; i<n; i++){
		dat = buf->at(i);
		if( !(isinf(dat) || isnan(dat))){
			if( dat < MIN) MIN = dat;
			if( dat > MAX) MAX = dat;
		}
		else{
			(*isBad)[i]=1;
		}
	}
	printf(" MIN %e MAX %e\n", MIN, MAX);

	for(i=0; i< buf->size(); i++){
		dat = buf->at(i);
		buf->at(i) =(dat-MIN)/(MAX-MIN);
		dat = buf->at(i);
	}
}

int main(int argc, char *argv[]){
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

  system("mkdir -p output");

	NRow = atoi(argv[1]);
	NCol = atoi(argv[2]);
	NDESIRED = atoi(argv[3]);
	KNN_USE  = atoi(argv[4]);
	RAND_ITER_MAX = atoi(argv[5]);

	int n = NRow*NCol;

	threadcreated=false;
	myglut::ostopthread=false;

  // open data files
	int bandnoffset = 6;
	N = argc-bandnoffset;
	SA<char*> filenames(N);
	SA<FILE*> files(N);
	register int i;
	for(i=0; i<N; i++){
		filenames[i]=argv[i+bandnoffset];
		files[i]=NULL;
		files[i]=fopen(filenames[i], "rb");
		filehandles.push_back(files[i]);
	}

  // check files opened
	int file_error = false;
	for(i=0; i<N; i++){
		if(!files[i]){
			file_error=true;
			printf("Error: could not open file: %s\n", filenames.at(i));
		}
	}
	if(file_error) quit();

  // buffer data
	SA< SA< float > * > fbufs(N);
	for(i=0; i<N; i++){
		fbufs[i] = (SA<float> *) new SA<float>(NRow, NCol);
		float_buffers.push_back(fbufs[i]);
	}
  isBad = new SA<int>(NRow * NCol);
  i_coord = new SA<int>(NRow * NCol);
  j_coord = new SA<int>(NRow * NCol);

	for(i=0; i<N; i++){
    // read data band
		int nr = fread(&(((fbufs[i])->elements)[0]),
                   sizeof(float),
                   NRow*NCol,
                   files[i]);
		printf("Read %d bytes\n", nr);
    // scale data band to [0, 1]
		scaleband(fbufs[i]);
	}

  if(false){
    // write data in ascii format
	  FILE * afile = fopen("./ascii.txt", "wb");
    int kk;
    for(kk=0; kk<NRow*NCol; kk++){
	    for(i=0; i<N; i++){
        fprintf(afile, "%7.7e,", fbufs[i]->at(kk));
      }
      fprintf(afile, "\n");
    }
	  fclose(afile);
  }
  //set default band vis.: 1, 2, 3
	for(i=0; i<3; i++) {
		bi[i]=i;
	}

	glutInit(&argc,argv);
	GLUT2d img( NRow, NCol);
  GLUT2d_windows.push_back(&img);

	img.setPos(0, 0);
	img.setRGB(fbufs[0], fbufs[1], fbufs[2], 0, 1, 2);
	img.mark();
	myglut2d_img = &img;
  img.glbusy=false;

	GLUT2d clasi( NRow, NCol);  GLUT2d_windows.push_back(&clasi);

	clasi.setRightOf(&img);
	clasi.setRGB(fbufs[0], fbufs[1], fbufs[2], 0, 1, 2);
	clasi.mark();
	clasi.enableClassification();
	clasi.glbusy=false;

	number_of_classes=0;
	laststate = -1;
	beforelaststate = -1;

  // init scatter plot
	GLUT3d scatter(NCol * 2, NCol * 2, N);
	scatter.setRGB( fbufs[0], fbufs[1], fbufs[2],0,1,2);
	GLUT3d_windows.push_back(&scatter);
	scatter.setBelow(&img);
	scatter.mark();

	clust_knn myKNNclust(NRow, NCol);
	myglut::myclust_knn = &myKNNclust;

  // init clustering
	myKNNclust.init(&scatter,
					        &clasi,
					        &float_buffers,
					        floor(((float)n)/((float)NDESIRED)),
					        KNN_USE);

	myKNNclust.set_Rand_Iter_Max(RAND_ITER_MAX);
	knn_clusterings.push_back(&myKNNclust);

	scatter.set_clust( &myKNNclust);
	clasi.set_clust(&myKNNclust);
	clasi.enableClassification();

	img.mark();
  img.refresh();
	img.isClassification = false;

	glutMainLoop();
	quit();
	return 0;
}
