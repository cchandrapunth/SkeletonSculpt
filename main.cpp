#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "NuiApi.h"
#include <GL/glut.h>


float hand_l_x =0, hand_l_y =0, hand_l_z =0;
float hand_r_x =0, hand_r_y =0, hand_r_z =0;
FILE *pFile1;
FILE *pFile2;

bool bFoundSkeleton = false;
int* m_depthRGBX;
HANDLE		m_pDepthStreamHandle;
const NUI_IMAGE_FRAME *imageFrame;
NUI_SKELETON_FRAME SkeletonFrame;

static const int w = 800;
static const int h = 800;
static const int cDepthWidth  = 640;
static const int cDepthHeight = 480;


float z = -2.5;
float bd = 1.9;

void draw_rect(float x1, float y1, float x2, float y2){
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1, z-0.1);
	glVertex3f(x2, y1, z-0.1);
	glVertex3f(x2, y2, z-0.1);
	glVertex3f(x1, y2, z-0.1);
	glEnd();
}

void draw_line(float ax,float ay, float bx, float by){
	glBegin(GL_LINES); 
    glVertex3f( ax, ay, z); 
    glVertex3f( bx, by, z); 
    glEnd(); 
}
void draw_background(){
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND); 
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glColor3f(0.5f, 0.5f, 0.5f);
    draw_rect(-(bd+0.1), bd+0.1, bd+0.1, -(bd+0.1));

	//scale
	glColor3f(0.1, 0.1, 0.1);
	draw_line(0, bd, 0, -bd);
	draw_line(bd, 0, -bd, 0);

	//grid
	glColor3f(0.6, 0.6, 0.6);
	for(int i=0; i<20;i ++){		
		draw_line(-bd+(0.2*i), bd, -bd+(0.2*i), -bd);
		draw_line(bd, -bd+(0.2*i), -bd, -bd+(0.2*i));
	}
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);//line shows more
}



void renderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	draw_background();
	glLoadIdentity();

	NuiSkeletonGetNextFrame( 0, &SkeletonFrame );

	// smooth out the skeleton data
	NuiTransformSmooth(&SkeletonFrame,NULL);
	// we found a skeleton, re-start the timer
	//NuiSkeletonTrackingEnable;


	bFoundSkeleton = false;

	for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )//////////////////hier werden mehrere Skeletons abgefragt!!!!!!!!!!!!!!!
	{

		////////////////////////hier wird nur ein skeleton abgefragt!!!!!!!
		if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )////////////////pr�ft ob skeletondaten verf�gbar sind
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
		glColor3f(0, 1, 0);
		glBegin(GL_POINTS);
		glVertex3f(handPixelX/cDepthWidth*4 -2, -(handPixelY/cDepthHeight*4 -2), 0);
		glEnd();
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
						*/

						*(rgbrun++) = depth;


						int depthint = (int) depth;
						int min = (hand_r_z*1000) -70;
						int max = (hand_r_z*1000) +50;


						if(depth > min && depth <  max){

							fprintf(pFile2, "%f [%d, %d] [%.2f, %.2f] | %d\n", hand_r_z*1000,  nX, nY, handPixelX, handPixelY, depthint);

							if(bFoundSkeleton) glVertex3f((((float)nX/cDepthWidth)*4)-2, -((((float)nY/cDepthHeight)*4)-2), 0);

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
		glVertex3f(hand_r_x*4, hand_r_y*4, 0);
		glEnd();
	}
	glutSwapBuffers();
	glFlush();
}


int x;
int null = 0;
int m_eins = 1; 
HRESULT hr = S_OK;
int nCount = 0;


HANDLE        m_hNextDepthFrameEvent;
HANDLE        m_hNextVideoFrameEvent;
HANDLE        m_hNextSkeletonEvent;

HANDLE        m_pVideoStreamHandle;


/**********************************************************
						Initialize
***********************************************************/
int NUIinit()
{
	hr = NuiInitialize( NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_SKELETON | NUI_INITIALIZE_FLAG_USES_COLOR);
	if( FAILED( hr ) )
	{
		//printf ("nicht initialisiert!! KINECT verbunden? \n");
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

	pFile1 = fopen("newhand.txt", "w");	
	pFile2 = fopen("depthmap.txt", "w");

	return 0;
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


/******************************************************
					MAINLOOP
******************************************************/
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	NUIinit();
	initRender();

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 0);
	glutInitWindowSize(w,h);
	glutCreateWindow("hello");

	glutReshapeFunc(reshape);
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutMainLoop();
                      
	return 0;
}