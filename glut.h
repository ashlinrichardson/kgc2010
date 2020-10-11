#pragma once
#include "global.h"
#include "clust_knn.h"
#include "zpr.h"
#include "pick.h"
#include "console.h"

void quit();
void idle();

namespace myglut{

  #define WindowX NCol
  #define WindowY NRow

  class GLUTWindow;
  class GLUT3d;
  class GLUT2d;
  class clust_knn;

  static void display3d();
  static void display2d();
  static void displayd();
  static void processKeys(unsigned char key, int x, int y) ;

  //previously key functionality
  void extern show_classes();
  void extern toggle_display();

  class GLUTWindow{
    public:
    clust_knn * myclust;
    unsigned int curband[3];
    SA<float> * b1;
    SA<float> * b2;
    SA<float> * b3;

    int writelock;
    int busy(){
      return writelock;
    }
    void setBusy(){
      writelock = true;
    }
    void setNotBusy(){
      writelock = false;
    }

    int isClassification;
    void enableClassification(){
      isClassification = true;
    }

    GLUTWindow();
    void focus();
    int initGLUT(int _NRow, int _NCol, const char * title_string);
    void setRGB( SA<float> * r, SA<float> * g, SA<float> * b,
    unsigned int A, unsigned int B, unsigned int C);

    void setDisplayFunc( void (*f)());
    void setKeys( void (*f)(unsigned char, int, int) , void (*g)(int, int, int) );
    void setPos(int x, int y);
    void setRightOf(GLUTWindow * other);
    void setBelow(GLUTWindow * other);
    void set_clust(clust_knn * k){
      myclust = k;
    }
    int myID();
    int nCol();
    int nRow();
    int myX();
    int myY();

    int XPos, YPos;
    int WindowX, WindowY;
    int WindowID;
    int Update;
  };

  class GLUT2d;

  class GLUT2d: public GLUTWindow{
    public:
    SA<float> * dat;
    SA<float> datClust; //data transformed to display clusters...
    SA<float> datBinary; //black or white image (to reveal one class).
    SA<int> datResult; //class centre index image...
    SA<int> indClosest; //record the index of the pixel (from the clustered pixels)
    //that is closest so far to the given (full image) pixel.
    SA<int> indCentre; //records the cluster index.

    SA<int> surprise;
    int render_clusters; //true/false

    GLUT2d(int _NRow, int _NCol, const char * title_string);
    int initGLUT(int _NRow, int _NCol, const char * title_string);

    void setView();
    void quickdraw();
    void draw2d();
    void rebuffer();
    void refresh();
    void mark();
    void lock(){
      render_clusters = false;
    }
    void unlock(){
      render_clusters = true;
    }
    void recalc_binary_quick(int mypick);
    void recalc_binary(int mypick);
    void reclass_point(int i, int j);

    int reboot();

    int recalc_classes();
    void draw_classes();

    void idlethreadfunc();
    int thread_exists;
    int glbusy;
  };

  class GLUT3d: public GLUTWindow{
    public:
    int lock;
    int N;
    vector<float> class_centres;
    GLUT3d(int _NRow, int _NCol, int _N);
    int initGLUT(int _NRow, int _NCol);
    void set_clust( clust_knn * k);
    void runclust();
    void setView();
    void display_picked_points(int PickThis);
    void draw3d();
    void refresh();
    void mark();
  };
}
