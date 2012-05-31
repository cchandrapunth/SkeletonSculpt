#include "StdAfx.h"
#include <gl/glut.h>
#include "ui.h"

#include <string.h>

static float ui_depth = 3.0;

void *font = GLUT_BITMAP_TIMES_ROMAN_24;

ui_button::ui_button(const char *name, long id, float begin_x, float begin_y, float width, float height, cb_function cb)
{
	h = height;
	w = width;

	butt_name = name;
	user_id = id;
	currently_inside = false;
	x = begin_x;
	y = begin_y;
	callback = cb;

	execute = true;
	activate = true;
	wait = 0;
}

ui_button::ui_button(const char *name, long id, float begin_x, float begin_y, cb_function cb)
{
	common_init();
	butt_name = name;
	user_id = id;
	currently_inside = false;
	x = begin_x;
	y = begin_y;
	callback = cb;

	execute = true;
	activate = true;
	wait = 0;
}

int  ui_button::hand_down_handler( int local_x, int local_y ){
	currently_inside = true;
	draw();

	return false;
}

void ui_button::reactivate(){
	activate = true;
}

void ui_button::deactivate(){
	activate = false;
}

int  ui_button::hand_up_handler( int local_x, int local_y, bool inside ){
	currently_inside = false; 
	draw();

	if ( inside ) {
		if(!execute){
			//make the button disappear after click
			if (strcmp ("Menu", butt_name) == 0){
				//don't show any buttons
				Master_ui->disable_menu();
				activate = false;

			}
			execute = true;
			
			callback();

			return true;
		}
	}
	else{
		execute = false; 
	}
	return false;
}


void ui_button::draw(){
	if(activate){
		glDisable(GL_TEXTURE_2D);
		if(currently_inside) draw_pressed();
		else {
			draw_text(0);
		}
	}
}

void ui_button::draw_pressed(){

  draw_text( 1 );
  glColor3f( 0.0, 0.0, 0.0 );
  glBegin( GL_LINE_LOOP );
  glVertex2f( x, y );         glVertex2f( x+w, y );
  glVertex2f( x+w, y+h );         glVertex2f( x, y+h );
  glEnd();

  glBegin( GL_LINE_LOOP );
  glVertex3f( x+0.01, y+0.01 , ui_depth+0.1);         glVertex3f( x+w-0.01, y+0.01 , ui_depth+0.1);
  glVertex3f( x+w-0.01, y+h-0.01 , ui_depth+0.1);     glVertex3f( x+0.01, y+h-0.01, ui_depth+0.1 );
  glEnd();
}

void ui_button::draw_text(int sunken){

  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  

  if(sunken == 1) {
	glColor3f(0.0, 0.0, 0.0);
	
	output(x+w/3, y+h/4, (char*) butt_name);
	glColor3f(1.0,1.0,1.0);
  }
  else{ //dont' wrtie name for color button
	  if (strcmp ("orange", butt_name) == 0)
		  glColor3f(255.0/255, 69.0/255, 0.0/255);
	  else if(strcmp ("green", butt_name) == 0)
		  glColor3f(107.0/255, 142.0/255, 35.0/255);
	  else if(strcmp ("blue", butt_name) == 0)
		  glColor3f(30.0/255, 144.0/255, 1.0);
	  else if(strcmp ("yellow", butt_name) == 0)
		  glColor3f(1, 215.0/255, 0);
	  else if(strcmp ("purple", butt_name) == 0)
		  glColor3f(186.0/255, 85.0/255, 211.0/255);
	  else if(strcmp ( "black", butt_name) ==0)
		  glColor3f(0, 0, 0);

	  else{
		  //write name for general button
		 
		  glColor3f(0.0, 0.0, 0.0);
		  float len = strlen(butt_name)*0.1;
		  output(x+w/3, y+h/4, (char*) butt_name);
		  glColor3f(0.3,0.3,0.3);
	  }
  }

  glBegin( GL_QUADS );
  glVertex3f( x+0.02, y+0.02, ui_depth);         glVertex3f( x+w-0.02, y+0.02, ui_depth);
  glVertex3f( x+w-0.02, y+h-0.02, ui_depth);     glVertex3f( x+0.02, y+h-0.02, ui_depth);
  glEnd();

}

void ui_button::draw_bar(float a){
  float perc = min(a/60*w, w);
  glBegin( GL_QUADS );
  glVertex3f( x+0.02, y+0.02, ui_depth+0.1);         glVertex3f( x+perc, y+0.02, ui_depth+0.1);
  glVertex3f( x+perc, y+h-0.02, ui_depth+0.1);     glVertex3f( x+0.02, y+h-0.02, ui_depth+0.1);
  glEnd();


}

void ui_button::output(float x, float y, char *string)
{
  int len, i;

  glColor3f(0,0,0);
  glRasterPos3f(x, y, ui_depth);
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(font, string[i]);
  }
}