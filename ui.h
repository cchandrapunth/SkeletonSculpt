
#include "ui_button.h"

#pragma once
#define MAX_LIST 10

///define type of the function that take no arguments, and return void
//this is to pass the function around for callback
typedef void (*cb_function)();

class ui{
public:

	ui_button** button_list; //list of pointer to button
	int count;	//size of the list
	bool activate_menu;

	ui(void);
	void add_button(const char* name, float x, float y, cb_function cb);
	void add_button(const char* name, float x, float y, float ww, float hh, cb_function cb);
	void draw();
	void check_click(float hand_x, float hand_y);

	void remove_menu();
	void disable_menu();

	void add_panel();

private:
	void draw_circle(float center_x, float center_y, int angle);
};

extern ui* Master_ui;