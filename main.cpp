#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "SkeletonBasics.h"

FILE *pFile1;
FILE *pFile2;
bool bFoundSkeleton = false;
float hand_l_x =0, hand_l_y =0, hand_l_z =0;
float hand_r_x =0, hand_r_y =0, hand_r_z =0;
CSkeletonBasics application;
HINSTANCE hIns;
HANDLE                  m_pDepthStreamHandle;
int nCmd;
const NUI_IMAGE_FRAME *imageFrame;
NUI_SKELETON_FRAME SkeletonFrame;
int* m_depthRGBX;

    static const int        cDepthWidth  = 640;
    static const int        cDepthHeight = 480;
    static const int        cBytesPerPixel = 1;
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









		NuiSkeletonGetNextFrame( 0, &SkeletonFrame );

		// smooth out the skeleton data
		NuiTransformSmooth(&SkeletonFrame,NULL);
		// we found a skeleton, re-start the timer
		//NuiSkeletonTrackingEnable;


		bFoundSkeleton = false;

		for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )//////////////////hier werden mehrere Skeletons abgefragt!!!!!!!!!!!!!!!
		{

			////////////////////////hier wird nur ein skeleton abgefragt!!!!!!!
			if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )////////////////prüft ob skeletondaten verfügbar sind
			{
				bFoundSkeleton = true;

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
					
					fprintf(pFile1, "HandRight X=%4.2f  Y=%4.2f  Z=%4.2f  | convert %.1f %.1f \n",hand_r_x,hand_r_y,hand_r_z*1000, (hand_r_x+1)* cDepthWidth/2, (hand_r_y+ 1)* cDepthHeight/2);    

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
		if(!bFoundSkeleton )
		{
			//printf("NOT FOUND");
			//system("pause");
			//return 0;
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
							int min = (hand_r_z*1000) -50;
							int max = (hand_r_z*1000) +50;


							if(depth > min && depth <  max){

								fprintf(pFile2, "%f [%d, %d] [%.2f, %.2f] | %d\n", hand_r_z*1000,  nX, nY, handPixelX, handPixelY, depthint);
								
								glVertex3f((((float)nX/cDepthWidth)*4)-2, -((((float)nY/cDepthHeight)*4)-2), 0);

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



		glColor3f(1, 1, 1);
		glPointSize(8);
		glBegin(GL_POINTS);
		glVertex3f(hand_l_x*4, hand_l_y*4, 0);
		glVertex3f(hand_r_x*4, hand_r_y*4, 0);
		glEnd();
		glFlush();
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

HANDLE        m_pVideoStreamHandle;



int connect()
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

		m_depthRGBX = new int[cDepthWidth*cDepthHeight*cBytesPerPixel];
		
	}

	pFile1 = fopen("newhand.txt", "w");	
	pFile2 = fopen("depthmap.txt", "w");

	return 0;
}

void reshape(int w, int h){

	glViewport(0, 0, w, h); 

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); 

	glOrtho(-2, 2, -2, 2,-5, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	connect();

	
	//hr = NuiSkeletonGetNextFrame( 0, &SkeletonFrame );
	
	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);

glutInitWindowPosition(100,100);

glutInitWindowSize(cDepthWidth,cDepthHeight);

glutCreateWindow("hello");

glutReshapeFunc(reshape);

glutDisplayFunc(renderScene);

glutIdleFunc(renderScene);
glutMainLoop();
	
	






	system ("pause");                        
	return 0;
}