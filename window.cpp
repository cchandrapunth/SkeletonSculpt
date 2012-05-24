
#include "stdafx.h"
#include <GL/glut.h>

#include "window.h"
#include "undo.h"
#include "vertex.h"
#include "vmmodel.h"

#define RED 1
#define GREEN 2
#define BLUE 3
#define ORANGE 4


void createGLUTMenus() {

	int menu;

	// create the menu and
	// tell glut that "processMenuEvents" will
	// handle the events
	menu = glutCreateMenu(processMenuEvents);

	//add entries to our menu
	glutAddMenuEntry("SAVE",RED);
	glutAddMenuEntry("Undo",BLUE);
	glutAddMenuEntry("Green",GREEN);
	glutAddMenuEntry("Orange",ORANGE);

	// attach the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


void processMenuEvents(int option) {

	switch (option) {
		case RED :
			printf("Save the model to modeloutput.txt\n"); 
			export_vm();
			break;
		case BLUE :
			printf("Undo action"); 
			undo_vmmodel();
			break;
		case GREEN :
			break;
		case ORANGE :
			break;
	}
}

