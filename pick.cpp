#include "pick.h"

using namespace myglut;

void extern myglut::pick(GLint name){
		PickThis = name;

		if(name >=0 ) {
			lastpick = PickThis;

			printf("Pick: %d\n",name);
			fflush(stdout);
			if(!threadcreated) {
				myglut2d->draw2d();
			}

			myglut3d->draw3d();
			if(myglut::myglut2d){
			}
			else{
				printf("Warning: myglut::myglut2d not initialized.\n");
				printf("myglut2d ptr: %p \n", (myglut::myglut2d));
			}

			PickThis = lastpick;
			beforelaststate = laststate;
			laststate = PickThis;
		}
	}
