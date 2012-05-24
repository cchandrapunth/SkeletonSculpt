#include "stdafx.h"
#include <gl/glut.h>
#include <stdio.h>
#include "XnPoint3D.h"
#include "NuiApi.h"
#include "hand_history.h"
#include "vertex.h"
#include "gesture.h"
#include "window.h"
#include "vmmodel.h"
#include "drawhand.h"
#include "Pair.h"
///////////#include "svmpredict.h"
#include "smoothing.h"
#include <map>
#include <list>


#define GESTURE_TO_USE "Click" 
#define HANDDEPTH 60 //in mm
#define HANDRADIUS 80
#define GRAB_THRESHOLD 0.45
#define m_nHistorySize 25

static XnPoint3D RpalmPos;
static XnPoint3D LpalmPos;
static bool RGRAB  = false;
static bool LGRAB  = false;

FILE *pFile1; 
FILE *pFile2;

int printDebug = 0; 
int printTraining = 0; //0 - don't output 1 - close hand, 2 - open hand 
FILE *pFile;
//int nXRes, nYRes;
static bool SHOWHAND;

const int nb = 4;
int svm_grid[2*HANDRADIUS][2*HANDRADIUS]; // size 2HANDRADIUS*2HANDRADIUS
int numpoint[nb][nb];
int len[100];	//length

static std::list<XnPoint3D> m_History;	//point history (one hand only)
static std::list<int> handId;
int prime_id = 0;
float aspect = 1; 

NUI_SKELETON_FRAME SkeletonFrame;
bool bFoundSkeleton;
const NUI_IMAGE_FRAME *imageFrame;

HRESULT hr = S_OK;
HANDLE      m_hNextDepthFrameEvent;
HANDLE		m_pDepthStreamHandle;
int* m_depthRGBX;
static const int cDepthWidth  = 640;
static const int cDepthHeight = 480;


//-----------------------------------------------------
//					   LOG
//-----------------------------------------------------
void enableDebugGesture(){

	for(int i=0; i< 2*HANDRADIUS; i++){
		for(int j=0; j< 2*HANDRADIUS; j++){
			svm_grid[i][j] = 0;
		}
	}	
}

//------------------------------------------------------
//                    HELPER
//------------------------------------------------------
void setAspect(float asp){
	aspect = asp; 
}
float getAspect(){
	return aspect;
}

void set_print_training(int i){
	printTraining = i;
}

bool hasTwoHands(){
	return handId.size() > 1 ? true: false;
}

//RIGHT HAND
bool isGrab(){ return RGRAB; }
XnPoint3D getPalm(){ return RpalmPos; }

//LEFT HAND
bool isLGrab(){ return LGRAB; }
XnPoint3D getLPalm(){ return LpalmPos; }

void switchShowHand(){
	SHOWHAND = !SHOWHAND;
		if(SHOWHAND) printf("switch hand mode to SHOW\n");
		else 	printf("switch hand mode to NOT_SHOW\n");
}

float convertX(float x){
	 
	float left = (getCenter().x- getDiam()); //get the leftmost coordinate
	//return left + (nXRes-x)*(2*getDiam())/nXRes;	//scale to the viewport

	return left+ (x*2*getDiam())/cDepthWidth;
}

float convertY(float y){
	
	float bottom = getCenter().y- getDiam(); //get the leftmost coordinate
	//return bottom + (nYRes-y)*(2*getDiam())/nYRes;
	return -(bottom + y*(2*getDiam())/cDepthHeight);
}


void set_primary(){
	/*
	if(handId.size() ==0) 
		prime_id = 0;
	if(handId.size() ==1) 
		prime_id = handId.front();
	else{
	
		int rh = 0;
		float xcoor = 100000;

		//find id of a right hand 
		std::map<int, std::list<XnPoint3D> >::const_iterator Iter;
		for(Iter = m_History.begin(); Iter != m_History.end(); ++Iter){
			int id = Iter->first;
			XnPoint3D point = Iter->second.front();

			if(point.X < xcoor){
				rh = id;
				xcoor = point.X;
			}
		}
		prime_id = rh;
	}
	*/
}

// Colors for the points
float Colors[][3] =
{
	{0.5,0.5,0.5},	// Grey
	{0,1,0},	// Green
	{0,0.5,1},	// Light blue
	{1,1,0},	// Yellow
	{1,0.5,0},	// Orange
	{1,0,1},	// Purple
	{1,1,1}		// White. reserved for the primary point
};
int nColors = 6;
/*
//-----------------------------------------------------
//					Register generator
//-----------------------------------------------------
xn::DepthGenerator* getDepthGenerator(Context ctx){
	nRetVal = g_DepthGenerator.Create(ctx);
	CHECK_RC(nRetVal, "Create depth");
	return &g_DepthGenerator;
}
xn::HandsGenerator* getHandGenerator(Context ctx){
	nRetVal = g_HandsGenerator.Create(ctx);
	CHECK_RC(nRetVal, "Create hands");
	return &g_HandsGenerator;
}
xn::GestureGenerator* getGestureGenerator(Context ctx){
	nRetVal = g_GestureGenerator.Create(ctx);
	CHECK_RC(nRetVal, "Create gesture");
	return &g_GestureGenerator;
}


//--------------------------------------------------
//					Gesture Callback
//--------------------------------------------------

//remove gesture generator and start hand generator
void XN_CALLBACK_TYPE Gesture_Recognized(GestureGenerator& generator, const XnChar* strGesture, 
	const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition,void* pCookie){
		//printf("Gesture recognized: %s\n", strGesture);
		g_GestureGenerator.RemoveGesture(strGesture);
		g_HandsGenerator.StartTracking(*pEndPosition);
}

//Process Gesture
//
void XN_CALLBACK_TYPE Gesture_Process(GestureGenerator& generator, const XnChar* strGesture, 
	const XnPoint3D* pPosition, XnFloat fProgress,void* pCookie)
{}

//Detect hand 
//Callback to be called when a new hand is created 
void XN_CALLBACK_TYPE Hand_Create(HandsGenerator& generator,XnUserID nId,const XnPoint3D* pPosition,
		XnFloat fTime, void* pCookie){

		XnPoint3D projPosition;
		g_DepthGenerator.ConvertRealWorldToProjective(1, pPosition, &projPosition);
		

		printf("New Hand: %d @ (%f, %f, %f)\n", nId, projPosition.X, projPosition.Y, projPosition.Z);

		//reset map
		m_History[nId].clear();
		handId.push_front(nId);
	
		//reset primary hand
		set_primary();

		if(nId == prime_id)
			RpalmPos = projPosition;
		else 
			LpalmPos = projPosition;

		
}
	
//Callback to be called when an existing hand has a new position
void XN_CALLBACK_TYPE Hand_Update(HandsGenerator& generator,XnUserID nId, const XnPoint3D* pPosition, 
	XnFloat fTime, void* pCookie){

		//printf("New Position: %d @ (%f, %f, %f)\n", nId, pPosition->X, pPosition->Y, pPosition->Z);
		XnPoint3D projPosition;
		g_DepthGenerator.ConvertRealWorldToProjective(1, pPosition, &projPosition);

		//store in history
		XnPoint3D* ptProjective = new XnPoint3D();
		ptProjective->X = projPosition.X;
		ptProjective->Y = projPosition.Y;
		ptProjective->Z = projPosition.Z;

		m_History[nId].push_front(*ptProjective);
		// Keep size of history buffer
		if (m_History[nId].size() > m_nHistorySize)
			m_History[nId].pop_back();

		//printf("id: %d: %f %f %f \n", (int)nId, m_History[nId].front().X, m_History[nId].front().Y, m_History[nId].front().Z  );
		//reset primary hand
		set_primary();	

		if(nId == prime_id)
			RpalmPos = projPosition;
		else 
			LpalmPos = projPosition;

}

//Lost hand
void XN_CALLBACK_TYPE Hand_Destroy(HandsGenerator& generator,XnUserID nId, XnFloat fTime,void* pCookie){

		printf("Lost Hand: %d\n", nId);
		g_GestureGenerator.AddGesture(GESTURE_TO_USE, NULL);

		m_History.erase(nId);
		handId.remove(nId);
		deleteSmHand(nId);

		//reset primary hand
		set_primary();
}
*/

//hands

/**********************************************************
						Initialize
***********************************************************/

int NUIinit()
{
	hr = NuiInitialize( NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_SKELETON | NUI_INITIALIZE_FLAG_USES_COLOR);
	if( FAILED( hr ) )
	{
		system("pause");

	}
	else
	{ 
		// m_hNextSkeletonEvent = NULL;
		//m_hNextSkeletonEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

		NUI_SKELETON_FRAME SkeletonFrame;
		NuiSkeletonTrackingEnable;

		//NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, 0 );

		hr = NuiImageStreamOpen(
			NUI_IMAGE_TYPE_DEPTH,
			NUI_IMAGE_RESOLUTION_640x480,
			0,
			2,
			m_hNextDepthFrameEvent,
			&m_pDepthStreamHandle);

		m_depthRGBX = new int[cDepthWidth*cDepthHeight];

	}

	//debuging
	pFile = fopen("depthmap.txt", "w");	
	pFile1 = fopen("newhand.txt", "w");	
	pFile2 = fopen("depthdata.txt", "w");
	return 0;
}

void extract_Skeleton(){
	NuiSkeletonGetNextFrame( 0, &SkeletonFrame );
	NuiTransformSmooth(&SkeletonFrame,NULL);		// smooth out the skeleton data

	bFoundSkeleton = false;

	for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
	{
		if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
		{
			bFoundSkeleton = true;

			//right hand
			XnPoint3D* ptProjective = new XnPoint3D();
			ptProjective->X =SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x;
			ptProjective->Y =SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y;
			ptProjective->Z =SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].z;

			//store palm pos to use as a cursor in display
			//convert to estimated pixel 
			//depthmap return [-1, 1]
			XnPoint3D* ptResolution = new XnPoint3D();
			ptResolution->X = (ptProjective->X+1)* cDepthWidth/2;
			ptResolution->Y = (ptProjective->Y+1)* cDepthHeight/2;
			ptResolution->Z = ptProjective->Z*1000; 
			RpalmPos = *ptResolution;

			if (ptProjective->Z!=0 ) 
			{
				m_History.push_front(*ptProjective);
				// Keep size of history buffer
				if (m_History.size() > m_nHistorySize)
					m_History.pop_back();

				
				fprintf(pFile1, "HandRight X=%4.2f  Y=%4.2f  Z=%4.2f  | convert %.1f %.1f \n",ptProjective->X,ptProjective->Y,ptResolution->Z, 
																	ptResolution->X, ptResolution->Y);    
			
			}
			//////////////////////////////////////////////////////////////        
			else
			{
				NuiSkeletonGetNextFrame( 0, &SkeletonFrame );
			}

		}        
	}
}

/**********************************************************
						Draw hands 
***********************************************************/

void draw_hand(XnPoint3D* handPointList)
{
	extract_Skeleton();

	//disable all lighting effect
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);	
		
	std::list<XnPoint3D> ::const_iterator PointIterator = m_History.begin();
	float* m_pfPositionBuffer;
	
	// Go over each existing hand
	//for (PointIterator = m_History.begin();PointIterator != m_History.end();++PointIterator) {
	if(bFoundSkeleton){
		// Clear buffer
		int nPoints = 0;
		int i = 0;
		//int Id = PointIterator->first;
		XnPoint3D pt= *PointIterator;

		//draw histogram		
		int hpoint = draw_map(handPointList, pt);

		//predict
		if(hpoint > 0){
			predict_gesture(handPointList, pt, hpoint);

			//set color for each person
			int nColor = 2;
			glPointSize(8);
			glColor4f(Colors[nColors][0],
				Colors[nColors][1],
				Colors[nColors][2],
				1.0f);

			//draw palm 
			drawRHand(RGRAB, pt.X*4, pt.Y*4, pt.Z);
		}
		//else use the old gesture 
		else{
			//set color for each person
			int nColor = 2;
			glPointSize(8);
			glColor4f(Colors[nColors][0],
				Colors[nColors][1],
				Colors[nColors][2],
				1.0f);

			//draw palm 
			drawRHand(RGRAB, pt.X*4, pt.Y*4, pt.Z);
		}
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}
//hand histogram
int draw_map(XnPoint3D* handPointList, XnPoint3D palm){
	
	if(palm.Z == 0) return 0;

	int nX, nY;
	int n = 0; //number of points

	//depth threshold
	int minZ = palm.Z- HANDDEPTH;
	int maxZ = palm.Z+ HANDDEPTH;

	//svm
	int svm_index = 0;
	/*
	//depth data from kinect
	xn::DepthMetaData depthMD;
	g_DepthGenerator.GetMetaData(depthMD);
	nXRes = depthMD.XRes();
	nYRes = depthMD.YRes();
	//Pointer to depthmap
	const XnUInt16* pDepth = depthMD.Data();
	XnUInt16 nValue;
	*/
	//image
	HRESULT hr = NuiImageStreamGetNextFrame(m_pDepthStreamHandle, 0, &imageFrame);
	if (FAILED(hr))
	{
		return 0;
	}

	INuiFrameTexture * pTexture = imageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	// Lock the frame data so the Kinect knows not to modify it while we're reading it
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	//set color
	glPointSize(2);
	
	if(!RGRAB) glColor3f(0, 1, 0);
	else glColor3f(1, 0.0, 0.0);
	
	XnPoint3D *p = new XnPoint3D;

	/*
	glBegin(GL_POINTS);
	//draw histogram from depthmap
	//Iterate through the pixel from top to bottom/ right to left
	for (nY= 0; nY<nYRes; nY++)
	{
		for (nX=0; nX<nXRes; nX++)
		{
			nValue = *pDepth;

			//in the bounding box only
			if((nX < palm.X+HANDRADIUS && nX > palm.X-HANDRADIUS) && 
				(nY < palm.Y+HANDRADIUS && nY > palm.Y-HANDRADIUS)){

					svm_index++;

					//the point falls inside the threshold
					if(nValue > minZ && nValue <maxZ){

						//the depth map is mirroring. As glOrtho is scaled to 0,nXRes and 0,nYRes
						//flip the coordinate using nXRes and nYRes
						//convert in relative to viewport 
						if(SHOWHAND)
							glVertex3f(convertX(nX), convertY(nY), 3.0f);

						(*p).X = nX;
						(*p).Y = nY;
						(*p).Z = nValue;
						//store the point that could be in hand region
						handPointList[n] = *p;
						n++;	

						//training data for svm
						if(printTraining){
							//add to svm_grid
							int px = nX - (int)palm.X + HANDRADIUS;
							int py = nY - (int)palm.Y + HANDRADIUS;

							svm_grid[px][py] = 1;
						}
					}
			}
			pDepth++;
		}
	}		
	delete(p);
	glEnd();
	*/

	// Make sure we've received valid data
	if (LockedRect.Pitch != 0)
	{

		int* rgbrun = m_depthRGBX;
		const USHORT * pBufferRun = (const USHORT *)LockedRect.pBits;

		// end pixel is start + width*height - 1
		const USHORT * pBufferEnd = pBufferRun + (cDepthWidth * cDepthHeight);

		float handPixelX = convertPalmPixelX(palm.X);
		float handPixelY = convertPalmPixelY(palm.Y);
		
		glPointSize(2);
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		//while ( pBufferRun < pBufferEnd )
		for (int nY= 0; nY<cDepthHeight; nY++)
		{
			for (int nX=0; nX<cDepthWidth; nX++){

				if((nY < handPixelY+HANDRADIUS && nY > handPixelY-HANDRADIUS) && 
					(nX < handPixelX+HANDRADIUS && nX > handPixelX-HANDRADIUS)){

						// discard the portion of the depth that contains only the player index
						USHORT depth = NuiDepthPixelToDepth(*pBufferRun);

						//get player index 
						USHORT player = NuiDepthPixelToPlayerIndex(*pBufferRun);

						*(rgbrun++) = depth;


						int depthint = (int) depth;
						int min = (palm.Z*1000) -50;
						int max = (palm.Z*1000) +70;


						if(depth > min && depth <  max){

							//fprintf(pFile2, "palm: %f |\t pixel= [%d, %d, %d]\n", palm.Z*1000,  nX, nY, depthint);
							//fprintf(pFile2, "*");

							//if(bFoundSkeleton){
								if(SHOWHAND)
									glVertex3f(convertX(nX), convertY(nY), 3.0f);
									//glVertex3f((((float)nX/cDepthWidth)*4)-2, -((((float)nY/cDepthHeight)*4)-2), 3.0);

								(*p).X = nX;
								(*p).Y = nY;
								(*p).Z = depth;
								//store the point that could be in hand region
			
								handPointList[n] = *p;
								n++;
							//}
						}
						//else fprintf(pFile2, "-");



						//if(player > 0) fprintf(pFile2, "*");
						//else fprintf(pFile2, "%u ", depth);

						// We're outputting BGR, the last byte in the 32 bits is unused so skip it
						// If we were outputting BGRA, we would write alpha here.
						//++rgbrun;
				}

				// Increment our index into the Kinect's depth buffer
				++pBufferRun;
				
			}
			fprintf(pFile2,"\n");
		}
	}
	glEnd();
	// We're done with the texture so unlock it
	pTexture->UnlockRect(0);
	delete(p);

	// Release the frame
	NuiImageStreamReleaseFrame(m_pDepthStreamHandle, imageFrame);
	return n;
}

//use svm prediction for gesture prediction
void predict_gesture(XnPoint3D* handPointList, XnPoint3D palm, int n){

	bool result;

	//Edge predict
	result = getEdge(handPointList, n, palm);

	if(result)fprintf(pFile, "-1\t");
	else fprintf(pFile, "1\t");
	
	//smoothing algorithm
	smoothHand(result);
	bool smooth_result = isGrabsmooth();

	//print
	if(smooth_result) {
		fprintf(pFile, "-1");
		RGRAB = true;
	}
	else {
		fprintf(pFile, "1");
		RGRAB = false;
	}
	fprintf(pFile,"\n");

}

/**********************************************************
					Machine Learning
**********************************************************/
bool find_finger(XnPoint3D* List, int nNumberOfPoints, XnPoint3D palm){
	/*
	//giftwrap technique 

	int count = 0; 
	int MAXH = 10000;

	//find lowest point
	XnPoint3D *lowest = new XnPoint3D;
	(*lowest).X = 0; (*lowest).Y = 0;
	XnPoint3D* ptr = List; 
	while(count < nNumberOfPoints){	
		unsigned int xx = ptr->X;
		unsigned int yy = ptr->Y;

		if(xx > (*lowest).X){
			lowest->X = ptr->X;
			lowest->Y = ptr->Y;
		}
		count++;
		ptr++;
	}

	
	//find convex hull
	XnPoint3D* base = new XnPoint3D;
	base = lowest;

	XnPoint3D* next = new XnPoint3D;
	XnPoint3D convexList[200];
	convexList[0].X = base->X;
	convexList[0].Y = base->Y;
	int last = 1; 
	int min = 360;
	
	//first one compare to y axis
	for(int i=0; i< nNumberOfPoints; i++){
		float a = findAngle(base->X, base->Y-5, base->X, base->Y, List[i].X, List[i].Y);
		if(a <= min && base->X != List[i].X && base->Y != List[i].Y){
			*next = List[i];
			min = a;
		}
	}
	convexList[last] = *next;
	last++;

	//the rest 
	int k=0;
	while(k < 30 ){
	
		min = 360;
		for(int i=0; i< nNumberOfPoints; i++){
			//angle between two line
			float a = findAngle(convexList[last-2].X, convexList[last-2].Y, 
				convexList[last-1].X, convexList[last-1].Y, List[i].X, List[i].Y);
			
			//find min angle. not itself
			if(a <= min && a != 0 && 
					convexList[last-1].X != List[i].X && convexList[last-1].Y != List[i].Y){
				*next = List[i];
				min = a;
			}
		} 
		convexList[last].X = next->X;
		convexList[last].Y = next->Y;
		last++;
		k++;
	}
	
	for(int i=0; i< 100; i++){
		len[i] = 0;
	}

	//distance from the center hand
	for(int n=0; n< last; n++){
		//only consider point above the central hand
			int d = (int)dis(convexList[n].X, convexList[n].Y, palm.X, palm.Y);
			len[d] +=1;
	}

	//for training
	//svm label 
	if(printTraining == 1){
		fprintf(pFile, "-1\t");	//close hand
	}else if(printTraining == 2){
		fprintf(pFile, "1\t");	//open hand
	}
	if(printTraining){
		for(int i=0; i< 100; i++){
			if(len[i]) fprintf(pFile, "%d:%d ", i, len[i]);
		}
		fprintf(pFile, "\n");
		//set_print_training(0);	 
	}

	Pair *p = (Pair *)malloc(sizeof(Pair)*100);
	int size = 0;
	for(int i=0; i< 100; i++){
		if(len[i]){
			p[size].index = i;
			p[size].value = len[i];
			size++;
		}
	}
	int predict = svm_rt_predict(p, size);
	delete(p);
	
	//draw finnger tips
	if(SHOWHAND){
		glPointSize(10);
		glColor3f(0,0,1);
		glBegin(GL_POINTS);
		for(int n=0; n< last; n++){
				glVertex3f(convertX(convexList[n].X), convertY(convexList[n].Y), 3.0);
			
		}

		glEnd();
	}	

	if(predict >0) return false;
	else return true;
	*/

	///////////////////////////////
	return false;
}


/**********************************************************
					filter method
**********************************************************/
bool getEdge(XnPoint3D* List, int nNumberOfPoints, XnPoint3D palm){

	int nX, nY;
	int count = 0; 
	int MAXH = 10000;

	//find highest, lowest, lefmost and rightmost
	XnPoint3D *highest = new XnPoint3D; 
	(*highest).X = MAXH; (*highest).Y = MAXH;
	XnPoint3D *lowest = new XnPoint3D;
	(*lowest).X = 0; (*lowest).Y = 0;
	XnPoint3D *leftmost = new XnPoint3D; 
	(*leftmost).X = MAXH; (*leftmost).Y = MAXH;
	XnPoint3D *rightmost = new XnPoint3D;
	(*rightmost).X = 0; (*rightmost).Y = 0;
	XnPoint3D* ptr = List; 

	//Find top and bottom of the hand area. 
	while(count < nNumberOfPoints){	
		unsigned int xx = ptr->X;
		unsigned int yy = ptr->Y;

		if(yy < (*highest).Y)
			highest = ptr;

		else if(yy > (*lowest).Y)
			lowest = ptr;

		if(xx < (*leftmost).X)
			leftmost = ptr;

		else if(xx > (*rightmost).X)
			rightmost = ptr;

		count++;
		ptr++;
	}
	
	/*
	glColor3f(0, 0, 0);
	glBegin(GL_POINTS);
	glVertex3f(convertX(highest->X), convertY(highest->Y), 4.1f);
	glVertex3f(convertX(lowest->X), convertY(lowest->Y), 4.1f);
	glVertex3f(convertX((highest->X+lowest->X)/2), convertY((highest->Y+lowest->Y)/2), 4.1f);
	glEnd();
	*/

	//hand length
	return estimateGrab(List, nNumberOfPoints, highest, lowest, leftmost, rightmost, palm);
}

//calculate the area half top of the hand area 
//greater than 50% => grab 
bool estimateGrab(XnPoint3D* list, int n, 
	XnPoint3D* highest, XnPoint3D* lowest, 
	XnPoint3D* leftmost, XnPoint3D* rightmost, XnPoint3D palm){
	

	int nX, nY, nom=0, denom=0; 

	//onvert from [-1, 1] to pixel estimation
	int y = (int) convertPalmPixelY(palm.Y);
	int xr = (int)(*rightmost).X; 
	int xl = (int)(*leftmost).X; 
	int yh = (int)(*highest).Y;

	if(printDebug){
		fprintf(pFile, "top = %d,%d ---> %d,%d\n", xr,yh,xl,yh );
		fprintf(pFile, "bottom = %d,%d ---> %d,%d\n", xr,y,xl,y);
	}

	XnPoint3D* ptr = list;
	int count = 0;
	//draw bonding box 
	// (max) xr,yh----------------xl, yh 
	//		 |						|
	//		 |						|
	//		 xr,y-------x,y-------xl, y
	
	
	while (count < n){
		int xx = ptr->X;
		int yy = ptr->Y;
		//fprintf(pFile,"%d,%d ", xx, yy);
		
		if(xx < xr && xx> xl && yy < y && yy > yh) nom++;
		count++;
		ptr++;
	}
	denom = (xr-xl)*(y-yh);
	float percent =  ((float)nom)/denom;
	if(denom != 0 && printDebug) {
		fprintf(pFile, "estimate hand point = %d, %d, %f\n", nom, denom, ((float)nom)/denom);
	}

	//estimate hand area > 0.5 => grab
	if (percent > GRAB_THRESHOLD) return true; 
	else return false;

}

/*******************************************************
				Helper Function
********************************************************/

//angle from x-axis to vector x2,y2 -> x1,y1
float findAngle(float x1, float y1, float x2, float y2, float x3, float y3){
	float PI = 3.14159265;
	float angle1= atan2((y2-y1),(x2-x1));	//vector 2->1
	float angle2= atan2((y2-y3),(x2-x3));	//vector 2->3

	float anglebtw= (angle1-angle2)*(180/PI);
	if(anglebtw < 0){
		anglebtw+=360;
	}
	return anglebtw;
}

float dis(float x1, float y1, float x2, float y2){
	return sqrt(pow(x1-x2, (float)2.0) +pow(y1-y2, (float) 2));
}


float convertPalmPixelX(float px){
		return (px+0.5)* cDepthWidth;
}

float convertPalmPixelY(float py){
	return cDepthHeight- ((py+0.5)* cDepthHeight);
}