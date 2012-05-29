
#include "stdafx.h"
#include <GL/glut.h>
#include "print_screen.h"

int zz = 4.6;
char *buff;

void set_text(char* x){
	buff = x;
}
void printscr()
{
  void *font = GLUT_BITMAP_TIMES_ROMAN_24;
  int len, i;
  
  glColor3f(0,0,0);
  glRasterPos3f(-1.85, 1.8, zz);
  len = (int) strlen(buff);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(font, buff[i]);
  }
  
}