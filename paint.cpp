#include "stdafx.h"
#include <GL/glut.h>
#include "paint.h"

int pColor = 2;

int getBrushColor(){return pColor;}
void setBlack(){pColor = 1;}
void setRed(){pColor = 2;}
void setGreen(){pColor = 3;}
void setBlue(){pColor = 4;}
void setYellow(){pColor = 5;}
void setWhite(){pColor = 6;}

void setGLbrushColor(int cid){
	
	switch(cid){
	case 1://black
		glColor3f(0,0,0);
		break;
	case 2://red
		glColor3f(255.0/255, 69.0/255, 0.0/255);
		break;
	case 3://green
		glColor3f(107.0/255, 142.0/255, 35.0/255);
		break;
	case 4://blue
		glColor3f(30.0/255, 144.0/255, 1.0);
		break;
	case 5://yellow
		glColor3f(1.0, 215.0/255, 0);
		break;
	case 6://white
		glColor3f(186.0/255, 85.0/255, 211.0/255);
		break;
	case 7://grey
		glColor3f(0.3, 0.3, 0.3);
		break;
	}

}