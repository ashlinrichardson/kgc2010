#pragma once
/*
 * Zoom-pan-rotate mouse manipulation module for GLUT
 * Version 0.4, October 2003
 *
 * Nigel Stewart
 * School of Computer Science and Information Technology
 * RMIT University
 * nigels@cs.rmit.edu.au
 *
 */

#ifdef WIN32
#include <windows.h>
#endif

#include "global.h"
#include "console.h"

namespace zpr{
	/* Mouse Manipulation API */
	
	
	void zprInit();
	void zprSetWindowID(int w);
	
	static GLfloat zprReferencePoint[4];
	
	/* Picking API (Optional) */
	
	extern void zprSelectionFunc(void (*f)(void));   /* Selection-mode draw function */
	extern void zprPickFunc(void (*f)(GLint name));  /* Pick event handling function */
	
	static void (*selection)(void);
	static void (*pick)(GLint name);

	static int windowid;
	
	static double _left   = 0.0;
	static double _right  = 0.0;
	static double _bottom = 0.0;
	static double _top    = 0.0;
	static double _zNear  = -10.0;
	static double _zFar   = 10.0;
	
	static int  _mouseX      = 0;
	static int  _mouseY      = 0;
	static int _mouseMiddle = false;
	
	static double _dragPosX  = 0.0;
	static double _dragPosY  = 0.0;
	static double _dragPosZ  = 0.0;
	
	static double _matrix[16];
	static double _matrixInverse[16];
		
	static double vlen(double x,double y,double z);
	static void   pos(double *px,double *py,double *pz,const int x,const int y,const int *viewport);
	static void   getMatrix();
	static void   invertMatrix(const GLdouble *m, GLdouble *out );
	
	static void zprReshape(int w,int h);
	static void zprMouse(int button, int state, int x, int y);
	static void zprMotion(int x, int y);
	
	static void zprPick(GLdouble x, GLdouble y,GLdouble delX, GLdouble delY);	
	
}

