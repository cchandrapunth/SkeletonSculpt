#include "stdafx.h"
#include <GL/glut.h>

void print_to_screen(float x, float y, float z, char* msg){
	
	glRasterPos3f(x, y, z);
	char buff[256];
	sprintf(buff, "%c", msg);
	for(int i=0; i< strlen(buff); i++){
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, buff[i]);
	}	
}