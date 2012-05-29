#include "stdafx.h"

#include <GL/glut.h>
#include "NuiApi.h"
#include "XnPoint3D.h"
#include "picking.h"
#include "gesture.h"
#include "hand_history.h"
#include "window.h"
#include "undo.h"
#include "ui.h"
#include "vertex.h"
#include "vmmodel.h"
#include "drawmodel.h"
#include "paint.h"
#include "drawbackground.h"
#include "mode.h" 
#include "display.h"

#include <sapi.h>
#include <sphelper.h>
#include "print_screen.h"

//float hand_l_x =0, hand_l_y =0, hand_l_z =0;
//float hand_r_x =0, hand_r_y =0, hand_r_z =0;

ui *Master_ui =new ui();
bool selection = false;
bool preview = false;
static XnPoint3D *handPointList;

static const int w = 800;
static const int h = 800;

#define MAXPOINT 30000

//viewport
float zNear;
float zFar; 
GLdouble left; 
GLdouble right;
GLdouble bottom; 
GLdouble top; 

hand_h* rhand;
hand_h* lhand;

//signature
void UIhandler();


void display(void) {

	if(handPointList == NULL){
		printf("error. can't allocate memory for handPointList");
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(!preview){
		draw_background();

		glLoadIdentity();
		UIhandler(); //check ui touch


		//display
		//printscr();
		mode_selection(handPointList, rhand);

	}else{
		preview_scene();
	}

	glFlush();
}

void initRender(){
	GLfloat lmodel_ambient[] = {0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_specular[] = {0.8, 0.8, 0.8, 1.0};
	GLfloat diffuseMaterial[4] = {0.4, 0.4, 0.4, 1.0};
	GLfloat diffuse[] = {1.0,1.0,1.0,1.0};
	GLfloat light_position[] = {1.0, 2.0, 1, 1.0};
	GLfloat light_position1[] = {-1.0, 2.0, 1, 1.0};
	GLfloat light_position2[] = {0.0, -0.2, 8, 1.0};

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 20.0);

	//light0-1
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);	
	

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	//new
	import_vm();
	copy_vmmodel();
	findBoundingSphere();

	handPointList = new XnPoint3D[MAXPOINT];
	rhand = new hand_h();
	lhand = new hand_h();

	glEnable(GL_NORMALIZE);			//automatically rescale normal when transform the surface

}
/**********************************************************
				OPENGL routine
***********************************************************/
void reshape(int w, int h){


	glViewport(0, 0, w, h); 

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); 

	glOrtho(-2, 2, -2, 2,-5, 100);
	
	gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/***********************************************************
					Mouse & Key control
************************************************************/

void mouse(int button, int state, int x, int y){

	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

	//only when the left button is clicked 
	set_cursor(x, y);
	set_state(1);
}

void processNormalKeys(unsigned char key, int x, int y){
	
	if(key ==27){			//'esc' to exit
		exit(0);
	}
	else if(key == 104){	//'h' to show handmap or palmpoint
		switchShowHand();
	}
	else if(key == 100){	//'d' to show front buffer or back bufferr
		switch_buffer();
		if(get_buffer()) printf("switch buffer to front\n");
		else 	printf("switch buffer to back\n");
	}
	else if(key == 111) {//'o' to train value = open hand
		//////////////set_print_training(2);
		// grab
		set_print_training(true);
	}

	else if(key == 112) {// 'p' to train value = close hand
		///////////////set_print_training(1);
		// no grab
		set_print_training(false);
	}
	else if(key == 49){ //'1' for rotate X
			commitScene(-2, 0, 0);
			recalNormal();
	}
	else if(key == 50){ //'2' for rotate y
			commitScene(2, 0, 0);
			recalNormal();
	}
	else if(key == 51){ //'3' for line effect
			commitScene(0, -2, 0);
			recalNormal();
	}
	else if(key == 52){ //'4' for line effect
			commitScene(0, 2, 0);
			recalNormal();
	}
	else if(key == 53){ //'5' for line effect
			switchLine();
	}
	else if(key == 54){
		preview = !preview;
	}
	else
		printf("key: %d\n", key);
}

/**********************************************************
				MENU
**********************************************************/
//-----------------push menu-------------------
//sculpting
void option1(){
	set_mode(1);
	Master_ui->remove_menu();
}
//paint brush
void option2(){
	set_mode(2);
	Master_ui->remove_menu();

	float width = right -left;
	float height = top -bottom; 
	float off = height/80;

	Master_ui->add_button("black", left+ width/15, top-height/12, width/12, height/12-off, setBlack);	
	Master_ui->add_button("orange", left+ width/15, top-height*2/12, width/12, height/12-off, setRed);	
	Master_ui->add_button("yellow", left+ width/15, top-height*3/12, width/12, height/12-off, setYellow);	
	Master_ui->add_button("green", left+ width/15, top-height*4/12, width/12, height/12-off, setGreen);	
	Master_ui->add_button("blue", left+ width/15, top-height*5/12, width/12, height/12-off, setBlue);	
	Master_ui->add_button("purple", left+ width/15, top-height*6/12, width/12, height/12-off, setWhite);	

}
//selection?
void option3(){
	set_mode(3);
	Master_ui->remove_menu();
}

void up(){
	if(is_mode(1)) upEffect();
	else upBrush();
}

void down(){
	if(is_mode(1)) downEffect();
	else downBrush();
}

//reload model
void reload(){
	import_vm();
}

void UIhandler(){
	Master_ui->check_click(((getPalm().X/800)*4)-2, (getPalm().Y/800)*4-2);
}

void push_menu(){
	//draw panel 
	Master_ui->activate_menu = true; //addpanel
	printf("pushing menu\n");

	float width = right -left;
	float height = top -bottom; 
	float off = width/20;

	Master_ui->add_button("Sculpt", left+ width*1/4, bottom+height/3, width/4-off, height/3, option1);
	Master_ui->add_button("Paint", left+ width*2/4, bottom+height/3, width/4-off, height/3, option2);
	//Master_ui->add_button("Slice", left+ width*3/5, bottom+height/3, width/5-off, height/3, option3);
}

//all ui in here
void uiInit(){

	vertex c = getCenter();
	float diam = getDiam();

	zNear = -5;
    zFar = 100;
	left = c.x - diam;
    right = c.x + diam;
    bottom = c.y - diam;
    top = c.y + diam;

	//main menu button
	Master_ui->add_button("Menu", left+(right-left)/15, bottom+0.5, 0.5, 0.3, push_menu);
	Master_ui->add_button("-", left+(right-left)/15, bottom+0.8, 0.4, 0.3, down);
	Master_ui->add_button("+", left+(right-left)/15, bottom+1.1, 0.4, 0.3, up);

	//Master_ui->add_button("reset", right-(right-left)/5, bottom+0.5, 0.5, 0.3, reload);	//if remove, fix ui.cpp (count)
	//Master_ui->add_button("select", right-(right-left)/5, bottom+0.8, 0.5, 0.3, selectionMode);
	Master_ui->add_button("rotate", right-(right-left)/5, bottom+0.8, 0.5, 0.3, activate_rotate);
	Master_ui->add_button("undo", right-(right-left)/5, bottom+1.1, 0.5, 0.3, undo_vmmodel);
}

/***********************************************************
					MAINLOOP
************************************************************/
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 0);
	glutInitWindowSize(w,h);
	glutCreateWindow("hello");

	glutKeyboardFunc(processNormalKeys);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(mouse);

	NUIinit();
	initRender();
	uiInit();
	createGLUTMenus();
	glutMainLoop();

	
	return 0;
}