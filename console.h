#pragma once
#ifndef HEADER_CONSOLE_H
#define HEADER_CONSOLE_H
#include "glut.h"
#include <stdio.h>
#include "global.h"
#include <stdlib.h>

namespace console{
  #define STR_MAX 16384

  // #define MYFONT GLUT_BITMAP_HELVETICA_18
  // #define MYFONT GLUT_BITMAP_8_BY_13
  #define MYFONT GLUT_BITMAP_9_BY_15
  // #define MYFONT GLUT_BITMAP_TIMES_ROMAN_10
  // #define MYFONT GLUT_BITMAP_TIMES_ROMAN_24
  // #define MYFONT GLUT_BITMAP_HELVETICA_10
  // #define MYFONT GLUT_BITMAP_HELVETICA_12
  // #define MYFONT GLUT_BITMAP_HELVETICA_18

  class GLUT2d;
  class GLUT3d;

  extern GLUT2d* myglut2d;
  extern GLUT3d* myglut3d;
  extern GLUT2d* myglut2d_img;

  extern void show_classes();
  extern void toggle_display();

  extern int WINDOWX;
  extern int WINDOWY;
  extern int console_position;
  extern char console_string[STR_MAX];

  void quitme();
  void display();
  void drawText();
  int grabint(char * p);
  void process_string();
  void setrgb(int r, int g, int b);
  void getrgb(int & r, int & g, int & b);
  void keyboard(unsigned char key, int x, int y);
  void renderBitmapString(float x, float y, void *font, char *string);
}

#endif
