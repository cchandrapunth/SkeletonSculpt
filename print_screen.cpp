
#include "stdafx.h"
#include <GL/glut.h>

int zz = 4.5;
char *buff;

void set_text(char* x){
	buff = x;
}
void printscr()
{
  void *font = GLUT_BITMAP_TIMES_ROMAN_24;
  int len, i;

  glRasterPos3f(0, 0, zz);
  len = (int) strlen(buff);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(font, buff[i]);
  }
}