#pragma once
#ifndef HEADER_CONSOLE_H
#define HEADER_CONSOLE_H
#include "glut.h"
#include <stdio.h>
#include "global.h"
#include <stdlib.h>

namespace console{
	#define STR_MAX 10000

	// #define MYFONT  GLUT_BITMAP_HELVETICA_18 
	// #define MYFONT  GLUT_BITMAP_8_BY_13
	#define MYFONT  GLUT_BITMAP_9_BY_15
	// #define MYFONT  GLUT_BITMAP_TIMES_ROMAN_10
	// #define MYFONT  GLUT_BITMAP_TIMES_ROMAN_24
	// #define MYFONT  GLUT_BITMAP_HELVETICA_10
	// #define MYFONT  GLUT_BITMAP_HELVETICA_12
	// #define MYFONT  GLUT_BITMAP_HELVETICA_18
	
	class GLUT2d;
	class GLUT3d;
	
	extern GLUT2d* myglut2d;
	extern GLUT2d* myglut2d_img;
	extern GLUT3d* myglut3d;
	extern void show_classes();
	extern void toggle_display();
	
  // might need to delete these:
	extern pthread_t opthread;
	extern void * othread(void *);
	
  extern char console_string[STR_MAX];
	extern int console_position;
	extern int WINDOWX;
	extern int WINDOWY;
	
	void setrgb(int r, int g, int b);
	void getrgb(int & r, int & g, int & b);		
	int grabint(char * p );
	void display();
	void process_string();
	void renderBitmapString(float x, float y, void *font, char *string);
	void drawText();
	void keyboard(unsigned char key, int x, int y);
	void quitme();	
}

#endif
