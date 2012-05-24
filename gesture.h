
//OpenNI and NITE setting for the scene


void enableDebugGesture();

//-----------------------------------------
//			error checking
//-----------------------------------------
#define CHECK_RC(rc, what)										\
if(rc != XN_STATUS_OK){											\
	printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
	exit(1);													\
}			

int NUIinit();
//read hand area 
void draw_hand(XnPoint3D* handPointList);
int draw_map(XnPoint3D* handPointList, XnPoint3D m_palm);
void predict_gesture(XnPoint3D* handPointList, XnPoint3D palm, int n);

bool getEdge(XnPoint3D* List, int nNumberOfPoints, XnPoint3D palm);

//helper
bool isGrab();
XnPoint3D getPalm();
bool isLGrab();
XnPoint3D getLPalm();

void switchShowHand();
bool estimateGrab(
	XnPoint3D* list, 
	int n, XnPoint3D* highest, 
	XnPoint3D* lowest, 
	XnPoint3D* leftmost, 
	XnPoint3D* rightmost, 
	XnPoint3D palm);

bool find_finger(XnPoint3D* List, int nNumberOfPoints, XnPoint3D palm);
float findAngle(float x1, float y1, float x2, float y2, float x3, float y3);

float convertX(float x);
float convertY(float y);
float convertPalmPixelX(float px);
float convertPalmPixelY(float py);

void set_print_training(int i);
void setAspect(float asp);
float getAspect();

///helper
bool hasTwoHands();
float dis(float x1, float y1, float x2, float y2);