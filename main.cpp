//////////////////////////////////////////////////////////////////////////
//  Contributors License Agreement
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//    By downloading, copying, installing or using the software you agree 
//    to this license.
//    If you do not agree to this license, do not download, install,
//    copy or use the software.
//
//  Copyright (c) 2015, vLava, balaDin.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met: 
//
//	   * Redistributions of source code must retain the above copyright
//  	 notice, this list of conditions and the following disclaimer. 
//	   * Redistributions in binary form must reproduce the above copyright
//  	 notice, this list of conditions and the following disclaimer in the
//		 documentation and/or other materials provided with the distribution.
//	   * Neither the name of the National Chiao-Tung University nor the
//		 names of its contributors may be used to endorse or promote products
//		 derived from this software without specific prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//	DISCLAIMED. IN NO EVENT SHALL vLava BE LIABLE FOR ANY
//	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//////////////////////////////////////////////////////////////////////////



 #include "opencv2/objdetect/objdetect.hpp"
 #include "opencv2/highgui/highgui.hpp"
 #include "opencv2/imgproc/imgproc.hpp"

 #include <iostream>
 #include <stdio.h>

 using namespace std;
 using namespace cv;

 typedef struct Alarm
 {
    int status;
    int trigger;   
 }Alarm;

 /** Function Headers */
void ETDetectionInit( Mat frame );
void detectAndDisplayAndAlarm( Mat frame );

 /** Global variables */
 String pedestrian_cascade_name = "haarcascade_mcs_upperbody.xml";
 String logo_cascade_name = "haarcascade_EELogo.xml";
 CascadeClassifier pedestrian_cascade;
 CascadeClassifier logo_cascade;
 Alarm PedAlarm;
 Alarm EELogoAlarm;
 int LogoAlramBuffer;
 
 int EEInitFlag = 0;
 int EELogoNum = 0;
 int EELogoNum_TMP = 0;
 int mode = 1;
 int pauseFlag = 0;

 string window_name = "Capture - YO detection";
 RNG rng(12345);

 /** @function main */
 int main( int argc, const char** argv )
 {
   CvCapture* capture;
   Mat frame;

   //-- 1. Load the cascades
   if( !pedestrian_cascade.load( pedestrian_cascade_name ) ){ printf("--(!)Error loading4\n"); return -1; };
   if( !logo_cascade.load( logo_cascade_name ) ){ printf("--(!)Error loading4\n"); return -1; };

   //-- 2. Read the video stream
   capture = cvCaptureFromCAM( 0 );

   if( capture )
   {
     while( true )
     {
       frame = cvQueryFrame( capture );
       
       //-- 3. Apply the classifier to the frame
       if( !frame.empty() )
       { 
         //Initialize logo number

         //Detection and Alarm  
         detectAndDisplayAndAlarm( frame );
       }
       else
       { printf(" --(!) No captured frame -- Break!"); break; }
       

       //If a new pedestrian is detected, send alarm msg to parse server
       if(PedAlarm.trigger==1 && PedAlarm.status==1)
       {
            printf("new pedestrian is detected!!\n");
       }

       //If a new pedestrian is detected, send alarm msg to parse server
       if(EELogoAlarm.trigger==1 && EELogoAlarm.status==1)
       {
            printf("logo number is changed!!\n");
       }

       int c = waitKey(10);
       if( (char)c == 'c' ) { break; }
       else if ((char)c == 'i')//Initialize Logo number
       {
           EELogoNum = EELogoNum_TMP;
           EEInitFlag=1;
           printf("YO Init %d\n", EELogoNum);
       }
	   else if((char)c == '1')//Pedestrian detection
	   {
			mode = 1;
			printf("Pedestrian detection!!\n");
	   }
	   else if((char)c == '2')//Logo detection
	   {
			mode = 2;
			printf("Logo detection!!\n");
	   }
	   else if((char)c == '3')//Logo detection, something invade
	   {
			mode = 3;
			printf("Logo detection, something invade!!\n");
	   }
	   else if((char)c == 'p')//Pause
	   {
			if(pauseFlag == 1)
			{
				mode = 1;
				printf("continue!!   pedestrian detection starts!!\n");
				pauseFlag = 0;
			}
			else
			{	
				mode = 0;
				pauseFlag = 1;
				printf("pause!!\n");
				continue;
			}
	   }
     }
   }
   return 0;
 }

/** Initialization of  **/
void ETDetectionInit( Mat frame )
{
  std::vector<Rect> Logo;
  Mat frame_gray;
	
  resize(frame, frame, cv::Size(320, 240));
  cvtColor( frame, frame_gray, CV_BGR2GRAY );

  logo_cascade.detectMultiScale( frame_gray, Logo, 1.2, 2, 0|CV_HAAR_DO_CANNY_PRUNING, Size(25, 25), Size(100, 100) );
  
}

/** @function detectAndDisplay */
void detectAndDisplayAndAlarm( Mat frame )
{
  std::vector<Rect> Pedestrian;
  std::vector<Rect> Logo;
  Mat frame_gray;
	
  resize(frame, frame, cv::Size(320, 240));
  cvtColor( frame, frame_gray, CV_BGR2GRAY );

  //select method
  if(mode == 1)
  {
	pedestrian_cascade.detectMultiScale( frame_gray, Pedestrian, 1.2, 2, 0|CV_HAAR_DO_CANNY_PRUNING, Size(110, 100), Size(220, 200) );
	
	//Alarm
	//Pedestrian
	if(Pedestrian.size() > 0)
	{
		if(PedAlarm.status == 0)
			PedAlarm.trigger=1;
		else
			PedAlarm.trigger=0;

		PedAlarm.status = 1;
	  }
	  else
	  {
		 if(PedAlarm.status == 1)
			PedAlarm.trigger=1;
		 else
			PedAlarm.trigger=0;

		 PedAlarm.status = 0;
	  }
  }

  else if(mode == 2 || mode == 3)
  {
	logo_cascade.detectMultiScale( frame_gray, Logo, 1.1, 1, 0|CV_HAAR_DO_CANNY_PRUNING, Size(25, 25), Size(100, 100) );

	//EE Logo
	  if(Logo.size() < EELogoNum && LogoAlramBuffer < 10 && EEInitFlag==1)
		  LogoAlramBuffer++;
	  else if (Logo.size() == EELogoNum && LogoAlramBuffer > 0)
		  LogoAlramBuffer--;

	  if(LogoAlramBuffer > 8)
	  {
		  if(EELogoAlarm.status == 0)
			  EELogoAlarm.trigger=1;
		  else
			  EELogoAlarm.trigger=0;

		  EELogoAlarm.status = 1;
	  }
	  else if(LogoAlramBuffer < 3)
	  {
		  if(EELogoAlarm.status == 1)
			  EELogoAlarm.trigger=1;
		  else
			  EELogoAlarm.trigger=0;

		  EELogoAlarm.status = 0;
	  }
	  EELogoNum_TMP = Logo.size();
  }
  

  
  
  

  //Display
  for( size_t i = 0; i < Pedestrian.size(); i++ )
  {
    Point center( Pedestrian[i].x + Pedestrian[i].width*0.5, Pedestrian[i].y + Pedestrian[i].height*0.5 );
    ellipse( frame, center, Size( Pedestrian[i].width*0.5, Pedestrian[i].height*0.5*2), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
  }

  for( size_t i = 0; i < Logo.size(); i++ )
  {
    Point center( Logo[i].x + Logo[i].width*0.5, Logo[i].y + Logo[i].height*0.5 );
    ellipse( frame, center, Size( Logo[i].width*0.5, Logo[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 0 ), 4, 8, 0 );
  }

  if(PedAlarm.trigger==1 && PedAlarm.status==1 && mode ==1)
  {
	system("chmod 777 push_Ped.sh");
    system("./push_LOGO.sh");
  }
  else if(EELogoAlarm.trigger==1 && EELogoAlarm.status==1 && (mode ==2 || mode ==3))    
  {
	 if(mode ==2)
	 {
		 system("chmod 777 push_LOGO.sh");
		 system("./push_LOGO.sh");
	 }
	 else if(mode ==3)
	 {
		 system("chmod 777 push_ItemLost.sh");
		 system("./push_ItemLost.sh");
	 }
  }
 

  //-- Show what you got
  imshow( window_name, frame );
 }
