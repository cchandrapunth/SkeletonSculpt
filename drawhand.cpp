#include "stdafx.h"
#include <gl/glut.h>
#include "XnPoint3D.h"
#include <math.h>
#include "gesture.h"
#include "hand_history.h"
#include "drawhand.h"
#include "vertex.h"
#include "vmmodel.h"
#include "picking.h"
#include "display.h"
#include "drawmodel.h"
#include "paint.h"
#include "mode.h"
#include "paint.h"

int initZ = 4.5;


void drawRHand(bool grab, float x, float y, float z){
	
		if(isRotate()){
			drawArrow(x, y, grab);
			return;
		}
		else if(getSelection() >= 0){
			drawSelection(grab);
			return;
		}


		glPointSize(12);
		if(grab) glColor3f(0.0, 0, 1.0);
		else glColor3f(0.9,0.9,0.8);
		float dis = 0.1;
 
		float aspect = getAspect();
		glBegin(GL_POINTS);
		glVertex3f(x, y, initZ);
		glEnd();
		
		glBegin(GL_LINES);
		glVertex3f((GLfloat) x, (GLfloat) (y+dis), initZ);
		glVertex3f((GLfloat) x, (GLfloat) (y-dis), initZ);
		glVertex3f((GLfloat) (x+dis), (GLfloat) y, initZ);
		glVertex3f((GLfloat) (x-dis), (GLfloat) y, initZ);
		glEnd();
		
}

void drawLHand(bool grab, float x, float y, float z){
		
		glPointSize(12);
		if(grab) glColor3f(1.0, 0, 0);
						
		float dis = 0.1;

		glBegin(GL_POINTS);
		glVertex3f(x, y, initZ);
		glEnd();
		
}

//input selected mesh 
//draw sphere 
void drawSelection(bool grab){
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	float *center = getCenterSelection();
	glPushMatrix();
	
	if(grab){//action
		if(is_mode(1)) glColor3f(1,0,0);  //sculpting
		else glColor3f(1,1,1);
	}
	else{//no action
		if(is_mode(1)) glColor3f(0, 0, 1);	//painting
		else setGLbrushColor(getBrushColor());
	}
	float rotx = getRotX();
	glRotated(-rotx, 0, 1, 0);	//rotate around y axis

	glTranslatef(center[0]*getZoom(), center[1]*getZoom(), center[2]*getZoom());
	float eff = getEffect();
	glutSolidSphere	(eff/4, 16, 16);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}

void drawArrow(float x, float y, bool grab){

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	if(!grab) glColor3f(0.5,0.3,0.5);
	else glColor3f(0.8f,0.4f,0.8f);



	glBegin(GL_TRIANGLES);
	glVertex3f(x-0.08, y-0.08, initZ);
	glVertex3f(x-0.08, y+0.08, initZ);
	glVertex3f(x-0.15, y, initZ);
	glVertex3f(x+0.08, y-0.08, initZ);
	glVertex3f(x+0.08, y+0.08, initZ);
	glVertex3f(x+0.15, y, initZ);

	glVertex3f(x-0.07, y-0.08, initZ);
	glVertex3f(x+0.07, y-0.08, initZ);
	glVertex3f(x, y-0.15, initZ);
	glVertex3f(x-0.07, y+0.08, initZ);
	glVertex3f(x+0.07, y+0.08, initZ);
	glVertex3f(x, y+0.15, initZ);
	glEnd();

	//circle
	glTranslatef(x, y, initZ);
	glutSolidSphere	(0.05, 8, 8);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

}
