
#include "stdafx.h"
#include <GL/glut.h>
#include "drawbackground.h"

float z = -2.5;
float bd = 1.9;
void draw_background(){
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND); 
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glColor3f(0.5f, 0.5f, 0.5f);
    draw_rect(-(bd+0.1), bd+0.1, bd+0.1, -(bd+0.1));

	//scale
	glColor3f(0.1, 0.1, 0.1);
	draw_line(0, bd, 0, -bd);
	draw_line(bd, 0, -bd, 0);

	//grid
	glColor3f(0.6, 0.6, 0.6);
	for(int i=0; i<20;i ++){		
		draw_line(-bd+(0.2*i), bd, -bd+(0.2*i), -bd);
		draw_line(bd, -bd+(0.2*i), -bd, -bd+(0.2*i));
	}
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);//line shows more
}

void draw_rect(float x1, float y1, float x2, float y2){
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1, z-0.1);
	glVertex3f(x2, y1, z-0.1);
	glVertex3f(x2, y2, z-0.1);
	glVertex3f(x1, y2, z-0.1);
	glEnd();
}

void draw_line(float ax,float ay, float bx, float by){
	glBegin(GL_LINES); 
    glVertex3f( ax, ay, z); 
    glVertex3f( bx, by, z); 
    glEnd(); 
}