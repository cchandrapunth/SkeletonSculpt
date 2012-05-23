#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "SkeletonBasics.h"

FILE *pFile1;


float hand_l_x =0, hand_l_y =0, hand_l_z =0;
float hand_r_x =0, hand_r_y =0, hand_r_z =0;
CSkeletonBasics application;
HINSTANCE hIns;
int nCmd;
NUI_SKELETON_FRAME SkeletonFrame;

void renderScene(void) {


glClear(GL_COLOR_BUFFER_BIT);
glLoadIdentity();
//glRotatef(n, 0.0f, 1.0f, 0.0f);
/*
glBegin(GL_TRIANGLES);
glVertex3f(-0.5,-0.5,0.0);
glVertex3f(0.5,0.0,0.0);
glVertex3f(0.0,0.5,0.0);
glEnd();
*/

glColor3f(1, 1, 1);
glPointSize(8);
glBegin(GL_POINTS);
glVertex3f(hand_l_x*2, hand_l_y*2, 0);
glVertex3f(hand_r_x*2, hand_r_y*2, 0);
glEnd();

glFlush();



		NuiSkeletonGetNextFrame( 0, &SkeletonFrame );


		bool bFoundSkeleton = true;
		for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )//////////////////hier werden mehrere Skeletons abgefragt!!!!!!!!!!!!!!!
		{

			////////////////////////hier wird nur ein skeleton abgefragt!!!!!!!
			if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )////////////////prüft ob skeletondaten verfügbar sind
			{
				//bFoundSkeleton = false;

				//printf(" Found skeleton");


				hand_l_x=SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].x;
				hand_l_y=SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].y;
				hand_l_z=SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].z;    


				//right hand
				hand_r_x=SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x;
				hand_r_y=SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y;
				hand_r_z=SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].z;
				
				//if (hand_l_x!=0 && hand_l_y!=0) 
				if (hand_l_x!=0 && hand_r_z!=0 ) 
				{
					//printf("HandLeft X=%4.2f    ",hand_l_x,"HandLeft Y=%4.2f    ",hand_l_y,"HandLeft Z=%4.2f    \r\n",hand_l_z);
					//printf("HandRight X=%4.2f    ",hand_r_x,"HandRight Y=%4.2f    ",hand_r_y,"HandRight Z=%4.2f    \r\n",hand_r_z);    
					//fprintf(pFile, "HandLeft X=%4.2f    ",hand_l_x,"HandLeft Y=%4.2f    ",hand_l_y,"HandLeft Z=%4.2f    \r",hand_l_z);
					fprintf(pFile1, "HandRight X=%4.2f   HandRight Y=%4.2f  HandRight Z=%4.2f    \n",hand_r_x,hand_r_y,hand_r_z);    

				}

				//////////////////////////////////////////////////////////////        
				else
				{

					NuiSkeletonGetNextFrame( 0, &SkeletonFrame );
				}

			}        
		}

		// no skeletons!
		//
		if( bFoundSkeleton )
		{
			//    printf("NOT FOUND");
			//system("pause");
			//return 0;
		}

		// smooth out the skeleton data
		NuiTransformSmooth(&SkeletonFrame,NULL);
		// we found a skeleton, re-start the timer
		//NuiSkeletonTrackingEnable;

	

}

/*
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
hIns = hInstance;
nCmd = nCmdShow;

//glutInit(&argc, argv);
//application.CreateFirstConnected();

glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);

glutInitWindowPosition(100,100);

glutInitWindowSize(320,320);

glutCreateWindow("hello");

glutDisplayFunc(renderScene);

glutIdleFunc(renderScene);
glutMainLoop();



}
*/

int x;
int null = 0;
int m_eins = 1; 
HRESULT hr = S_OK;
int nCount = 0;


HANDLE        m_hNextDepthFrameEvent;
HANDLE        m_hNextVideoFrameEvent;
HANDLE        m_hNextSkeletonEvent;
HANDLE        m_pDepthStreamHandle;
HANDLE        m_pVideoStreamHandle;



int connect()
{
	hr = NuiInitialize( NUI_INITIALIZE_FLAG_USES_SKELETON);
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
	}

	pFile1 = fopen("newhand.txt", "w");	


	return 0;
}


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	connect();

	
	//hr = NuiSkeletonGetNextFrame( 0, &SkeletonFrame );
	
	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);

glutInitWindowPosition(100,100);

glutInitWindowSize(320,320);

glutCreateWindow("hello");

glutDisplayFunc(renderScene);

glutIdleFunc(renderScene);
glutMainLoop();
	
	






	system ("pause");                        
	return 0;
}