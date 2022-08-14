#include "pick.h"

using namespace myglut;

void extern myglut::pick(GLint name){
  if(name < 0) return;
  printf("Pick(%d)\n", name);
  PickThis = name;
  lastpick = PickThis;

  /*
  int tmp = laststate;
  laststate = beforelaststate;
  beforelaststate = tmp;
  */

  printf("Pick: %d\n", name);
  fflush(stdout);

  myglut2d->draw2d();
  myglut3d->draw3d();

  /*
  source = (GLvoid *)(&((myglut2d->datBinary.elements)[0]));
  myglut2d->recalc_binary_quick(PickThis);
  myglut2d->quickdraw();
  */

  //int tmp = laststate;
  //laststate = beforelaststate;
  //beforelaststate = tmp;

  /*if(PickThis != laststate)*/

  if(false){
	  // only recalculate classes if we change K
    myglut2d->recalc_classes();
  }
  // myglut2d->recalc_binary_quick(PickThis);
  source = (GLvoid *)(&((myglut2d->datBinary.elements)[0]));
  myglut2d->quickdraw();


  PickThis = lastpick;
  beforelaststate = laststate;
  laststate = PickThis;
}
