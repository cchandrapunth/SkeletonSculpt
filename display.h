


void mode_selection(XnPoint3D* handPointList, hand_h* rhand);
void checkRCursor(int func, hand_h* rhand);
void checkLCursor(hand_h* lhand);
bool isRotate();

//screen resolution
void set_cursor(float x, float y);

//helper
void rotate();
void switch_buffer();
bool get_buffer();
void activate_rotate();
float* getCursor();
void drawSelection();

//preview 
void preview_scene();

//return where the hand is [pixel]
float* getCursor();