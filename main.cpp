#include "stdafx.h"

#include <GL/glut.h>
#include "NuiApi.h"
#include "XnPoint3D.h"
#include "picking.h"
#include "gesture.h"
#include "hand_history.h"
#include "window.h"
#include "undo.h"
#include "vertex.h"
#include "vmmodel.h"
#include "drawmodel.h"
#include "paint.h"
#include "drawbackground.h"
#include "mode.h" 
#include "display.h"
//float hand_l_x =0, hand_l_y =0, hand_l_z =0;
//float hand_r_x =0, hand_r_y =0, hand_r_z =0;


bool selection = false;
bool preview = false;
static XnPoint3D *handPointList;






static const int w = 800;
static const int h = 800;


#define MAXPOINT 30000

hand_h* rhand;
hand_h* lhand;

void NUIhand(){
	/*
	NuiSkeletonGetNextFrame( 0, &SkeletonFrame );

	// smooth out the skeleton data
	NuiTransformSmooth(&SkeletonFrame,NULL);
	// we found a skeleton, re-start the timer
	//NuiSkeletonTrackingEnable;


	bFoundSkeleton = false;

	for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
	{

		if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
		{
			bFoundSkeleton = true;


			//right hand
			hand_r_x=SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x;
			hand_r_y=SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y;
			hand_r_z=SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].z;

			//if (hand_l_x!=0 && hand_l_y!=0) 
			if (hand_r_z!=0 ) 
			{
				fprintf(pFile1, "HandRight X=%4.2f  Y=%4.2f  Z=%4.2f  | convert %.1f %.1f \n",hand_r_x,hand_r_y,hand_r_z*1000, (hand_r_x+1)* cDepthWidth/2, (hand_r_y+ 1)* cDepthHeight/2);    
			}

			//////////////////////////////////////////////////////////////        
			else
			{
				NuiSkeletonGetNextFrame( 0, &SkeletonFrame );
			}

		}        
	}
	*/
	/*
	//image
	HRESULT hr = NuiImageStreamGetNextFrame(m_pDepthStreamHandle, 0, &imageFrame);
	if (FAILED(hr))
	{
		return;
	}

	INuiFrameTexture * pTexture = imageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;

	// Lock the frame data so the Kinect knows not to modify it while we're reading it
	pTexture->LockRect(0, &LockedRect, NULL, 0);
	// Make sure we've received valid data
	if (LockedRect.Pitch != 0)
	{

		int* rgbrun = m_depthRGBX;
		const USHORT * pBufferRun = (const USHORT *)LockedRect.pBits;

		// end pixel is start + width*height - 1
		const USHORT * pBufferEnd = pBufferRun + (cDepthWidth * cDepthHeight);

		float handPixelX = (hand_r_x+0.5)* cDepthWidth;
		float handPixelY = cDepthHeight- ((hand_r_y+0.5)* cDepthHeight);
		int HANDRADIUS = 80; 
		
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
						/*
						// to convert to a byte we're looking at only the lower 8 bits
						// by discarding the most significant rather than least significant data
						// we're preserving detail, although the intensity will "wrap"
						BYTE intensity = static_cast<BYTE>(depth % 256);


						// Write out blue byte
						*(rgbrun++) = intensity;

						// Write out green byte
						*(rgbrun++) = intensity;

						// Write out red byte
						*(rgbrun++) = intensity;
						

						*(rgbrun++) = depth;


						int depthint = (int) depth;
						int min = (hand_r_z*1000) -70;
						int max = (hand_r_z*1000) +50;


						if(depth > min && depth <  max){

							fprintf(pFile2, "%f [%d, %d] [%.2f, %.2f] | %d\n", hand_r_z*1000,  nX, nY, handPixelX, handPixelY, depthint);

							if(bFoundSkeleton) glVertex3f((((float)nX/cDepthWidth)*4)-2, -((((float)nY/cDepthHeight)*4)-2), 3.0);

						}



						//if(player > 0) fprintf(pFile2, "*");
						//else fprintf(pFile2, "%u ", depth);

						// We're outputting BGR, the last byte in the 32 bits is unused so skip it
						// If we were outputting BGRA, we would write alpha here.
						//++rgbrun;
				}
				// Increment our index into the Kinect's depth buffer
				++pBufferRun;

			}
		}

		// Draw the data 
		//m_pDrawDepth->Draw(m_depthRGBX, cDepthWidth * cDepthHeight * cBytesPerPixel);
	}
	glEnd();
	// We're done with the texture so unlock it
	pTexture->UnlockRect(0);

	// Release the frame
	NuiImageStreamReleaseFrame(m_pDepthStreamHandle, imageFrame);

	if(bFoundSkeleton){
		glColor3f(1, 1, 1);
		glPointSize(8);
		glBegin(GL_POINTS);
		glVertex3f(hand_r_x*4, hand_r_y*4, 3.0);
		glEnd();
	}
	//glutSwapBuffers();
	glFlush();
	*/	
}

void display(void) {

	if(handPointList == NULL){
		printf("error. can't allocate memory for handPointList");
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(!preview){
		draw_background();

		glLoadIdentity();
		//UIhandler(); //check ui touch

		//display
		//NUIhand();
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
	}

	else if(key == 112) {// 'p' to train value = close hand
		///////////////set_print_training(1);
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
	createGLUTMenus();
	glutMainLoop();
                      
	return 0;
}