#include "console.h"
using namespace myglut;

namespace console{
  char console_string[STR_MAX];
  int console_position, WINDOWX, WINDOWY;

  void quitme(){
    exit(0);
  }

  void display(){
    myglut::myglut3d->refresh();
  }

  void renderBitmapString(float x, float y, void *font, char *string){
    char *c;
    glRasterPos2f(x,y);
    for(c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
  }

  void drawText(){
    glPushMatrix();
    glLoadIdentity();
    glColor3f(0,1,0);
    float tx = -1. * (float)WINDOWX / (float)WINDOWY;
    float ty = -1. + (float)15 / (float)WINDOWY;
    renderBitmapString(tx, ty, MYFONT, console_string);
    glPopMatrix();
  }

  int grabint(char * p){
    return atoi(p);
  }

  void setrgb(int r, int g, int b){
    vector < SA<float> * > * fb = &myglut::float_buffers;

    myglut::myglut3d->setRGB(fb->at(r), fb->at(g), fb->at(b), r, g, b);
    myglut::myglut2d->setRGB(fb->at(r), fb->at(g), fb->at(b), r, g, b);
    myglut::myglut2d_img->setRGB(fb->at(r), fb->at(g), fb->at(b), r, g, b);

    myglut::myglut3d->mark();
    myglut::myglut2d->mark();
    myglut::myglut2d_img->mark();

    myglut::myglut3d->refresh();
    myglut::myglut2d->refresh();
    myglut::myglut2d_img->refresh();
  }

  void getrgb(int & r, int & g, int & b){
    r = myglut::myglut3d->curband[0];
    g = myglut::myglut3d->curband[1];
    b = myglut::myglut3d->curband[2];
  }

  void recluster(){
    myglut::myclust_knn->knn_clustering();
    
    myglut::myglut2d->reboot();
    myglut::myglut2d->unlock();
    myglut::myglut2d->recalc_classes();
    myglut::myglut2d->draw_classes();
    myglut::myglut2d->mark();
    myglut::myglut3d->mark();

    myglut::myglut3d->lock = false;
    myglut::myglut2d->refresh();
    myglut::myglut3d->refresh();
  }

  void process_string(){
    int i = 0;
    vector < SA<float> * > * fb = &myglut::float_buffers;

    while(console_string[i]!='\0') i++;
    
    int count = i + 1;
    SA<char> s(count);
    for0(i, count) s[i] = console_string[i];
    
    int r, g, b, tk;
    i = grabint(&s[1]);
    int ndim = (myglut::float_buffers).size();
    switch(s[0]){

      // change density estimate
      case 'd':
      if(i >= 1 && i <= myglut::n_density_estimates){
        printf("Selecting density estimate: %d\n", i);
        density_estimate = i - 1;
        recluster();
      }
      break;

      // select red
      case 'r':
      if(i < 1 || i > ndim) break; // printf("Select band in range 1-N\n");
      getrgb(r, g, b);
      setrgb(i - 1, g, b);
      // clasi->recalc_classes();
      break;

      // select green
      case 'g': // printf("Select band in range 1-N\n");
      if(i < 1 || i > ndim) break;
      getrgb(r, g, b);
      setrgb(r, i - 1, b);
      break;

      // select blue 
      case 'b': // printf("Select band in range 1-N\n");
      if(i < 1 || i > ndim) break;
      getrgb(r, g, b);
      setrgb(r, g, i - 1);
      break;

      // select K
      case 'k':
      if(i < 1 || i > KNN_MAX){
        printf("please select K in [1, %d]\n", KNN_MAX);
	break;
      }
      printf("Running knn classification with %d nearest neighbors\n", i);
      myglut::myclust_knn->reinit(n_skip, i);
      recluster();
      break;

      // change metric
      case 'm':
      printf("Attempting to switch to metric %d\n", i);
      select_distance_function = i - 1;
      break;

      // display parameters
      case 'p':
      getrgb(r, g, b);
      printf("r band %d g band %d b band %d\n", r + 1, g + 1, b + 1);
      printf("Number of dimensions: %d\n", (int)(fb->size()));
      printf("Number of clusters : %d\n", myglut::number_of_classes);
      //printf("Metric used: : %d of ?\n", myglut::select_distance_function);
      //printf("Density estimate : %d of %d\n", density_estimate+1, n_density_estimates);
      //printf("Estimates available : %d\n", myglut::n_density_estimates);
      printf("Number of neighbors : %d\n", KNN_USE);

      default:
      break;
    }
  }

  void wipe(){
    console_string[0] = '\0';
    console_position = 0;
    display();
  }

  void keyboard(unsigned char key, int x, int y){

    // show classes
    if(key == 'c'){
      wipe();
      myglut::show_classes();
      return;
    }

    // toggle class display: from class means encoding, to binary classification: or vice-versa
    if(key == 't'){
      wipe();
      myglut::toggle_display();
      return;
    }

    switch(key){
      case 127:
      case 8:

      if(console_position > 0){
        console_position --;
        console_string[console_position] = '\0';
        display();
      }
      break;

      case 13: // enter
      process_string();
      console_string[0] = '\0';
      console_position = 0;
      display();
      break;

      case 27: // esc
      quitme();
      exit(0);
      break;

      /* case 127 : //delete */

      default:
      console_string[console_position++] = (char)key;
      console_string[console_position] = '\0';
      display();
      break;
    }
  }
}
