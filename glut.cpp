#include "glut.h"
#include<string>
using namespace myglut;
using namespace std;

void hwrite(str hfn, size_t nrow, size_t ncol, size_t nband, size_t data_type){
  cout << "+w " << hfn << " nrow " << nrow << " ncol " << ncol << " nband " << nband << endl;
  ofstream hf(hfn);
  if(!hf.is_open()){
    printf("Error: failed to open header file for writing: %s\n", hfn.c_str());
    exit(1);
  }
  hf << "ENVI" << endl;
  hf << "samples = " << ncol << endl;
  hf << "lines = " << nrow << endl;
  hf << "bands = " << nband << endl;
  hf << "header offset = 0" << endl;
  hf << "file type = ENVI Standard" << endl;
  hf << "data type = " << data_type << endl;
  hf << "interleave = bsq" << endl;
  hf << "byte order = 0" << endl;
  hf.close();
}

GLUTWindow::GLUTWindow(){
  WindowID = Update = false;
  myclust = NULL;
}

// void
void GLUTWindow::focus(){
  int my_id = myID();
  // printf("glutSetWindow(%d)\n", my_id);
  glutSetWindow(my_id);
}

// int
int GLUTWindow::initGLUT(int _NRow, int _NCol, const char * title_string){
  printf("initGLUT(%d,%d)\n", _NRow, _NCol);
  Update = false;
  WindowY = _NRow;
  WindowX = _NCol;

  glutInitWindowSize(WindowX, WindowY);
  glutInitDisplayMode(GLUT_RGB);
  WindowID = glutCreateWindow(title_string);
  printf("initGLUT(%d)\n", WindowID);
  return WindowID;
}

//void
void GLUTWindow::setRGB(SA<float> * r, SA<float> * g, SA<float> * b, unsigned int A, unsigned int B, unsigned int C){
  b1 = r;
  b2 = g;
  b3 = b;
  curband[0] = A;
  curband[1] = B;
  curband[2] = C;
}

// void
void GLUTWindow::setDisplayFunc(void (*f)()){
  focus();
  glutDisplayFunc(f);
}

// void
void GLUTWindow::setKeys(void (*f)(unsigned char, int, int), void (*g)(int, int, int)){
  focus();
  glutKeyboardFunc(f);
  glutSpecialFunc(g);
}

// void
void GLUTWindow::setPos(int x, int y){
  focus();
  glutPositionWindow(x, y);
  XPos = x;
  YPos = y;
}

//void
void GLUTWindow::setRightOf(GLUTWindow * other){
  focus();
  setPos(other->XPos + other->WindowX, other->YPos);
}

//void
void GLUTWindow::setBelow(GLUTWindow * other){
  focus();
  setPos(other->XPos, 40 + other->YPos + other->WindowY);
}

// int
int GLUTWindow::myID(){
  return WindowID;
}

int GLUTWindow::nCol(){
  return WindowX;
}

int GLUTWindow::nRow(){
  return WindowY;
}

int GLUTWindow::myX(){
  return WindowX;
}

int GLUTWindow::myY(){
  return WindowY;
}

static void display3d(){
  myglut3d->draw3d();
}

static void display2d(){
  return;
}

static void displayd(){
  (myglut2d_img)->refresh();
}

int GLUT2d::reboot(){
  datClust.init(NRow * NCol * 3);
  datBinary.init(NRow * NCol * 3);
  datResult.init(NRow * NCol);
  indClosest.init(NRow, NCol);
  indCentre.init(NRow, NCol);
  surprise.init(NRow, NCol);
  int n = NRow * NCol;
  int i;
  for0(i, n){
    indClosest[i] = -1;
    indCentre[i] = -1;
    surprise[i] = 0;
  }
  return 0;
}

GLUT2d::GLUT2d(int _NRow, int _NCol, const char * title_string){
  thread_exists = false;
  render_clusters = false;
  NRow = _NRow;
  NCol = _NCol;
  dat = new SA<float>(NRow * NCol * 3);
  datClust.init(NRow * NCol * 3);
  datBinary.init(NRow * NCol * 3);
  datResult.init(NRow * NCol);
  indClosest.init(NRow, NCol);
  indCentre.init(NRow, NCol);
  surprise.init(NRow, NCol);

  int n = NRow * NCol;
  int i;
  for0(i, n){
    indClosest[i] = -1;
    indCentre[i] = -1;
    surprise[i] = 0;
  }

  GLUTWindow();
  initGLUT(NRow, NCol, title_string);
  glutDisplayFunc(displayd);
  glutKeyboardFunc(processKeys);
  glutDisplayFunc(idle);
}

int GLUT2d::initGLUT(int _NRow, int _NCol, const char * title_string){
  lock();
  myclust = NULL;
  Update = false;
  isClassification = false;
  WindowY = _NRow;
  WindowX = _NCol;
  glutInitWindowSize(WindowX, WindowY);
  glutInitDisplayMode(GLUT_RGB);
  WindowID = glutCreateWindow(title_string);
  printf("initGLUT(%d)\n", WindowID);
  return WindowID;
}

void GLUT2d::setView(){
  focus();
  glViewport(0, 0, NCol, NRow);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLfloat)NCol, 0.0, (GLfloat)NRow);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRasterPos2f(0., 0.);
}

void GLUT2d::refresh(){
  // if(isClassification && threadcreated) return;
  focus();
  if(Update == true){
    rebuffer();
    int did_recalc = false;
    if(false){
    if(!isClassification) did_recalc = recalc_classes();
    }
    Update = false;
  }
  draw2d();
  glbusy = false;
}

void GLUT2d::mark(){
  Update = true;
}

void GLUT2d::draw_classes(){
  beforelaststate = laststate;
  laststate = -2;
  focus();
  setView();
  glPixelStoref(GL_UNPACK_ALIGNMENT, 1);
  glDrawPixels(NCol, NRow, GL_RGB, GL_FLOAT, (GLvoid *) (datClust.elements));
  glFlush();
  //glutSwapBuffers();
}

void GLUT2d::idlethreadfunc(){
  // do we need to turn this back on?
  return;
  if(thread_exists) return;
  thread_exists = true;
  int optim = 1;
  //while(!ostopthread){
    printf("GLUT2d::idlethreadfunc() iter %d\n", optim++);
    int i;
    int nc = myclust->get_n_knn_centres();
    for0(i, nc){
      PickThis = i; laststate = i;
      source = (GLvoid *)(&((myglut2d->datBinary.elements)[0]));
      myglut2d->recalc_binary_quick(i);
      myglut2d->recalc_binary(i);
      myglut2d->quickdraw();

      PickThis = i; laststate = i;
      source = (GLvoid *)(&((myglut2d->datBinary.elements)[0]));
      myglut2d->recalc_binary_quick(i);
      myglut2d->recalc_binary(i);
      myglut2d->quickdraw();
    // }
  }
  thread_exists = false;
}

void GLUT2d::draw2d(){
  focus();
  if(!isClassification){
    printf("draw2d(%d) notClassification\n", myID());
    source = (GLvoid *)(&((dat->elements)[0])); // classification
    quickdraw();
    return;
  }
  else{
    printf("draw2d(%d) isClassification\n", myID());
    source = (GLvoid *)(&((dat->elements)[0]));
    if(!render_clusters){
      source = (GLvoid *)(&((dat->elements)[0]));
    }
    else{
      if(PickThis < 0){
        source = (GLvoid *)(&((datClust.elements)[0]));
      }
      else{
        source = (GLvoid *)(&((datBinary.elements)[0]));
        if(PickThis != laststate){
          recalc_binary_quick(PickThis);
        }
        recalc_binary_quick(PickThis);
        quickdraw();
        if(_lastmouseRight) return;

        if(false) recalc_classes(); // 20220813
        quickdraw();
        recalc_binary(PickThis);
        source = (GLvoid *)(&((datBinary.elements)[0])); //binary classification
      }
    }
    quickdraw();
  }
}

void GLUT2d::quickdraw(){
  printf("quickdraw(%d)\n", myID());
  focus();
  setView();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // was missing this
  glPixelStoref(GL_UNPACK_ALIGNMENT, 1);
  glDrawPixels(NCol, NRow, GL_RGB, GL_FLOAT, source);
  glFlush();
  glutSwapBuffers();
}

void GLUT2d::recalc_binary_quick(int mypick){
  int i, j, k, rs, ind, ind2, result;
  FILE * outfile1 = fopen("output/out_binary.bin", "wb");
  FILE * outfile2 = fopen("output/out_class.bin", "wb");
  float d;

  for0(i, NRow){
    rs = NRow - i - 1;

    for0(j, NCol){

      ind = ((rs * NCol) + j);
      ind2 = 3 * ((rs * NCol) + j);
      k = datResult.at(ind);

      if(k == mypick){
        result = 1;
        datBinary.at(ind2++) = result;
        datBinary.at(ind2++) = result;
        datBinary.at(ind2 ) = result;
      }
      else{
        result = 0;
        datBinary.at(ind2++) = result;
        datBinary.at(ind2++) = result;
        datBinary.at(ind2 ) = result;
      }

      d = (float)result; // write outputs in type 4
      fwrite(&d, sizeof(float), 1, outfile1);
      d = (float)k;
      fwrite(&d, sizeof(float), 1, outfile2);

    }
  }
  fclose(outfile1);
  fclose(outfile2);

  hwrite(string("output/out_binary.hdr"), NRow, NCol, 1, 4);
}

void GLUT2d::recalc_binary(int mypick){
  return;
  printf("recalc_binary(%d)\n", mypick);
  int i, j, k, rs;
  int ind, ind2;
  int comp = NRow / 4;
  int state = datResult.at(0);
  int count_same_state = 0; // # of preceding pixels same in spatial ordering

  for0(i, NRow){
    rs = NRow - i - 1;
    for0(j, NCol){

      ind = ((rs * NCol) + j);
      ind2 = 3 * ((rs * NCol) + j);
      k = datResult.at(ind);

      surprise.at(i, j) = count_same_state;
      avg_surprise += (double)count_same_state;

      if(k == state){
        count_same_state++;
      }
      else{
        state = k;
        count_same_state = 0;
      }

      if(k == mypick){
        datBinary.at(ind2++) = 1.;
        datBinary.at(ind2++) = 1.;
        datBinary.at(ind2 ) = 1.;
      }
      else{
        datBinary.at(ind2++) = 0.;
        datBinary.at(ind2++) = 0.;
        datBinary.at(ind2 ) = 0.;
      }
    }
  }

  float as = (float)( avg_surprise / ((double)NRow * (double)NCol));
  surprising_count = 0;

  for0(i, NRow){
    for0(j, NCol){
      if((float)surprise.at(i,j) >= as){
        surprising_count++;
        reclass_point(i, j);
      }
    }
  }

/* think this is:
"Furthermore, a larger proportion of random checks were
performed when labeling spatially non-homogeneous pixels,
reducing the number of point mis-registrations introduced
by the randomized algorithm" */

}

void GLUT2d::reclass_point(int i, int j){
  if(!myclust) return;

  vector < SA<float> * > * fb = myclust->float_buffers;
  if(!fb) return;

  int rs = NRow - i - 1;
  int cMIN = 0;
  float d, dMIN, tmp;
  int K, J, I, ind, n_elem, rj;
  int N = fb->size();
  int nc = myclust->get_n_knn_centres();
  int rim = myclust->get_Rand_Iter_Max();
  rim = rim * 5;

  float x = b1->at(rs, j);
  float y = b2->at(rs, j);
  float z = b3->at(rs, j);
  int rjmin = indClosest.at(i, j);

  if(rjmin >= 0){
    cMIN = indCentre.at(i, j);
    d = 0;
    for0(K, N){
      tmp = (fb->at(K)->at(i,j)) - (myclust->get_centre_coord(cMIN, rjmin, K));
      d += tmp * tmp;
    }
    dMIN = d;
  }
  else{
    dMIN = FLT_MAX;
    cMIN = 0;
  }

  for0(J, nc){
    //for each centre.
    n_elem = myclust->get_n_knn_elements(J);

    for0(I, rim){
      //for each iteration.
      rj = crappy_rand() % n_elem; //select random element from cluster.

      d = 0.;
      for0(K, N){
        tmp = (fb->at(K)->at(i, j)) - (myclust->get_centre_coord(J, rj, K));
        d += tmp * tmp;
      }
      if(d < dMIN){
        //found closer element.
        dMIN = d;
        cMIN = J;
        rjmin = rj;
      }
    }
  }

  // use repr. element from centre to assign color to cluster.
  ind = 3 * ((rs * NCol) + j);
  datClust.at(ind++) = myclust->get_centre_coord(cMIN, curband[0]);
  datClust.at(ind++) = myclust->get_centre_coord(cMIN, curband[1]);
  datClust.at(ind ) = myclust->get_centre_coord(cMIN, curband[2]);
  datResult.at((rs * NCol) + j) = cMIN;
  indClosest.at(i, j) = rjmin;
  indCentre.at(i, j) = cMIN;

  // should have this transformation into all of the bands at once, too!
}

/* 20220813 parallelized by row */

void recalc_classes_row(size_t i){
  int NRow = myglut2d->NRow;
  int NCol = myglut2d->NCol;
  int rs = NRow - i - 1;
  if(i % 50 == 0){
    printf("recalc_classes_row %zu of %d rand_iter %d\n", i, NRow, (int)myglut2d->myclust->get_Rand_Iter_Max());
  }
  SA<float> * dat = myglut2d->dat;
  vector< SA<float> * > * fb = myglut2d->myclust->float_buffers;

  int j, I, J, K, p, q, r, N, n, nc, comp, n_elem, rim, cMIN, ind, rj, rjmin;
  float x, y, z, d, dMIN,tmp;
  
  nc = myglut2d->myclust->get_n_knn_centres();
  rim = myglut2d->myclust->get_Rand_Iter_Max();
  N = fb->size();
  n = NRow * NCol * N;
  comp = NRow / 5;

  float dd;  // coordinate for arbitrary data point
  float cc; // coordinate for centre/hilltop point, associated with a given clustera
  
  clust_knn * my_clust = myglut2d->myclust;

  if((i % comp) == 0) printf("Applying: %d/100\n", (int)(floor(100. * ((float)(i + 1) / (float)(NRow)))));
    for0(j, NCol){
      rjmin = myglut2d->indClosest.at(i, j);

        cMIN = myglut2d->indCentre.at(i, j);
        dMIN = FLT_MAX;

        for0(J, nc){  // for each cluster "centre" / hilltop..

          // get the index in the non-decimated / original data for the hilltop point. 
          int orig_idx = my_clust->origIndex[my_clust->knn_J_indices[J]];
          d = 0.;
          for0(K, N){  // for each dimension
            dd = (fb->at(K)->at(i, j)); // data value this row=i, col=j, band=K
            cc = fb->at(K)->at(orig_idx);
            tmp = dd - cc;
            d += tmp * tmp;
          }
          if(d < dMIN){
            dMIN = d;
            cMIN = J;
          }
        }
   
   if(rim > 0){
      for0(J, nc){
        n_elem = myglut2d->myclust->get_n_knn_elements(J); // for each centre

        // for each prescribed random iteration:
        for0(I, rim){
          (rj = crappy_rand() % n_elem), d = 0.; //for each iteration, select random element from clust
          for0(K, N){
            tmp = (fb->at(K)->at(i, j)) - (myglut2d->myclust->get_centre_coord(J, rj, K));
            d += tmp * tmp;
          }
          if(d < dMIN){
            (dMIN = d), (cMIN = J), (rjmin = rj); // found a closer element
          }
        }
      }
    }

      //use repr. element from centre to assign color to clust
      int orig_idx = my_clust->origIndex[my_clust->knn_J_indices[cMIN]];

      ind = 3 * ((rs * NCol) + j);
      myglut2d->datClust.at(ind++) = fb->at(myglut3d->curband[0])->at(orig_idx); // myglut2d->myclust->get_centre_coord(cMIN, myglut3d->curband[0]);
      myglut2d->datClust.at(ind++) = fb->at(myglut3d->curband[1])->at(orig_idx);//myglut2d->myclust->get_centre_coord(cMIN, myglut3d->curband[1]);
      myglut2d->datClust.at(ind) = fb->at(myglut3d->curband[2])->at(orig_idx); //myglut2d->myclust->get_centre_coord(cMIN, myglut3d->curband[2]);
      myglut2d->datResult.at((rs * NCol) + j) = cMIN;
      myglut2d->indClosest.at(i, j) = rjmin;
      myglut2d->indCentre.at(i, j) = cMIN;
    }
}

int GLUT2d::recalc_classes(){
  // how about try colour with mean instead of top?
  // how about log-sized circles to indicate membership size?
  printf("GLUT2d::recalc_classes()\n");
  if(!myclust) return false;
  // srand(time(NULL));

  long int i, j, I, J, K, p, q, r, N, n, nc, comp, n_elem, rim, cMIN, ind, rs, rj, rjmin;
  float x, y, z, d, dMIN,tmp;

  vector< SA<float> * > * fb = myclust->float_buffers;
  if(!fb) return false;

  nc = myclust->get_n_knn_centres();
  rim = myclust->get_Rand_Iter_Max();
  N = fb->size();
  n = NRow * NCol * N;
  comp = NRow / 5;

  parfor(0, NRow, &recalc_classes_row); //distance_calculation);

  // output classification results
  FILE * f = fopen("output/class.bin", "wb");
  if(!f){
    printf("Error: failed to open output/class.bin\n");
    exit(1);
  }

  for0(i, NRow){
    rs = NRow - i - 1;
    for0(j, NCol){
      d = datResult.at((rs * NCol) + j);
      fwrite(&d, sizeof(float), 1, f);
    }
  }
  fclose(f);
  hwrite(string("output/class.hdr"), NRow, NCol, 1, 4);
  return true;
}

void GLUT2d::rebuffer(){
  printf("GLUT2d::rebuffer(%d)\n", myID());
  int i, j, k, ri;
  k = 0;

  for0(i, NRow){
    ri = NRow - i - 1;
    for0(j, NCol){
      //remember, computer screen coordinates different..
      dat->at(k++) = b1->at(ri, j);
      dat->at(k++) = b2->at(ri, j);
      dat->at(k++) = b3->at(ri, j);
    }
  }
}

void GLUT3d::set_clust(clust_knn * k){
  myclust = k;
}

void GLUT3d::runclust(){
  myclust->knn_clustering();
}

GLUT3d::GLUT3d(int _NRow, int _NCol, int _N){
  N = _N;
  NCol=_NCol;
  NRow = _NRow;
  PickThis = -1;
  myclust = NULL;
  lock = writelock = false;

  printf("******* GLUT3d\n");
  GLUTWindow();
  initGLUT(NRow, NCol);
}

int GLUT3d::initGLUT(int _NRow, int _NCol){
  printf("GLUT3d::initGLUT\n");
  Update = false;
  WindowY = _NRow;
  WindowX = _NCol;
  glutInitWindowSize(WindowX, WindowY);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glEnable(GL_DEPTH_TEST);
  WindowID = glutCreateWindow("3d Cluster Plot");
  printf("initGLUT(%d)\n", WindowID);

  zpr::zprInit(WindowID);
  zpr::zprSetWindowID(WindowID);
  zpr::zprPickFunc(pick); /* Pick event client callback */
  zpr::zprSelectionFunc(display3d); /* Selection mode draw function */
  glutDisplayFunc(display3d);
  glutKeyboardFunc(processKeys);
  return WindowID;
}

void GLUT3d::setView(){
  focus();
}

void GLUT3d::display_picked_points(int PickThis){
  if(! myclust || PickThis < 0){
    return;
  }
  focus();

  /*
  int get_n_knn_centres() //return the number of classes.
  int get_n_knn_elements(int centre_index)
  //get the number of dat elements in a given class.
  float get_centre_coord(int centre_index, int elem_ind, int coord)
  //get the i'th coordinate of a given N-dimensional dat element.
  */

  int n_centres = myclust->get_n_knn_centres();
  int i, j, n_elem;
  float x, y, z;

  glColor3f(0., 0., 1.);
  glPointSize(3.);
  glBegin(GL_POINTS);
  i = PickThis;

  n_elem = myclust->get_n_knn_elements(i);
  printf("draw |picked elements|= %zu\n", (long unsigned int)n_elem);

  for0(j, n_elem){
    x = myclust->get_centre_coord(i, j, (unsigned int)curband[0]);
    y = myclust->get_centre_coord(i, j, (unsigned int)curband[1]);
    z = myclust->get_centre_coord(i, j, (unsigned int)curband[2]);
    glVertex3f(x, y, z);

  }
  glEnd();
}

void GLUT3d::draw3d(){
  /* update dimensions in common from 3d projection of 4d in
  multiple windows would be nice */

  focus();
  int i, j;
  float x, y, z;
  int n = b1->size();

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // axes
  float X[3];
  for(i = 0; i < 3; i++){
    for(j = 0; j < 3; j++){
      X[j] = (j==i)? 1.: 0.;
    }
    glColor3f(X[0], X[1], X[2]); 
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(X[0], X[1], X[2]);
    glEnd();
  }

  // points
  glColor3f(0., 1., 0.);
  glPointSize(1.);

  glBegin(GL_POINTS);
  for(i = 0; i < n; i += 20){
    if(i < n){
      glColor3f(b1->at(i), b2->at(i), b3->at(i));
      glVertex3f(b1->at(i), b2->at(i), b3->at(i));
    }
  }
  glEnd();

  if(!lock){
    if(class_centres.size() >= N){
      glInitNames();

      int j, k;
      k = 0;
      SA<float> xyz(N);
      int nn = class_centres.size() / N;
      if(nn != number_of_classes){
        printf("Error: count of number of classes did not match.\n");
        exit(1);
      }

      for0(i, nn){
        glPushName(i);
        for0(j, N){
          xyz[j] = class_centres[k++];
        }

        x = xyz[(unsigned int)curband[0]];
        y = xyz[(unsigned int)curband[1]];
        z = xyz[(unsigned int)curband[2]];

        if(PickThis == i) glColor3f(1., 1., 1.);
        else glColor3f(0., 0., 1.);

        glPushMatrix();
        glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
        glutSolidSphere(0.005, 7, 7);
        glPopMatrix();
        glPopName();
      }
    }
  }

  if(PickThis >= 0){
    display_picked_points(PickThis);
  }

  drawText();
  glFlush();
  glutSwapBuffers();
  PickThis = -1;
}

void GLUT3d::refresh(){
  draw3d();
}
void GLUT3d::mark(){
  Update = true;
}

static void processKeys(unsigned char key, int x, int y) {
  keyboard(key, x, y);
}

void extern show_classes(){
  myglut2d->draw_classes();
}

void extern toggle_display(){

  int state_change = true; // beforelaststate != laststate;
  int last_state_good = beforelaststate >= 0 && beforelaststate < number_of_classes;

  source = (GLvoid *)(&((myglut2d->datBinary.elements)[0]));

  if(state_change && last_state_good){
    printf("toggle_display CASE 1\n");
    // swap states
    int tmp = laststate;
    laststate = beforelaststate;
    beforelaststate = tmp;

    //source = (GLvoid *)(&((myglut2d->datBinary.elements)[0]));

    if(PickThis != laststate) myglut2d->recalc_binary_quick(laststate);

    myglut2d->quickdraw();
  }
  else if((beforelaststate != laststate) && ((beforelaststate == -2) || (laststate==-2))){
    printf("toggle_display CASE 2\n");
    int tmp;
    tmp = laststate;
    laststate = beforelaststate;
    beforelaststate = tmp;
    if( beforelaststate == -2){
      // source = (GLvoid *)(&((myglut2d->datBinary.elements)[0]));
      if(PickThis != laststate){
        myglut2d->recalc_binary_quick(laststate);
      }
      myglut2d->quickdraw();
    }
    else if(laststate == -2){
      source = (GLvoid *)(&((myglut2d->datClust.elements)[0]));
      if(PickThis != laststate){
        myglut2d->recalc_binary_quick(laststate);
      }
      myglut2d->quickdraw();
    }
  }
  else{
    printf("****** LOST CASE..\n");
  }
}
